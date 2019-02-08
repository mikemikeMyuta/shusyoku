#pragma once

#include "../BASE/BASE.h"
#include "../WINDOW/CreateWindow.h"
#include "../charcter/character.h"

#include <deque>
#include <string>
//#include <xnamath.h>
#include <Windows.h>



struct D3D_INIT
{
	HWND hWnd;
};



struct ReturnClientRect
{
	int width;
	int height;
};
class DIRECT3D11 //設定するクラス
{
public:
	
	//Data
	HWND m_hWnd;
	ID3D11Device* m_pDevice;
	ID3D11DeviceContext* m_pDeviceContext;
	IDXGISwapChain* m_pSwapChain;
	ID3D11RenderTargetView* m_pBackBuffer_TexRTV;
	ID3D11RenderTargetView* m_pDrawingLikeAnime;
	ID3D11DepthStencilView* m_pBuckBuffer_DSTexDSV;
	ID3D11Texture2D* m_pBuckBuffer_DSTex;
	ID3D11DepthStencilState* m_pBuckBuffer_DSTexState;
	ID3D11BlendState* m_pBlendState;

	ID3D11VertexShader *pVertexShader;
	ID3D11PixelShader *pPixelShader;
	ID3D11Buffer *pConstantBuffer;
	ID3D11InputLayout *pVertexLayout;

	ID3D11RasterizerState *m_pRasterizerState;

	ID3D11Buffer *VerBuffer;
	ID3D11Buffer *IndBuffer;
	D3D11_VIEWPORT vp;

	ReturnClientRect CRdata;

	//Method
	DIRECT3D11();
	~DIRECT3D11();

	HRESULT Init(D3D_INIT*); 
	void ShowFPS(); 
	void Clear();//画面クリア用メソッド
	HRESULT Present();//貼り付け
	
private: 
	
};

