#include "Director.h"
#include "../PMX/modelList.h"
#include "../shader/shaderList.h"
#include "../imgui/CImguiMine.h"

Director::Director()
{
	ZeroMemory(this, sizeof(Director));
	FrechanDance = new VmdMotionController();
	KanadeDance = new VmdMotionController();
	AiDance = new VmdMotionController();
	MikuDance = new VmdMotionController();
	IMGUIDrawdata::create();
	m_Scene = GAMEMAIN;
}

Director::~Director()
{
	AllDelete();

	/*SAFE_DELETE(m_pSound);*/
}

void Director::AllDelete()
{
	SAFE_DELETE(m_pCamera);
	SAFE_DELETE(m_pD3d);
	SAFE_DELETE(m_pWindow);
	UninitInput();


	//ダンス
	SAFE_DELETE(FrechanDance);
	SAFE_DELETE(KanadeDance);
	SAFE_DELETE(AiDance);
	SAFE_DELETE(MikuDance);

	//モデル

	////フレデリカ
	//SAFE_DELETE(FreChan);
	//SAFE_DELETE(FreChanShadow);
	//SAFE_DELETE(FreChanOutline);

	////ミク
	//SAFE_DELETE(Miku);
	//SAFE_DELETE(MikuShadow);
	//SAFE_DELETE(MikuOutline);

	////奏
	//SAFE_DELETE(Kanade);
	//SAFE_DELETE(KanadeShadow);
	//SAFE_DELETE(KanadeOutline);
	//
	////あい
	//SAFE_DELETE(Ai);
	//SAFE_DELETE(AiShadow);
	//SAFE_DELETE(AiOutline);


	ImGui_ImplDX11_Shutdown();
	IMGUIDrawdata::destroy();

}

void Director::Run(HINSTANCE hInstance)
{
	m_pbase = new Base();
	char dir[512];
	GetCurrentDirectory(sizeof(dir), dir);
	m_pbase->InitDirectory(dir);

	m_hInstance = hInstance;
	if (FAILED(Init()))
	{
		return;
	}
	ShowWindow(m_hWnd, SW_SHOW);
	UpdateWindow(m_hWnd);

	// メッセージループ
	MSG msg = { 0 };
	ZeroMemory(&msg, sizeof(msg));


	while (msg.message != WM_QUIT)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{

			MainLoop();
		}
	}
}

void Director::RenderRoom()
{
	m_pD3d->m_pDeviceContext->PSSetSamplers(0, 1, &SampleLinear);
	CPmx::Render(m_pD3d->m_pDeviceContext);
	//light->Render(m_pD3d->m_pDeviceContext);
}

void Director::MainLoop()
{
	m_pD3d->Clear();

	switch (m_Scene)
	{
	case TITLE:
		if (GetKeyboardPress(DIK_RETURN))
		{
			m_Scene = GAMEMAIN;
		}
		//titleRender();
		break;
	case GAMEMAIN:
		Update();
		Draw();
		RenderRoom();
		break;
	case GAMEOVER:
		break;
	}
	m_pCamera->move();

	FixFPS60();
	m_pD3d->Present();
	UpdateInput();
}

