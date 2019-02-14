#include "CPmx.h"
#include "../CAMERA/Camera.h"
#include "modelList.h"

map<LPCSTR, PMX_DATA> CPmx::RetrievalData;
deque<DrawingAllDataObject> CPmx::RenderingDataObject;
deque<DrawingAllDataMainCharcter> CPmx::RenderingDataMain;
deque<DrawingAllDataShadow> CPmx::RenderingDataShadow;
ID3D11ShaderResourceView* CPmx::m_ClearTex;
ID3D11ShaderResourceView* CPmx::m_ClearTex2;

void CPmx::Render(ID3D11DeviceContext *context)
{
	//レンダリング処理をみてやりましょうか
	D3D11_MAPPED_SUBRESOURCE pdata;

	//Other than maincharcter Rendering
	for (auto itr : RenderingDataObject)
	{
		int index = 0;
		//コンテキストデータを反映
		itr.IndiviData.ReflectionData(context, itr.IndiviData.Buffer);
		context->Map(itr.IndiviData.Buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &pdata);
		memcpy_s(pdata.pData, pdata.RowPitch, (void*)(&itr.constantBuffer), sizeof(itr.constantBuffer));
		context->Unmap(itr.IndiviData.Buffer, 0);


		for (int i = 0; i < itr.pmxdata.s_PmxMaterialNum; i++)
		{
			if (itr.pmxdata.s_pPmxMaterial[i].TextureIndex >= 0) {
				context->PSSetShaderResources(0, 1, &itr.Texture[itr.pmxdata.s_pPmxMaterial[i].TextureIndex]);//テクスチャ張りかえる
			}
			else//何も貼らない
			{
				context->PSSetShaderResources(0, 1, &m_ClearTex2);//テクスチャ張りかえる
			}
			context->DrawIndexed(itr.pmxdata.s_pPmxMaterial[i].MaterialFaceNum, index, 0);
			//テクスチャを張り替えるため　だすIndexの数　出す場所を書き込む
			index += itr.pmxdata.s_pPmxMaterial[i].MaterialFaceNum;//現在総数を格納
		}
	}

	//clear RenderingDataObject 
	RenderingDataObject.clear();

	for (auto itr : RenderingDataMain)
	{
		HRESULT hr;
		int index = 0;

		hr = context->Map(itr.IndiviData.Buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &pdata);
		if (hr == S_OK)
			memcpy_s(pdata.pData, pdata.RowPitch, (void*)(&itr.constantBuffer), sizeof(itr.constantBuffer));
		//11/15アンビエント ok
		context->Unmap(itr.IndiviData.Buffer, 0);


		//ボーン
		hr = context->Map(itr.IndiviData.BoneBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &pdata);
		if (hr == S_OK)
			memcpy_s(pdata.pData, pdata.RowPitch, (void*)(&itr.constantBoneBuffer), sizeof(itr.constantBoneBuffer));
		//11/15アンビエント ok
		context->Unmap(itr.IndiviData.BoneBuffer, 0);


		hr = context->Map(itr.IndiviData.pVerBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &pdata);
		if (hr == S_OK)
			memcpy_s(pdata.pData, pdata.RowPitch, (void*)(itr.sendData), sizeof(PMX_SEND_DATA)*itr.pmxdata.s_PmxVertexNum);
		//11/15アンビエント ok
		context->Unmap(itr.IndiviData.pVerBuffer, 0);

		itr.IndiviData.ReflectionData(context, itr.IndiviData.Buffer, itr.IndiviData.BoneBuffer);
		for (int i = 0; i < itr.pmxdata.s_PmxMaterialNum; i++)
		{
			if (itr.pmxdata.s_pPmxMaterial[i].TextureIndex >= 0) {
				if (itr.pmxdata.s_pPmxMaterial[i].ToonTextureIndex >= 0) {
					ID3D11ShaderResourceView* Textures[] = { itr.Texture[itr.pmxdata.s_pPmxMaterial[i].TextureIndex], itr.Texture[itr.pmxdata.s_pPmxMaterial[i].ToonTextureIndex] }; //格納

					context->PSSetShaderResources(0, 2, Textures);//テクスチャ張りかえる
				}
				else
				{
					ID3D11ShaderResourceView* Textures[] = { itr.Texture[itr.pmxdata.s_pPmxMaterial[i].TextureIndex], m_ClearTex }; //格納
					context->PSSetShaderResources(0, 2, Textures);//テクスチャ張りかえる
				}
			}
			else//ともに違うなら
			{
				ID3D11ShaderResourceView* Textures[] = { m_ClearTex, m_ClearTex }; //格納
				context->PSSetShaderResources(0, 2, Textures);//テクスチャ張りかえる
			}

			context->DrawIndexed(itr.pmxdata.s_pPmxMaterial[i].MaterialFaceNum, index, 0);
			//テクスチャを張り替えるため　だすIndexの数　出す場所を書き込む
			index += itr.pmxdata.s_pPmxMaterial[i].MaterialFaceNum;//現在総数を格納

		}


	}
	RenderingDataMain.clear();
	//for (auto itr : RenderingDataShadow)
	//{
	//	itr.IndiviData.ReflectionData(context, itr.IndiviData.Buffer);
	//	context->Map(itr.IndiviData.Buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &pdata);
	//	memcpy_s(pdata.pData, pdata.RowPitch, (void*)(&itr.constantBuffer), sizeof(itr.constantBuffer));
	//	//11/15アンビエント ok
	//	context->Unmap(itr.IndiviData.Buffer, 0);
	//	context->DrawIndexed(itr.pmxdata.s_PmxFaceNum * 3, 0, 0);
	//}

	////clear RenderingDataMain 
	RenderingDataShadow.clear();

}


