#pragma once

#include "SkinMeshStruct.h"

class InitMatCalc {	// InitMatをボーンローカル座標系に変換する再帰関数を定義したクラス
public:
	static void Run(Bone* me, XMMATRIX *parentoffsetMat)
	{
		if (me->firstChild) Run(me->firstChild, &me->offsetMat);
		if (me->sibling)	Run(me->sibling, parentoffsetMat);
		if (parentoffsetMat) me->initMat *= *parentoffsetMat;
	}
};

class WorldsCalc {	// ワールド変換行列の配列を計算する再帰関数を定義したクラス
public:
	static void Run(Bone* me, const XMMATRIX *parentWorldMat, vector<XMMATRIX>* worlds) {
		XMMATRIX boneMat = me->boneMat*(*parentWorldMat);
		(*worlds)[me->id] = me->offsetMat * boneMat;
		if (me->firstChild) 
			Run(me->firstChild, &boneMat, worlds);
		if (me->sibling)	
			Run(me->sibling, parentWorldMat, worlds);
	}
};