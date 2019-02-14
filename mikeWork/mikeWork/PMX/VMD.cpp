#include "VMD.h"

using namespace std;

VmdMotionController::VmdMotionController(void) {
};

VmdMotionController::VmdMotionController(LPCTSTR filename, vector<Bone>* b, vector<MmdStruct::PmdIkData>* p) : bones(b), pmdIkData(p), time() {
	const int frame_rate = 60;			// 本プログラムのフレームレート
	const int mmd_frame_rate = 30;		// MMDのフレームレート
										 //VMDファイルからVMDデータを抽出
	ifstream ifs(filename, ios::binary);
	if (ifs.fail()) throw TEXT("ファイルがありません");
	MmdStruct::VmdHeader vmdHeader;
	ifs.read((char*)&vmdHeader, sizeof(MmdStruct::VmdHeader));
	unsigned long numVmdMotion;
	ifs.read((char*)&numVmdMotion, sizeof(numVmdMotion));
	vector<MmdStruct::VmdMotion> vmdMotions(numVmdMotion);
	ifs.read((char*)&vmdMotions[0], sizeof(MmdStruct::VmdMotion)*numVmdMotion);
	// KeyFramesに格納
	keyFrames.resize(bones->size());
	for (unsigned int i = 0; i < vmdMotions.size(); ++i) {
		KeyFrame keyFrame;
		keyFrame.boneName = vmdMotions[i].boneName;
		keyFrame.frameNo = vmdMotions[i].frameNo;
		keyFrame.frameNo *= frame_rate / mmd_frame_rate;
		keyFrame.position = XMFLOAT3(vmdMotions[i].location[0], vmdMotions[i].location[1], vmdMotions[i].location[2]);
		keyFrame.position.x *= MmdStruct::scale;
		keyFrame.position.y *= MmdStruct::scale;
		keyFrame.position.z *= MmdStruct::scale;
		keyFrame.rotation = XMFLOAT4(vmdMotions[i].rotation[0], vmdMotions[i].rotation[1], vmdMotions[i].rotation[2], vmdMotions[i].rotation[3]);
		keyFrame.interpolation_x[0] = XMFLOAT2(vmdMotions[i].interpolation[0], vmdMotions[i].interpolation[4]);
		keyFrame.interpolation_x[1] = XMFLOAT2(vmdMotions[i].interpolation[8], vmdMotions[i].interpolation[12]);
		keyFrame.interpolation_y[0] = XMFLOAT2(vmdMotions[i].interpolation[1], vmdMotions[i].interpolation[5]);
		keyFrame.interpolation_y[1] = XMFLOAT2(vmdMotions[i].interpolation[9], vmdMotions[i].interpolation[13]);
		keyFrame.interpolation_z[0] = XMFLOAT2(vmdMotions[i].interpolation[2], vmdMotions[i].interpolation[6]);
		keyFrame.interpolation_z[1] = XMFLOAT2(vmdMotions[i].interpolation[10], vmdMotions[i].interpolation[14]);
		keyFrame.interpolation_r[0] = XMFLOAT2(vmdMotions[i].interpolation[3], vmdMotions[i].interpolation[7]);
		keyFrame.interpolation_r[1] = XMFLOAT2(vmdMotions[i].interpolation[11], vmdMotions[i].interpolation[15]);
		for (unsigned int j = 0; j < bones->size(); ++j) {
			if (keyFrame.boneName == (*bones)[j].name) {	// ボーン名からボーン番号を探す
				keyFrames[j].push_back(keyFrame);
				break;
			}
		}
	}
	for (unsigned int i = 0; i < bones->size(); ++i) {
		keyFrames[i].sort();
		ite_keyFrames.push_back(keyFrames[i].begin());
		boneRot.push_back(XMFLOAT4(0, 0, 0, 0));
		bonePos.push_back(XMFLOAT3(0, 0, 0));
	}
	UpdateBoneMatrix();
}

