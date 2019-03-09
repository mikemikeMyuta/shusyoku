#include "../PMX/CPmx.h"

deque< DrawingAllDataShadowMap> CShadow::shadowRender;
ID3D11RenderTargetView *CShadow::DepthMap_TexRTV;
ID3D11DepthStencilView *CShadow::DepthMap_DSTexDSV;
ID3D11ShaderResourceView* CShadow::DepthMap_TexSRV;
ID3D11SamplerState *CShadow::Smp;
ID3D11Texture2D *CShadow::DepthMap_Tex;
ID3D11Texture2D *CShadow::DepthMap_DSTex;
D3D11_VIEWPORT CShadow::viewport;


CShadow::CShadow()
{
	indiviData = new ObjectIndividualData();
}

CShadow::~CShadow()
{
	delete indiviData;
}

void CShadow::Init(ID3D11Device *device, PmxStruct::PMX_DATA *PmxData, const LPCWSTR psShaderName, const LPCWSTR gsShaderName, const LPCWSTR vsShaderName, vector<Bone> *animeBone, vector<MmdStruct::PmdIkData> *pmdIkData)
{
	pmxData = PmxData;

	IndexdataForPoint(device);//create buffer
	IndexdataForVertex(device);//create buffer

	if (psShaderName != NULL)
	{
		SetShadowUsingPixelShader(device, psShaderName);//create pixel shader
	}

	if (gsShaderName != NULL)
	{
		//nothing
	}

	if (vsShaderName != NULL)
	{
		SetShadowUsingVertexShader(device, vsShaderName);//create vertex shader
	}

	CreateShadowLayout(device);//create layout

	//深度マップテクスチャ
	if (DepthMap_TexRTV == NULL || DepthMap_DSTexDSV == NULL)
	{
		HRESULT hr;
		//深度マップテクスチャーを作成
		D3D11_TEXTURE2D_DESC tdesc;// テクスチャリソース記述用構造体（どんなテクスチャリソースを作るかということ）
		ZeroMemory(&tdesc, sizeof(D3D11_TEXTURE2D_DESC));	// ゼロクリア
		tdesc.Width = DEPTHTEX_WIDTH;			// 幅（単位：ピクセル）
		tdesc.Height = DEPTHTEX_HEIGHT;			// 幅（単位：ピクセル）	
		tdesc.MipLevels = 1;					// ミップマップレベルの最大数
		tdesc.ArraySize = 1;					// 配列数
		tdesc.MiscFlags = 0;					// 一般性の低いリソースオプション（０：未使用）
		//tdesc.Format = DXGI_FORMAT_GG8B8_UNORM;		// ３２ビット浮動小数点フォーマット
		tdesc.Format = DXGI_FORMAT_R32_FLOAT;		// ３２ビット浮動小数点フォーマット
		tdesc.SampleDesc.Count = 1;				// サンプリングの数
		tdesc.SampleDesc.Quality = 0;				// サンプリングの品質
		tdesc.Usage = D3D11_USAGE_DEFAULT;			// ＧＰＵ読み取り　書き込みアクセス必要
		tdesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
		// リソースをレンダリングパイプラインにバインドする方法
		tdesc.CPUAccessFlags = 0;				// ＣＰＵアクセス不要

		hr = device->CreateTexture2D(&tdesc, nullptr, &DepthMap_Tex);	// 2Dテクスチャリソース生成
		if (FAILED(hr)) {
			MessageBox(nullptr, "CreateTexture2D error", "error", MB_OK);
			exit(true);
		}

		//深度マップテクスチャー用　レンダーターゲットビュー作成
		D3D11_RENDER_TARGET_VIEW_DESC DescRT;		// どんなＲＴＶを作るかを指定する構造体
		DescRT.Format = tdesc.Format;				// フォーマット指定
		DescRT.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;		// リソースの種類
		DescRT.Texture2D.MipSlice = 0;				// 

		// RTV作成
		hr = device->CreateRenderTargetView(DepthMap_Tex, &DescRT, &DepthMap_TexRTV);
		if (FAILED(hr)) {
			MessageBox(nullptr, "CreateRenderTargetView error", "error", MB_OK);
			exit(true);
		}

		//深度マップテクスチャをレンダーターゲットにする際のデプスステンシルビュー用のテクスチャーを作成
		D3D11_TEXTURE2D_DESC descDepth;
		descDepth.Width = DEPTHTEX_WIDTH;
		descDepth.Height = DEPTHTEX_HEIGHT;
		descDepth.MipLevels = 1;
		descDepth.ArraySize = 1;
		descDepth.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		descDepth.SampleDesc.Count = 1;
		descDepth.SampleDesc.Quality = 0;
		descDepth.Usage = D3D11_USAGE_DEFAULT;
		descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		descDepth.CPUAccessFlags = 0;
		descDepth.MiscFlags = 0;

		hr = device->CreateTexture2D(&descDepth, nullptr, &DepthMap_DSTex);
		if (FAILED(hr)) {
			MessageBox(nullptr, "CreateTexture2D error", "error", MB_OK);
			exit(true);
		}
		//そのテクスチャーに対しデプスステンシルビュー(DSV)を作成
		hr = device->CreateDepthStencilView(DepthMap_DSTex, nullptr, &DepthMap_DSTexDSV);
		if (FAILED(hr)) {
			MessageBox(nullptr, "CreateDepthStencilView error", "error", MB_OK);
			exit(true);
		}

		//深度マップテクスチャ用　シェーダーリソースビュー(SRV)作成	
		D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc;
		ZeroMemory(&SRVDesc, sizeof(SRVDesc));
		SRVDesc.Format = tdesc.Format;
		SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		SRVDesc.Texture2D.MipLevels = 1;

		hr = device->CreateShaderResourceView(DepthMap_Tex, &SRVDesc, &DepthMap_TexSRV);
		if (FAILED(hr)) {
			MessageBox(nullptr, "CreateShaderResourceView error", "error", MB_OK);
			exit(true);
		}


		// サンプラーステートを生成
		D3D11_SAMPLER_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
		desc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
		desc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
		desc.BorderColor[0] = 1.0f;
		desc.BorderColor[1] = 1.0f;
		desc.BorderColor[2] = 1.0f;
		desc.BorderColor[3] = 1.0f;
		desc.ComparisonFunc = D3D11_COMPARISON_LESS_EQUAL;
		desc.Filter = D3D11_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR;
		desc.MaxAnisotropy = 1;
		desc.MipLODBias = 0;
		desc.MinLOD = -FLT_MAX;
		desc.MaxLOD = +FLT_MAX;

		hr = device->CreateSamplerState(&desc, &Smp);
		if (FAILED(hr))
		{
			MessageBox(0, "CreateSamplerState shadow error", NULL, MB_OK);
			exit(true);
		}

		viewport.TopLeftX = 0.0f;
		viewport.TopLeftY = 0.0f;
		viewport.Width = DEPTHTEX_WIDTH;
		viewport.Height = DEPTHTEX_HEIGHT;
		viewport.MinDepth = 0.0f;
		viewport.MaxDepth = 1.0f;

	}
	D3D11_BUFFER_DESC cb;
	ZeroMemory(&cb, sizeof(cb));
	cb.ByteWidth = sizeof(CONSTANT_BUFFER);//16の倍数になるように調整
	cb.StructureByteStride = sizeof(CONSTANT_BUFFER);
	cb.Usage = D3D11_USAGE_DYNAMIC;
	cb.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cb.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cb.MiscFlags = 0;

	if (device->CreateBuffer(&cb, NULL, &indiviData->Buffer))
	{
		MessageBox(0, "Bufferが作成されませんでした。おそらく16の倍数ではないかと思われます", NULL, MB_OK);
		return;
	}
	else {
		OutputDebugString("\nCreateBuffer成功\n");
	}

	//データ同期
	SetBone(animeBone);
	SetPmdIKdata(pmdIkData);
}

