#pragma once
#include "../WINDOW/CreateWindow.h"
#include "../CAMERA/Camera.h"
#include "../Configuration/ConfigurationDirectX11.h"
#include "../BASE/BASE.h"
#include "../charcter/character.h"
#include "../DirectXTex/DirectXTex.h"
#include <d3dcompiler.h>
#include "../PMX/motionList.h"
#include "../imgui/imgui_impl_dx11.h"


#define TEX_SHADER_PASS (L"shader/texShader.hlsl")
#define TEX_PASS (L"asset/TEX/ssr3040027000_02.png")

//頂点の構造体
class VECTOR3
{
public:
	float x, y, z;
	VECTOR3(float a, float b, float c)
	{
		x = a;y = b;z = c;
	}
};
struct VECTOR2
{
public:
	float u, v;
	VECTOR2(float a, float b)
	{
		u = a;v = b;
	}

};
struct SimpleVertex
{
	VECTOR3 Pos;  //位置
	VECTOR2 Uv; //UV座標
};

enum SCENE
{
	TITLE,
	/*PLAY_STAGE1,
	PLAY_STAGE2,
	PLAY_STAGE3,
	WIN,*/
	GAMEMAIN,
	GAMEOVER,
};

class Director
{
public:
	//Data
	HINSTANCE m_hInstance;
	Window* m_pWindow;
	HWND m_hWnd;
	SCENE m_Scene;
	DIRECT3D11* m_pD3d;
	Camera* m_pCamera;
	Base* m_pbase;
	/*SOUND* m_psound;*/
	//制作次第追記

	//pmx

	CMainchar *FreChan;
	CMainchar *FreChanShadow;
	
	CMainchar *MayuSuki;
	CMainchar *MayuShadow;

	CMainchar *Kanade;
	CMainchar *KanadeShadow;

	CPmx* stage;

	//VMD
	VmdMotionController *dance;


	//Method
	Director();
	~Director();
	HRESULT Init();
	void Run(HINSTANCE);
	void TitleInitShader();
	void Draw();

	void RenderRoom();

	void MainLoop();
	void FixFPS60();
	void Update();

	ID3D11ShaderResourceView* Texture;
	ID3D11ShaderResourceView* TextureTwo;
	ID3D11SamplerState* SampleLinear;
	ID3D11Buffer* buffer;
	ID3D11VertexShader *VertexShader;
	ID3D11PixelShader *PixelShader;
	ID3D11InputLayout *VertexLayout;
	void titleRender();
};