bool VmdMotionController::LoadVmdFile(LPCTSTR filename, vector<Bone>* _bones, vector<MmdStruct::PmdIkData>* _pmdIkData, int loopflg)
{
	const int frame_rate = 60;			// 本プログラムのフレームレート
	const int mmd_frame_rate = 30;		// MMDのフレームレート

	bones = _bones;
	pmdIkData = _pmdIkData;
	time = 0;
	m_isfinished = false;
	m_loop_flg = loopflg;
	//VMDファイルからVMDデータを抽出
	ifstream ifs(filename, ios::binary);
	if (ifs.fail()) return false;
	MmdStruct::VmdHeader vmdHeader;
	ifs.read((char*)&vmdHeader, sizeof(MmdStruct::VmdHeader));
	unsigned long numVmdMotion;
	ifs.read((char*)&numVmdMotion, sizeof(numVmdMotion));
	vector<MmdStruct::VmdMotion> vmdMotions(numVmdMotion);
	ifs.read((char*)&vmdMotions[0], sizeof(MmdStruct::VmdMotion)*numVmdMotion);
	//KeyFramesに格納
	keyFrames.resize(bones->size());
	for (unsigned int i = 0; i < vmdMotions.size(); ++i) {
		KeyFrame keyFrame;
		keyFrame.boneName = vmdMotions[i].boneName;
		keyFrame.frameNo = vmdMotions[i].frameNo;
		keyFrame.frameNo *= frame_rate / mmd_frame_rate;
		keyFrame.position = XMFLOAT3(vmdMotions[i].location[0], vmdMotions[i].location[1], vmdMotions[i].location[2]);
		keyFrame.position.x *= MmdStruct::scale;
		keyFrame.position.y *= MmdStruct::scale;
		keyFrame.position.z *= MmdStruct::scale;
		keyFrame.rotation = XMFLOAT4(vmdMotions[i].rotation[0], vmdMotions[i].rotation[1], vmdMotions[i].rotation[2], vmdMotions[i].rotation[3]);
		keyFrame.interpolation_x[0] = XMFLOAT2(vmdMotions[i].interpolation[0], vmdMotions[i].interpolation[4]);
		keyFrame.interpolation_x[1] = XMFLOAT2(vmdMotions[i].interpolation[8], vmdMotions[i].interpolation[12]);
		keyFrame.interpolation_y[0] = XMFLOAT2(vmdMotions[i].interpolation[1], vmdMotions[i].interpolation[5]);
		keyFrame.interpolation_y[1] = XMFLOAT2(vmdMotions[i].interpolation[9], vmdMotions[i].interpolation[13]);
		keyFrame.interpolation_z[0] = XMFLOAT2(vmdMotions[i].interpolation[2], vmdMotions[i].interpolation[6]);
		keyFrame.interpolation_z[1] = XMFLOAT2(vmdMotions[i].interpolation[10], vmdMotions[i].interpolation[14]);
		keyFrame.interpolation_r[0] = XMFLOAT2(vmdMotions[i].interpolation[3], vmdMotions[i].interpolation[7]);
		keyFrame.interpolation_r[1] = XMFLOAT2(vmdMotions[i].interpolation[11], vmdMotions[i].interpolation[15]);
		for (unsigned int j = 0; j < bones->size(); ++j) {
			if (keyFrame.boneName == (*bones)[j].name) {	// ボーン名からボーン番号を探す
				keyFrames[j].push_back(keyFrame);
				break;
			}
		}
	}
	for (unsigned int i = 0; i < bones->size(); ++i) {
		keyFrames[i].sort();
		ite_keyFrames.push_back(keyFrames[i].begin());
		boneRot.push_back(XMFLOAT4(0, 0, 0, 0));
		bonePos.push_back(XMFLOAT3(0, 0, 0));
	}
	UpdateBoneMatrix();
	return true;
}