HRESULT Director::Init()
{
	//window
	m_pWindow = new Window();
	if (!m_pWindow)
	{
		return E_FAIL;
	}
	MFAIL(m_pWindow->InitWindow(m_hInstance, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, APP_NAME), "ウィンドウ作成失敗");
	m_hWnd = m_pWindow->m_hWnd;
	//direct3D11
	D3D_INIT di;
	m_pD3d = new DIRECT3D11();
	if (m_pD3d == NULL)
	{
		MSG("Direct3Dの初期化失敗");
		return E_FAIL;
	}
	di.hWnd = m_hWnd;
	MFAIL(m_pD3d->Init(&di), "Direct3D初期化失敗");

	//メッシュ読み込み


	//camera
	m_pCamera = new Camera();
	m_pCamera->Init(WINDOW_WIDTH, WINDOW_HEIGHT);
	m_pCamera->SetCameraPositionGaze(0.0f, 10, -20, 0, 10, 0);

	////Sound(XAuido2)
	//m_pSound = new SOUND;
	//MFAIL(m_pSound->Init(), "サウンド初期化失敗");
	////サウンド読み込み
	//SetSoundDirectory();
	//m_pSound->LoadSound("Chorus.wav");

	//init pmx

	//フレデリカ
	FreChan = new CMainchar(DrawingType::player, 0);
	FreChan->Init(m_pD3d->m_pDevice, FREDERICA, SHADER_CHARCTER_NAME, NULL, SHADER_CHARCTER_NAME, (char*)FREDERICA_PASS);
	//コンストラクタですべきだけど　調整場所わかりやすくするために分けています　
	FreChan->setPosition(XMFLOAT3(3, 0, 0));

	FreChanOutline = new CMainchar(DrawingType::shadow, 0);
	FreChanOutline->Init(m_pD3d->m_pDevice, FREDERICA, SHADER_SHADOW_NAME, NULL, SHADER_SHADOW_NAME, (char*)FREDERICA_PASS);



	//奏
	Kanade = new CMainchar(DrawingType::player, 0);
	Kanade->Init(m_pD3d->m_pDevice, KANADE, SHADER_CHARCTER_NAME, NULL, SHADER_CHARCTER_NAME, (char*)KANADE_PASS);
	//コンストラクタですべきだけど　調整場所わかりやすくするために分けています　
	Kanade->setPosition(XMFLOAT3(-10, 0, 0));

	KanadeOutline = new CMainchar(DrawingType::shadow, 0);
	KanadeOutline->Init(m_pD3d->m_pDevice, KANADE, SHADER_SHADOW_NAME, NULL, SHADER_SHADOW_NAME, (char*)KANADE_PASS);
	


	//雛鶴愛
	Ai = new CMainchar(DrawingType::player, 0);
	Ai->Init(m_pD3d->m_pDevice, AI, SHADER_CHARCTER_NAME, NULL, SHADER_CHARCTER_NAME, (char*)AI_PASS);
	//コンストラクタですべきだけど　調整場所わかりやすくするために分けています　
	Ai->setPosition(XMFLOAT3(10, 0, 0));

	AiOutline = new CMainchar(DrawingType::shadow, 0);
	AiOutline->Init(m_pD3d->m_pDevice, AI, SHADER_SHADOW_NAME, NULL, SHADER_SHADOW_NAME, (char*)AI_PASS);


	//ミク
	Miku = new CMainchar(DrawingType::player, 0);
	Miku->Init(m_pD3d->m_pDevice, MIKU, SHADER_CHARCTER_NAME, NULL, SHADER_CHARCTER_NAME, (char*)MIKU_PASS);
	//コンストラクタですべきだけど　調整場所わかりやすくするために分けています　
	Miku->setPosition(XMFLOAT3(-2.5, 0, 0));

	MikuOutline = new CMainchar(DrawingType::shadow, 0);
	MikuOutline->Init(m_pD3d->m_pDevice, MIKU, SHADER_SHADOW_NAME, NULL, SHADER_SHADOW_NAME, (char*)MIKU_PASS);



	//ステージ
	stage = new CPmx(DrawingType::object);

	stage->Init(m_pD3d->m_pDevice, STAGE, SHADER_SKYDOME_NAME, NULL, SHADER_SKYDOME_NAME, (char*)STAGE_PASS);
	stage->setPosition(XMFLOAT3(0, 0, 0));


	//VMD
	FrechanDance->LoadVmdFile((char*)ONESHIN, FreChan->DeliverBones(), FreChan->DeliverPmdIkData(), true);
	KanadeDance->LoadVmdFile((char*)LOVELETTER, Kanade->DeliverBones(), Kanade->DeliverPmdIkData(), true);
	AiDance->LoadVmdFile((char*)GOKURAKUJOUDO, Ai->DeliverBones(), Ai->DeliverPmdIkData(), true);
	MikuDance->LoadVmdFile((char*)YUKISUKI, Miku->DeliverBones(), Miku->DeliverPmdIkData(), true);

	//ロードしてきたVMDを輪郭線に同期させる　（後　VMDのUpdateだけで同期される）
	FreChanOutline->SetBone(FreChan->DeliverBones());
	KanadeOutline->SetBone(Kanade->DeliverBones());
	AiOutline->SetBone(Ai->DeliverBones());
	MikuOutline->SetBone(Miku->DeliverBones());


	//モーションセット 
	FreChan->SetMotion(FrechanDance);
	FreChanOutline->SetMotion(FrechanDance);


	Kanade->SetMotion(KanadeDance);
	KanadeOutline->SetMotion(KanadeDance);

	Ai->SetMotion(AiDance);
	AiOutline->SetMotion(AiDance);


	Miku->SetMotion(MikuDance);
	MikuOutline->SetMotion(MikuDance);
	//11/10 init ok 他もしましょう

	/*fbxの読み込み*/

	/*light = new CLight();
	light->Init(m_pD3d->m_pDevice, m_pD3d->m_pDeviceContext, SHADER_POINT_LIGHT, NULL, SHADER_POINT_LIGHT);*/
	InitInput(m_hInstance, m_pD3d->m_hWnd);
	TitleInitShader();
	//imgui
	ImGui::CreateContext();//これないとエラー出るよ！ getIOで
	ImGui_ImplDX11_Init(m_pD3d->m_hWnd, m_pD3d->m_pDevice, m_pD3d->m_pDeviceContext);
	return S_OK;
}

