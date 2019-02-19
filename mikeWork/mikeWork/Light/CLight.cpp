#include "CLight.h"


void CLight::Init(ID3D11Device *device, ID3D11DeviceContext* context, const LPCWSTR psShaderName, const LPCWSTR gsShaderName, const LPCWSTR vsShaderName)
{

	//シェーダーコンパイル
	if (psShaderName != NULL)
		SetPmxUsingPixelShader(device, psShaderName);

	if (gsShaderName != NULL)
		SetPmxUsingGeometryShader(device, gsShaderName);

	if (vsShaderName != NULL)
		SetPmxUsingVertexShader(device, vsShaderName);

	//レイアウト作成
	CreateLayout(device);

	//コンスタンスバッファ作成
	CreateContextBuffer(device, context);

	//テクスチャロード
	TexLoad(device);//textureLoad

	LIGHT::POINT_LIGHT::SEND_DATA vertices[] =
	{
	{XMFLOAT4(100,0,-100.0f,1.0f),XMFLOAT3(1,0,0),XMFLOAT2(0,0)},
	{XMFLOAT4(-100,0,100.0f,1.0f),XMFLOAT3(0,0,1),XMFLOAT2(0,0) },
	{XMFLOAT4(100,-0,-100.0f,1.0f),XMFLOAT3(1,1,1),XMFLOAT2(0,0)},

	{XMFLOAT4(-100,0,100.0f,1.0f),XMFLOAT3(0,0,1),XMFLOAT2(0,0) },
	{XMFLOAT4(100,-0,-100.0f,1.0f),XMFLOAT3(1,1,1),XMFLOAT2(0,0)},
	{XMFLOAT4(-100,-0,100.0f,1.0f),XMFLOAT3(0,0,1),XMFLOAT2(0,0)}

	};
	D3D11_BUFFER_DESC bd;
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(LIGHT::POINT_LIGHT::SEND_DATA) * 6;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;
	bd.MiscFlags = 0;
	D3D11_SUBRESOURCE_DATA InitData;
	InitData.pSysMem = vertices;
	if (FAILED(device->CreateBuffer(&bd, &InitData, &Buffer)))
	{
		MessageBox(0, "bd error", NULL, MB_OK);
		exit(true);
	}

}