void CPmx::Draw(const PMX_DATA data, const CONSTANT_BUFFER_OBJECT cbo, const ObjectIndividualData *IndiviData)
{
	DrawingAllDataObject DrawingData; //格納用の変数
	DrawingData.pmxdata = data;//PMXデータを格納
	DrawingData.constantBuffer = cbo;
	DrawingData.IndiviData = *IndiviData;
	DrawingData.Texture = TexData;
	RenderingDataObject.push_back(DrawingData);//pushBack
}

void CPmx::Draw(const PMX_DATA data, const CONSTANT_BUFFER_MAINCHARCTER cbo, const CONSTANT_BONE_MATRIX cbm, const ObjectIndividualData *IndiviData)
{
	DrawingAllDataMainCharcter DrawingData; //格納用の変数
	DrawingData.pmxdata = data;//PMXデータを格納
	DrawingData.constantBuffer = cbo;
	DrawingData.constantBoneBuffer = cbm;
	DrawingData.IndiviData = *IndiviData;
	DrawingData.Texture = TexData;
	DrawingData.sendData = VertexBufferUpdate;
	RenderingDataMain.push_back(DrawingData);//pushBack

}

void CPmx::Draw(const PMX_DATA data, const CONSTANT_BUFFER cbo, const ObjectIndividualData *IndiviData)
{
	DrawingAllDataShadow DrawingData; //格納用の変数
	DrawingData.pmxdata = data;//PMXデータを格納
	DrawingData.constantBuffer = cbo;
	DrawingData.IndiviData = *IndiviData;
	RenderingDataShadow.push_back(DrawingData);//pushBack
}

void CPmx::PmxLoad(const LPCSTR filename)
{
	if (RetrievalData.find(filename) == RetrievalData.end())
	{
		//データが存在しないとき
		CPmx::LoadPMXFile(filename);//ロード

		RetrievalData.emplace(filename, m_pmx_data);//挿入
	}
	else
	{
		//データが存在するとき
		m_pmx_data = RetrievalData[filename];//存在するデータ使用してデータ格納
	}
}

void CPmx::IndexdataForPoint(ID3D11Device* pDevice)
{
	D3D11_BUFFER_DESC bd_index;
	ZeroMemory(&bd_index, sizeof(bd_index));

	//データを格納しなおす
	unsigned short* indexdata = new unsigned short[m_pmx_data.s_PmxFaceNum * 3];
	for (unsigned int i = 0; i < m_pmx_data.s_PmxFaceNum; i++)
	{
		indexdata[i * 3] = m_pmx_data.s_pPmxFace[i].VertexIndex[0];
		indexdata[(i * 3) + 1] = m_pmx_data.s_pPmxFace[i].VertexIndex[1];
		indexdata[(i * 3) + 2] = m_pmx_data.s_pPmxFace[i].VertexIndex[2];
	}

	bd_index.ByteWidth = sizeof(unsigned short) * m_pmx_data.s_PmxFaceNum * 3;//Indexの数格納
	bd_index.Usage = D3D11_USAGE_DEFAULT;
	bd_index.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bd_index.CPUAccessFlags = 0;
	bd_index.MiscFlags = 0;
	bd_index.StructureByteStride = sizeof(unsigned short);

	D3D11_SUBRESOURCE_DATA data_index;
	ZeroMemory(&data_index, sizeof(data_index));
	data_index.pSysMem = indexdata;//Index配列を格納

	if (FAILED(pDevice->CreateBuffer(&bd_index, &data_index, &IndiviData->pIndBuffer)))
	{
		MessageBox(0, "CreateBuffer error", NULL, MB_OK);
		exit(1);
	}

	delete[] indexdata;//削除
}

