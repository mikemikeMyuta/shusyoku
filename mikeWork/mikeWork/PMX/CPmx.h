#pragma once

#include "PmxLoad.h"
#include "VMD.h"
#include "../shader/shaderList.h"
#include <map>
#include <deque>
#include <vector>
#include <d3dcompiler.h>
#include "StaticCalc.h"
#include "../shadow/CShadow.h"

//use namespace
using namespace PmxStruct;
using namespace std;

//構造体


enum DrawingType
{
	player = 0,
	object,
	shadow,
	shadowMap,
	Final //配列に使うかも・・・
};

//enum 

//Control Draw image


//pmx class 
class CPmx :public CLoadPMX//VMDも継承する これを継承か使用して制作する
{
public:
	static void Render(ID3D11DeviceContext*);//描画実行

	void Init(ID3D11Device*, const LPCSTR, const LPCWSTR, const LPCWSTR, const LPCWSTR, LPSTR);
	/*
	第一引数　デバイス
	第二引数　使用するモデルの名前
	第三引数　ピクセルシェーダー
	第四引数　ジオメトリシェーダー
	第五引数　バーテックスシェーダー
	第六引数	   テクスチャのパス
	*/

	//11/15　真ん中求める処理がいるかも　注視点的な意味で　（追記　11/27）いる？

	void ProcessingCalc(XMFLOAT3);//描画の計算処理とコンスタンスバッファ作成　そのままDrawを呼び出す。 DrawingType mean selecting mainchar or other 

	static map<LPCSTR, PMX_DATA> RetrievalData;//addするときに必要ならロード必要ないならそのデータを受け渡しするために検索するためのデータ
	static deque<DrawingAllDataObject> RenderingDataObject;//描画するときに使用
	//or
	static deque<DrawingAllDataMainCharcter> RenderingDataMain;//描画するときに使用
											   //VMDもいる感じですかぁ～　いります
	static deque<DrawingAllDataShadow> RenderingDataShadow;//描画するときに使用

	ObjectIndividualData *IndiviData;//独自でデータを持たせる

	vector<ID3D11ShaderResourceView*> TexData;//テクスチャのデータを格納

	CShadow Cshadow;
	UINT MorphNum;//モーフナンバー格納
	UINT WorldMorphNum;//モーフナンバー格納

	PMX_SEND_DATA *VertexBufferUpdate;

	//drawを変える 11/9　clear (11/9)

	CONSTANT_BUFFER_OBJECT cbo;
	CONSTANT_BUFFER_MAINCHARCTER cbm;
	CONSTANT_BONE_MATRIX cbmr;
	CONSTANT_BUFFER cb;
	//11/9：draw後の処理をRenderに clear ()

	//create input layout
	void Cretaelayout(ID3D11Device* Device);


	void setPosition(XMFLOAT3 pos) { Position = pos; }
	void setRotetation(XMFLOAT3 rot) { Rotetation = rot; }
	XMFLOAT3 getPosition() { return Position; }
	XMFLOAT3 getRotetation() { return Rotetation; }
	ID3D11ShaderResourceView* getTexResource(UINT indexNumber) { return TexData[indexNumber]; }//格納したテクスチャ受け渡す。
	//使用理想	ID3D11ShaderResourceView* Textures =getTexResource(getIndexNum);Texturesを使用する

	void SetMotion(VmdMotionController* motion) { m_VmdMotionController = motion; }

	vector<Bone>* DeliverBones() { return &bones; }
	vector<MmdStruct::PmdIkData>* DeliverPmdIkData() { return &pmdIkData; }
	void SetBone(vector<Bone>* b) {
		bones.resize(b->size());
		copy(b->begin(), b->end(), bones.begin());
	}
	void SetPmdIKdata(vector<MmdStruct::PmdIkData>* data) {
		pmdIkData.resize(data->size());
		copy(data->begin(), data->end(), pmdIkData.begin());
	}

protected:

	void switchColling(ID3D11Device*);//shadowを判断しています。

	//Object status
	//you use subclass. so, you dont use this class
	XMFLOAT3 Position;
	XMFLOAT3 Rotetation;
	