void CShadow::CreateShadowLayout(ID3D11Device* Device)
{
	HRESULT hr;
	// 頂点レイアウト
	D3D11_INPUT_ELEMENT_DESC layout[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "BONEINDEX",		 0,		 DXGI_FORMAT_R32G32B32A32_FLOAT,   0,	 D3D11_APPEND_ALIGNED_ELEMENT,		D3D11_INPUT_PER_VERTEX_DATA, 0 },	// ボーン行列インデクッス
		{ "BONEWEIGHT",		 0,		 DXGI_FORMAT_R32G32B32A32_FLOAT,   0,	  D3D11_APPEND_ALIGNED_ELEMENT,	D3D11_INPUT_PER_VERTEX_DATA, 0 },	// ボーンウェイト
	};

	unsigned int LayoutCreateConut = ARRAYSIZE(layout);//レイアウトサイズ取得

	hr = Device->CreateInputLayout(layout, LayoutCreateConut
		, pCompileVS->GetBufferPointer(), pCompileVS->GetBufferSize(), &indiviData->pVertexLayout);
	if (FAILED(hr))
	{
		MessageBox(0, "layout error", NULL, MB_OK);
		exit(true);
	}
	OutputDebugString("\n Cretaelayout成功\n");
	pCompilePS->Release();
	pCompilePS = NULL;
	pCompileVS->Release();
	pCompileVS = NULL;
}

