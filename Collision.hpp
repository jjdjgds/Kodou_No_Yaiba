/*==========================================================================
当たり判定 [Collision.hpp]

Author : ムラムラ
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

	

	//===------------------------------------------------------
	// 任意オブジェクト vs 任意オブジェクト
	// Object が getRect() / getCircle() を持つことを前提
	//===------------------------------------------------------
	/*template <class T1, class T2>
	inline bool Check(const T1& obj1, const T2& obj2)
	{
		if constexpr (requires { obj1.getRect(); obj2.getRect(); })
		{
			return RectToRect(obj1.getRect(), obj2.getRect());
		}
		else if constexpr (requires { obj1.getCircle(); obj2.getCircle(); })
		{
			return CircleToCircle(obj1.getCircle(), obj2.getCircle());
		}
		else if constexpr (requires { obj1.getRect(); obj2.getCircle(); })
		{
			return RectToCircle(obj1.getRect(), obj2.getCircle());
		}
		else if constexpr (requires { obj1.getCircle(); obj2.getRect(); })
		{
			return RectToCircle(obj2.getRect(), obj1.getCircle());
		}
		else
		{
			return false;
		}
	}*/
}
