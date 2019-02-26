cbuffer  global:register(b0) {
	float4x4 World;    // 変換行列
	float4x4 View;
	float4x4 Projection;
}
//------------------------------------------------
// 頂点シェーダ
//------------------------------------------------
void VS(float3 in_pos		: POSITION,
	out float4 out_pos : POSITION,
	out float4 out_depth : TEXCOORD0)
{
	// 座標変換
	out_pos = mul(float4(in_pos, 1.0f), g_world);
	out_pos = mul(out_pos, g_view);
	out_pos = mul(out_pos, g_projection);

	// スクリーン座標をそのままピクセルシェーダーへ渡す
	out_depth = out_pos;
}


//------------------------------------------------
// ピクセルシェーダ
//------------------------------------------------
void PS(float4 in_depth : TEXCOORD0,
	out float4 out_color : COLOR0)
{
	// Z値を出力カラーとする
	out_color = in_depth.z / in_depth.w;
}