void CShadow::SetShadowUsingPixelShader(ID3D11Device* m_pDevice, const LPCWSTR shaderName)
{
	ID3D10Blob *pErrors = NULL;
	HRESULT hr;

	//コンパイルで使用するのを作成
#ifdef _DEBUG
	UINT flags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
	UINT flags = 0;
#endif 

	if (FAILED(D3DCompileFromFile(shaderName, nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, "ps_main", "ps_5_0", flags, 0, &pCompilePS, &pErrors)))
	{
		MessageBox(0, (LPSTR)pErrors->GetBufferPointer(), NULL, MB_OK);
		exit(true);
	}
	OutputDebugString("\nPShlsl読み込み成功\n");

	hr = m_pDevice->CreatePixelShader(pCompilePS->GetBufferPointer(), pCompilePS->GetBufferSize(), NULL, &indiviData->pPixelShader);

	if (FAILED(hr))
	{
		MessageBox(0, "CreatePixelShader error", NULL, MB_OK);
		exit(true);
	}
	OutputDebugString("\nCreatePixelShaderr成功\n");
}

void CShadow::SetShadowUsingVertexShader(ID3D11Device* m_pDevice, const LPCWSTR shaderName)
{
	ID3D10Blob *pErrors = NULL;
	HRESULT hr;

#ifdef _DEBUG
	UINT flags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
	UINT flags = 0;
#endif 

	if (FAILED(D3DCompileFromFile(shaderName, nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, "vs_main", "vs_5_0", flags, 0, &pCompileVS, &pErrors)))
	{
		MessageBox(0, (LPSTR)pErrors->GetBufferPointer(), NULL, MB_OK);
		exit(true);
	}
	OutputDebugString("\nVShlsl読み込み成功\n");
	hr = m_pDevice->CreateVertexShader(pCompileVS->GetBufferPointer(), pCompileVS->GetBufferSize(), NULL, &indiviData->pVertexShader);

	if (FAILED(hr))
	{
		MessageBox(0, "CreateVertexShader error", NULL, MB_OK);
		exit(true);
	}
	OutputDebugString("\nCreateVertexShader成功\n");
}

void CShadow::IndexdataForPoint(ID3D11Device* pDevice)
{
	D3D11_BUFFER_DESC bd_index;
	ZeroMemory(&bd_index, sizeof(bd_index));

	//データを格納しなおす
	unsigned short* indexdata = new unsigned short[pmxData->s_PmxFaceNum * 3];
	for (unsigned int i = 0; i < pmxData->s_PmxFaceNum; i++)
	{
		indexdata[i * 3] = pmxData->s_pPmxFace[i].VertexIndex[0];
		indexdata[(i * 3) + 1] = pmxData->s_pPmxFace[i].VertexIndex[1];
		indexdata[(i * 3) + 2] = pmxData->s_pPmxFace[i].VertexIndex[2];
	}

	bd_index.ByteWidth = sizeof(unsigned short) * pmxData->s_PmxFaceNum * 3;//Indexの数格納
	bd_index.Usage = D3D11_USAGE_DEFAULT;
	bd_index.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bd_index.CPUAccessFlags = 0;
	bd_index.MiscFlags = 0;
	bd_index.StructureByteStride = sizeof(unsigned short);

	D3D11_SUBRESOURCE_DATA data_index;
	ZeroMemory(&data_index, sizeof(data_index));
	data_index.pSysMem = indexdata;//Index配列を格納

	if (FAILED(pDevice->CreateBuffer(&bd_index, &data_index, &indiviData->pIndBuffer)))
	{
		MessageBox(0, "CreateBuffer error", NULL, MB_OK);
		exit(1);
	}

	delete[] indexdata;//削除
}

