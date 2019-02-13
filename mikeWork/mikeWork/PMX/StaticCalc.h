#pragma once

#include "SkinMeshStruct.h"

class InitMatCalc {	// InitMat���{�[�����[�J�����W�n�ɕϊ�����ċA�֐����`�����N���X
public:
	static void Run(Bone* me, XMMATRIX *parentoffsetMat)
	{
		if (me->firstChild) Run(me->firstChild, &me->offsetMat);
		if (me->sibling)	Run(me->sibling, parentoffsetMat);
		if (parentoffsetMat) me->initMat *= *parentoffsetMat;
	}
};

class WorldsCalc {	// ���[���h�ϊ��s��̔z����v�Z����ċA�֐����`�����N���X
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