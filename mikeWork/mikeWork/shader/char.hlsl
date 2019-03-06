// 定数バッファ(CPU側からの値受け取り場)

//グローバル
Texture2D g_tex: register(t0);//テクスチャー
Texture2D g_texAni: register(t1);//テクスチャー
Texture2D g_shadowMap: register(t2);//テクスチャー

SamplerState g_samLinear : register(s0);//サンプラー
SamplerComparisonState g_samShadow : register(s1);//サンプラー

//コンスタンスバッファ
cbuffer  global: register(b0) {
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
	matrix BoneMatrix[200];//200個のボーンしか使えない
}

//
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
	float4 pos : SV_POSITION;
	float3 pospass : POSITIONPASS;
	float3 normal : NORMAL;
	float4 col : COLORR;
	float4 col_specular : COLORRR;
	float2 Tex : TEXCOORD;
	float4  SdwCoord  : SHADOW_COORD;
};
// 頂点シェーダ
VS_OUT vs_main(VS_IN input)
{
	VS_OUT output = (VS_OUT)0;//返還用構造体変数

	//頂点ブレンド

	float4x4 comb = (float4x4)0;
	for (int i = 0; i < 4; i++)
	{
		comb += BoneMatrix[input.BoneIndex[i]] * input.BoneWeight[i];
	}
	//座標変換
	output.pos = mul(float4(input.pos, 1), comb);

	output.pos = mul(output.pos, World);
	output.pospass = output.pos.xyz;
	output.pos = mul(output.pos, View);
	output.pos = mul(output.pos, Projection);

	output.normal = normalize(mul(input.normal, (float3x3)World));
	output.col = input.col;
	output.col_specular = input.col_specular;
	output.Tex = input.Tex;


	float4 localPos = float4(input.pos, 1.0f);
	float4 worldPos = mul(World, localPos);

	output.SdwCoord= mul(Shadow, worldPos);
	return output;//返還

}


struct PS_OUT
{
	float4 col : SV_Target;
};

PS_OUT ps_main(VS_OUT input)
{
	PS_OUT output;

	float3 N;//ワールド空間上の法線ベクトル;
	float3 L;//光の差し込む方向
	float3 P;//ワールド空間上頂点座標
	float3 V;//ベクトル
	float3 H;//ハーフベクトル

	 // シャドウマップの深度値と比較する.
	float3 shadowCoord = input.SdwCoord.xyz / input.SdwCoord.w;
	// 最大深度傾斜を求める.
	float  maxDepthSlope = max(abs(ddx(shadowCoord.z)), abs(ddy(shadowCoord.z)));

	float  shadowThreshold = 1.0f;      // シャドウにするかどうかの閾値です.
	float  bias = 0.01f;     // 固定バイアスです.
	float  slopeScaledBias = 0.01f;     // 深度傾斜.
	float  depthBiasClamp = 0.1f;      // バイアスクランプ値.

	float  shadowBias = bias + slopeScaledBias * maxDepthSlope;
	shadowBias = min(shadowBias, depthBiasClamp);

	float3 shadowColor = float3(1.0f, 0.50f, 0.7f);
	shadowThreshold = g_shadowMap.SampleCmpLevelZero(g_samShadow, shadowCoord.xy, shadowCoord.z - shadowBias);
	shadowColor = lerp(shadowColor, float3(1.0f, 1.0f, 1.0f), shadowThreshold);

	P = input.pospass;
	//単位ベクトル化
	N = normalize(input.normal*(2.0f* g_tex.Sample(g_samLinear, input.Tex).xyz - 1.0f));

	L = normalize(-light_dir);
	V = camera_pos - P;
	V = normalize(V);

	//ハーフベクトル
	H = normalize(L + V);

	//ハーフランバート用計算
	float dotD;
	dotD = dot(L, N);
	dotD = dotD * 0.5 + 0.5;
	dotD = dotD * dotD;


	//出力カラー
	float4 colDif;
	float4 colSpe;

	float s = pow(max(0, dot(N, H)), 5);

	//ランバート
	//colDif = ambient + diffuse * max(0.0, dotD);
	//colSpe = specular * pow(max(0, dot(N, H)), 10);

	//ハーフランバート

	colDif = ambient;
	colSpe = specular * s;

	float2 toonUV;
	toonUV.x = dotD;
	toonUV.y = dotD;


	float4 toon = g_texAni.Sample(g_samLinear, toonUV);
	//(11/15)shader ok これに合うように作り替えるソース

	float4 shadowColorFloat4 = float4(shadowColor, 1.0f);
	float4 SetColor = (colDif + colSpe) * shadowColorFloat4;

	output.col = g_tex.Sample(g_samLinear, input.Tex)* toon* SetColor;
	return output;
}
