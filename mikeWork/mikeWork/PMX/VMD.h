#pragma once

#pragma once
#include "Mmdstrcut.h"
#include "SkinMeshStruct.h"
#include "StaticCalc.h"
#include "../imgui/CImguiMine.h"

#include <list>
#include <iostream>
#include <sstream>
#include <fstream>

using namespace std;

class VmdMotionController sealed //�p���֎~
{
private:
	INT time;							// ����
	BOOL m_isfinished;
	INT m_loop_flg;
	vector<Bone>* bones;
	vector<MmdStruct::PmdIkData>* pmdIkData;
	/// �L�[�t���[���A�j���[�V����
	vector<XMFLOAT4> boneRot;					// ���݂̃{�[���̉�]
	vector<XMFLOAT3> bonePos;					// ���݂̃{�[���̈ʒu
	vector<list<KeyFrame>> keyFrames;				// �{�[�����ƂɃL�[�t���[�������X�g�Ƃ��Ă���
	vector<list<KeyFrame>::iterator> ite_keyFrames;	// �L�[�t���[���̃C�e���[�^
													/// IK
	void UpdateIK(const MmdStruct::PmdIkData&);		// IK�{�[���e�����{�[���̍s����X�V
public:
	VmdMotionController(LPCTSTR filename, vector<Bone>* bones, vector<MmdStruct::PmdIkData>* pmdIkData);
	VmdMotionController(void);
	bool LoadVmdFile(LPCTSTR filename, vector<Bone>* bones, vector<MmdStruct::PmdIkData>* pmdIkData, int _loopflg);
	void ResetTime() {
		time = 0;
		m_isfinished = false;
		m_loop_flg = false;
	}
	int IsFinished(void) {
		return m_isfinished;
	}
	void UpdateBoneMatrix();				// �L�[�t���[���f�[�^�����Ƀ{�[���s����X�V
	void AdvanceTime();
	void timeset(float timedif)
	{
		time = timedif;
	}
};

