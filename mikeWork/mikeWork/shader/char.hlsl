// �萔�o�b�t�@(CPU������̒l�󂯎���)

//�O���[�o��
Texture2D g_tex: register(t0);//�e�N�X�`���[
Texture2D g_texAni: register(t1);//�e�N�X�`���[

SamplerState g_samLinear : register(s0);//�T���v���[

//�R���X�^���X�o�b�t�@
cbuffer  global: register(b0) {
	float4x4 World;    // �ϊ��s��
	float4x4 View;
	float4x4 Projection;
	float4 ambient;
	float4 diffuse;
	float4 specular;
	float3 light_dir;
	float3 camera_pos;
};

cbuffer BoneMatrix:register(b1)
{
	matrix BoneMatrix[400];//�l�S�̃{�[�������g���Ȃ�
}

//
struct VS_IN
{
	float3 pos : POSITION;
	float3 normal : NORMAL;
	float4 col : COLORR;
	float4 col_specular : COLORRR;
	float2 Tex : TEXCOORD;
	float4 BoneIndex : BONEINDEX;
	float4 BoneWeight : BONEWEIGHT;
};

struct VS_OUT
{
	float4 pos : SV_POSITION;
	float3 pospass : POSITIONPASS;
	float3 normal : NORMAL;
	float4 col : COLORR;
	float4 col_specular : COLORRR;
	float2 Tex : TEXCOORD;
};
// ���_�V�F�[�_
VS_OUT vs_main(VS_IN input)
{
	VS_OUT output = (VS_OUT)0;//�Ԋҗp�\���̕ϐ�

	//���_�u�����h

	float4x4 comb = (float4x4)0;
	for (int i = 0; i < 4; i++)
	{
		comb += BoneMatrix[input.BoneIndex[i]] * input.BoneWeight[i];
	}
	//���W�ϊ�
	output.pos = mul(float4(input.pos,1), comb);

	output.pos = mul(output.pos, World);
	output.pospass = output.pos.xyz;
	output.pos = mul(output.pos, View);
	output.pos = mul(output.pos, Projection);

	output.normal = normalize(mul(input.normal, (float3x3)World));
	output.col = input.col;
	output.col_specular = input.col_specular;
	output.Tex = input.Tex;
	return output;//�Ԋ�

}


struct PS_OUT
{
	float4 col : SV_Target;
};

PS_OUT ps_main(VS_OUT input)
{
	PS_OUT output;

	float3 N;//���[���h��ԏ�̖@���x�N�g��;
	float3 L;//���̍������ޕ���
	float3 P;//���[���h��ԏ㒸�_���W
	float3 V;//�x�N�g��
	float3 H;//�n�[�t�x�N�g��

	P = input.pospass;
	//�P�ʃx�N�g����
	N = normalize(input.normal)*(2.0f* g_tex.Sample(g_samLinear, input.Tex).xyz - 1.0f);

	L = normalize(-light_dir);
	V = camera_pos - P;
	V = normalize(V);

	//�n�[�t�x�N�g��
	H = normalize(L + V);

	//�n�[�t�����o�[�g�p�v�Z
	float dotD;
	dotD = dot(L, N);
	dotD = dotD * 0.5 + 0.5;
	dotD = dotD * dotD;


	//�o�̓J���[
	float4 colDif;
	float4 colSpe;

	float s = pow(max(0, dot(N, H)), 30);

	//�����o�[�g
	//colDif = ambient + diffuse * max(0.0, dotD);
	//colSpe = specular * pow(max(0, dot(N, H)), 10);

	//�n�[�t�����o�[�g
	colDif = ambient + diffuse;
	colSpe = specular * s;

	float2 toonUV;
	toonUV.x = dotD;
	toonUV.y = dotD;

	float4 toon = g_texAni.Sample(g_samLinear, toonUV);
	//(11/15)shader ok ����ɍ����悤�ɍ��ւ���\�[�X


	output.col = g_tex.Sample(g_samLinear, input.Tex) * toon* saturate(colDif + colSpe);
	return output;
}