void CPmx::IndexdataForVertex(ID3D11Device* pDevice)
{
	D3D11_BUFFER_DESC bd_vertex;
	ZeroMemory(&bd_vertex, sizeof(bd_vertex));

	//必要な形にデータを格納しなおす
	PMX_SEND_DATA *VertexBuffer = new PMX_SEND_DATA[m_pmx_data.s_PmxVertexNum];

	for (int i = 0; i < m_pmx_data.s_PmxVertexNum; i++)
	{
		VertexBuffer[i].pos[0] = m_pmx_data.s_pPmxVertex[i].Position[0];
		VertexBuffer[i].pos[1] = m_pmx_data.s_pPmxVertex[i].Position[1];
		VertexBuffer[i].pos[2] = m_pmx_data.s_pPmxVertex[i].Position[2];

		VertexBuffer[i].normal[0] = m_pmx_data.s_pPmxVertex[i].Normal[0];
		VertexBuffer[i].normal[1] = m_pmx_data.s_pPmxVertex[i].Normal[1];
		VertexBuffer[i].normal[2] = m_pmx_data.s_pPmxVertex[i].Normal[2];

		VertexBuffer[i].uv[0] = m_pmx_data.s_pPmxVertex[i].UV[0];
		VertexBuffer[i].uv[1] = m_pmx_data.s_pPmxVertex[i].UV[1];

		VertexBuffer[i].col[0] = 1; //R
		VertexBuffer[i].col[1] = 1; 	//G
		VertexBuffer[i].col[2] = 1; 	//B
		VertexBuffer[i].col[3] = 1;	//A

		VertexBuffer[i].col2[0] = 1; //R
		VertexBuffer[i].col2[1] = 1; 	//G
		VertexBuffer[i].col2[2] = 1; 	//B
		VertexBuffer[i].col2[3] = 1;	//A

		VertexBuffer[i].boneIndex[0] = m_pmx_data.s_pPmxVertex[i].BoneIndex[0];
		VertexBuffer[i].boneIndex[1] = m_pmx_data.s_pPmxVertex[i].BoneIndex[1];
		VertexBuffer[i].boneIndex[2] = m_pmx_data.s_pPmxVertex[i].BoneIndex[2];
		VertexBuffer[i].boneIndex[3] = m_pmx_data.s_pPmxVertex[i].BoneIndex[3];

		VertexBuffer[i].boneWeight[0] = m_pmx_data.s_pPmxVertex[i].BoneWeight[0];
		VertexBuffer[i].boneWeight[1] = m_pmx_data.s_pPmxVertex[i].BoneWeight[1];
		VertexBuffer[i].boneWeight[2] = m_pmx_data.s_pPmxVertex[i].BoneWeight[2];
		VertexBuffer[i].boneWeight[3] = m_pmx_data.s_pPmxVertex[i].BoneWeight[3];
	}

	bd_vertex.ByteWidth = sizeof(PMX_SEND_DATA) * m_pmx_data.s_PmxVertexNum;
	bd_vertex.Usage = D3D11_USAGE_DYNAMIC;
	bd_vertex.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd_vertex.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bd_vertex.MiscFlags = 0;
	bd_vertex.StructureByteStride = sizeof(PMX_SEND_DATA);

	D3D11_SUBRESOURCE_DATA data_vertex;
	ZeroMemory(&data_vertex, sizeof(data_vertex));
	data_vertex.pSysMem = VertexBuffer;//vertex格納


	if (FAILED(pDevice->CreateBuffer(&bd_vertex, &data_vertex, &IndiviData->pVerBuffer)))
	{
		MessageBox(0, "CreateBuffer error", NULL, MB_OK);
		exit(1);
	}

	delete[] VertexBuffer;//削除
}

void CPmx::SetPmxUsingPixelShader(ID3D11Device* m_pDevice, const LPCWSTR shaderName)
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

	hr = m_pDevice->CreatePixelShader(pCompilePS->GetBufferPointer(), pCompilePS->GetBufferSize(), NULL, &IndiviData->pPixelShader);

	if (FAILED(hr))
	{
		MessageBox(0, "CreatePixelShader error", NULL, MB_OK);
		exit(true);
	}
	OutputDebugString("\nCreatePixelShaderr成功\n");
}

void CPmx::SetPmxUsingGeometryShader(ID3D11Device* m_pDevice, const LPCWSTR shaderName)
{
	//#ifdef _Debug
	//	UINT flags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
	//#else
	//	UINT flags = 0;
	//#endif 
}

void  CPmx::SetPmxUsingVertexShader(ID3D11Device* m_pDevice, const LPCWSTR shaderName)
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
	hr = m_pDevice->CreateVertexShader(pCompileVS->GetBufferPointer(), pCompileVS->GetBufferSize(), NULL, &IndiviData->pVertexShader);

	if (FAILED(hr))
	{
		MessageBox(0, "CreateVertexShader error", NULL, MB_OK);
		exit(true);
	}
	OutputDebugString("\nCreateVertexShader成功\n");
}

void CPmx::Cretaelayout(ID3D11Device* Device)
{
	HRESULT hr;
	// 頂点レイアウト
	D3D11_INPUT_ELEMENT_DESC layout[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLORR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLORRR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "BONEINDEX",		 0,		 DXGI_FORMAT_R32G32B32A32_FLOAT,   0,	 D3D11_APPEND_ALIGNED_ELEMENT,		D3D11_INPUT_PER_VERTEX_DATA, 0 },	// ボーン行列インデクッス
		{ "BONEWEIGHT",		 0,		 DXGI_FORMAT_R32G32B32A32_FLOAT,   0,	  D3D11_APPEND_ALIGNED_ELEMENT,	D3D11_INPUT_PER_VERTEX_DATA, 0 },	// ボーンウェイト
	};

	unsigned int LayoutCreateConut = ARRAYSIZE(layout);//レイアウトサイズ取得

	hr = Device->CreateInputLayout(layout, LayoutCreateConut
		, pCompileVS->GetBufferPointer(), pCompileVS->GetBufferSize(), &IndiviData->pVertexLayout);
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

