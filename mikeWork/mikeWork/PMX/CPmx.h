#pragma once

#include "PmxLoad.h"
#include "VMD.h"
#include "../shader/shaderList.h"
#include <map>
#include <deque>
#include <vector>
#include <d3dcompiler.h>
#include "StaticCalc.h"
#include "../shadow/CShadow.h"

//use namespace
using namespace PmxStruct;
using namespace std;

//�\����


enum DrawingType
{
	player = 0,
	object,
	shadow,
	shadowMap,
	Final //�z��Ɏg�������E�E�E
};

//enum 

//Control Draw image


//pmx class 
class CPmx :public CLoadPMX//VMD���p������ ������p�����g�p���Đ��삷��
{
public:
	static void Render(ID3D11DeviceContext*);//�`����s

	void Init(ID3D11Device*, const LPCSTR, const LPCWSTR, const LPCWSTR, const LPCWSTR, LPSTR);
	/*
	�������@�f�o�C�X
	�������@�g�p���郂�f���̖��O
	��O�����@�s�N�Z���V�F�[�_�[
	��l�����@�W�I���g���V�F�[�_�[
	��܈����@�o�[�e�b�N�X�V�F�[�_�[
	��Z����	   �e�N�X�`���̃p�X
	*/

	//11/15�@�^�񒆋��߂鏈�������邩���@�����_�I�ȈӖ��Ł@�i�ǋL�@11/27�j����H

	void ProcessingCalc(XMFLOAT3);//�`��̌v�Z�����ƃR���X�^���X�o�b�t�@�쐬�@���̂܂�Draw���Ăяo���B DrawingType mean selecting mainchar or other 

	static map<LPCSTR, PMX_DATA> RetrievalData;//add����Ƃ��ɕK�v�Ȃ烍�[�h�K�v�Ȃ��Ȃ炻�̃f�[�^���󂯓n�����邽�߂Ɍ������邽�߂̃f�[�^
	static deque<DrawingAllDataObject> RenderingDataObject;//�`�悷��Ƃ��Ɏg�p
	//or
	static deque<DrawingAllDataMainCharcter> RenderingDataMain;//�`�悷��Ƃ��Ɏg�p
											   //VMD�����銴���ł������`�@����܂�
	static deque<DrawingAllDataShadow> RenderingDataShadow;//�`�悷��Ƃ��Ɏg�p

	ObjectIndividualData *IndiviData;//�Ǝ��Ńf�[�^����������

	vector<ID3D11ShaderResourceView*> TexData;//�e�N�X�`���̃f�[�^���i�[

	CShadow Cshadow;
	UINT MorphNum;//���[�t�i���o�[�i�[
	UINT WorldMorphNum;//���[�t�i���o�[�i�[

	PMX_SEND_DATA *VertexBufferUpdate;

	//draw��ς��� 11/9�@clear (11/9)

	CONSTANT_BUFFER_OBJECT cbo;
	CONSTANT_BUFFER_MAINCHARCTER cbm;
	CONSTANT_BONE_MATRIX cbmr;
	CONSTANT_BUFFER cb;
	//11/9�Fdraw��̏�����Render�� clear ()

	//create input layout
	void Cretaelayout(ID3D11Device* Device);


	void setPosition(XMFLOAT3 pos) { Position = pos; }
	void setRotetation(XMFLOAT3 rot) { Rotetation = rot; }
	XMFLOAT3 getPosition() { return Position; }
	XMFLOAT3 getRotetation() { return Rotetation; }
	ID3D11ShaderResourceView* getTexResource(UINT indexNumber) { return TexData[indexNumber]; }//�i�[�����e�N�X�`���󂯓n���B
	//�g�p���z	ID3D11ShaderResourceView* Textures =getTexResource(getIndexNum);Textures���g�p����

	void SetMotion(VmdMotionController* motion) { m_VmdMotionController = motion; }

	vector<Bone>* DeliverBones() { return &bones; }
	vector<MmdStruct::PmdIkData>* DeliverPmdIkData() { return &pmdIkData; }
	void SetBone(vector<Bone>* b) {
		bones.resize(b->size());
		copy(b->begin(), b->end(), bones.begin());
	}
	void SetPmdIKdata(vector<MmdStruct::PmdIkData>* data) {
		pmdIkData.resize(data->size());
		copy(data->begin(), data->end(), pmdIkData.begin());
	}

protected:

	void switchColling(ID3D11Device*);//shadow�𔻒f���Ă��܂��B

