//グローバル
Texture2D g_texDecal: register(t0);//テクスチャー
Texture2D g_texDecal1: register(t1);//テクスチャー

SamplerState g_samLinear : register(s0);//サンプラー


//構造体
struct VS_OUTPUT
{
	float4 Pos : SV_POSITION;
	float2 Tex : TEXCOORD;
	float4 Col: COLOR0;
};

//頂点シェーダー
VS_OUTPUT VS(float4 Pos : POSITION, float2 Tex : TEXCOORD)
{

	VS_OUTPUT output = (VS_OUTPUT)0;
	output.Pos = Pos;
	output.Tex = Tex;

	output.Col = float4(1.0f*Tex.x, 1.0f*Pos.z, 1.0f*Tex.y, 1.0f);//カラースケール

	//output.Col = float4(1.0f, 1.0f, 1.0f, 1.0f); //nonrmal

	//output.Col = float4(1.0f, 0, 0, 1.0f); //red

	//output.Col = float4(0, 0, 0, 1.0f);//black

	//output.Col = float4(Tex.y,Tex.y,  Tex.y, 1.0f);//white->black


	//黒
	//float Y = output.Col.r * 0.29891f + output.Col.g * 0.58661f + output.Col.b * 0.11448f;


	////カラー
	/*output.Col = float4(0.29891f + fps, 0.58661f + fps, 0.81448f + fps, 1.0f);
	output.Col = float4(0.02  *output.Col.r* Tex.x*Pos.y, 0.01 *Pos.z* Tex.x* output.Col.b, Tex.y * output.Col.g* 0.1, 1.0f);*/
	//output.Col = float4(0.2, 0.5, 0.2, 0.5f);
	return output;
}

//ピクセルシェーダー
float4 PS(VS_OUTPUT input) : SV_Target
{
	//float4 color = g_texDecal.Sample(g_samLinear, input.Tex);

	//グレイスケール
	//float4 gray = color.r * 0.299 + color.g * 0.587 + color.b * 0.114;
	//gray.a = 1;
	//input.Col = gray;
	//return gray;

	//セピア
	//const float3 mono = float3(0.298912, 0.586611, 0.114478);
	//const float3 sepia = float3(1.07, 0.74, 0.43);

	//color.rgb = dot(color.rgb, mono) *color.a;
	//color.rgb *= sepia;
		return g_texDecal.Sample(g_samLinear, input.Tex)*input.Col;

		//return g_texDecal.Sample(g_samLinear, input.Tex)*input.Col;
}