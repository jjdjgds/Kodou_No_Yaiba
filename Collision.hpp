/*==========================================================================
当たり判定 [Collision.hpp]

Author : hidetoshi muramatu
Date   : 2025/
---------------------------------------------------------------------------
シンプルな当たり判定ユーティリティ。
RectF や Circle 同士の衝突判定を提供する。
=========================================================================*/
#pragma once
#include <Siv3D.hpp>

namespace Collision
{
	//===------------------------------------------------------
	// Rect vs Rect
	//===------------------------------------------------------
	inline bool RectToRect(const RectF& a, const RectF& b)
	{
		return a.intersects(b) || a.contains(b);
	}

	//===------------------------------------------------------
	// Circle vs Circle
	//===------------------------------------------------------
	inline bool CircleToCircle(const Circle& a, const Circle& b)
	{
		return a.intersects(b);
	}

	//===------------------------------------------------------
	// Rect vs Circle
	//===------------------------------------------------------
	inline bool RectToCircle(const RectF& rect, const Circle& circle)
	{
		return rect.intersects(circle);
	}

	//===------------------------------------------------------
	// Line vs Rect
	//===------------------------------------------------------
	inline bool LineToRect(const Line& line, const RectF& rect)
	{
		return line.intersects(rect);
	}

	//===------------------------------------------------------
	// Line vs Circle
	//===------------------------------------------------------
	inline bool LineToCircle(const Line& line, const Circle& circle)
	{
		return line.intersects(circle);
	}

	

	
}