void CPmx::Init(ID3D11Device *device, const LPCSTR ModelName, const LPCWSTR psShaderName, const LPCWSTR gsShaderName, const LPCWSTR vsShaderName, LPSTR Texpass)
{
	PmxLoad(ModelName);//load model

	IndexdataForPoint(device);//create buffer
	IndexdataForVertex(device);//create buffer


	if (psShaderName != NULL)
	{
		SetPmxUsingPixelShader(device, psShaderName);//create pixel shader
	}

	if (gsShaderName != NULL)
	{
		SetPmxUsingGeometryShader(device, gsShaderName);//create geometry shader
	}

	if (vsShaderName != NULL)
	{
		SetPmxUsingVertexShader(device, vsShaderName);//create vertex shader
	}

	Cretaelayout(device);//create layout

	//pmxのデータを格納　（shaderに送るために）

	//カラーは
	/*
	X → R
	Y → G
	Z → B
	W → A

	に対応しています。
	*/

	//ambient
	ambient = new PMX_VERTEX_USE_FLOAT4[m_pmx_data.s_PmxVertexNum];
	diffuse = new PMX_VERTEX_USE_FLOAT4[m_pmx_data.s_PmxVertexNum];
	for (int i = 0; i < m_pmx_data.s_PmxMaterialNum; i++)
	{
		ambient[i].data.x = m_pmx_data.s_pPmxMaterial[i].Ambient[0];
		ambient[i].data.y = m_pmx_data.s_pPmxMaterial[i].Ambient[1];
		ambient[i].data.z = m_pmx_data.s_pPmxMaterial[i].Ambient[2];
		ambient[i].data.w = 1;



		diffuse[i].data.x = m_pmx_data.s_pPmxMaterial[i].Diffuse[0]; //R
		diffuse[i].data.y = m_pmx_data.s_pPmxMaterial[i].Diffuse[1]; ;	//G
		diffuse[i].data.z = m_pmx_data.s_pPmxMaterial[i].Diffuse[2]; ;	//B
		//diffuse[i].data.x = 0.0f; //R
		//diffuse[i].data.y = 0.0f;	//G
		//diffuse[i].data.z = 0.0f;	//B
		diffuse[i].data.w = 1;	//A
	}
	//specular 個人設定
	specular.data.x = 1;		//R
	specular.data.y = 0;	   //G
	specular.data.z = 0.5f;	  //B
	specular.data.w = 1;	  	//A

	//normal
	normal = new PMX_VERTEX_USE_FLOAT4[m_pmx_data.s_PmxVertexNum];
	UV = new PMX_VERTEX_USE_FLOAT2[m_pmx_data.s_PmxVertexNum];
	for (int i = 0; i < m_pmx_data.s_PmxVertexNum; i++)
	{
		normal[i].data.x = m_pmx_data.s_pPmxVertex[i].Normal[0];
		normal[i].data.y = m_pmx_data.s_pPmxVertex[i].Normal[1];
		normal[i].data.z = m_pmx_data.s_pPmxVertex[i].Normal[2];
		normal[i].data.w = 1;

		UV[i].data.x = 0;
		UV[i].data.y = 0;
	}


	D3D11_BUFFER_DESC cb;
	ZeroMemory(&cb, sizeof(cb));
	switch (type)
	{
	case player:

		cb.ByteWidth = sizeof(CONSTANT_BUFFER_MAINCHARCTER) + 8;//16の倍数になるように調整
		cb.StructureByteStride = sizeof(CONSTANT_BUFFER_MAINCHARCTER) + 8;

		D3D11_BUFFER_DESC cbBone;
		ZeroMemory(&cbBone, sizeof(cbBone));
		cbBone.ByteWidth = sizeof(CONSTANT_BONE_MATRIX);
		cbBone.StructureByteStride = sizeof(CONSTANT_BONE_MATRIX);
		cbBone.Usage = D3D11_USAGE_DYNAMIC;
		cbBone.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		cbBone.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		cbBone.MiscFlags = 0;

		if (device->CreateBuffer(&cbBone, NULL, &IndiviData->BoneBuffer))
		{
			MessageBox(0, "Bufferが作成されませんでした。おそらく16の倍数ではないかと思われます", NULL, MB_OK);
			return;
		}
		else {
			OutputDebugString("\CreateBuffer成功\n");
		}
		break;
	case object:
		cb.ByteWidth = sizeof(CONSTANT_BUFFER_OBJECT) + 8;
		cb.StructureByteStride = sizeof(CONSTANT_BUFFER_OBJECT) + 8;
		break;

	case Final:
	default:
		cb.ByteWidth = sizeof(CONSTANT_BUFFER);
		cb.StructureByteStride = sizeof(CONSTANT_BUFFER);
		break;
	}
	cb.Usage = D3D11_USAGE_DYNAMIC;
	cb.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cb.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cb.MiscFlags = 0;

	if (device->CreateBuffer(&cb, NULL, &IndiviData->Buffer))
	{
		MessageBox(0, "Bufferが作成されませんでした。おそらく16の倍数ではないかと思われます", NULL, MB_OK);
		return;
	}
	else {
		OutputDebugString("\CreateBuffer成功\n");
	}

	//VMDで使う
	CreateBone();
	CreateIkData();

	TexLoad(device, Texpass);//textureLoad

	switchColling(device);//カリング制御用


	if (m_ClearTex == NULL) {
		//真っ白なテクスチャ生成
		TexMetadata meta;
		GetMetadataFromWICFile(ClearTex, 0, meta);
		ScratchImage *image = new ScratchImage();
		LoadFromWICFile(ClearTex, 0, &meta, *image);

		if (FAILED(CreateShaderResourceView(device, image->GetImages(), image->GetImageCount(), meta, &m_ClearTex)))
		{
			MessageBox(0, "テクスチャー作成 error", NULL, MB_OK);
			return;
		}
	}
	if (m_ClearTex2 == NULL) {
		//透明なテクスチャ生成
		TexMetadata meta;
		GetMetadataFromWICFile(ClearTex2, 0, meta);
		ScratchImage *image = new ScratchImage();
		LoadFromWICFile(ClearTex2, 0, &meta, *image);

		if (FAILED(CreateShaderResourceView(device, image->GetImages(), image->GetImageCount(), meta, &m_ClearTex2)))
		{
			MessageBox(0, "テクスチャー作成 error", NULL, MB_OK);
			return;
		}
	}
}

