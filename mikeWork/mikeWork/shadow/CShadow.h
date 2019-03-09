/*
	�쐬��2/24
*/
#pragma once

#include <d3d11.h>
//#include <DirectXMath.h>//���ꂾ���ł͉����N���Ȃ��E�E�E�Ȃ�ŁE�E�E
#include "../PMX/PmxStruct.h"//���������ƂȂ���DirectXMath�̃C���N���[�h�����܂�����
#include "../PMX/VMD.h"
#include <deque>

#define DEPTHTEX_WIDTH (2048)
#define DEPTHTEX_HEIGHT (2048)

static const XMMATRIX SHADOW_BIAS = XMMATRIX(
	0.5f, 0.0f, 0.0f, 0.0f,
	0.0f, -0.5f, 0.0f, 0.0f,
	0.0f, 0.0f, 1.0f, 0.0f,
	0.5f, 0.5f, 0.0f, 1.0f);

struct DrawingAllDataShadowMap//�`�掞�ɕK�v�ȃf�[�^
{
	PmxStruct::PMX_DATA pmxdata;//�I�u�W�F�N�g�f�[�^
	CONSTANT_BUFFER constantBuffer;//�R���X�^���X�o�b�t�@
	CONSTANT_BONE_MATRIX boneMat;
	ObjectIndividualData IndiviData;
	PmxStruct::PMX_SEND_DATA_SHADOW *sendData;
};
//�e���o�����߂Ɏg��class
//�V���h�E�e�N�X�`������邽��
//���ۂ̉e�̓L������shader�ɂȂ�Ǝv���܂��B

class CShadow final
{
public:
	CShadow();
	~CShadow();

	void CreateShadowLayout(ID3D11Device* Device);
	void SetShadowUsingPixelShader(ID3D11Device* m_pDevice, const LPCWSTR shaderName);
	void SetShadowUsingVertexShader(ID3D11Device* m_pDevice, const LPCWSTR shaderName);
	void IndexdataForPoint(ID3D11Device*);//create buffer
	void IndexdataForVertex(ID3D11Device*);//create buffer
	void Init(ID3D11Device *device, PmxStruct::PMX_DATA *pmxData, const LPCWSTR psShaderName, const LPCWSTR gsShaderName, const LPCWSTR vsShaderName, vector<Bone> *animeBone, vector<MmdStruct::PmdIkData> *pmdIkData);
	/*
		�������@�f�o�C�X
		�������@�e�Ɏg�p���������f���̃f�[�^
		��O�����@�s�N�Z���V�F�[�_�[
		��l�����@�W�I���g���V�F�[�_�[
		��܈����@�o�[�e�b�N�X�V�F�[�_�[
		��Z�����@�������Ń��f���Ɏg�p����VMD���Z�b�g�����{�[��
		�掵�����@�������Ń��f����IK
	*/

	static void Render(ID3D11DeviceContext*);
	void Draw(XMFLOAT4X4 World, XMFLOAT4X4 View, XMFLOAT4X4 Projection, CONSTANT_BONE_MATRIX* boneMat);//�o�b�t�@���Z�b�g�B	   

	static deque< DrawingAllDataShadowMap> shadowRender;
	//DO:����O���[�o���Ɂ@�f�[�^����Ă���
	PmxStruct::PMX_SEND_DATA_SHADOW *VertexBufferUpdate;//�f�[�^�������N�����邽��
	CONSTANT_BUFFER constantBuffer;
	CONSTANT_BONE_MATRIX constantBufferMatrix;
	static ID3D11RenderTargetView *DepthMap_TexRTV;
	static ID3D11DepthStencilView *DepthMap_DSTexDSV;

	//�Z�b�g�������
	void SetPmxData(PmxStruct::PMX_DATA *data) { pmxData = data; }
	void SetMotion(VmdMotionController* motion) { VmdMotionController = motion; }

	static ID3D11ShaderResourceView*  DepthMap_TexSRV;//������g��

	ObjectIndividualData *indiviData;//����g���@�����������炷��
	static ID3D11SamplerState *Smp;
private:
	static ID3D11Texture2D *DepthMap_Tex;
	static ID3D11Texture2D *DepthMap_DSTex;
	static D3D11_VIEWPORT viewport;



	PmxStruct::PMX_DATA *pmxData;
	ID3DBlob *pCompileVS, *pCompilePS;


	//���[�V�����֘A
	VmdMotionController *VmdMotionController;

	//���f���̃{�[����IK���󂯓n�����邽�߂ɂ�����
	vector<Bone> bones;
	vector<MmdStruct::PmdIkData> pmdIkData;


	void SetBone(vector<Bone>* b) {
		bones.resize(b->size());
		copy(b->begin(), b->end(), bones.begin());
	}
	void SetPmdIKdata(vector<MmdStruct::PmdIkData>* data) {
		pmdIkData.resize(data->size());
		copy(data->begin(), data->end(), pmdIkData.begin());
	}
};
