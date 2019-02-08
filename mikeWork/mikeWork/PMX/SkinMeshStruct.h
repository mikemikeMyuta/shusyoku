#pragma once
///// スキンメッシュアニメーション用構造体 /////

///// 名称定義 ・・・ 本ヘッダ中では以下のようにボーンの座標系の名称を定義する	/////
///// ワールド座標系： 絶対座標系												/////
///// モデルローカル座標系：ワールド座標変換を含まない3Dモデルのローカル座標系	/////
///// ボーンローカル座標系：自身の親ボーンを基準にしたローカル座標系			/////

#include <d3d11.h>
#include <DirectXMath.h>
#include <string>

using namespace std;
using namespace DirectX;

// ブレンド重み付メッシュの頂点データ
struct BlendVertex {
	XMFLOAT3 position;
	float weight[3];
	unsigned short matrixIndex[4];
	XMFLOAT3 normal;
	XMFLOAT2 texture;
};

// ボーン
struct Bone {
	int id;					// ボーンID（通し番号）
	string name;			// ボーン名
	unsigned char type;		// ボーンタイプ (MMDの場合 0:回転 1:回転と移動 2:IK 3:不明 4:IK影響下 5:回転影響下 6:IK接続先 7:非表示 8:捻り 9:回転運動 )
	Bone* parent;			// 親ボーン
	Bone* firstChild;		// 第1子ボーン
	Bone* sibling;			// 次の兄弟ボーン
	XMMATRIX initMat;		// 初期姿勢行列(ボーンローカル座標系)
	XMMATRIX boneMat;		// ボーン行列(ボーンローカル座標系)
	XMMATRIX offsetMat;	// ボーンオフセット行列(モデルローカル座標系)


	Bone() : type(), id(), parent(), firstChild(), sibling() {	// コンストラクタ
		initMat = XMMatrixIdentity();
		boneMat = XMMatrixIdentity();
		offsetMat = XMMatrixIdentity();
	};
	XMMATRIX GetModelLocalBoneMat() {				// モデルローカル座標系でのボーン行列を取得
		if (parent) return boneMat * parent->GetModelLocalBoneMat();
		else return boneMat;
	}
	XMFLOAT3 GetModelLocalPosition() {		// モデルローカル座標系でのボーン位置を取得
		XMFLOAT3 f3(0, 0, 0);
		XMVECTOR v = XMLoadFloat3(&f3);//ベクターに変換
		v = XMVector3TransformCoord(v, GetModelLocalBoneMat());//トランスフォーム
		XMStoreFloat3(&f3, v);//再度変換し、Floatにする
		return f3;

	}
	~Bone() {}

	//コピーコンストラクタ　９だと勝手にしてくれる感じ？
	Bone(const Bone &b)
	{
		id = b.id;
		name = b.name;
		type = b.type;
		parent = b.parent;
		firstChild = b.firstChild;
		sibling = b.sibling;
		initMat = b.initMat;
		boneMat = b.boneMat;
		offsetMat = b.offsetMat;
	}
};

// キーフレームアニメーションデータ
struct KeyFrame {


	string boneName;		// ボーン名

	unsigned long frameNo;	// フレーム番号
	XMFLOAT3 position;	// 位置
	XMFLOAT4 rotation;// 回転 変換時　vectorにしてクォータニオンを使う
	XMFLOAT2 interpolation_x[2];	// position.xのベジェ補完パラメータ
	XMFLOAT2 interpolation_y[2];	// position.yのベジェ補完パラメータ
	XMFLOAT2 interpolation_z[2];	// position.zのベジェ補完パラメータ
	XMFLOAT2 interpolation_r[2];	// rotationのベジェ補完パラメータ

	bool operator < (const KeyFrame &k) const {	// フレーム番号でソートするための比較演算子
		return frameNo < k.frameNo;
	}

	KeyFrame() {}
	//なんでかこれでビルドが通る　消すとエラー
	//再度バグ原因になる　コピーコンストラクタによりデータ消失　
	KeyFrame(const KeyFrame &k)
	{
		boneName = k.boneName;

		frameNo = k.frameNo;
		position = k.position;
		rotation = k.rotation;
		interpolation_x[0] = k.interpolation_x[0];
		interpolation_x[1] = k.interpolation_x[1];
		interpolation_y[0] = k.interpolation_y[0];
		interpolation_y[1] = k.interpolation_y[1];
		interpolation_z[0] = k.interpolation_z[0];
		interpolation_z[1] = k.interpolation_z[1];
		interpolation_r[0] = k.interpolation_r[0];
		interpolation_r[1] = k.interpolation_r[1];
	}
	~KeyFrame() {}
};