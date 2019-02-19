#pragma once

#include "../BASE/BASE.h"
#include "../PMX/modelList.h"

using namespace std;
using namespace DirectX;

namespace LIGHT {
	namespace POINT_LIGHT {
		struct POINT_LIGHT
		{
			// ���s����
			static const unsigned int DIRLIGHT_NUM = 4;
			XMVECTOR vDirLight[DIRLIGHT_NUM];
			XMVECTOR colDirLight[DIRLIGHT_NUM];

			//�������C�e�B���O
			XMVECTOR vHemiDir;
			XMVECTOR colHemiSky;
			XMVECTOR colHemiGrd;

			//�_����
			static const unsigned int PLIGHT_NUM = 4;
			XMVECTOR posPLight[PLIGHT_NUM];
			XMVECTOR colPLight[PLIGHT_NUM];
			XMVECTOR prmPLight[PLIGHT_NUM];

			//
			XMVECTOR diffuse;
		};

		struct SEND_DATA
		{
			XMFLOAT4 Pos;
			XMFLOAT3 Normal;
			XMFLOAT2 Tex;
		};
	}
	struct SCENE
	{
		XMMATRIX mtxWorld;	// ViewProjection�s��
		XMMATRIX mtxProj;	// ViewProjection�s��
		XMMATRIX mtxView;	// WorldView�s��
	};
}

//���C�g�̃N���X

class CLight sealed
{
private:
	XMFLOAT3 position;
	XMFLOAT3 rotation;

	ID3D11ShaderResourceView* Tex;//�e�N�X�`���p

	ID3DBlob *pCompilePS;
	ID3DBlob *pCompileVS;

	ID3D11Buffer *SceneBuffer;
	ID3D11Buffer *LightBuffer;
	ID3D11Buffer *Buffer;

	ID3D11PixelShader* pPixelShader;
	ID3D11VertexShader* pVertexShader;
	ID3D11InputLayout* pVertexLayout;
	ID3D11SamplerState* SampleLinear;

	LIGHT::SCENE scn;
	LIGHT::POINT_LIGHT::POINT_LIGHT PLight;
	LIGHT::POINT_LIGHT::SEND_DATA sendData;

	ID3D11ShaderResourceView* Texture;


	void SetPmxUsingPixelShader(ID3D11Device *, LPCWSTR);
	void SetPmxUsingGeometryShader(ID3D11Device *, LPCWSTR);
	void SetPmxUsingVertexShader(ID3D11Device *, LPCWSTR);
	void CreateLayout(ID3D11Device*);
	void CreateContextBuffer(ID3D11Device*, ID3D11DeviceContext*);
	void TexLoad(ID3D11Device*);//textureLoad

	//�Ȃ��@�\�[�X�����Ă��郉�C�g�撣�낤
public:


	void setPosition(XMFLOAT3 data) { position = data; }
	void setRotation(XMFLOAT3 data) { rotation = data; }

	void Init(ID3D11Device*, ID3D11DeviceContext*, const LPCWSTR, const LPCWSTR, const LPCWSTR);
	/*
	1:�f�o�C�X
	2:�R���e�L�X�g
	3:�s�N�Z���V�F�[�_�[�̖��O
	4:�W�I���g���V�F�[�_�[�̖��O
	5:�o�[�e�b�N�X�V�F�[�_�[�̖��O
	*/
	void Render(ID3D11DeviceContext*);
	CLight()
	{
		ZeroMemory(&scn, sizeof(scn));
		ZeroMemory(&PLight, sizeof(PLight));
	}
	~CLight(){}
};