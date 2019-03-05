cbuffer  global:register(b0) {
	float4x4 World;
	float4x4 View;
	float4x4 Projection;
	float4 ambient;
	float4 diffuse;
	float4 specular;
	float3 light_dir;
	float3 camera_pos;
	float4x4 Shadow;
};

cbuffer BoneMatrix:register(b1)
{
	matrix BoneMatrix[200];//四百個のボーンしか使えない
}

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
	float4 pos:SV_POSITION;
};

VS_OUT vs_main(VS_IN input)
{
	VS_OUT output;

	float3 P;
	input.normal = normalize(input.normal);

	float4x4 comb = (float4x4)0;
	for (int i = 0; i < 4; i++)
	{
		comb += BoneMatrix[input.BoneIndex[i]] * input.BoneWeight[i];
	}



	P = (float3)input.pos + input.normal*0.03f;
	
	output.pos = mul(float4(P, 1.0f), comb);



	output.pos = mul(output.pos, World);
	output.pos = mul(output.pos, View);
	output.pos = mul(output.pos, Projection);

	return output;
	
}

struct PS_OUT
{
	float4 col : SV_Target;
};

PS_OUT ps_main()
{
	PS_OUT output;
	output.col = float4( 0.1f, 0.1f, 0.1f,1.0f);
	return output;
}