void VmdMotionController::UpdateBoneMatrix() {
	bool finish_flg = true;
	for (unsigned int i = 0; i < bones->size(); i++) {
		//キーフレーム補完
		unsigned long t0, t1;
		XMFLOAT4 q0, q1;
		XMFLOAT3 p0, p1;
		XMVECTOR keepVec;//保管用
		if (ite_keyFrames[i] != keyFrames[i].end()) {
			t0 = (*ite_keyFrames[i]).frameNo;
			boneRot[i] = q0 = (*ite_keyFrames[i]).rotation;
			bonePos[i] = p0 = (*ite_keyFrames[i]).position;
			if (++ite_keyFrames[i] != keyFrames[i].end()) {
				t1 = (*ite_keyFrames[i]).frameNo;
				q1 = (*ite_keyFrames[i]).rotation;
				p1 = (*ite_keyFrames[i]).position;
				float s = (float)(time - t0) / (float)(t1 - t0);	// 線形補完
				keepVec = XMQuaternionSlerp(XMLoadFloat4(&q0), XMLoadFloat4(&q1), s);
				XMStoreFloat4(&boneRot[i], keepVec);
				keepVec = XMVectorAdd(XMLoadFloat3(&p0), 
					(XMVectorSubtract(XMLoadFloat3(&p1), XMLoadFloat3(&p0))*s)
				);
				XMStoreFloat3(&bonePos[i], keepVec);
				if (time != t1) --ite_keyFrames[i];
				finish_flg = false;
			}
		}

		//ここまで10/1

		//再開11/30

		 //親ボーン座標系のボーン行列を求める
		XMMATRIX rot, trans;
		rot = XMMatrixRotationQuaternion(XMLoadFloat4(&boneRot[i]));
		trans = XMMatrixTranslation(bonePos[i].x, bonePos[i].y, bonePos[i].z);

		(*bones)[i].boneMat = rot * trans*(*bones)[i].initMat;
		//IK影響下のボーンも含めてリセットする → 動きが不連続的になりやすい
		//if ((*bones)[i].type != 4) (*bones)[i].boneMat = rot * trans*(*bones)[i].initMat;	// IK影響下のボーンをリセットしない → 動きが連続的になるが時間と共にねじれていく
	}
	for (unsigned int i = 0; i < pmdIkData->size(); ++i) {
		UpdateIK((*pmdIkData)[i]);
	}
	m_isfinished = finish_flg;
	if (finish_flg && m_loop_flg) {
		time = 0;
		for (unsigned int i = 0; i < bones->size(); i++) {
			ite_keyFrames[i] = keyFrames[i].begin();
		}
	}
}

