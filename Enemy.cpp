
#include "Game.hpp"
#include "Enemy.hpp"
#include "Collision.hpp"
#include "Player.hpp"

using namespace Collision;

RectF Enemy::hurtRect() const // 被弾判定用
{
	const SizeF sz{ m_hitBox.x * m_Scale.x, m_hitBox.y * m_Scale.y };
	return RectF{ Arg::center = m_Position.movedBy(0, m_hitOffsetY * m_Scale.y), sz };
}

RectF Enemy::hurtRectAt(const Vec2& pos) const
{
	const SizeF sz{ m_hitBox.x * m_Scale.x, m_hitBox.y * m_Scale.y };
	return RectF{ Arg::center = pos.movedBy(0, m_hitOffsetY * m_Scale.y), sz };
}

RectF Enemy::attackRect() const // 攻撃判定用
{
	const double xOffset = (m_FaceRight ? 17.0 : -17.0);
	const SizeF sz{ m_hitBox.x * m_Scale.x, m_hitBox.y * m_Scale.y };
	return RectF{ Arg::center = m_Position.movedBy(xOffset * m_Scale.x, m_hitOffsetY * m_Scale.y), sz };
}

Line Enemy::makeGroundProbeLine() const // 地面チェック
{
	const double fwd = 17 * m_Scale.x;
	const double down = 40.0 * m_Scale.y;
	const Vec2 dir = (m_FaceRight ? Vec2{ +fwd, +down } : Vec2{ -fwd, +down });
	return Line{ m_Position, m_Position + dir };
}

//----------------------------------------------
// ここから update()
//----------------------------------------------
void Enemy::update(Player& player, Game_Map& map)
{
	const double dt = Scene::DeltaTime();

	//=== 落下処理 ===//
	m_velY += m_gravity * dt;
	Vec2 tryPos = m_Position;
	tryPos.y += m_velY * dt;

	RectF testY = hurtRectAt(tryPos);
	if (map.CheckCollision(testY))
	{
		const double step = 2.0;
		int guard = 0;
		while (map.CheckCollision(testY) && guard++ < 200)
		{
			tryPos.y -= Math::Sign(m_velY) * step;
			testY = hurtRectAt(tryPos);
		}
		m_Position.y = tryPos.y;
		m_velY = 0.0;
		m_onGround = true;
	}
	else
	{
		m_Position.y = tryPos.y;
		m_onGround = false;
	}

	//=== 巡回範囲制御 ===//
	if (m_Position.x > m_patrolR) { m_Position.x = m_patrolR; m_FaceRight = false; }
	if (m_Position.x < m_patrolL) { m_Position.x = m_patrolL; m_FaceRight = true; }

	const Line probe = makeGroundProbeLine();
	if (!map.CheckCollision_Line(probe))
	{
		m_FaceRight = !m_FaceRight;
	}

	//=== 矩形生成 ===//
	const RectF eBoxHurt = hurtRect();
	const RectF eAttack = attackRect();
	const RectF pHitBox(Arg::center = player.GetPlayerPosition(), player.GetPlayerHitBox());
	const RectF pAttackBox(Arg::center = player.GetPlayerPosition(), player.GetPlayerAttackRengeBox());

	//------------------------------------------
	// テストクリックで敵ダメージ
	//------------------------------------------
	if (eBoxHurt.leftClicked())
	{
		takeDamage(1);
	}

	//------------------------------------------
	// 敵攻撃 → プレイヤーダメージ
	//------------------------------------------
	if (RectToRect(eAttack, pHitBox) && (m_state == AnimState::Attack))
	{
		// 回避中や被弾中は無視（Player側が判断）
		player.takeDamage(1);
	}

	//------------------------------------------
	// プレイヤー攻撃 → 敵ダメージ
	//------------------------------------------
	bool gotHit = (RectToRect(pAttackBox, eBoxHurt) && (player.GetPlayerState() == StateMode::Attack)) || m_takeDamage;

	if (gotHit)
	{
		if (m_state != AnimState::Hurt)
		{
			setState(AnimState::Hurt);
			m_Speed = 0.0f;
		}
	}
	else if (m_state == AnimState::Attack)
	{
		// 攻撃中は行動固定
	}
	else if (KeySpace.down() && !AttackFlag)
	{
		setState(AnimState::Attack);
		m_Speed = 0.0f;
	}
	else
	{
		m_Speed = (KeyS.pressed()) ? m_speedBase : 0.0;

		float vx = (m_FaceRight ? 1.0f : -1.0f) * m_Speed;
		m_Position.x += vx * dt;

		if (std::abs(vx) > 1.0f)
			setState(AnimState::Run);
		else
			setState(AnimState::Idle);
	}

	//------------------------------------------
	// アニメーション更新
	//------------------------------------------
	m_time += Scene::DeltaTime();
	const auto& A = m_anims[m_state];

	while (m_time >= A.frameTime)
	{
		m_time -= A.frameTime;

		if (A.loop)
		{
			m_frameIndex = (m_frameIndex + 1) % A.frames;
		}
		else
		{
			if (m_frameIndex < (A.frames - 1))
			{
				++m_frameIndex;
			}
			else
			{
				if (m_state == AnimState::Hurt)
				{
					m_takeDamage = false;
					setState(AnimState::Idle);
				}
				else if (m_state == AnimState::Attack)
				{
					AttackFlag = false;
					setState(AnimState::Idle);
				}
				break;
			}
		}
	}

	//------------------------------------------
	// デバッグ描画
	//------------------------------------------
	if (m_debugDraw)
	{
		eBoxHurt.drawFrame(2.0, Palette::Red);
		eAttack.drawFrame(2.0, Palette::Yellow);
		probe.draw(2, Palette::Aqua);
	}
}

//----------------------------------------------
// draw()
//----------------------------------------------
void Enemy::draw() const
{
	const auto& A = m_anims.at(m_state);
	const Texture& tex = TextureAsset(A.asset);

	const int32 frameW = tex.width() / A.frames;
	const int32 frameH = tex.height();
	const int32 sx = m_frameIndex * frameW;

	auto reg = tex(sx, 0, frameW, frameH);
	(m_FaceRight ? reg : reg.mirrored())
		.scaled(m_Scale.x, m_Scale.y)
		.drawAt(m_Position);
}

//----------------------------------------------
// takeDamage()
//----------------------------------------------
void Enemy::takeDamage(int damage)
{
	if (!m_takeDamage)
	{
		m_takeDamage = true;
		m_frameIndex = 0;
		m_time = 0.0;
	}
}
