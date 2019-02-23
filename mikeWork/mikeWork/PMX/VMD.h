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

class VmdMotionController sealed //継承禁止
{
private:
	INT time;							// 時間
	BOOL m_isfinished;
	INT m_loop_flg;
	vector<Bone>* bones;
	vector<MmdStruct::PmdIkData>* pmdIkData;
	/// キーフレームアニメーション
	vector<XMFLOAT4> boneRot;					// 現在のボーンの回転
	vector<XMFLOAT3> bonePos;					// 現在のボーンの位置
	vector<list<KeyFrame>> keyFrames;				// ボーンごとにキーフレームをリストとしてもつ
	vector<list<KeyFrame>::iterator> ite_keyFrames;	// キーフレームのイテレータ
													/// IK
	void UpdateIK(const MmdStruct::PmdIkData&);		// IKボーン影響下ボーンの行列を更新
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
	void UpdateBoneMatrix();				// キーフレームデータを元にボーン行列を更新
	void AdvanceTime();
	void timeset(float timedif)
	{
		time = timedif;
	}
};