void VmdMotionController::UpdateIK(const MmdStruct::PmdIkData& ikData) {


	XMFLOAT3 localEffectorPos, localTargetPos;
	XMVECTOR localEffectorPosVec, localTargetPosVec;

	for (unsigned int j = 0; j < ikData.iterations; ++j) {												// IKデータの数分ループ

		for (unsigned int i = 0; i < ikData.ik_child_bone_index.size(); ++i) {								// 子供ボーンの数分ループ

			unsigned short attentionIdx = ikData.ik_child_bone_index[i];									// ＩＫ影響下のボーン番号

			XMFLOAT3 effectorPos = (*bones)[ikData.ik_target_bone_index].GetModelLocalPosition();		// エフェクタの位置

			XMFLOAT3 targetPos = (*bones)[ikData.ik_bone_index].GetModelLocalPosition();					// ターゲットの位置

			XMMATRIX invCoord;
			invCoord = XMMatrixInverse(0, (*bones)[attentionIdx].GetModelLocalBoneMat());

		

			localEffectorPosVec = XMVector3TransformCoord(XMLoadFloat3(&effectorPos), invCoord);						// 注目ボーン基準に変換（エフェクター）
			XMStoreFloat3(&localEffectorPos, localEffectorPosVec);//Float3に変換

			localTargetPosVec = XMVector3TransformCoord(XMLoadFloat3(&targetPos), invCoord);							// 注目ボーン基準に変換（ターゲット）
			XMStoreFloat3(&localTargetPos, localTargetPosVec);//Float3に変換

			XMFLOAT3 localEffectorDir;	// エフェクタのローカル方向（注目ボーンの位置基準）
			XMVECTOR localEffectorDirVec;
			localEffectorDirVec = XMVector3Normalize(localEffectorPosVec);
			XMStoreFloat3(&localEffectorDir, localEffectorDirVec);//Float3に変換

			XMFLOAT3 localTargetDir;															// エフェクタのローカル方向（注目ボーンの位置基準）
			XMVECTOR localTargetDirVec;// ターゲットのローカル方向（注目ボーンの位置基準）
			localTargetDirVec = XMVector3Normalize(localTargetPosVec);
			XMStoreFloat3(&localTargetDir, localTargetDirVec);//Float3に変換

			if ((*bones)[attentionIdx].name.find("ひざ") != string::npos) {

				//正規化　エフェクト
				localEffectorDir = XMFLOAT3(0, localEffectorDir.y, localEffectorDir.z);
				localEffectorDirVec = XMLoadFloat3(&localEffectorDir);
				localEffectorDirVec = XMVector3Normalize(localEffectorDirVec);
				XMStoreFloat3(&localEffectorDir, localEffectorDirVec);//Float3に変換

				//正規化　ターゲット
				localTargetDir = XMFLOAT3(0, localTargetDir.y, localTargetDir.z);
				localTargetDirVec = XMLoadFloat3(&localTargetDir);
				localTargetDirVec = XMVector3Normalize(localTargetDirVec);
				XMStoreFloat3(&localTargetDir, localTargetDirVec);//Float3に変換
			}

			XMVECTOR pVec = XMVector3Dot(localEffectorDirVec, localTargetDirVec);
			float p=0;
			XMStoreFloat(&p, pVec);
			if (p > 1.0f) {
				p = 1.0f;
			}
			if (p < 0.0f) {
				p = 0.0f;
			}
			if (p > 1 - 1.0e-8f) continue;	// 計算誤差により1を越えるとacos()が発散するので注意!

			//角度を求める
			float angle = acos(p);

		/*	if (angle > 4 * ikData.control_weight) {
				angle = 4.0f*ikData.control_weight;
			}*/
			if (angle > ikData.control_weight) {
				angle = ikData.control_weight;
			}

			XMVECTOR axis;
			axis=XMVector3Cross(localEffectorDirVec, localTargetDirVec);
		
			XMMATRIX rotation;
			rotation=XMMatrixRotationAxis(axis, angle);

			if ((*bones)[attentionIdx].name.find("ひざ") != string::npos) {
				XMMATRIX inv;
				inv=XMMatrixInverse( 0, (*bones)[attentionIdx].initMat);

				XMMATRIX def = rotation * (*bones)[attentionIdx].boneMat*inv;
				XMFLOAT3 t(0, 0, 1);
				XMVECTOR tVec = XMLoadFloat3(&t);

				tVec=XMVector3TransformCoord(tVec, def);

				XMStoreFloat3(&t, tVec);

				if (t.y < 0) {
					rotation=XMMatrixRotationAxis(axis, -angle);
				}

				//膝ボーンがエフェクター（ターゲットボーン）より近いときは回転量を追加する(
					XMVECTOR lVec = XMVector3Length(localTargetPosVec) / XMVector3Length(localEffectorPosVec);
					FLOAT l = 0;
					XMStoreFloat(&l, lVec);
				if (fabs(angle) <= XM_PI / 2 && l < 1.0f) {
					static const float a = 0.5f;
					float diff = acosf(l)*a;
					static const float diff_limit = XM_PI / 6.0f;
					if (diff > diff_limit) {
						diff = diff_limit;
					}
					if (fabs(angle) > 1.0e-6f) {
						diff *= angle / fabs(angle);
					}
					angle += diff;
				}
			}

			(*bones)[attentionIdx].boneMat = rotation * (*bones)[attentionIdx].boneMat;
		}

		const float errToleranceSq = 0.000001f;
		XMVECTOR ComparisonVec = XMVector3LengthSq(localEffectorPosVec - localTargetPosVec);
		FLOAT Comparison = 0;
		XMStoreFloat(&Comparison, ComparisonVec);
		if (Comparison < errToleranceSq) {
			return;
		}
	}

}

void VmdMotionController::AdvanceTime() {
	++time;
}

