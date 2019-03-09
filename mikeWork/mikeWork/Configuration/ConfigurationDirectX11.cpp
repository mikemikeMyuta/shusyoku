#include"ConfigurationDirectX11.h"
#include "../CAMERA/Camera.h"
#include <random>
#include "../imgui/CImguiMine.h"

 D3D11_VIEWPORT DIRECT3D11::vp;
 ID3D11ShaderResourceView*  DIRECT3D11::TexSRV;//これを使う

DIRECT3D11::DIRECT3D11()
{
	ZeroMemory(this, sizeof(DIRECT3D11));
}

DIRECT3D11::~DIRECT3D11()
{
	SAFE_RELEASE(m_pRasterizerState);
	SAFE_RELEASE(m_pSwapChain);
	SAFE_RELEASE(m_pBackBuffer_TexRTV);
	SAFE_RELEASE(m_pDrawingLikeAnime);
	SAFE_RELEASE(m_pBuckBuffer_DSTexDSV);
	SAFE_RELEASE(m_pBuckBuffer_DSTex);
	SAFE_RELEASE(m_pBuckBuffer_DSTexState);
	SAFE_RELEASE(m_pBlendState);
	SAFE_RELEASE(m_pDevice);
}

HRESULT DIRECT3D11::Init(D3D_INIT* pcd)
{
		HRESULT hr;
	m_hWnd = pcd->hWnd;
	DXGI_SWAP_CHAIN_DESC sd;
	ZeroMemory(&sd, sizeof(sd));
	sd.BufferCount = 1;
	sd.BufferDesc.Width = WINDOW_WIDTH;
	sd.BufferDesc.Height = WINDOW_HEIGHT;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.OutputWindow = m_hWnd;
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	sd.Windowed = TRUE;

	D3D_FEATURE_LEVEL pFeatureLevels = D3D_FEATURE_LEVEL_11_0;

	UINT cdev_flag = 0;
#ifdef _DEBUG
	cdev_flag |= D3D11_CREATE_DEVICE_DEBUG; // 更新して解除しておく
#endif
	if (FAILED(D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL,
		cdev_flag, &pFeatureLevels, 1, D3D11_SDK_VERSION, &sd, &m_pSwapChain, &m_pDevice,
		NULL, &m_pDeviceContext)))
	{
		return FALSE;
	}
	//各種テクスチャーと、それに付帯する各種ビューを作成
	 

	//バックバッファーテクスチャーを取得（既にあるので作成ではない）
	ID3D11Texture2D *pBackBuffer_Tex;


	m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer_Tex);
	//そのテクスチャーに対しレンダーターゲットビュー(RTV)を作成
	m_pDevice->CreateRenderTargetView(pBackBuffer_Tex, NULL, &m_pBackBuffer_TexRTV);
	//m_pDevice->CreateRenderTargetView(pBackBuffer_Tex, NULL, &m_pDrawingLikeAnime);
	//SAFE_RELEASE(pBackBuffer_Tex);

	//デプスステンシルビュー用のテクスチャーを作成
	D3D11_TEXTURE2D_DESC descDepth;
	descDepth.Width = WINDOW_WIDTH;
	descDepth.Height = WINDOW_HEIGHT;
	descDepth.MipLevels = 1;
	descDepth.ArraySize = 1;
	descDepth.Format = DXGI_FORMAT_D32_FLOAT;
	descDepth.SampleDesc.Count = 1;
	descDepth.SampleDesc.Quality = 0;
	descDepth.Usage = D3D11_USAGE_DEFAULT;
	descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	descDepth.CPUAccessFlags = 0;
	descDepth.MiscFlags = 0;
	m_pDevice->CreateTexture2D(&descDepth, NULL, &m_pBuckBuffer_DSTex);
	//そのテクスチャーに対しデプスステンシルビュー(DSV)を作成
	m_pDevice->CreateDepthStencilView(m_pBuckBuffer_DSTex, NULL, &m_pBuckBuffer_DSTexDSV);

	//レンダーターゲットビューとデプスステンシルビューをパイプラインにセット
	m_pDeviceContext->OMSetRenderTargets(1, &m_pBackBuffer_TexRTV, m_pBuckBuffer_DSTexDSV);
	
	


	//深度ステンシルステートを作成
	D3D11_DEPTH_STENCIL_DESC dc;
	ZeroMemory(&dc, sizeof(dc));
	dc.DepthEnable = true;
	dc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	dc.DepthFunc = D3D11_COMPARISON_LESS;
	dc.StencilEnable = false;
	if (FAILED(m_pDevice->CreateDepthStencilState(&dc, &m_pBuckBuffer_DSTexState)))
	{
		return E_FAIL;
	}
	//深度ステンシルステートを適用
	m_pDeviceContext->OMSetDepthStencilState(m_pBuckBuffer_DSTexState, 0);
	//ビューポートの設定

	vp.Width = WINDOW_WIDTH;
	vp.Height = WINDOW_HEIGHT;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	m_pDeviceContext->RSSetViewports(1, &vp);

	//ラスタライズ設定
	D3D11_RASTERIZER_DESC rdc;
	ZeroMemory(&rdc, sizeof(rdc));
	rdc.CullMode = D3D11_CULL_NONE;
	rdc.FillMode = D3D11_FILL_WIREFRAME;
	rdc.FrontCounterClockwise = TRUE;

	m_pDevice->CreateRasterizerState(&rdc, &m_pRasterizerState);
	m_pDeviceContext->RSSetState(m_pRasterizerState);

	//アルファブレンド用ブレンドステート作成
	D3D11_BLEND_DESC bd;
	ZeroMemory(&bd, sizeof(D3D11_BLEND_DESC));
	bd.IndependentBlendEnable = false;
	bd.AlphaToCoverageEnable = false;
	bd.RenderTarget[0].BlendEnable = true;
	bd.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	bd.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	bd.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	bd.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	bd.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	bd.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	bd.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	if (FAILED(m_pDevice->CreateBlendState(&bd, &m_pBlendState)))
	{
		return E_FAIL;
	}

	UINT mask = 0xffffffff;
	m_pDeviceContext->OMSetBlendState(m_pBlendState, NULL, mask);

	// 定数バッファの設定
	D3D11_BUFFER_DESC cb;
	cb.ByteWidth = sizeof(CONSTANT_BUFFER);
	cb.Usage = D3D11_USAGE_DYNAMIC;
	cb.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cb.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cb.MiscFlags = 0;
	cb.StructureByteStride = 0;
	m_pDevice->CreateBuffer(&cb, NULL, &pConstantBuffer);
	return S_OK;
}
void DIRECT3D11::Change()
{
	
	m_pDeviceContext->OMSetRenderTargets(1, &m_pBackBuffer_TexRTV, m_pBuckBuffer_DSTexDSV);
}
void DIRECT3D11::Clear()
{
	//画面クリア（実際は単色で画面を塗りつぶす処理）
	float ClearColor[4] = { 0,0,0,1 };// クリア色作成　RGBAの順

	m_pDeviceContext->ClearRenderTargetView(m_pBackBuffer_TexRTV, ClearColor);//画面クリア


	m_pDeviceContext->ClearDepthStencilView(m_pBuckBuffer_DSTexDSV, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);//深度バッファクリア
}

HRESULT DIRECT3D11::Present()
{
	m_pSwapChain->Present(0, 0);//画面更新（バックバッファをフロントバッファに）
	return S_OK;
}

void DIRECT3D11::ShowFPS()
{
	static INT Frames = 0, FPS = 0;
	static LARGE_INTEGER Frq = { 0 }, PreviousTime = { 0 }, CurrentTime = { 0 };
	DOUBLE Time = 0;
	char sz[11] = { 0 };

	QueryPerformanceFrequency(&Frq);

	QueryPerformanceCounter(&CurrentTime);
	Time = CurrentTime.QuadPart - PreviousTime.QuadPart;
	Time *= (DOUBLE)1100.0 / (DOUBLE)Frq.QuadPart;
	if (Time >= 1100.000)
	{
		PreviousTime = CurrentTime;
		FPS = Frames;
		Frames = 0;
	}

	Frames++;
}
