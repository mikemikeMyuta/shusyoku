#include "Camera.h"
#include "../imgui/CImguiMine.h"

XMMATRIX Camera::m_mView;
XMMATRIX Camera::m_mProj;

//カメラの位置を修正

Camera::Camera()
{
	ZeroMemory(this, sizeof(Camera));
}

Camera::~Camera()
{

}

void Camera::SetCameraPosition(float x, float y, float z)
{
	m_f, x = x;
	m_f.y = y;
	m_f.z = z;
	m_vPos = XMLoadFloat3(&m_f);
	SetViewProj();
}

void Camera::SetCameraPositionGaze(float x, float y, float z, float gx, float gy, float gz)
{
	m_f.x = x;
	m_f.y = y;
	m_f.z = z;
	m_vPos = XMLoadFloat3(&m_f);
	m_fG.x = gx;
	m_fG.y = gy;
	m_fG.z = gz;

	m_boGaze = true;
	SetViewProj();
}

void Camera::move()
{

	//エラー　カメラ11/14 　（11/15追記)カメラの挙動がおかしいかもな
	//fixed(2/18):これ注視点があるからです 
	

	//カメラの位置
	
	//横向き
	if (GetKeyboardPress(DIK_LEFTARROW))
	{
		m_f.x-= 0.1f;
	}
	else if (GetKeyboardPress(DIK_RIGHTARROW))
	{
		m_f.x+= 0.1f;

	}

	//縦向き
	if (GetKeyboardPress(DIK_UPARROW))
	{
		m_f.y+= 0.1f;
	}
	else if (GetKeyboardPress(DIK_DOWNARROW))
	{
		m_f.y-= 0.1f;
	}


	if (GetKeyboardPress(DIK_RSHIFT))
	{
		m_f.z+= 0.1f;
	}
	else if (GetKeyboardPress(DIK_RCONTROL))
	{
		m_f.z-= 0.1f;

	}

	//注視点
	if (GetKeyboardPress(DIK_S))
	{
		m_fG.y-=0.1f;
	}
	else if (GetKeyboardPress(DIK_W))
	{
		m_fG.y+= 0.1f;
	}
	
	if (GetKeyboardPress(DIK_A))
	{
		m_fG.x-= 0.1f;
	}
	else if (GetKeyboardPress(DIK_D))
	{
		m_fG.x+= 0.1f;
	}

	if (GetKeyboardPress(DIK_Q))
	{
		m_fG.z -= 0.1f;
	}
	else if (GetKeyboardPress(DIK_Z))
	{
		m_fG.z += 0.1f;
	}


	//番号でキャラに対する場所に行く
	if (GetKeyboardPress(DIK_1))//初期位置
	{
		m_f = XMFLOAT3(0.0f, 10, -20);
		m_fG = XMFLOAT3(0.0f, 10, 0);
	}
	else if (GetKeyboardPress(DIK_2))//一番左キャラ
	{
		m_f = XMFLOAT3(-20.0f, 10, -10);
		m_fG = XMFLOAT3(-20.0f, 10, 0);
	}
	else if (GetKeyboardPress(DIK_3))//左から二番目
	{
		m_f = XMFLOAT3(-5.0f, 10, -10);
		m_fG = XMFLOAT3(-5.0f, 10,0);
	}
	else if (GetKeyboardPress(DIK_4))//左から三番目
	{
		m_f = XMFLOAT3(6.0f, 10, -10);
		m_fG = XMFLOAT3(6.0f, 10, 0);
	}
	else if (GetKeyboardPress(DIK_5))//一番右
	{
		m_f = XMFLOAT3(20.0f, 10, -10);
		m_fG = XMFLOAT3(20.0f, 10, 0);
	}


	m_boGaze = true;
	IMGUIDrawdata::get_instance()->setCameraPos(m_f);
	IMGUIDrawdata::get_instance()->setCameraGaze(m_fG);
	SetViewProj();
}

HRESULT Camera::SetViewProj()
{
	// ビュー
	if (!m_boGaze)
	{
		XMVECTOR vEyePt = XMVectorSet(m_f.x, m_f.y, m_f.z, 1.0f);//カメラ（視点）位置
		XMVECTOR vLookatPt = XMVectorSet(m_f.x, m_f.y, m_f.z + 1, 1.0f);//注視位置
		XMVECTOR vUpVec = XMVectorSet(0.0f, 1.0f, 0.0f, 1.0f);//上方位置
		m_mView = XMMatrixLookAtLH(vEyePt, vLookatPt, vUpVec);

		XMMATRIX keeper[3];//回転用
		keeper[0] = XMMatrixRotationX(XMConvertToRadians(rad.x));
		keeper[1] = XMMatrixRotationY(XMConvertToRadians(rad.y));
		keeper[2] = XMMatrixRotationZ(XMConvertToRadians(rad.z));

		m_mView = keeper[0] * keeper[1] * keeper[2] * m_mView;
	}
	else
	{
		XMVECTOR vEyePt = XMVectorSet(m_f.x, m_f.y, m_f.z, 1.0f);//カメラ（視点）位置
		XMVECTOR vLookatPt = XMVectorSet(m_fG.x, m_fG.y, m_fG.z, 1.0f);//注視位置
		XMVECTOR vUpVec = XMVectorSet(0.0f, 1.0f, 0.0f, 1.0f);//上方位置
		m_mView = XMMatrixLookAtLH(vEyePt, vLookatPt, vUpVec);

		XMMATRIX keeper[3];//回転用
		keeper[0] = XMMatrixRotationX(XMConvertToRadians(rad.x));
		keeper[1] = XMMatrixRotationY(XMConvertToRadians(rad.y));
		keeper[2] = XMMatrixRotationZ(XMConvertToRadians(rad.z));

		m_mView = keeper[0] * keeper[1] * keeper[2]* m_mView;
	}
	//m_boGaze = false;
	// プロジェクション

	m_mProj = XMMatrixPerspectiveFovLH(XM_PI / 2, (FLOAT)m_dwWindowWidth / (FLOAT)m_dwWindowHeight, 0.1f, 100000.0f);
	//rad = XMFLOAT3(0, 0, 0); //11/15あとで解除する
	return S_OK;
}

HRESULT Camera::Init(DWORD dwWidth, DWORD dwHeight)
{
	m_dwWindowWidth = dwWidth;
	m_dwWindowHeight = dwHeight;
	return S_OK;
}

