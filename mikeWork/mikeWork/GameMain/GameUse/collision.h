#pragma once
#include "../../BASE/BASE.h"

// 平面の方程式
struct	PLANE {
	float	a, b, c, d;			// 平面方程式の係数（ ax+by+cz+d=0 )
};

	// プロトタイプ宣言
	void CreatePlaneInfo(const XMVECTOR& p1, const XMVECTOR& p2, const XMVECTOR& p3, PLANE& plane);

	bool LinetoPlaneCross(const PLANE& plane,			//平面の方程式
		const XMVECTOR& line_o,	//直線が通る点
		const XMVECTOR& line_v,	//直線の方向ベクトル
		XMVECTOR& ans);			//交点座標

	bool CheckInTriangle(const XMVECTOR& a,
		const XMVECTOR& b,
		const XMVECTOR& c,
		const XMVECTOR& p);

	float LengthPointToPlane(const XMVECTOR& p, const PLANE& plane, XMVECTOR& crosspoint);
