// 定数バッファ(CPU側からの値受け取り場)

//グローバル
Texture2D g_tex: register(t0);//テクスチャー
SamplerState g_samLinear : register(s0);//サンプラー

cbuffer  global {
	float4x4 World;    // 変換行列
	float4x4 View;
	float4x4 Projection;
	float4 ambient;
	float4 diffuse;
	float4 specular;
	float3 light_dir;
	float3 camera_pos;
};

struct VS_IN
{
	float4 pos : POSITION;
	float3 normal : NORMAL;
	float4 col : COLORR;
	float4 col_specular : COLORRR;
	float2 Tex : TEXCOORD;
};

struct VS_OUT
{
	float4 pos : SV_POSITION;
	float3 pospass : POSITIONPASS;
	float3 normal : NORMAL;
	float4 col : COLORR;
	float4 col_specular : COLORRR;
	float2 Tex : TEXCOORD;
};
// 頂点シェーダ
VS_OUT vs_main(VS_IN input)
{
	VS_OUT output;//返還用構造体変数


	//座標変換
	output.pos = mul(input.pos, World);
	output.pospass = output.pos.xyz;
	output.pos = mul(output.pos, View);
	output.pos = mul(output.pos, Projection);
	output.normal = normalize(mul(input.normal, (float3x3)World));
	output.col = input.col;
	output.col_specular = input.col_specular;
	output.Tex = input.Tex;
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

	P = input.pospass;
	//単位ベクトル化
	N = normalize(input.normal);

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

	float s = pow(max(0, dot(N, H)), 30);

	//ランバート
	//colDif = ambient + diffuse * max(0.0, dotD);
	//colSpe = specular * pow(max(0, dot(N, H)), 10);

	//ハーフランバート
	colDif = ambient + diffuse;
	colSpe = specular * s;

	//(11/15)shader ok これに合うように作り替えるソース


	output.col = g_tex.Sample(g_samLinear, input.Tex) * saturate(colDif + colSpe);

	return output;
}
