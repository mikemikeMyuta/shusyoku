#pragma once

#include <stdio.h>
#include <windows.h>
#include <d3d11.h>
#include <d3dCompiler.h>
#include <DirectXMath.h>
#include "../DirectXTex/DirectXTex.h"
#include "../GameMain/GameUse/input.h"


#pragma comment(lib,"d3d11.lib")
#pragma comment(lib,"d3dCompiler.lib")
#pragma comment(lib,"winmm.lib")

#ifdef _DEBUG
#pragma comment(lib,"DirectXTex/DirectXTexD.lib")
#else
#pragma comment(lib,"DirectXTex/DirectXTex.lib")
#endif

#pragma warning(disable : 4305)
#pragma warning(disable : 4996)
#pragma warning(disable : 4018)
#pragma warning(disable : 4111)
#pragma warning(disable : 4005)
#pragma warning(disable : 4244)
#pragma warning(disable : 4838)


#define WINDOW_WIDTH 1920
#define WINDOW_HEIGHT 1080
#define SAFE_DELETE(p) { if(p) { delete (p); (p)=NULL; } }
#define SAFE_DELETE_ARRAY(p) { if(p) { delete[] (p); (p)=NULL; } }
#define SAFE_RELEASE(p) { if(p) { (p)->Release(); (p)=NULL; } }
#define MFAIL(code,string) if(FAILED( code ) ) { MessageBox(0,string,"error",MB_OK); return E_FAIL; }
#define MFALSE(code,string) if(!( code ) ) { MessageBox(0,string,"error",MB_OK); return E_FAIL; }
#define MSG(t) MessageBox(0,t,0,MB_OK);


#define SOUND_DIRECTORY ("../asset/SOUND")//音楽のデータの場所
#define MODEL_DIRECTORY ("../asset/MODEL")//Xファイルとそれに用いるデータの場所
#define TEX_DIRECTORY ("../asset/TEX")//テクスチャーデータの場所
#define SHADER_DIRECTORY ("../shader")//シェーダーデータの場所

using namespace std;
using namespace DirectX;

//プロトタイプ
class Base
{
private:
	void SetRootDirectory();
	void SetTexDirectory();
	void SetModelDirectory();
	void SetShaderDirectory();
	void SetSoundDirectory();
public:
	void InitDirectory(char* root);
	char szRootPath[1124];
	Base();
	~Base(){}
};