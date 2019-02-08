cbuffer  global {
	float4x4 World;    // ïœä∑çsóÒ
	float4x4 View;
	float4x4 Projection;
};
struct VS_IN
{
	float4 pos : POSITION;
	float3 normal : NORMAL;
	float4 col : COLORR;
	float4 col_specular : COLORRR;
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

	P = (float3)input.pos + input.normal*0.05f;

	output.pos = mul(float4(P, 1.0f), World);
	output.pos = mul(output.pos, View);
	output.pos = mul(output.pos, Projection);

	return output;
	
}

struct PS_IN
{
	float4 pos : POSITION;
};

struct PS_OUT
{
	float4 col : SV_Target;
};

PS_OUT ps_main()
{
	PS_OUT output;
	output.col = float4(0, 0, 0, 1);
	return output;
}