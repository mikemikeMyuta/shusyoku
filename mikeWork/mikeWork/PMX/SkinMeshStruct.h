#pragma once
///// �X�L�����b�V���A�j���[�V�����p�\���� /////

///// ���̒�` �E�E�E �{�w�b�_���ł͈ȉ��̂悤�Ƀ{�[���̍��W�n�̖��̂��`����	/////
///// ���[���h���W�n�F ��΍��W�n												/////
///// ���f�����[�J�����W�n�F���[���h���W�ϊ����܂܂Ȃ�3D���f���̃��[�J�����W�n	/////
///// �{�[�����[�J�����W�n�F���g�̐e�{�[������ɂ������[�J�����W�n			/////

#include <d3d11.h>
#include <DirectXMath.h>
#include <string>

using namespace std;
using namespace DirectX;

// �u�����h�d�ݕt���b�V���̒��_�f�[�^
struct BlendVertex {
	XMFLOAT3 position;
	float weight[3];
	unsigned short matrixIndex[4];
	XMFLOAT3 normal;
	XMFLOAT2 texture;
};

// �{�[��
struct Bone {
	int id;					// �{�[��ID�i�ʂ��ԍ��j
	string name;			// �{�[����
	unsigned char type;		// �{�[���^�C�v (MMD�̏ꍇ 0:��] 1:��]�ƈړ� 2:IK 3:�s�� 4:IK�e���� 5:��]�e���� 6:IK�ڑ��� 7:��\�� 8:�P�� 9:��]�^�� )
	Bone* parent;			// �e�{�[��
	Bone* firstChild;		// ��1�q�{�[��
	Bone* sibling;			// ���̌Z��{�[��
	XMMATRIX initMat;		// �����p���s��(�{�[�����[�J�����W�n)
	XMMATRIX boneMat;		// �{�[���s��(�{�[�����[�J�����W�n)
	XMMATRIX offsetMat;	// �{�[���I�t�Z�b�g�s��(���f�����[�J�����W�n)


	Bone() : type(), id(), parent(), firstChild(), sibling() {	// �R���X�g���N�^
		initMat = XMMatrixIdentity();
		boneMat = XMMatrixIdentity();
		offsetMat = XMMatrixIdentity();
	};
	XMMATRIX GetModelLocalBoneMat() {				// ���f�����[�J�����W�n�ł̃{�[���s����擾
		if (parent) return boneMat * parent->GetModelLocalBoneMat();
		else return boneMat;
	}
	XMFLOAT3 GetModelLocalPosition() {		// ���f�����[�J�����W�n�ł̃{�[���ʒu���擾
		XMFLOAT3 f3(0, 0, 0);
		XMVECTOR v = XMLoadFloat3(&f3);//�x�N�^�[�ɕϊ�
		v = XMVector3TransformCoord(v, GetModelLocalBoneMat());//�g�����X�t�H�[��
		XMStoreFloat3(&f3, v);//�ēx�ϊ����AFloat�ɂ���
		return f3;

	}
	~Bone() {}

	//�R�s�[�R���X�g���N�^�@�X���Ə���ɂ��Ă���銴���H
	Bone(const Bone &b)
	{
		id = b.id;
		name = b.name;
		type = b.type;
		parent = b.parent;
		firstChild = b.firstChild;
		sibling = b.sibling;
		initMat = b.initMat;
		boneMat = b.boneMat;
		offsetMat = b.offsetMat;
	}
};

// �L�[�t���[���A�j���[�V�����f�[�^
struct KeyFrame {


	string boneName;		// �{�[����

	unsigned long frameNo;	// �t���[���ԍ�
	XMFLOAT3 position;	// �ʒu
	XMFLOAT4 rotation;// ��] �ϊ����@vector�ɂ��ăN�H�[�^�j�I�����g��
	XMFLOAT2 interpolation_x[2];	// position.x�̃x�W�F�⊮�p�����[�^
	XMFLOAT2 interpolation_y[2];	// position.y�̃x�W�F�⊮�p�����[�^
	XMFLOAT2 interpolation_z[2];	// position.z�̃x�W�F�⊮�p�����[�^
	XMFLOAT2 interpolation_r[2];	// rotation�̃x�W�F�⊮�p�����[�^

	bool operator < (const KeyFrame &k) const {	// �t���[���ԍ��Ń\�[�g���邽�߂̔�r���Z�q
		return frameNo < k.frameNo;
	}

	KeyFrame() {}
	//�Ȃ�ł�����Ńr���h���ʂ�@�����ƃG���[
	//�ēx�o�O�����ɂȂ�@�R�s�[�R���X�g���N�^�ɂ��f�[�^�����@
	KeyFrame(const KeyFrame &k)
	{
		boneName = k.boneName;

		frameNo = k.frameNo;
		position = k.position;
		rotation = k.rotation;
		interpolation_x[0] = k.interpolation_x[0];
		interpolation_x[1] = k.interpolation_x[1];
		interpolation_y[0] = k.interpolation_y[0];
		interpolation_y[1] = k.interpolation_y[1];
		interpolation_z[0] = k.interpolation_z[0];
		interpolation_z[1] = k.interpolation_z[1];
		interpolation_r[0] = k.interpolation_r[0];
		interpolation_r[1] = k.interpolation_r[1];
	}
	~KeyFrame() {}
};