void CPmx::switchColling(ID3D11Device *dev)
{
	D3D11_RASTERIZER_DESC rdc;
	ZeroMemory(&rdc, sizeof(rdc));
	switch (type)
	{
	case shadow:
		rdc.CullMode = D3D11_CULL_BACK;
		break;
	default:
		rdc.CullMode = D3D11_CULL_NONE;
		break;
	}
	rdc.FillMode = D3D11_FILL_SOLID;
	rdc.FrontCounterClockwise = TRUE;

	dev->CreateRasterizerState(&rdc, &IndiviData->pRasterizerState);
}

void CPmx::ProcessingCalc()
{
	XMMATRIX rotemat;//回転用行列
	XMMATRIX keepMat;//逆行列を格納
	XMVECTOR useInverse;
	vector<XMMATRIX> data;//ボーン行列を持っている
	//rotetation 
	rotemat = XMMatrixRotationY(XMConvertToRadians(Rotetation.y));
	//Rotetation = XMFLOAT3(0, 0, 0);

	//position

	XMMATRIX objectPos = XMMatrixTranslation(Position.x, Position.y, Position.z);

	XMFLOAT3 light_dir = XMFLOAT3(1, 1, 0);
	XMFLOAT3 camerapos = XMFLOAT3(0.0f, 10, -14);//仮置き (11/15現在)→解除待ち

	//11/10　コンスタンスバッファ別で作成

	//計算処理をする その後 call Draw func
	switch (type)
	{
	case player:

		workmat = rotemat * objectPos * workmat;
		data = updateVerBuf(&workmat);
		//keepMat = XMMatrixInverse(&useInverse, workmat);


		//この下消すと追従カメラに
		//Camera::m_mView = Camera::m_mView*keepMat;



		XMStoreFloat4x4(&cbm.World, XMMatrixTranspose(workmat));
		XMStoreFloat4x4(&cbm.View, XMMatrixTranspose(Camera::m_mView));
		XMStoreFloat4x4(&cbm.Projection, XMMatrixTranspose(Camera::m_mProj));
		XMStoreFloat4(&cbm.ambient, XMLoadFloat4((const XMFLOAT4*)&ambient->data));
		XMStoreFloat4(&cbm.diffuse, XMLoadFloat4((const XMFLOAT4*)&diffuse->data));
		XMStoreFloat4(&cbm.specular, XMLoadFloat4((const XMFLOAT4*)&specular.data));
		XMStoreFloat3(&cbm.light_dir, XMLoadFloat3((const XMFLOAT3*)&light_dir));
		XMStoreFloat3(&cbm.camera_pos, XMLoadFloat3((const XMFLOAT3*)&camerapos));

		for (int i = 0; i < data.size(); i++) {
			cbmr.boneMatrix[i] = XMMatrixTranspose(data[i]);
		}
		Draw(m_pmx_data, cbm, cbmr, IndiviData);//insert data

		//OutputDebugString("\n SetPlayerData\n");

		break;
	case object:
		workmat = rotemat * objectPos * workmat;
		XMStoreFloat4x4(&cbo.World, XMMatrixTranspose(workmat));
		XMStoreFloat4x4(&cbo.View, XMMatrixTranspose(Camera::m_mView));
		XMStoreFloat4x4(&cbo.Projection, XMMatrixTranspose(Camera::m_mProj));
		XMStoreFloat4(&cbo.ambient, XMLoadFloat4((const XMFLOAT4*)&ambient->data));
		XMStoreFloat4(&cbo.diffuse, XMLoadFloat4((const XMFLOAT4*)&diffuse->data));
		XMStoreFloat4(&cbo.specular, XMLoadFloat4((const XMFLOAT4*)&specular.data));
		XMStoreFloat3(&cbo.light_dir, XMLoadFloat3((const XMFLOAT3*)&light_dir));
		XMStoreFloat3(&cbo.camera_pos, XMLoadFloat3((const XMFLOAT3*)&camerapos));
		Draw(m_pmx_data, cbo, IndiviData);//insert data

		//OutputDebugString("\n SetObjectData\n");

		break;
	case shadow:
		workmat = rotemat * objectPos * workmat;
		XMStoreFloat4x4(&cb.World, XMMatrixTranspose(workmat));
		XMStoreFloat4x4(&cb.View, XMMatrixTranspose(Camera::m_mView));
		XMStoreFloat4x4(&cb.Projection, XMMatrixTranspose(Camera::m_mProj));
		Draw(m_pmx_data, cb, IndiviData);//insert data

		//OutputDebugString("\n SetshadowData\n");

		break;
	}
	Position = XMFLOAT3(0, 0, 0);
}