	//Object status
	//you use subclass. so, you dont use this class
	XMFLOAT3 Position;
	XMFLOAT3 Rotetation;
	

	static ID3D11ShaderResourceView* m_ClearTex;//�^�����ȃe�N�X�`���p
	static ID3D11ShaderResourceView* m_ClearTex2;//�����ȃe�N�X�`���p


	//���[�V�����֘A
	VmdMotionController *m_VmdMotionController;


	//���f���̃{�[����IK���󂯓n�����邽�߂ɂ�����
	vector<Bone> bones;
	vector<MmdStruct::PmdIkData> pmdIkData;


private:
	ID3DBlob *pCompilePS;
	ID3DBlob *pCompileVS;
	DrawingType type;
	XMMATRIX workmat;//��ƂɎg���s��


	PMX_VERTEX_USE_FLOAT4 *ambient;
	PMX_VERTEX_USE_FLOAT4 *normal;
	PMX_VERTEX_USE_FLOAT4 *diffuse;
	PMX_VERTEX_USE_FLOAT4 specular;

	PMX_VERTEX_USE_FLOAT2* UV;


	void PmxLoad(const LPCSTR);//�f�[�^�����[�h����̂��ꊇ�ɂ���
	void TexLoad(ID3D11Device*, LPSTR);//���[�h���ꂽ�f�[�^��p���ăe�N�X�`���̓ǂݍ��݂��s��
	void Draw(const PMX_DATA, const  CONSTANT_BUFFER_OBJECT, const ObjectIndividualData*);//prepare drawing
	void Draw(const PMX_DATA, const  CONSTANT_BUFFER_MAINCHARCTER, const CONSTANT_BONE_MATRIX, const ObjectIndividualData*);//prepare drawing
	void Draw(const PMX_DATA, const  CONSTANT_BUFFER, const ObjectIndividualData*);//prepare drawing
		//create buffer
	void IndexdataForPoint(ID3D11Device* pDevice);//fbx����C���f�b�N�X�ɕϊ�����
	void IndexdataForVertex(ID3D11Device* pDevice);// �C���f�b�N�X�f�[�^�̎��o���ƃo�b�t�@�̐ݒ�

	//create shader
	void SetPmxUsingPixelShader(ID3D11Device* m_pDevice, const LPCWSTR shaderName);
	void SetPmxUsingGeometryShader(ID3D11Device* m_pDevice, const LPCWSTR shaderName);
	void SetPmxUsingVertexShader(ID3D11Device* m_pDevice, const  LPCWSTR shaderName);



	
	vector<XMMATRIX> updateVerBuf(XMMATRIX*);

	//CreateBone
	void CreateBone();
	//�{�[���̍쐬����i�[�܂� 
	//CreateIKData
	void CreateIkData();


public:
	
	//�R���X�g���N�^�A�f�X�g���N�^
	CPmx()
	{
		pCompilePS = NULL;
		pCompileVS = NULL;
		IndiviData = new ObjectIndividualData();
		type = DrawingType::object;
		workmat = XMMatrixIdentity();
		Position = XMFLOAT3(0, 0, 0);
		Rotetation = XMFLOAT3(0, 1, 0);
		MorphNum = 0;
	}

	CPmx(DrawingType inputType)
	{
		pCompilePS = NULL;
		pCompileVS = NULL;
		IndiviData = new ObjectIndividualData();
		type = inputType;
		workmat = XMMatrixIdentity();
		Position = XMFLOAT3(0, 0, 0);
		Rotetation = XMFLOAT3(0, 0, 0);
		MorphNum = 0;
	}

	CPmx(DrawingType inputType, UINT inputMorphNum)
	{
		pCompilePS = NULL;
		pCompileVS = NULL;
		IndiviData = new ObjectIndividualData();
		type = inputType;
		workmat = XMMatrixIdentity();
		Position = XMFLOAT3(0, 0, 0);
		Rotetation = XMFLOAT3(0, 0, 0);
		MorphNum = inputMorphNum;
	}

	virtual ~CPmx() {
		delete IndiviData;
		TexData.clear();
		delete &TexData;
		RetrievalData.clear();
		delete &RetrievalData;
		RenderingDataObject.clear();
		delete &RenderingDataObject;
		RenderingDataMain.clear();
		delete &RenderingDataMain;
		RenderingDataShadow.clear();
		delete &RenderingDataMain;
		delete VertexBufferUpdate;
	}

};
