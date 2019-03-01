//コンスタンスバッファ
cbuffer  global: register(b0) {
	float4x4 World;    // 変換行列
	float4x4 View;
	float4x4 Projection;
};

cbuffer BoneMatrix:register(b1)
{
	matrix BoneMatrix[200];//二百個のボーンしか使えない
}


struct VS_IN
{
	float3 pos : POSITION;
	float4 BoneIndex : BONEINDEX;
	float4 BoneWeight : BONEWEIGHT;
};

//------------------------------------------------
// 頂点シェーダ
//------------------------------------------------
void vs_main(VS_IN　input,
	out float4 out_pos : POSITION,
	out float4 out_depth : TEXCOORD0)
{

	float4x4 comb = (float4x4)0;
	for (int i = 0; i < 4; i++)
	{
		comb += BoneMatrix[input.BoneIndex[i]] * input.BoneWeight[i];
	}
	//座標変換
	out_pos = mul(float4(input.pos, 1), comb);


	// 座標変換
	out_pos = mul(output.pos, World);
	out_pos = mul(output.pos, View);
	out_pos = mul(output.pos, Projection);
	// スクリーン座標をそのままピクセルシェーダーへ渡す
	out_depth = out_pos;
}


struct PS_OUT
{
	float4 col : SV_Target;
};

//------------------------------------------------
// ピクセルシェーダ
//------------------------------------------------
PS_OUT ps_main(float4 in_depth : TEXCOORD0)
{
	// Z値を出力カラーとする
	PS_OUT out_color;
	out_color.col = in_depth.z / in_depth.w;
	return out_color;
}