void CPmx::CreateBone()
{
	unsigned int size = m_pmx_data.s_PmxBoneNum;
	bones.resize(size);
	for (unsigned int i = 0; i < size; ++i) {
		int parentIdx(-1), firstChildIdx(-1), siblingIdx(-1);
		if (m_pmx_data.s_pPmxBone[i].ParentBoneIndex != 0xFFFF) parentIdx = m_pmx_data.s_pPmxBone[i].ParentBoneIndex;
		for (unsigned int j = i + 1; j < size; ++j) {
			if (m_pmx_data.s_pPmxBone[i].ParentBoneIndex == m_pmx_data.s_pPmxBone[j].ParentBoneIndex) {
				siblingIdx = j;
				break;
			}
		}
		for (unsigned int j = 0; j < size; ++j) {
			if (i == m_pmx_data.s_pPmxBone[j].ParentBoneIndex) {
				firstChildIdx = j;
				break;
			}
		}
		XMMATRIX modelLocalInitMat;
		modelLocalInitMat = XMMatrixTranslation(MmdStruct::scale*m_pmx_data.s_pPmxBone[i].Position[0], MmdStruct::scale*m_pmx_data.s_pPmxBone[i].Position[1], MmdStruct::scale*m_pmx_data.s_pPmxBone[i].Position[2]);

		char boneName[21] = { 0 };	// ボーン名が20byteのときのために最後に0を追加
		memcpy(boneName, m_pmx_data.s_pPmxBone[i].Name, 20);
		bones[i].id = i;
		if (parentIdx != -1) bones[i].parent = &bones[parentIdx];
		if (firstChildIdx != -1) bones[i].firstChild = &bones[firstChildIdx];
		if (siblingIdx != -1) bones[i].sibling = &bones[siblingIdx];
		bones[i].name = boneName;
		bones[i].type = 0;
		if (m_pmx_data.s_pPmxBone[i].Flag_IK)
			bones[i].type = 7;
		bones[i].initMat = modelLocalInitMat;	// モデルローカル座標系
		bones[i].offsetMat = XMMatrixInverse(0, bones[i].initMat);
	}
	InitMatCalc::Run(&bones[0], 0);	// ボーンローカル座標系に変換
	for (unsigned int i = 0; i < size; ++i) bones[i].boneMat = bones[i].initMat;
}

void CPmx::CreateIkData()
{
	int iknum = 0;
	pmdIkData.resize(m_pmx_data.s_PmxIKNum);
	for (unsigned short i = 0; i < m_pmx_data.s_PmxBoneNum; ++i) {
		if (m_pmx_data.s_pPmxBone[i].Flag_IK) {
			MmdStruct::PmdIkDataWithoutArray pmdIkDataWithoutArray;
			pmdIkDataWithoutArray.ik_bone_index = i;
			pmdIkDataWithoutArray.ik_target_bone_index = m_pmx_data.s_pPmxBone[i].IKInfo.TargetBoneIndex;
			pmdIkDataWithoutArray.control_weight = m_pmx_data.s_pPmxBone[i].IKInfo.RotLimit;
			pmdIkDataWithoutArray.iterations = m_pmx_data.s_pPmxBone[i].IKInfo.LoopNum;
			pmdIkDataWithoutArray.ik_chain_length = m_pmx_data.s_pPmxBone[i].IKInfo.LinkNum;
			vector<unsigned short> ik_child_bone_index(pmdIkDataWithoutArray.ik_chain_length);
			for (unsigned char j = 0; j < pmdIkDataWithoutArray.ik_chain_length; ++j)
				ik_child_bone_index[j] = m_pmx_data.s_pPmxBone[i].IKInfo.Link[j].BoneIndex;
			pmdIkData[iknum].ik_bone_index = pmdIkDataWithoutArray.ik_bone_index;
			pmdIkData[iknum].ik_target_bone_index = pmdIkDataWithoutArray.ik_target_bone_index;
			pmdIkData[iknum].ik_chain_length = pmdIkDataWithoutArray.ik_chain_length;
			pmdIkData[iknum].iterations = pmdIkDataWithoutArray.iterations;
			pmdIkData[iknum].control_weight = pmdIkDataWithoutArray.control_weight;
			pmdIkData[iknum].ik_child_bone_index = ik_child_bone_index;
			iknum++;
		}
	}
}

