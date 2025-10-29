/*==========================================================================

時間停止操作[TimeStopManager.h]

												Author : hidetoshi muramatu
												Date   : 2025/10/29
---------------------------------------------------------------------------






=========================================================================*/


#pragma once
#include <Siv3D.hpp>

class TimeStopManager
{
private:
	// 各タイプ別の時間スケール
	static inline double s_playerScale = 1.0;   // プレイヤー
	static inline double s_friendScale = 1.0;   // 味方（徐々に遅く）
	static inline double s_enemyScale = 1.0;   // 敵（ほぼ停止）
	static inline double s_effectScale = 1.0;   // エフェクト（完全停止）

	static inline bool s_isActive = false;
	static inline double s_transition = 0.0;    // 補間用（0.0 → 1.0）

public:

	// ======== 時止め開始 ========
	static void Start()
	{
		if (s_isActive) return;
		s_isActive = true;
		s_transition = 0.0;
	}

	// ======== 時止め解除 ========
	static void Stop()
	{
		if (!s_isActive) return;
		s_isActive = false;
		s_transition = 0.0;
	}

	// ======== 更新（補間） ========
	static void Update()
	{
		const double dt = Scene::DeltaTime();

		if (s_isActive)
		{
			// ゆっくり停止に近づける
			s_transition = Min(s_transition + dt * 2.0, 1.0);

			// 各タイプごとにスケール設定（Lerpで滑らかに）
			s_playerScale = Math::Lerp(1.0, 0.1, s_transition);
			s_friendScale = Math::Lerp(1.0, 0.4, s_transition);
			s_enemyScale = Math::Lerp(1.0, 0.1, s_transition);
			s_effectScale = Math::Lerp(1.0, 0.0, s_transition);
		}
		else
		{
			// 再開（解除時も補間）
			s_transition = Min(s_transition + dt * 2.0, 1.0);

			s_playerScale = Math::Lerp(0.1, 1.0, s_transition);
			s_friendScale = Math::Lerp(0.4, 1.0, s_transition);
			s_enemyScale = Math::Lerp(0.1, 1.0, s_transition);
			s_effectScale = Math::Lerp(0.0, 1.0, s_transition);
		}
	}




	// ======== ゲッター ========
	static bool IsActive() { return s_isActive; }
	static double GetPlayerScale() { return s_playerScale; }
	static double GetFriendScale() { return s_friendScale; }
	static double GetEnemyScale() { return s_enemyScale; }
	static double GetEffectScale() { return s_effectScale; }
};