void Director::FixFPS60()
{
	static INT Frames = 0;
	static LARGE_INTEGER Frq = { 0 }, PreviousTime = { 0 }, CurrentTime = { 0 };
	DOUBLE Time = 0;
	char sz[11] = { 0 };

	while (Time < 16.6666)//1100ms / 60frame=16.6666 
	{
		QueryPerformanceFrequency(&Frq);

		QueryPerformanceCounter(&CurrentTime);
		Time = CurrentTime.QuadPart - PreviousTime.QuadPart;
		Time *= (DOUBLE)1100.0 / (DOUBLE)Frq.QuadPart;
		IMGUIDrawdata::get_instance()->setFPS(1000 / Time);
	}
	PreviousTime = CurrentTime;
}

void Director::Draw()
{

	//stage

	stage->ProcessingCalc(m_pCamera->m_f);

	//キャラクター
	if (IMGUIDrawdata::get_instance()->getModelDisplay(0)) {
		FreChanOutline->setPosition(FreChan->getPosition());
		FreChanOutline->setRotetation(FreChan->getRotetation());
		FreChanOutline->charDraw(m_pCamera->m_f);
		FreChan->charDraw(m_pCamera->m_f);
	}

	if (IMGUIDrawdata::get_instance()->getModelDisplay(1)) {
		KanadeOutline->setPosition(Kanade->getPosition());
		KanadeOutline->setRotetation(Kanade->getRotetation());
		KanadeOutline->charDraw(m_pCamera->m_f);
		Kanade->charDraw(m_pCamera->m_f);
	}


	if (IMGUIDrawdata::get_instance()->getModelDisplay(2)) {
		AiOutline->setPosition(Ai->getPosition());
		AiOutline->setRotetation(Ai->getRotetation());
		AiOutline->charDraw(m_pCamera->m_f);
		Ai->charDraw(m_pCamera->m_f);
	}

	if (IMGUIDrawdata::get_instance()->getModelDisplay(3)) {
		MikuOutline->setPosition(Miku->getPosition());
		MikuOutline->setRotetation(Miku->getRotetation());
		MikuOutline->charDraw(m_pCamera->m_f);
		Miku->charDraw(m_pCamera->m_f);
	}

	IMGUIDrawdata::get_instance()->Draw();
}
void Director::Update()
{
	

	if (IMGUIDrawdata::get_instance()->getAnimationSpeed() == 1 || (GetKeyboardPress(DIK_SPACE) && IMGUIDrawdata::get_instance()->getFrameAdvance()))
	{
		//モーション時間更新
		FrechanDance->AdvanceTime();
		KanadeDance->AdvanceTime();
		AiDance->AdvanceTime();
		MikuDance->AdvanceTime();

		FrechanDance->UpdateBoneMatrix();
		KanadeDance->UpdateBoneMatrix();
		AiDance->UpdateBoneMatrix();
		MikuDance->UpdateBoneMatrix();
	}

}
void Director::TitleInitShader()
{
	ID3DBlob *pCompileVS = NULL;
	ID3DBlob *pCompilePS = NULL;
	ID3D10Blob *pErrors = NULL;
	HRESULT hr;

#ifdef _Debug
	UINT flags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
	UINT flags = 0;
#endif 

	if (FAILED(D3DCompileFromFile(TEX_SHADER_PASS, nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, "VS", "vs_5_0", flags, 0, &pCompileVS, &pErrors)))
	{
		MessageBox(0, (LPSTR)pErrors->GetBufferPointer(), NULL, MB_OK);
		exit(true);
	}
	hr = m_pD3d->m_pDevice->CreateVertexShader(pCompileVS->GetBufferPointer(), pCompileVS->GetBufferSize(), NULL, &VertexShader);
	if (FAILED(hr))
	{
		MessageBox(0, "CreateVertexShader error", NULL, MB_OK);
		exit(true);
	}

	if (FAILED(D3DCompileFromFile(TEX_SHADER_PASS, nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, "PS", "ps_5_0", flags, 0, &pCompilePS, &pErrors)))
	{
		MessageBox(0, "PShlsl読み込み失敗", NULL, MB_OK);
		exit(true);
	}
	hr = m_pD3d->m_pDevice->CreatePixelShader(pCompilePS->GetBufferPointer(), pCompilePS->GetBufferSize(), NULL, &PixelShader);

	if (FAILED(hr))
	{
		MessageBox(0, "CreateVertexShader error", NULL, MB_OK);
		exit(true);
	}
	// 頂点レイアウト
	D3D11_INPUT_ELEMENT_DESC layout[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	hr = m_pD3d->m_pDevice->CreateInputLayout(layout, 3, pCompileVS->GetBufferPointer(), pCompileVS->GetBufferSize(), &VertexLayout);
	if (FAILED(hr))
	{
		MessageBox(0, "layout error", NULL, MB_OK);
		exit(true);
	}
	pCompileVS->Release();
	pCompilePS->Release();

	//四角形
	SimpleVertex vertices[] =
	{
		VECTOR3(0.5f,0.8f,-1.0f),VECTOR2(1,0),
		VECTOR3(-0.5f,0.8f,1.0f),VECTOR2(0,0),
		VECTOR3(0.5f,-0.8f,-1.0f),VECTOR2(1,1),

		VECTOR3(-0.5f,0.8f,1.0f),VECTOR2(0,0),
		VECTOR3(0.5f,-0.8f,-1.0f),VECTOR2(1,1),
		VECTOR3(-0.5f,-0.8f,1.0f),VECTOR2(0,1)

	};
	D3D11_BUFFER_DESC bd;
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(SimpleVertex) * 6;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;
	bd.MiscFlags = 0;
	D3D11_SUBRESOURCE_DATA InitData;
	InitData.pSysMem = vertices;
	if (FAILED(m_pD3d->m_pDevice->CreateBuffer(&bd, &InitData, &buffer)))
	{
		MessageBox(0, "bd error", NULL, MB_OK);
		exit(true);
	}

	//バーテックスバッファーをセット
	UINT stride = sizeof(SimpleVertex);
	UINT offset = 0;
	m_pD3d->m_pDeviceContext->IASetVertexBuffers(0, 1, &buffer, &stride, &offset);

	//プリミティブ・トポロジーをセット
	//m_pD3d->m_pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
	m_pD3d->m_pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//テクスチャー用サンプラー作成
	D3D11_SAMPLER_DESC SamDesc;
	ZeroMemory(&SamDesc, sizeof(D3D11_SAMPLER_DESC));

	SamDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	SamDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	SamDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	SamDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	m_pD3d->m_pDevice->CreateSamplerState(&SamDesc, &SampleLinear);

	//テクスチャー作成
	TexMetadata meta;
	GetMetadataFromWICFile(TEX_PASS, 0, meta);
	ScratchImage *image = new ScratchImage();
	LoadFromWICFile(TEX_PASS, 0, &meta, *image);
	if (FAILED(CreateShaderResourceView(m_pD3d->m_pDevice, image->GetImages(), image->GetImageCount(), meta, &Texture)))
	{
		MessageBox(0, "テクスチャー作成 error", NULL, MB_OK);
		exit(true);
	}

}
void Director::titleRender()
{
	//ここでまとめる
	ID3D11ShaderResourceView* Textures[] = { Texture, TextureTwo };


	//テクスチャーをシェーダーに渡す
	m_pD3d->m_pDeviceContext->PSSetSamplers(0, 1, &SampleLinear);
	m_pD3d->m_pDeviceContext->PSSetShaderResources(0, 1, &Texture);//ここをチェックする
	//m_pD3d->m_pDeviceContext->VSSetShaderResources(0, 1, Textures);
	//使用するシェーダーの登録
	m_pD3d->m_pDeviceContext->VSSetShader(VertexShader, NULL, 0);
	m_pD3d->m_pDeviceContext->PSSetShader(PixelShader, NULL, 0);

	m_pD3d->m_pDeviceContext->IASetInputLayout(VertexLayout);
	m_pD3d->m_pDeviceContext->VSSetConstantBuffers(0, 1, &m_pD3d->pConstantBuffer);
	m_pD3d->m_pDeviceContext->PSSetConstantBuffers(0, 1, &m_pD3d->pConstantBuffer);
	//プリミティブをレンダリング
	m_pD3d->m_pDeviceContext->Draw(6, 0);
}