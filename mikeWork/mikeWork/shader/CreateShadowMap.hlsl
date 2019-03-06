//�R���X�^���X�o�b�t�@
cbuffer  global: register(b0) {
	float4x4 World;    // �ϊ��s��
	float4x4 View;
	float4x4 Projection;
};

cbuffer BoneMatrix:register(b1)
{
	matrix BoneMatrix[200];//��S�̃{�[�������g���Ȃ�
}

struct VS_IN
{
	float3 pos : POSITION;
	float4 BoneIndex : BONEINDEX;
	float4 BoneWeight : BONEWEIGHT;
};

struct VS_OUT
{
	float4 pos : SV_POSITION;
	float4 depth : TEXCOORD0;
};
//------------------------------------------------
// ���_�V�F�[�_
//------------------------------------------------
VS_OUT vs_main(VS_IN input)
{
	VS_OUT vsout;
	float4x4 comb = (float4x4)0;
	for (int i = 0; i < 4; i++)
	{
		comb += BoneMatrix[input.BoneIndex[i]] * input.BoneWeight[i];
	}
	//���W�ϊ�
	vsout.pos = mul(float4(input.pos, 1), comb);


	// ���W�ϊ�
	vsout.pos = mul(vsout.pos, World);
	vsout.pos = mul(vsout.pos, View);
	vsout.pos = mul(vsout.pos, Projection);
	// �X�N���[�����W�����̂܂܃s�N�Z���V�F�[�_�[�֓n��
	vsout.depth = vsout.pos;
	return vsout;
}


struct PS_OUT
{
	float4 col : SV_TARGET0;
};

//------------------------------------------------
// �s�N�Z���V�F�[�_
//------------------------------------------------
PS_OUT ps_main(VS_OUT input)
{
	// Z�l���o�̓J���[�Ƃ���
	PS_OUT out_color;
	out_color.col = input.depth.z / input.depth.w;
	return out_color;
}