void CLight::SetPmxUsingPixelShader(ID3D11Device  *device, LPCWSTR shaderName)
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

	hr = device->CreatePixelShader(pCompilePS->GetBufferPointer(), pCompilePS->GetBufferSize(), NULL, &pPixelShader);

	if (FAILED(hr))
	{
		MessageBox(0, "CreatePixelShader error", NULL, MB_OK);
		exit(true);
	}
	OutputDebugString("\nCreatePixelShaderr成功\n");


}
void CLight::SetPmxUsingGeometryShader(ID3D11Device *, LPCWSTR)
{
	//#ifdef _Debug
	//	UINT flags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
	//#else
	//	UINT flags = 0;
	//#endif 
}
void CLight::SetPmxUsingVertexShader(ID3D11Device  *device, LPCWSTR shaderName)
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

	hr = device->CreateVertexShader(pCompileVS->GetBufferPointer(), pCompileVS->GetBufferSize(), NULL, &pVertexShader);

	if (FAILED(hr))
	{
		MessageBox(0, "CreateVertexShader error", NULL, MB_OK);
		exit(true);
	}
	OutputDebugString("\nCreateVertexShader成功\n");
}
void CLight::CreateLayout(ID3D11Device* device)
{
	HRESULT hr;
	D3D11_INPUT_ELEMENT_DESC layout[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0,D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD",    0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	unsigned int LayoutCreateConut = ARRAYSIZE(layout);//レイアウトサイズ取得
	hr = device->CreateInputLayout(layout, LayoutCreateConut
		, pCompileVS->GetBufferPointer(), pCompileVS->GetBufferSize(), &pVertexLayout);
	if (FAILED(hr))
	{
		MessageBox(0, "layout error", NULL, MB_OK);
		exit(true);
	}

	pCompilePS->Release();
	pCompilePS = NULL;
	pCompileVS->Release();
	pCompileVS = NULL;
}

void  CLight::CreateContextBuffer(ID3D11Device *device, ID3D11DeviceContext *context)
{
	HRESULT hr;
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	// 定数バッファ作成
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(LIGHT::SCENE);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = 0;
	hr = device->CreateBuffer(&bd, NULL, &SceneBuffer);
	if (FAILED(hr)) {
		MSG("ライトのバッファでエラーです(scene)");
		exit(1);
	}

	bd.ByteWidth = sizeof(LIGHT::POINT_LIGHT::POINT_LIGHT);
	hr = device->CreateBuffer(&bd, NULL, &LightBuffer);
	if (FAILED(hr)) {
		MSG("ライトのバッファでエラーです(light)");
		exit(1);
	}

	// 定数バッファ 変換行列（プロジェクション、ビュー）
	XMVECTOR Eye = XMVectorSet(0.0f, 140.0f, -350.0f, 0.0f);
	XMVECTOR At = XMVectorSet(0.0f, 90.0f, 0.0f, 0.0f);
	XMVECTOR Up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	scn.mtxView = XMMatrixTranspose(XMMatrixLookAtLH(Eye, At, Up));
	scn.mtxProj = XMMatrixTranspose(XMMatrixPerspectiveFovLH(XM_PIDIV4, WINDOW_WIDTH / (FLOAT)WINDOW_HEIGHT, 0.1f, 1000.0f));
	scn.mtxWorld = XMMatrixTranspose(XMMatrixTranslation(0, 10, -20));
	context->UpdateSubresource(SceneBuffer, 0, NULL, &scn, 0, 0);

	// バッファの内容更新
	PLight.vDirLight[0] = XMVectorSet(0.577f, 0.577f, -0.577f, 0);
	PLight.colDirLight[0] = XMVectorSet(0.7f, 0.7f, 0.7f, 0);
	PLight.vHemiDir = XMVectorSet(0, 1, 0, 0);
	PLight.colHemiSky = XMVectorSet(0.2f, 0.2f, 0.3f, 0.0f);
	PLight.colHemiGrd = XMVectorSet(0.3f, 0.3f, 0.2f, 0.0f);

	float plmin = 5.0f;//点光源の影響が１になる距離
	float plmax = 250.0;//点光源の影響が０になる距離
	PLight.posPLight[0] = XMVectorSet(100, 50, 0, 0);
	PLight.colPLight[0] = XMVectorSet(0.0f, 0.8f, 1, 0);
	PLight.prmPLight[0] = XMVectorSet(plmin, plmax, 1 / (plmin - plmax), -plmax / (plmin - plmax));
	PLight.posPLight[1] = XMVectorSet(-100, 50, 0, 0);
	PLight.colPLight[1] = XMVectorSet(1.0f, 0.8f, 0, 0);
	PLight.prmPLight[1] = XMVectorSet(plmin, plmax, 1 / (plmin - plmax), -plmax / (plmin - plmax));
	PLight.diffuse = XMVectorSet(1, 0, 1, 1);
	context->UpdateSubresource(LightBuffer, 0, NULL, &PLight, 0, 0);
}

void CLight::TexLoad(ID3D11Device* device)//textureLoad
{
	//ここで引数で持ってくる
	D3D11_SAMPLER_DESC SamDesc;
	ZeroMemory(&SamDesc, sizeof(D3D11_SAMPLER_DESC));

	SamDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	SamDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	SamDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	SamDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	device->CreateSamplerState(&SamDesc, &SampleLinear);
	//テクスチャー作成
	TexMetadata meta;
	GetMetadataFromWICFile(ClearTex, 0, meta);
	ScratchImage *image = new ScratchImage();
	LoadFromWICFile(ClearTex, 0, &meta, *image);

	if (FAILED(CreateShaderResourceView(device, image->GetImages(), image->GetImageCount(), meta, &Texture)))
	{
		MessageBox(0, "テクスチャー作成 error", NULL, MB_OK);
		return;
	}

}

void CLight::Render(ID3D11DeviceContext* context)
{
	UINT stride = sizeof(LIGHT::POINT_LIGHT::SEND_DATA);
	UINT offset = 0;

	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	context->IASetVertexBuffers(0, 1, &Buffer, &stride, &offset);

	context->PSSetSamplers(0, 1, &SampleLinear);
	context->PSSetShaderResources(0, 1, &Texture);//ここをチェックする
	context->VSSetConstantBuffers(0, 1, &SceneBuffer);
	context->VSSetConstantBuffers(1, 1, &LightBuffer);
	context->IASetInputLayout(pVertexLayout);
	context->VSSetShader(pVertexShader, NULL, 0);
	context->PSSetShader(pPixelShader, NULL, 0);

	context->Draw(6, 0);
}