#pragma once

#include "PmxLoad.h"
#include "VMD.h"
#include "../shader/shaderList.h"
#include <map>
#include <deque>
#include <vector>
#include <d3dcompiler.h>
#include "StaticCalc.h"

//use namespace
using namespace PmxStruct;
using namespace std;

//�\����


enum DrawingType
{
	player = 0,
	object,
	shadow,
	Final //�z��Ɏg�������E�E�E
};

class ObjectIndividualData//�ʂŐݒ肷��f�[�^����
{
public:
	// �i�[����K�v�̂���f�[�^
	ID3D11Buffer* pVerBuffer;
	ID3D11Buffer* pIndBuffer;
	ID3D11InputLayout *pVertexLayout;
	ID3D11VertexShader *pVertexShader;
	ID3D11PixelShader *pPixelShader;
	ID3D11RasterizerState *pRasterizerState;
	ID3D11Buffer* Buffer;
	ID3D11Buffer* BoneBuffer;

	//11/9�@context�̎Q�Ƃ��l���� �@Clear(11/12) �����ɓ��ꂸ�A��ŊǗ����܂��傤
	ObjectIndividualData()
	{
	}
	~ObjectIndividualData() { }

	void ReflectionData(ID3D11DeviceContext*, ID3D11Buffer*);//context�Ɋi�[
	void ReflectionData(ID3D11DeviceContext*, ID3D11Buffer*, ID3D11Buffer*);//context�Ɋi�[
};

//Constant Buffer type
struct CONSTANT_BUFFER_OBJECT //�R���X�^���X�o�b�t�@ object
{
#pragma pack(push,1)	//�A���C�������g������I�t
	XMFLOAT4X4 World;
	XMFLOAT4X4 View;
	XMFLOAT4X4 Projection;
	XMFLOAT4 ambient;
	XMFLOAT4 diffuse;
	XMFLOAT4 specular;
	XMFLOAT3 light_dir;
	XMFLOAT3 camera_pos;
#pragma pack(pop)	
	//11/15�Ȍ���
};

struct CONSTANT_BUFFER_MAINCHARCTER //�R���X�^���X�o�b�t�@ 
{
#pragma pack(push,1)	//�A���C�������g������I�t
	XMFLOAT4X4 World;
	XMFLOAT4X4 View;
	XMFLOAT4X4 Projection;
	XMFLOAT4 ambient;
	XMFLOAT4 diffuse;
	XMFLOAT4 specular;
	XMFLOAT3 light_dir;
	XMFLOAT3 camera_pos;
#pragma pack(pop)	
};

struct CONSTANT_BUFFER {
	XMFLOAT4X4 World;
	XMFLOAT4X4 View;
	XMFLOAT4X4 Projection;
};

struct CONSTANT_BONE_MATRIX
{
	XMMATRIX boneMatrix[200];
};

//Rendering Data Struct
struct DrawingAllDataObject//�`�掞�ɕK�v�ȃf�[�^
{
	PMX_DATA pmxdata;//�I�u�W�F�N�g�f�[�^
	CONSTANT_BUFFER_OBJECT constantBuffer;//�R���X�^���X�o�b�t�@
	ObjectIndividualData IndiviData;
	vector<ID3D11ShaderResourceView*> Texture;//�e�N�X�`��
};

struct DrawingAllDataMainCharcter//�`�掞�ɕK�v�ȃf�[�^
{
	PMX_DATA pmxdata;//�I�u�W�F�N�g�f�[�^
	CONSTANT_BUFFER_MAINCHARCTER constantBuffer;//�R���X�^���X�o�b�t�@
	CONSTANT_BONE_MATRIX constantBoneBuffer;//�{�[���Ɏg�p
	ObjectIndividualData IndiviData;
	vector<ID3D11ShaderResourceView*> Texture;//�e�N�X�`��
	PMX_SEND_DATA *sendData;
};

struct DrawingAllDataShadow//�`�掞�ɕK�v�ȃf�[�^
{
	PMX_DATA pmxdata;//�I�u�W�F�N�g�f�[�^
	CONSTANT_BUFFER constantBuffer;//�R���X�^���X�o�b�t�@
	ObjectIndividualData IndiviData;
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


	UINT MorphNum;//���[�t�i���o�[�i�[
	UINT WorldMorphNum;//���[�t�i���o�[�i�[


	//draw��ς��� 11/9�@clear (11/9)

	CONSTANT_BUFFER_OBJECT cbo;
	CONSTANT_BUFFER_MAINCHARCTER cbm;
	CONSTANT_BONE_MATRIX cbmr;
	CONSTANT_BUFFER cb;
	//11/9�Fdraw��̏�����Render�� clear ()

	//create input layout
	void Cretaelayout(ID3D11Device* m_pDevice);

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


	PMX_SEND_DATA *VertexBufferUpdate;
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
