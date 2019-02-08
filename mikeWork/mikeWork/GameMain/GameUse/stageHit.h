#pragma once
#include "collision.h"
// 地形データの面の方程式情報
struct PLANEINFO {
	PLANE	plane;				//平面の方程式
	XMVECTOR	p0, p1, p2;		//頂点座標
};