#include "collision.h"

//*****************************************************************************
//!	@file	collision.cpp
//!	@brief	
//!	@note	当たり判定に使用するための関数群
//!	@author	
//*****************************************************************************

//-----------------------------------------------------------------------------
//	Include header files.
//-----------------------------------------------------------------------------
#include	<math.h>
#include	"collision.h"

//==============================================================================
//!	@fn		CreatePlaneInfo
//!	@brief　平面の方程式を求める（頂点は時計回りで渡す）
//!	@param	頂点１座標
//!	@param	頂点２座標
//!	@param	頂点３座標
//!	@param	平面の方程式
//!	@retval	なし
//==============================================================================
void CreatePlaneInfo(const XMVECTOR& p1,
	const XMVECTOR& p2,
	const XMVECTOR& p3,
	PLANE& plane)
{
	XMVECTOR		p1p2;
	XMVECTOR		p2p3;
	XMVECTOR		normal;

	p1p2 = p2 - p1;
	p2p3 = p3 - p2;

	normal = XMVector3Cross(p1p2, p2p3);
	normal = XMVector3Normalize(normal);
	plane.a = normal.m128_f32[0];
	plane.b = normal.m128_f32[1];
	plane.c = normal.m128_f32[2];

	plane.d = -(plane.a * p1.m128_f32[0] + plane.b * p1.m128_f32[1] + plane.c * p1.m128_f32[2]);
}

//==============================================================================
//!	@fn		LinetoPlaneCross
//!	@brief　直線と平面の交点を求める
//!	@param	平面の方程式
//!	@param	直線の起点
//!	@param	直線の方向ベクトル
//!	@param	交点座標
//!	@retval	true 交点あり　false 交点なし
//==============================================================================
bool LinetoPlaneCross(const PLANE&		plane,		//平面の方程式
	const XMVECTOR&	line_o,						//直線の起点
	const XMVECTOR&	line_v,						//直線の方向ベクトル
	XMVECTOR&			ans)					//交点座標
{
	float Parametric;//媒介変数
	float CheckCross = plane.a*line_v.m128_f32[0] + plane.b*line_v.m128_f32[1] + plane.c*line_v.m128_f32[2];//交点の計算　ベクトル×平面のabcをすべて足す。
	if (fabs(CheckCross) > 0)//計算結果の絶対値がより大きい　つまり、交点がある
	{
		Parametric = -(plane.d + (plane.a*line_o.m128_f32[0] + plane.b*line_o.m128_f32[1] + plane.c*line_o.m128_f32[2])) / CheckCross;
		ans = line_o + Parametric*line_v;
		return true;
	}
	return false;
	// ここを完成させる
}

//==============================================================================
//!	@fn		CheckInTriangle
//!	@brief　３角形の内部にあるかどうかを判定
//!	@param	３角形の第１頂点座標
//!	@param	３角形の第２頂点座標
//!	@param	３角形の第３頂点座標
//!	@param	判定したい頂点座標
//!	@retval	true 内部　false 外部
//==============================================================================
bool CheckInTriangle(const XMVECTOR&	a,
	const XMVECTOR&	b,
	const XMVECTOR&	c,
	const XMVECTOR&	p) {

	XMVECTOR  ab, bc, ca;			// ３辺のベクトル
	XMVECTOR  ap, bp, cp;			// 内部の点とのベクトル
	XMVECTOR	 normal;			// ３角形の法線ベクトル
	XMVECTOR	 n1, n2, n3;			// ３辺と内部の点との法線ベクトル

	ab = b - a;
	bc = c - b;
	ca = a - c;

	ap = p - a;
	bp = p - b;
	cp = p - c;

	//D3DXVec3Cross(&normal, &ab, &bc);		// 外積を計算
	normal = XMVector3Cross(ab, bc);

	//D3DXVec3Cross(&n1, &ab, &ap);			// 外積を計算
	//D3DXVec3Cross(&n2, &bc, &bp);			// 外積を計算
	//D3DXVec3Cross(&n3, &ca, &cp);			// 外積を計算
	n1 = XMVector3Cross(ab, ap);
	n2 = XMVector3Cross(bc, bp);
	n3 = XMVector3Cross(ca, cp);

	//if (D3DXVec3Dot(&n1, &normal) < 0) return false;			// 為す角度が鈍角
	//if (D3DXVec3Dot(&n2, &normal) < 0) return false;			// 為す角度が鈍角
	//if (D3DXVec3Dot(&n3, &normal) < 0) return false;			// 為す角度が鈍角

	float workDot = n1.m128_f32[0] * normal.m128_f32[0] + n1.m128_f32[1] * normal.m128_f32[1] + n1.m128_f32[2] * normal.m128_f32[2];//内積の計算
	if (workDot < 0) return false;			// 為す角度が鈍角

	workDot = n2.m128_f32[0] * normal.m128_f32[0] + n2.m128_f32[1] * normal.m128_f32[1] + n2.m128_f32[2] * normal.m128_f32[2];//内積の計算
	if (workDot < 0) return false;			// 為す角度が鈍角

	workDot = n3.m128_f32[0] * normal.m128_f32[0] + n3.m128_f32[1] * normal.m128_f32[1] + n3.m128_f32[2] * normal.m128_f32[2];//内積の計算
	if (workDot < 0) return false;			// 為す角度が鈍角

	return true;
}

//==============================================================================
//!	@fn		LengthPointToPlane
//!	@brief　平面と点の距離を求める	また同時に交点の座標を求める
//!	@param	点の座標
//!	@param	平面の方程式
//!	@param	交点の座標
//!	@retval	点と平面の距離
//==============================================================================
float LengthPointToPlane(const XMVECTOR& p,
	const PLANE& plane,
	XMVECTOR& crosspoint)
{
	float dot_work = plane.a*p.m128_f32[0] + plane.b*p.m128_f32[1] + plane.c*p.m128_f32[2] + plane.d;
	float normal_len = plane.a*plane.a + plane.b*plane.b + plane.c*plane.c;

	// 交点を求める
	float t = -1.0f * dot_work / normal_len;

	crosspoint.m128_f32[0] = p.m128_f32[0] + plane.a * t;
	crosspoint.m128_f32[1] = p.m128_f32[1] + plane.b * t;
	crosspoint.m128_f32[2] = p.m128_f32[2] + plane.c * t;

	float length = fabsf(dot_work); // ÷ sqrtf(normal_len);	// 平面の法線を正規化しているのでsqrtは省きました

	return length;
}
//******************************************************************************
//	End of file.
//******************************************************************************