void CShadow::IndexdataForVertex(ID3D11Device* pDevice)
{
	D3D11_BUFFER_DESC bd_vertex;
	ZeroMemory(&bd_vertex, sizeof(bd_vertex));

	VertexBufferUpdate = new PMX_SEND_DATA_SHADOW[pmxData->s_PmxVertexNum];

	for (UINT i = 0; i < pmxData->s_PmxVertexNum; i++)
	{
		VertexBufferUpdate[i].pos[0] = MmdStruct::scale*pmxData->s_pPmxVertex[i].Position[0];
		VertexBufferUpdate[i].pos[1] = MmdStruct::scale*pmxData->s_pPmxVertex[i].Position[1];
		VertexBufferUpdate[i].pos[2] = MmdStruct::scale*pmxData->s_pPmxVertex[i].Position[2];

		VertexBufferUpdate[i].boneIndex[0] = (float)pmxData->s_pPmxVertex[i].BoneIndex[0];
		VertexBufferUpdate[i].boneIndex[1] = (float)pmxData->s_pPmxVertex[i].BoneIndex[1];
		VertexBufferUpdate[i].boneIndex[2] = (float)pmxData->s_pPmxVertex[i].BoneIndex[2];
		VertexBufferUpdate[i].boneIndex[3] = (float)pmxData->s_pPmxVertex[i].BoneIndex[3];

		VertexBufferUpdate[i].boneWeight[0] = pmxData->s_pPmxVertex[i].BoneWeight[0];
		VertexBufferUpdate[i].boneWeight[1] = pmxData->s_pPmxVertex[i].BoneWeight[1];
		VertexBufferUpdate[i].boneWeight[2] = pmxData->s_pPmxVertex[i].BoneWeight[2];
		VertexBufferUpdate[i].boneWeight[3] = pmxData->s_pPmxVertex[i].BoneWeight[3];
	}

	bd_vertex.ByteWidth = sizeof(PMX_SEND_DATA_SHADOW) * pmxData->s_PmxVertexNum;
	bd_vertex.Usage = D3D11_USAGE_DYNAMIC;
	bd_vertex.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd_vertex.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bd_vertex.MiscFlags = 0;
	bd_vertex.StructureByteStride = sizeof(PMX_SEND_DATA_SHADOW);

	D3D11_SUBRESOURCE_DATA data_vertex;
	ZeroMemory(&data_vertex, sizeof(data_vertex));
	data_vertex.pSysMem = VertexBufferUpdate;//vertex格納

	if (FAILED(pDevice->CreateBuffer(&bd_vertex, &data_vertex, &indiviData->pVerBuffer)))
	{
		MessageBox(0, "CreateBuffer error", NULL, MB_OK);
		exit(1);
	}
}

void CShadow::Draw(XMFLOAT4X4 World, XMFLOAT4X4 View, XMFLOAT4X4 Projection, CONSTANT_BONE_MATRIX* boneMat)
{
	DrawingAllDataShadowMap Drawdata;

	constantBuffer.World = World;
	constantBuffer.View = View;
	constantBuffer.Projection = Projection;
	constantBufferMatrix = *boneMat;


	Drawdata.constantBuffer = constantBuffer;
	Drawdata.boneMat = constantBufferMatrix;
	Drawdata.IndiviData = *indiviData;
	Drawdata.sendData = VertexBufferUpdate;
	Drawdata.pmxdata = *pmxData;
	shadowRender.push_back(Drawdata);
}

void CShadow::Render(ID3D11DeviceContext* context)
{
	D3D11_MAPPED_SUBRESOURCE pdata;
	HRESULT hr;

	
	ID3D11ShaderResourceView *const pSRV[3] = { NULL,NULL,NULL };
	context->PSSetShaderResources(0, 2, pSRV);
	float ClearColor[4] = { 0,1,1,1 };// クリア色作成　RGBAの順
	context->ClearRenderTargetView(DepthMap_TexRTV, ClearColor);//画面クリア
	context->ClearDepthStencilView(DepthMap_DSTexDSV, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);//深度バッファクリア
	context->OMSetRenderTargets(1, &DepthMap_TexRTV, DepthMap_DSTexDSV);//ターゲット変更

	context->RSSetViewports(1, &viewport);

	//ここにステージのレンダーする（中身消さない）
	for (auto itr : shadowRender) {

		itr.IndiviData.ReflectionDataShadow(context, itr.IndiviData.Buffer, itr.IndiviData.BoneBuffer);

		hr = context->Map(itr.IndiviData.Buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &pdata);
		if (hr == S_OK)
			memcpy_s(pdata.pData, pdata.RowPitch, (void*)(&itr.constantBuffer), sizeof(itr.constantBuffer));
		//11/15アンビエント ok
		context->Unmap(itr.IndiviData.Buffer, 0);


		//ボーン
		hr = context->Map(itr.IndiviData.BoneBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &pdata);
		if (hr == S_OK)
			memcpy_s(pdata.pData, pdata.RowPitch, (void*)(&itr.boneMat), sizeof(itr.boneMat));
		//11/15アンビエント ok
		context->Unmap(itr.IndiviData.BoneBuffer, 0);

		hr = context->Map(itr.IndiviData.pVerBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &pdata);
		if (hr == S_OK)
			memcpy_s(pdata.pData, pdata.RowPitch, (void*)(itr.sendData), sizeof(PMX_SEND_DATA_SHADOW)*itr.pmxdata.s_PmxVertexNum);
		//11/15アンビエント ok
		context->Unmap(itr.IndiviData.pVerBuffer, 0);


		context->DrawIndexed(0, itr.pmxdata.s_PmxVertexNum, 0);
	}

	shadowRender.clear();

	ID3D11RenderTargetView* nullRTV = nullptr;
	context->OMSetRenderTargets(1, &DepthMap_TexRTV, nullptr);
	context->PSSetShaderResources(0, 3, pSRV);
}