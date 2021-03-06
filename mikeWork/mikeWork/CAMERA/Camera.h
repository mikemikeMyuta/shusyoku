#pragma once
#include "../WINDOW/CreateWindow.h"
#include "../BASE/BASE.h"
#include "../Configuration/ConfigurationDirectX11.h"




class Camera
{
public:
	//Data
	DIRECT3D11* m_pD3D;

	XMFLOAT3 m_f;//位置

	XMFLOAT3 m_fG;//注視
	
	DWORD m_dwWindowWidth;
	DWORD m_dwWindowHeight;
	bool m_boGaze = true;// 注視ありモード
	XMVECTOR m_vPos;
	static XMMATRIX m_mView;
	static XMMATRIX m_mProj;
	
	void move();
	XMFLOAT3 rad=XMFLOAT3(0,0,0);

	//Method
	Camera();
	~Camera();
	void SetCameraPosition(float x, float y, float z);
	void SetCameraPositionGaze(float x, float y, float z, float gx, float gy, float gz);//注視あり

	HRESULT Init(DWORD dwWidth, DWORD dwHeight);
private:
	//Method
	HRESULT SetViewProj();

};




