#pragma once

#include "PmxLoad.h"
#include "VMD.h"
#include "../shader/shaderList.h"
#include <map>
#include <deque>
#include <vector>
#include <d3dcompiler.h>
#include "StaticCalc.h"

//use namespace
using namespace PmxStruct;
using namespace std;

//構造体


enum DrawingType
{
	player = 0,
	object,
	shadow,
	Final //配列に使うかも・・・
};

class ObjectIndividualData//個別で設定するデータたち
{
public:
	// 格納する必要のあるデータ
	ID3D11Buffer* pVerBuffer;
	ID3D11Buffer* pIndBuffer;
	ID3D11InputLayout *pVertexLayout;
	ID3D11VertexShader *pVertexShader;
	ID3D11PixelShader *pPixelShader;
	ID3D11RasterizerState *pRasterizerState;
	ID3D11Buffer* Buffer;
	ID3D11Buffer* BoneBuffer;

	//11/9　contextの参照を考える 　Clear(11/12) ここに入れず、一つで管理しましょう
	ObjectIndividualData()
	{
	}
	~ObjectIndividualData() { }

	void ReflectionData(ID3D11DeviceContext*, ID3D11Buffer*);//contextに格納
	void ReflectionData(ID3D11DeviceContext*, ID3D11Buffer*, ID3D11Buffer*);//contextに格納
};

//Constant Buffer type
struct CONSTANT_BUFFER_OBJECT //コンスタンスバッファ object
{
#pragma pack(push,1)	//アラインメント制御をオフ
	XMFLOAT4X4 World;
	XMFLOAT4X4 View;
	XMFLOAT4X4 Projection;
	XMFLOAT4 ambient;
	XMFLOAT4 diffuse;
	XMFLOAT4 specular;
	XMFLOAT3 light_dir;
	XMFLOAT3 camera_pos;
#pragma pack(pop)	
	//11/15以後やる
};

struct CONSTANT_BUFFER_MAINCHARCTER //コンスタンスバッファ 
{
#pragma pack(push,1)	//アラインメント制御をオフ
	XMFLOAT4X4 World;
	XMFLOAT4X4 View;
	XMFLOAT4X4 Projection;
	XMFLOAT4 ambient;
	XMFLOAT4 diffuse;
	XMFLOAT4 specular;
	XMFLOAT3 light_dir;
	XMFLOAT3 camera_pos;
#pragma pack(pop)	
};

struct CONSTANT_BUFFER {
	XMFLOAT4X4 World;
	XMFLOAT4X4 View;
	XMFLOAT4X4 Projection;
};

struct CONSTANT_BONE_MATRIX
{
	XMMATRIX boneMatrix[200];
};

//Rendering Data Struct
struct DrawingAllDataObject//描画時に必要なデータ
{
	PMX_DATA pmxdata;//オブジェクトデータ
	CONSTANT_BUFFER_OBJECT constantBuffer;//コンスタンスバッファ
	ObjectIndividualData IndiviData;
	vector<ID3D11ShaderResourceView*> Texture;//テクスチャ
};

struct DrawingAllDataMainCharcter//描画時に必要なデータ
{
	PMX_DATA pmxdata;//オブジェクトデータ
	CONSTANT_BUFFER_MAINCHARCTER constantBuffer;//コンスタンスバッファ
	CONSTANT_BONE_MATRIX constantBoneBuffer;//ボーンに使用
	ObjectIndividualData IndiviData;
	vector<ID3D11ShaderResourceView*> Texture;//テクスチャ
	PMX_SEND_DATA *sendData;
};

struct DrawingAllDataShadow//描画時に必要なデータ
{
	PMX_DATA pmxdata;//オブジェクトデータ
	CONSTANT_BUFFER constantBuffer;//コンスタンスバッファ
	ObjectIndividualData IndiviData;
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


	UINT MorphNum;//モーフナンバー格納
	UINT WorldMorphNum;//モーフナンバー格納


	//drawを変える 11/9　clear (11/9)

	CONSTANT_BUFFER_OBJECT cbo;
	CONSTANT_BUFFER_MAINCHARCTER cbm;
	CONSTANT_BONE_MATRIX cbmr;
	CONSTANT_BUFFER cb;
	//11/9：draw後の処理をRenderに clear ()

	//create input layout
	void Cretaelayout(ID3D11Device* m_pDevice);

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


	PMX_SEND_DATA *VertexBufferUpdate;
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
