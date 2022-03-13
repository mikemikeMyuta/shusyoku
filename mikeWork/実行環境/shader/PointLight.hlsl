struct Scene
{
	matrix mtxWorld;
	matrix mtxProj;
	matrix mtxView;
};

struct Light
{
	float4 vDirLight[4];
	float4 colDirLight[4];
	float4 vHemiDir;
	float4 colHemiSky;
	float4 colHemiGrd;
	float4 posPLight[4];
	float4 colPLight[4];
	float4 prmPLight[4];
	float4 vDiffuse;
};

Texture2D txDiffuse : register(t0);
SamplerState samLinear : register(s0);

cbuffer CB1 : register(b1)
{
	Scene scene;
};


cbuffer CB2 : register(b2)
{
	Light light;
};

struct VS_INPUT
{
	float4 Pos : POSITION;
	float3 Nor : NORMAL;
	float2 Tex : TEXCOORD;
};

struct PS_INPUT
{
	float4 Pos : SV_POSITION;
	float3 Nor : NORMAL;
	float4 PosW : WORLDPOS;
	float2 Tex : TEXCOORD0;
};

PS_INPUT vs_main(VS_INPUT input)
{
	PS_INPUT output = (PS_INPUT)0;
	output.PosW = mul(input.Pos, scene.mtxWorld);
	output.Pos = mul(output.PosW, scene.mtxView);
	output.Pos = mul(output.Pos, scene.mtxProj);
	output.Tex = input.Tex;
	output.Nor = normalize(mul(input.Nor, scene.mtxWorld));
	return output;
}

float4 ps_main(PS_INPUT input) : SV_Target
{
	float3 nor = normalize(input.Nor);

	// îºãÖ
	float hemi = dot(nor, light.vHemiDir);
	float3 rgb = 0.5*((1 + hemi)*light.colHemiSky + (1 - hemi)*light.colHemiGrd);

	//ïΩçsåıåπ
	for (int i = 0; i < 4; ++i) {
		float l = saturate(dot(nor, light.vDirLight[i].xyz));
		rgb += l * light.colDirLight[i];
	}

	//ì_åıåπ
	for (int i = 0; i < 4; ++i) {
		float3 pdir = light.posPLight[i].xyz - input.PosW.xyz;
		float r = length(pdir);
		pdir = normalize(pdir);
		rgb += light.colPLight[i].xyz
				*saturate(light.prmPLight[i].z*r + light.prmPLight[i].w)
				*saturate(dot(pdir,nor));
	}

	return float4(rgb,1)*txDiffuse.Sample(samLinear, input.Tex)*light.vDiffuse;
}
