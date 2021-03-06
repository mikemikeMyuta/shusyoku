/*
	作成日2/24
*/
#pragma once

#include <d3d11.h>
//#include <DirectXMath.h>//これだけでは何も起きない・・・なんで・・・
#include "../PMX/PmxStruct.h"//これを入れるとなぜかDirectXMathのインクルードがうまくいく
#include "../PMX/VMD.h"
#include <deque>

#define DEPTHTEX_WIDTH (2048)
#define DEPTHTEX_HEIGHT (2048)

static const XMMATRIX SHADOW_BIAS = XMMATRIX(
	0.5f, 0.0f, 0.0f, 0.0f,
	0.0f, -0.5f, 0.0f, 0.0f,
	0.0f, 0.0f, 1.0f, 0.0f,
	0.5f, 0.5f, 0.0f, 1.0f);

struct DrawingAllDataShadowMap//描画時に必要なデータ
{
	PmxStruct::PMX_DATA pmxdata;//オブジェクトデータ
	CONSTANT_BUFFER constantBuffer;//コンスタンスバッファ
	CONSTANT_BONE_MATRIX boneMat;
	ObjectIndividualData IndiviData;
	PmxStruct::PMX_SEND_DATA_SHADOW *sendData;
};
//影を出すために使うclass
//シャドウテクスチャを作るため
//実際の影はキャラのshaderになると思います。

class CShadow final
{
public:
	CShadow();
	~CShadow();

	void CreateShadowLayout(ID3D11Device* Device);
	void SetShadowUsingPixelShader(ID3D11Device* m_pDevice, const LPCWSTR shaderName);
	void SetShadowUsingVertexShader(ID3D11Device* m_pDevice, const LPCWSTR shaderName);
	void IndexdataForPoint(ID3D11Device*);//create buffer
	void IndexdataForVertex(ID3D11Device*);//create buffer
	void Init(ID3D11Device *device, PmxStruct::PMX_DATA *pmxData, const LPCWSTR psShaderName, const LPCWSTR gsShaderName, const LPCWSTR vsShaderName, vector<Bone> *animeBone, vector<MmdStruct::PmdIkData> *pmdIkData);
	/*
		第一引数　デバイス
		第二引数　影に使用したいモデルのデータ
		第三引数　ピクセルシェーダー
		第四引数　ジオメトリシェーダー
		第五引数　バーテックスシェーダー
		第六引数　第二引数でモデルに使用するVMDをセットしたボーン
		第七引数　第二引数でモデルのIK
	*/

	static void Render(ID3D11DeviceContext*);
	void Draw(XMFLOAT4X4 World, XMFLOAT4X4 View, XMFLOAT4X4 Projection, CONSTANT_BONE_MATRIX* boneMat);//バッファをセット。	   

	static deque< DrawingAllDataShadowMap> shadowRender;
	//DO:これグローバルに　データ入れておく
	PmxStruct::PMX_SEND_DATA_SHADOW *VertexBufferUpdate;//データをリンクさせるため
	CONSTANT_BUFFER constantBuffer;
	CONSTANT_BONE_MATRIX constantBufferMatrix;
	static ID3D11RenderTargetView *DepthMap_TexRTV;
	static ID3D11DepthStencilView *DepthMap_DSTexDSV;

	//セットするもの
	void SetPmxData(PmxStruct::PMX_DATA *data) { pmxData = data; }
	void SetMotion(VmdMotionController* motion) { VmdMotionController = motion; }

	static ID3D11ShaderResourceView*  DepthMap_TexSRV;//これを使う

	ObjectIndividualData *indiviData;//これ使う　ここを今からする
	static ID3D11SamplerState *Smp;
private:
	static ID3D11Texture2D *DepthMap_Tex;
	static ID3D11Texture2D *DepthMap_DSTex;
	static D3D11_VIEWPORT viewport;



	PmxStruct::PMX_DATA *pmxData;
	ID3DBlob *pCompileVS, *pCompilePS;


	//モーション関連
	VmdMotionController *VmdMotionController;

	//モデルのボーンとIKを受け渡すするためにいるやつ
	vector<Bone> bones;
	vector<MmdStruct::PmdIkData> pmdIkData;


	void SetBone(vector<Bone>* b) {
		bones.resize(b->size());
		copy(b->begin(), b->end(), bones.begin());
	}
	void SetPmdIKdata(vector<MmdStruct::PmdIkData>* data) {
		pmdIkData.resize(data->size());
		copy(data->begin(), data->end(), pmdIkData.begin());
	}
};

