#include "collision.h"

//*****************************************************************************
//!	@file	collision.cpp
//!	@brief	
//!	@note	�����蔻��Ɏg�p���邽�߂̊֐��Q
//!	@author	
//*****************************************************************************

//-----------------------------------------------------------------------------
//	Include header files.
//-----------------------------------------------------------------------------
#include	<math.h>
#include	"collision.h"

//==============================================================================
//!	@fn		CreatePlaneInfo
//!	@brief�@���ʂ̕����������߂�i���_�͎��v���œn���j
//!	@param	���_�P���W
//!	@param	���_�Q���W
//!	@param	���_�R���W
//!	@param	���ʂ̕�����
//!	@retval	�Ȃ�
//==============================================================================
void CreatePlaneInfo(const XMVECTOR& p1,
	const XMVECTOR& p2,
	const XMVECTOR& p3,
	PLANE& plane)
{
	XMVECTOR		p1p2;
	XMVECTOR		p2p3;
	XMVECTOR		normal;

	p1p2 = p2 - p1;
	p2p3 = p3 - p2;

	normal = XMVector3Cross(p1p2, p2p3);
	normal = XMVector3Normalize(normal);
	plane.a = normal.m128_f32[0];
	plane.b = normal.m128_f32[1];
	plane.c = normal.m128_f32[2];

	plane.d = -(plane.a * p1.m128_f32[0] + plane.b * p1.m128_f32[1] + plane.c * p1.m128_f32[2]);
}

//==============================================================================
//!	@fn		LinetoPlaneCross
//!	@brief�@�����ƕ��ʂ̌�_�����߂�
//!	@param	���ʂ̕�����
//!	@param	�����̋N�_
//!	@param	�����̕����x�N�g��
//!	@param	��_���W
//!	@retval	true ��_����@false ��_�Ȃ�
//==============================================================================
bool LinetoPlaneCross(const PLANE&		plane,		//���ʂ̕�����
	const XMVECTOR&	line_o,						//�����̋N�_
	const XMVECTOR&	line_v,						//�����̕����x�N�g��
	XMVECTOR&			ans)					//��_���W
{
	float Parametric;//�}��ϐ�
	float CheckCross = plane.a*line_v.m128_f32[0] + plane.b*line_v.m128_f32[1] + plane.c*line_v.m128_f32[2];//��_�̌v�Z�@�x�N�g���~���ʂ�abc�����ׂđ����B
	if (fabs(CheckCross) > 0)//�v�Z���ʂ̐�Βl�����傫���@�܂�A��_������
	{
		Parametric = -(plane.d + (plane.a*line_o.m128_f32[0] + plane.b*line_o.m128_f32[1] + plane.c*line_o.m128_f32[2])) / CheckCross;
		ans = line_o + Parametric*line_v;
		return true;
	}
	return false;
	// ����������������
}

//==============================================================================
//!	@fn		CheckInTriangle
//!	@brief�@�R�p�`�̓����ɂ��邩�ǂ����𔻒�
//!	@param	�R�p�`�̑�P���_���W
//!	@param	�R�p�`�̑�Q���_���W
//!	@param	�R�p�`�̑�R���_���W
//!	@param	���肵�������_���W
//!	@retval	true �����@false �O��
//==============================================================================
bool CheckInTriangle(const XMVECTOR&	a,
	const XMVECTOR&	b,
	const XMVECTOR&	c,
	const XMVECTOR&	p) {

	XMVECTOR  ab, bc, ca;			// �R�ӂ̃x�N�g��
	XMVECTOR  ap, bp, cp;			// �����̓_�Ƃ̃x�N�g��
	XMVECTOR	 normal;			// �R�p�`�̖@���x�N�g��
	XMVECTOR	 n1, n2, n3;			// �R�ӂƓ����̓_�Ƃ̖@���x�N�g��

	ab = b - a;
	bc = c - b;
	ca = a - c;

	ap = p - a;
	bp = p - b;
	cp = p - c;

	//D3DXVec3Cross(&normal, &ab, &bc);		// �O�ς��v�Z
	normal = XMVector3Cross(ab, bc);

	//D3DXVec3Cross(&n1, &ab, &ap);			// �O�ς��v�Z
	//D3DXVec3Cross(&n2, &bc, &bp);			// �O�ς��v�Z
	//D3DXVec3Cross(&n3, &ca, &cp);			// �O�ς��v�Z
	n1 = XMVector3Cross(ab, ap);
	n2 = XMVector3Cross(bc, bp);
	n3 = XMVector3Cross(ca, cp);

	//if (D3DXVec3Dot(&n1, &normal) < 0) return false;			// �ׂ��p�x���݊p
	//if (D3DXVec3Dot(&n2, &normal) < 0) return false;			// �ׂ��p�x���݊p
	//if (D3DXVec3Dot(&n3, &normal) < 0) return false;			// �ׂ��p�x���݊p

	float workDot = n1.m128_f32[0] * normal.m128_f32[0] + n1.m128_f32[1] * normal.m128_f32[1] + n1.m128_f32[2] * normal.m128_f32[2];//���ς̌v�Z
	if (workDot < 0) return false;			// �ׂ��p�x���݊p

	workDot = n2.m128_f32[0] * normal.m128_f32[0] + n2.m128_f32[1] * normal.m128_f32[1] + n2.m128_f32[2] * normal.m128_f32[2];//���ς̌v�Z
	if (workDot < 0) return false;			// �ׂ��p�x���݊p

	workDot = n3.m128_f32[0] * normal.m128_f32[0] + n3.m128_f32[1] * normal.m128_f32[1] + n3.m128_f32[2] * normal.m128_f32[2];//���ς̌v�Z
	if (workDot < 0) return false;			// �ׂ��p�x���݊p

	return true;
}

//==============================================================================
//!	@fn		LengthPointToPlane
//!	@brief�@���ʂƓ_�̋��������߂�	�܂������Ɍ�_�̍��W�����߂�
//!	@param	�_�̍��W
//!	@param	���ʂ̕�����
//!	@param	��_�̍��W
//!	@retval	�_�ƕ��ʂ̋���
//==============================================================================
float LengthPointToPlane(const XMVECTOR& p,
	const PLANE& plane,
	XMVECTOR& crosspoint)
{
	float dot_work = plane.a*p.m128_f32[0] + plane.b*p.m128_f32[1] + plane.c*p.m128_f32[2] + plane.d;
	float normal_len = plane.a*plane.a + plane.b*plane.b + plane.c*plane.c;

	// ��_�����߂�
	float t = -1.0f * dot_work / normal_len;

	crosspoint.m128_f32[0] = p.m128_f32[0] + plane.a * t;
	crosspoint.m128_f32[1] = p.m128_f32[1] + plane.b * t;
	crosspoint.m128_f32[2] = p.m128_f32[2] + plane.c * t;

	float length = fabsf(dot_work); // �� sqrtf(normal_len);	// ���ʂ̖@���𐳋K�����Ă���̂�sqrt�͏Ȃ��܂���

	return length;
}
//******************************************************************************
//	End of file.
//******************************************************************************