void CPmx::TexLoad(ID3D11Device* device, LPSTR TexPass)
{
	//Wcharに変換用
	LPWSTR TextureNameW = new WCHAR[100];//テクスチャの名前

	LPWSTR TextureNamePASSW = new WCHAR[100];//テクスチャがあるパス

	for (UINT i = 0; i < m_pmx_data.s_PmxTextureNum; i++)
		//テクスチャの数分
	{

		//リソース一時格納用
		ID3D11ShaderResourceView* Texture;

		int err = 0;
		//pass変換
		err = mbstowcs(TextureNamePASSW, TexPass, 1000);//変換
		if (err < 0)
		{
			MessageBox(0, "TexLoadにてパスの文字列がおかしいです。　 error", NULL, MB_OK);
			return;
		}

		err = mbstowcs(TextureNameW, m_pmx_data.s_pPmxTexture[i].TexturePath, 1000);//変換
		if (err < 0)
		{
			MessageBox(0, "TexLoad　テクスチャのロードがおかしいです error", NULL, MB_OK);
			return;
		}
		//現在必要なもの 11/27
		wcscat(TextureNamePASSW, TextureNameW);//合成
		//ここで引数で持ってくる

		//テクスチャー作成
		TexMetadata meta;
		GetMetadataFromWICFile(TextureNamePASSW, 0, meta);
		ScratchImage *image = new ScratchImage();
		LoadFromWICFile(TextureNamePASSW, 0, &meta, *image);

		if (FAILED(CreateShaderResourceView(device, image->GetImages(), image->GetImageCount(), meta, &Texture)))
		{
			MessageBox(0, "テクスチャー作成 error", NULL, MB_OK);
			return;
		}
		//成功時プッシュバック
		TexData.push_back(Texture);//格納

	}
	delete TextureNameW;
	TextureNameW = NULL;
	delete TextureNamePASSW;
	TextureNamePASSW = NULL;
}

vector<XMMATRIX> CPmx::updateVerBuf(XMMATRIX *world)
{
	D3D11_BUFFER_DESC bd_vertex;
	ZeroMemory(&bd_vertex, sizeof(bd_vertex));

	//必要な形にデータを格納しなおす
	if (VertexBufferUpdate == NULL) {
		VertexBufferUpdate = new PMX_SEND_DATA[m_pmx_data.s_PmxVertexNum];
	}
	int j = 0;//現在のマテリアルの番号を格納
	int index = m_pmx_data.s_pPmxMaterial[j].MaterialFaceNum;

	vector<XMMATRIX> worlds(bones.size());
	WorldsCalc::Run(&bones[0], world, &worlds);//これforいるくね？　いらないみたい


	for (int i = 0; i < m_pmx_data.s_PmxVertexNum; i++)
	{

		////ここをもう少し考える
		////うまい感じにマトリックスの座標をもってくる
		//XMMATRIX matwork;

		////12/13 計算をマトリックスでしてから座標算出でやってみる
		//fixed 頂点ブレンドでやってます。最終的にはこれ消したいですね


		VertexBufferUpdate[i].pos[0] = MmdStruct::scale*m_pmx_data.s_pPmxVertex[i].Position[0];
		VertexBufferUpdate[i].pos[1] = MmdStruct::scale*m_pmx_data.s_pPmxVertex[i].Position[1];
		VertexBufferUpdate[i].pos[2] = MmdStruct::scale*m_pmx_data.s_pPmxVertex[i].Position[2];

		VertexBufferUpdate[i].normal[0] = m_pmx_data.s_pPmxVertex[i].Normal[0];
		VertexBufferUpdate[i].normal[1] = m_pmx_data.s_pPmxVertex[i].Normal[1];
		VertexBufferUpdate[i].normal[2] = m_pmx_data.s_pPmxVertex[i].Normal[2];

		VertexBufferUpdate[i].uv[0] = m_pmx_data.s_pPmxVertex[i].UV[0];
		VertexBufferUpdate[i].uv[1] = m_pmx_data.s_pPmxVertex[i].UV[1];

		if (i == index)
		{
			j++;//マテリアル番号更新
			index += m_pmx_data.s_pPmxMaterial[j].MaterialFaceNum;
		}

		VertexBufferUpdate[i].col[0] = m_pmx_data.s_pPmxMaterial[j].Diffuse[0]; //R
		VertexBufferUpdate[i].col[1] = m_pmx_data.s_pPmxMaterial[j].Diffuse[1]; 	//G
		VertexBufferUpdate[i].col[2] = m_pmx_data.s_pPmxMaterial[j].Diffuse[2]; 	//B
		VertexBufferUpdate[i].col[3] = m_pmx_data.s_pPmxMaterial[j].Diffuse[3];	//A

		VertexBufferUpdate[i].col2[0] = m_pmx_data.s_pPmxMaterial[j].Specular[0]; //R
		VertexBufferUpdate[i].col2[1] = m_pmx_data.s_pPmxMaterial[j].Specular[1]; 	//G
		VertexBufferUpdate[i].col2[2] = m_pmx_data.s_pPmxMaterial[j].Specular[2]; 	//B
		VertexBufferUpdate[i].col2[3] = m_pmx_data.s_pPmxMaterial[j].Specular[3];	//A


		//ボーンインデックス
		VertexBufferUpdate[i].boneIndex[0] = m_pmx_data.s_pPmxVertex[i].BoneIndex[0];
		VertexBufferUpdate[i].boneIndex[1] = m_pmx_data.s_pPmxVertex[i].BoneIndex[1];
		VertexBufferUpdate[i].boneIndex[2] = m_pmx_data.s_pPmxVertex[i].BoneIndex[2];
		VertexBufferUpdate[i].boneIndex[3] = m_pmx_data.s_pPmxVertex[i].BoneIndex[3];

		VertexBufferUpdate[i].boneWeight[0] = m_pmx_data.s_pPmxVertex[i].BoneWeight[0];
		VertexBufferUpdate[i].boneWeight[1] = m_pmx_data.s_pPmxVertex[i].BoneWeight[1];
		VertexBufferUpdate[i].boneWeight[2] = m_pmx_data.s_pPmxVertex[i].BoneWeight[2];
		VertexBufferUpdate[i].boneWeight[3] = m_pmx_data.s_pPmxVertex[i].BoneWeight[3];


	}


	//モーフ計算
	static FLOAT timeMorph;//現在時間
	static FLOAT timedif;//時間差
	static BOOL MorphEyeOpen = true;//どっちの状態か

	if (timeMorph > 1 && !MorphEyeOpen)
	{
		MorphEyeOpen = true;
		timedif = -0.01;
	}
	else if (timeMorph <= 0 && MorphEyeOpen)
	{
		MorphEyeOpen = false;
		timedif = +0.01;
	}
	timeMorph += timedif;


	//モーフ設定
	for (int i = 0; i < m_pmx_data.s_pPmxMorph[17].DataNum; i++)
	{

		//計算するやつ
		XMVECTOR q1, q2, ans;
		FLOAT ansFloat;
		FLOAT Work;

		//X
		q1 = XMLoadFloat(&VertexBufferUpdate[m_pmx_data.s_pPmxMorph[17].Vertex[i].Index].pos[0]);

		Work = VertexBufferUpdate[m_pmx_data.s_pPmxMorph[17].Vertex[i].Index].pos[0] + m_pmx_data.s_pPmxMorph[17].Vertex[i].Offset[0];

		q2 = XMLoadFloat(&Work);

		ans = XMVectorLerp(q1, q2, timeMorph);

		XMStoreFloat(&ansFloat, ans);

		VertexBufferUpdate[m_pmx_data.s_pPmxMorph[17].Vertex[i].Index].pos[0] = ansFloat;

		//Y
		q1 = XMLoadFloat(&VertexBufferUpdate[m_pmx_data.s_pPmxMorph[17].Vertex[i].Index].pos[1]);

		Work = VertexBufferUpdate[m_pmx_data.s_pPmxMorph[17].Vertex[i].Index].pos[1] + m_pmx_data.s_pPmxMorph[17].Vertex[i].Offset[1];

		q2 = XMLoadFloat(&Work);

		ans = XMVectorLerp(q1, q2, timeMorph);

		XMStoreFloat(&ansFloat, ans);

		VertexBufferUpdate[m_pmx_data.s_pPmxMorph[17].Vertex[i].Index].pos[1] = ansFloat;

		//Z
		q1 = XMLoadFloat(&VertexBufferUpdate[m_pmx_data.s_pPmxMorph[17].Vertex[i].Index].pos[2]);

		Work = VertexBufferUpdate[m_pmx_data.s_pPmxMorph[17].Vertex[i].Index].pos[2] + m_pmx_data.s_pPmxMorph[17].Vertex[i].Offset[2];

		q2 = XMLoadFloat(&Work);

		ans = XMVectorLerp(q1, q2, timeMorph);

		XMStoreFloat(&ansFloat, ans);

		VertexBufferUpdate[m_pmx_data.s_pPmxMorph[17].Vertex[i].Index].pos[2] = ansFloat;


	}

	return worlds;
}

