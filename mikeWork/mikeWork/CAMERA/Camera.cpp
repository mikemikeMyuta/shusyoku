#include "Camera.h"

XMMATRIX Camera::m_mView;
XMMATRIX Camera::m_mProj;

//�J�����̈ʒu���C��

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

	////�G���[�@�J����11/14 �@�i11/15�ǋL)�J�����̋�������������������
	//if (GetKeyboardPress(DIK_LEFTARROW))
	//{
	//	rad.y--;
	//}
	//else if (GetKeyboardPress(DIK_RIGHTARROW))
	//{
	//	rad.y++;

	//}
	//if (GetKeyboardPress(DIK_UPARROW))
	//{
	//	rad.x++;
	//}
	//else if (GetKeyboardPress(DIK_DOWNARROW))
	//{
	//	rad.x--;
	//}

	//rad.y++;


	m_fG = XMFLOAT3(0, 10, 0);
	m_boGaze = true;
	SetViewProj();
}

HRESULT Camera::SetViewProj()
{
	// �r���[
	if (!m_boGaze)
	{
		XMVECTOR vEyePt = XMVectorSet(m_f.x, m_f.y, m_f.z, 1.0f);//�J�����i���_�j�ʒu
		XMVECTOR vLookatPt = XMVectorSet(m_f.x, m_f.y, m_f.z + 1, 1.0f);//�����ʒu
		XMVECTOR vUpVec = XMVectorSet(0.0f, 1.0f, 0.0f, 1.0f);//����ʒu
		m_mView = XMMatrixLookAtLH(vEyePt, vLookatPt, vUpVec);

		XMMATRIX keeper[3];//��]�p
		keeper[0] = XMMatrixRotationX(XMConvertToRadians(rad.x));
		keeper[1] = XMMatrixRotationY(XMConvertToRadians(rad.y));
		keeper[2] = XMMatrixRotationZ(XMConvertToRadians(rad.z));

		m_mView = keeper[0] * keeper[1] * keeper[2] * m_mView;
	}
	else
	{
		XMVECTOR vEyePt = XMVectorSet(m_f.x, m_f.y, m_f.z, 1.0f);//�J�����i���_�j�ʒu
		XMVECTOR vLookatPt = XMVectorSet(m_fG.x, m_fG.y, m_fG.z, 1.0f);//�����ʒu
		XMVECTOR vUpVec = XMVectorSet(0.0f, 1.0f, 0.0f, 1.0f);//����ʒu
		m_mView = XMMatrixLookAtLH(vEyePt, vLookatPt, vUpVec);

		XMMATRIX keeper[3];//��]�p
		keeper[0] = XMMatrixRotationX(XMConvertToRadians(rad.x));
		keeper[1] = XMMatrixRotationY(XMConvertToRadians(rad.y));
		keeper[2] = XMMatrixRotationZ(XMConvertToRadians(rad.z));

		m_mView = keeper[0] * keeper[1] * keeper[2] * m_mView;
	}
	//m_boGaze = false;
	// �v���W�F�N�V����

	m_mProj = XMMatrixPerspectiveFovLH(XM_PI / 2, (FLOAT)m_dwWindowWidth / (FLOAT)m_dwWindowHeight, 0.1f, 100000.0f);
	//rad = XMFLOAT3(0, 0, 0); //11/15���Ƃŉ�������
	return S_OK;
}

HRESULT Camera::Init(DWORD dwWidth, DWORD dwHeight)
{
	m_dwWindowWidth = dwWidth;
	m_dwWindowHeight = dwHeight;
	return S_OK;
}

