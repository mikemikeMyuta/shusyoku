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

//------------------------------------------------
// ���_�V�F�[�_
//------------------------------------------------
void vs_main(VS_IN�@input,
	out float4 out_pos : POSITION,
	out float4 out_depth : TEXCOORD0)
{

	float4x4 comb = (float4x4)0;
	for (int i = 0; i < 4; i++)
	{
		comb += BoneMatrix[input.BoneIndex[i]] * input.BoneWeight[i];
	}
	//���W�ϊ�
	out_pos = mul(float4(input.pos, 1), comb);


	// ���W�ϊ�
	out_pos = mul(output.pos, World);
	out_pos = mul(output.pos, View);
	out_pos = mul(output.pos, Projection);
	// �X�N���[�����W�����̂܂܃s�N�Z���V�F�[�_�[�֓n��
	out_depth = out_pos;
}


struct PS_OUT
{
	float4 col : SV_Target;
};

//------------------------------------------------
// �s�N�Z���V�F�[�_
//------------------------------------------------
PS_OUT ps_main(float4 in_depth : TEXCOORD0)
{
	// Z�l���o�̓J���[�Ƃ���
	PS_OUT out_color;
	out_color.col = in_depth.z / in_depth.w;
	return out_color;
}