// 定数バッファ(CPU側からの値受け取り場)

//グローバル
Texture2D g_tex: register(t0);//テクスチャー
Texture2D g_texAni: register(t1);//テクスチャー
Texture2D g_shadowMap: register(t2);//テクスチャー

SamplerState g_samLinear : register(s0);//サンプラー
SamplerComparisonState g_samShadow : register(s1);//サンプラー


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

struct VS_OUT
{
	float4 pos : SV_POSITION;
	float4 depth : TEXCOORD;
};
//------------------------------------------------
// 頂点シェーダ
//------------------------------------------------
VS_OUT vs_main(VS_IN input)
{
	VS_OUT vsout;
	float4x4 comb = (float4x4)0;
	for (int i = 0; i < 4; i++)
	{
		comb += BoneMatrix[input.BoneIndex[i]] * input.BoneWeight[i];
	}
	//座標変換
	vsout.pos = mul(float4(input.pos, 1), comb);


	// 座標変換
	vsout.pos = mul(vsout.pos, World);
	vsout.pos = mul(vsout.pos, View);
	vsout.pos = mul(vsout.pos, Projection);
	// スクリーン座標をそのままピクセルシェーダーへ渡す
	vsout.depth = vsout.pos;
	return vsout;
}


struct PS_OUT
{
	float4 col : SV_Target;
};

//------------------------------------------------
// ピクセルシェーダ
//------------------------------------------------
float4 ps_main(VS_OUT input) :SV_Target
{
	// Z値を出力カラーとする
	float4 out_color;
	out_color= input.depth.z / input.depth.w;
	return out_color;
}