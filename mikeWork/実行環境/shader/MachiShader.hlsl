// 定数バッファ(CPU側からの値受け取り場)
cbuffer global {
	float4x4 World;    // 変換行列
	float4x4 View;
	float4x4 Projection;
};
struct VS_IN
{
	float4 pos : POSITION0;
	float4 col : COLOR0;
};

struct VS_OUT
{
	float4 pos : SV_POSITION;
	float4 col : COLOR0;
};
// 頂点シェーダ
VS_OUT vs_main(VS_IN input)
{
	VS_OUT output;

	output.pos = mul(input.pos, World);
	output.pos = mul(output.pos, View);
	output.pos = mul(output.pos, Projection);
//	output.col = input.col;
	output.col = float4(0, 0, 1, 1);
	output.col.a = 0.6f;
	return output;
}

// ピクセルシェーダ
struct PS_IN
{
	float4 pos : SV_POSITION;
	float4 col : COLOR0;
};


float4 ps_main(PS_IN input) : SV_Target
{
	return input.col;
}