//-----------------------------------------------------------------------------------------------------------------------------------------
void ObjectIndividualData::ReflectionData(ID3D11DeviceContext* context, ID3D11Buffer* constantBuffer)//データ格納
{

	UINT stride = sizeof(PMX_SEND_DATA);	//座標に使用するサイズ XMFloat*3 
																	//posX,posY,posZに準ずる
	UINT offset = 0;
	context->IASetVertexBuffers(0, 1, &pVerBuffer, &stride, &offset);
	context->IASetIndexBuffer(pIndBuffer, DXGI_FORMAT_R16_UINT, 0);
	context->IASetInputLayout(pVertexLayout);
	context->VSSetShader(pVertexShader, NULL, 0);
	context->PSSetShader(pPixelShader, NULL, 0);
	context->VSSetConstantBuffers(0, 1, &constantBuffer);
	context->PSSetConstantBuffers(0, 1, &constantBuffer);
	context->RSSetState(pRasterizerState);
}

void ObjectIndividualData::ReflectionData(ID3D11DeviceContext* context, ID3D11Buffer* constantBuffer, ID3D11Buffer* constantBoneBuffer)//データ格納
{

	UINT stride = sizeof(PMX_SEND_DATA);	//座標に使用するサイズ XMFloat*3 
																	//posX,posY,posZに準ずる
	UINT offset = 0;
	context->IASetVertexBuffers(0, 1, &pVerBuffer, &stride, &offset);
	context->IASetIndexBuffer(pIndBuffer, DXGI_FORMAT_R16_UINT, 0);
	context->IASetInputLayout(pVertexLayout);
	context->VSSetShader(pVertexShader, NULL, 0);
	context->PSSetShader(pPixelShader, NULL, 0);
	context->VSSetConstantBuffers(0, 1, &constantBuffer);
	context->VSSetConstantBuffers(1, 1, &constantBoneBuffer);
	context->PSSetConstantBuffers(0, 1, &constantBuffer);
	context->RSSetState(pRasterizerState);
}


