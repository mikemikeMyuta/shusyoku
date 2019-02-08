#pragma once
#include "../../BASE/BASE.h"

// ���ʂ̕�����
struct	PLANE {
	float	a, b, c, d;			// ���ʕ������̌W���i ax+by+cz+d=0 )
};

	// �v���g�^�C�v�錾
	void CreatePlaneInfo(const XMVECTOR& p1, const XMVECTOR& p2, const XMVECTOR& p3, PLANE& plane);

	bool LinetoPlaneCross(const PLANE& plane,			//���ʂ̕�����
		const XMVECTOR& line_o,	//�������ʂ�_
		const XMVECTOR& line_v,	//�����̕����x�N�g��
		XMVECTOR& ans);			//��_���W

	bool CheckInTriangle(const XMVECTOR& a,
		const XMVECTOR& b,
		const XMVECTOR& c,
		const XMVECTOR& p);

	float LengthPointToPlane(const XMVECTOR& p, const PLANE& plane, XMVECTOR& crosspoint);