	static ID3D11ShaderResourceView* m_ClearTex;//真っ白なテクスチャ用
	static ID3D11ShaderResourceView* m_ClearTex2;//透明なテクスチャ用


	//モーション関連
	VmdMotionController *m_VmdMotionController;


	//モデルのボーンとIKを受け渡すするためにいるやつ
	vector<Bone> bones;
	vector<MmdStruct::PmdIkData> pmdIkData;


private:
	ID3DBlob *pCompilePS;
	ID3DBlob *pCompileVS;
	DrawingType type;
	XMMATRIX workmat;//作業に使う行列


	PMX_VERTEX_USE_FLOAT4 *ambient;
	PMX_VERTEX_USE_FLOAT4 *normal;
	PMX_VERTEX_USE_FLOAT4 *diffuse;
	PMX_VERTEX_USE_FLOAT4 specular;

	PMX_VERTEX_USE_FLOAT2* UV;


	void PmxLoad(const LPCSTR);//データをロードするのを一括にする
	void TexLoad(ID3D11Device*, LPSTR);//ロードされたデータを用いてテクスチャの読み込みを行う
	void Draw(const PMX_DATA, const  CONSTANT_BUFFER_OBJECT, const ObjectIndividualData*);//prepare drawing
	void Draw(const PMX_DATA, const  CONSTANT_BUFFER_MAINCHARCTER, const CONSTANT_BONE_MATRIX, const ObjectIndividualData*);//prepare drawing
	void Draw(const PMX_DATA, const  CONSTANT_BUFFER, const ObjectIndividualData*);//prepare drawing
		//create buffer
	void IndexdataForPoint(ID3D11Device* pDevice);//fbxからインデックスに変換する
	void IndexdataForVertex(ID3D11Device* pDevice);// インデックスデータの取り出しとバッファの設定

	//create shader
	void SetPmxUsingPixelShader(ID3D11Device* m_pDevice, const LPCWSTR shaderName);
	void SetPmxUsingGeometryShader(ID3D11Device* m_pDevice, const LPCWSTR shaderName);
	void SetPmxUsingVertexShader(ID3D11Device* m_pDevice, const  LPCWSTR shaderName);



	
	vector<XMMATRIX> updateVerBuf(XMMATRIX*);

	//CreateBone
	void CreateBone();
	//ボーンの作成から格納まで 
	//CreateIKData
	void CreateIkData();


public:
	
	//コンストラクタ、デストラクタ
	CPmx()
	{
		pCompilePS = NULL;
		pCompileVS = NULL;
		IndiviData = new ObjectIndividualData();
		type = DrawingType::object;
		workmat = XMMatrixIdentity();
		Position = XMFLOAT3(0, 0, 0);
		Rotetation = XMFLOAT3(0, 1, 0);
		MorphNum = 0;
	}

	CPmx(DrawingType inputType)
	{
		pCompilePS = NULL;
		pCompileVS = NULL;
		IndiviData = new ObjectIndividualData();
		type = inputType;
		workmat = XMMatrixIdentity();
		Position = XMFLOAT3(0, 0, 0);
		Rotetation = XMFLOAT3(0, 0, 0);
		MorphNum = 0;
	}

	CPmx(DrawingType inputType, UINT inputMorphNum)
	{
		pCompilePS = NULL;
		pCompileVS = NULL;
		IndiviData = new ObjectIndividualData();
		type = inputType;
		workmat = XMMatrixIdentity();
		Position = XMFLOAT3(0, 0, 0);
		Rotetation = XMFLOAT3(0, 0, 0);
		MorphNum = inputMorphNum;
	}

	virtual ~CPmx() {
		delete IndiviData;
		TexData.clear();
		delete &TexData;
		RetrievalData.clear();
		delete &RetrievalData;
		RenderingDataObject.clear();
		delete &RenderingDataObject;
		RenderingDataMain.clear();
		delete &RenderingDataMain;
		RenderingDataShadow.clear();
		delete &RenderingDataMain;
		delete VertexBufferUpdate;
	}

};
