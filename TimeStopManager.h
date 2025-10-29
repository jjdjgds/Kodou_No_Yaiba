/*==========================================================================

時間停止操作[TimeStopManager.h]

												Author : hidetoshi muramatu
												Date   : 2025/10/29
---------------------------------------------------------------------------






=========================================================================*/


#pragma once
class TimeStopManager
{
private:
	static double s_enemyScale;   // 敵用の時間スケール
	static double s_effectScale;  // エフェクト用の時間スケール
	static bool   s_isActive;     // 時止め中かどうか

public:
	static void Start()
	{
		s_isActive = true;
		s_enemyScale = 0.15;   // 敵は15%速度
		s_effectScale = 0.0;   // エフェクト完全停止
	}

	static void Stop()
	{
		s_isActive = false;
		s_enemyScale = 1.0;
		s_effectScale = 1.0;
	}

	static void Update() // 徐々に補間したい場合
	{
		const double targetEnemy = s_isActive ? 0.15 : 1.0;
		const double targetEffect = s_isActive ? 0.0 : 1.0;
		s_enemyScale = Math::Lerp(s_enemyScale, targetEnemy, 0.1);
		s_effectScale = Math::Lerp(s_effectScale, targetEffect, 0.1);
	}

	static double GetEnemyScale() { return s_enemyScale; }
	static double GetEffectScale() { return s_effectScale; }
	static bool   IsActive() { return s_isActive; }
};
