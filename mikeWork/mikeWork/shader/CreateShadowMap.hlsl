cbuffer  global:register(b0) {
	float4x4 World;    // �ϊ��s��
	float4x4 View;
	float4x4 Projection;
}
//------------------------------------------------
// ���_�V�F�[�_
//------------------------------------------------
void VS(float3 in_pos		: POSITION,
	out float4 out_pos : POSITION,
	out float4 out_depth : TEXCOORD0)
{
	// ���W�ϊ�
	out_pos = mul(float4(in_pos, 1.0f), g_world);
	out_pos = mul(out_pos, g_view);
	out_pos = mul(out_pos, g_projection);

	// �X�N���[�����W�����̂܂܃s�N�Z���V�F�[�_�[�֓n��
	out_depth = out_pos;
}


//------------------------------------------------
// �s�N�Z���V�F�[�_
//------------------------------------------------
void PS(float4 in_depth : TEXCOORD0,
	out float4 out_color : COLOR0)
{
	// Z�l���o�̓J���[�Ƃ���
	out_color = in_depth.z / in_depth.w;
}