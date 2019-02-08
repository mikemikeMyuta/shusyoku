#include "PmxLoad.h"
#include <iostream>
#include <fstream>

//構造体で定義したもので変換をかける
__inline void MV1LoadModelToPMX_GetString(BYTE **Src, char *DestBuffer, BYTE EncodeType)
// テキストデータをシフトJISに変換して取得　第一引数　データ　第二引数　どこバッファまで　第三引数　文字コードのエンコードタイプ 0:UTF16 1:UTF8
{
	char Buffer[1024];
	wchar_t WBuffer[1024];
	int TextSize;

	TextSize = *((DWORD *)*Src);
	*Src = *Src + 4;//次に進む

	if (EncodeType == 0)	// UTF16
	{


		memcpy(WBuffer, *Src, TextSize);
		((BYTE *)WBuffer)[TextSize] = 0;
		((BYTE *)WBuffer)[TextSize + 1] = 0;
		*Src = *Src + TextSize;

		// ワイド文字をシフトJISに変換

		//int WideCharToMultiByte(
		//UINT CodePage,         // コードページ　
		//	DWORD dwFlags,         // 処理速度とマッピング方法を決定するフラグ
		//	LPCWSTR lpWideCharStr, // ワイド文字列のアドレス
		//	int cchWideChar,       // ワイド文字列の文字数
		//	LPSTR lpMultiByteStr,  // 新しい文字列を受け取るバッファのアドレス
		//	int cchMultiByte,      // 新しい文字列を受け取るバッファのサイズ
		//	LPCSTR lpDefaultChar,  // マップできない文字の既定値のアドレス
		//	LPBOOL lpUsedDefaultChar   // 既定の文字を使ったときにセットするフラグのアドレス
		//	);

		WideCharToMultiByte(932, 0, WBuffer, -1, DestBuffer, 512, NULL, NULL);//第一引数の９３２は　シフトJISのコードページ

	}
	else	if (EncodeType == 1)	// UTF8
	{


		memcpy(Buffer, *Src, TextSize);
		Buffer[TextSize] = '\0';
		*Src = *Src + TextSize;

		// 最初に UTF-8 をワイド文字に変換
		MultiByteToWideChar(CP_UTF8, 0, Buffer, -1, WBuffer, 1024);

		// ワイド文字をシフトJISに変換
		WideCharToMultiByte(932, 0, WBuffer, -1, DestBuffer, 512, NULL, NULL);
	}
}

__inline int MV1LoadModelToPMX_GetInt(BYTE **Src, BYTE Size)// 整数値サイズに合わせた整数値を取得する(符号あり)
//第一引数　データ　第二引数　Indexサイズ ( 1 or 2 or 4 )
{
	int res = 0;

	switch (Size)
	{
	case 1:
		res = (char)((*Src)[0]);
		*Src = *Src + 1;
		break;

	case 2:
		res = (short)(((WORD *)*Src)[0]);
		*Src = *Src + 2;
		break;

	case 4:
		res = ((int *)*Src)[0];
		*Src = *Src + 4;
		break;
	}

	return res;//サイズを返す
}

__inline int MV1LoadModelToPMX_GetUInt(BYTE **Src, BYTE Size)// 整数値サイズに合わせた整数値を取得する(32bit以外符号なし)　VertexのIndexSizeに使用
{
	int res = 0;

	switch (Size)
	{
	case 1:
		res = (int)((*Src)[0]);
		*Src = *Src + 1;
		break;

	case 2:
		res = (int)(((WORD *)*Src)[0]);
		*Src = *Src + 2;
		break;

	case 4:
		res = ((int *)*Src)[0];
		*Src = *Src + 4;
		break;
	}

	return res;
}


bool CLoadPMX::LoadPMXFile(LPCSTR Filename)
{
	//--ローカル変数宣言--
	FILE* fp;
	BYTE* mem;
	BYTE* mem_head;
	BYTE AddHeadDataSize;
	int i, j;

	if (fopen_s(&fp, Filename, "rb ") != 0) {
		printf("エラー : %sがオープンできません!\n", Filename);
		OutputDebugString("\nエラー :ファイルがオープンできません!\n");
		return false;
	}
	else
	{
		OutputDebugString("\nPMXのデータ格納を始めます\n");
	}
	fseek(fp, 0, SEEK_END);
	int size = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	mem = (BYTE *)malloc(sizeof(BYTE) * size);
	fread(mem, sizeof(char), size, fp);
	fclose(fp);
	mem_head = mem;


	// PMXファイルかどうかを確認
	if (mem[0] != 'P' || mem[1] != 'M' || mem[2] != 'X' || mem[3] != ' ')//ver1.0は読み込めない　PMXヘッダがPmxのため
		return false;
	mem += 4;

	// Ver2.0 かを確認
	if (mem[0] != 0x00 || mem[1] != 0x00 || mem[2] != 0x00 || mem[3] != 0x40)//2.1は少し形が変わるので今回は読み込まない
	{
		printf("PMX Load Error : バージョン２．０ではありません\n");
		OutputDebugString("\nPMX Load Error : バージョン２．０ではありません\n");
		return false;
	}
	mem += 4;

	// 追加情報のサイズを取得
	AddHeadDataSize = mem[0];
	mem++;

	// 追加情報を取得
	m_pmx_data.s_PmxInfo.EncodeType = mem[0];
	m_pmx_data.s_PmxInfo.UVNum = mem[1];
	m_pmx_data.s_PmxInfo.VertexIndexSize = mem[2];
	m_pmx_data.s_PmxInfo.TextureIndexSize = mem[3];
	m_pmx_data.s_PmxInfo.MaterialIndexSize = mem[4];
	m_pmx_data.s_PmxInfo.BoneIndexSize = mem[5];
	m_pmx_data.s_PmxInfo.MorphIndexSize = mem[6];
	m_pmx_data.s_PmxInfo.RigidIndexSize = mem[7];
	mem += AddHeadDataSize;

	// モデル情報スキップ
	mem += *((DWORD *)mem) + 4;
	mem += *((DWORD *)mem) + 4;
	mem += *((DWORD *)mem) + 4;
	mem += *((DWORD *)mem) + 4;


	// 頂点数を取得
	m_pmx_data.s_PmxVertexNum = *((DWORD *)mem);
	mem += 4;


	m_pmx_data.s_pPmxVertex = (PMX_VERTEX *)malloc(sizeof(PMX_VERTEX) * m_pmx_data.s_PmxVertexNum);
	if (m_pmx_data.s_pPmxVertex == NULL)
	{
		printf("PMX Load Error : 頂点データの一時記憶領域の確保に失敗しました\n");
		OutputDebugString("\nPMX Load Error : 頂点データの一時記憶領域の確保に失敗しました\n");
		goto ENDLABEL;
	}

	// 頂点データを取得
	for (i = 0; (DWORD)i < m_pmx_data.s_PmxVertexNum; i++)
	{
		*((DWORD *)&m_pmx_data.s_pPmxVertex[i].Position[0]) = *((DWORD *)&mem[0]);
		*((DWORD *)&m_pmx_data.s_pPmxVertex[i].Position[1]) = *((DWORD *)&mem[4]);
		*((DWORD *)&m_pmx_data.s_pPmxVertex[i].Position[2]) = *((DWORD *)&mem[8]);
		*((DWORD *)&m_pmx_data.s_pPmxVertex[i].Normal[0]) = *((DWORD *)&mem[12]);
		*((DWORD *)&m_pmx_data.s_pPmxVertex[i].Normal[1]) = *((DWORD *)&mem[16]);
		*((DWORD *)&m_pmx_data.s_pPmxVertex[i].Normal[2]) = *((DWORD *)&mem[20]);
		*((DWORD *)&m_pmx_data.s_pPmxVertex[i].UV[0]) = *((DWORD *)&mem[24]);
		*((DWORD *)&m_pmx_data.s_pPmxVertex[i].UV[1]) = *((DWORD *)&mem[28]);

		mem += 32;
		for (j = 0; j < m_pmx_data.s_PmxInfo.UVNum; j++)
		{
			*((DWORD *)&m_pmx_data.s_pPmxVertex[i].AddUV[j][0]) = *((DWORD *)&mem[0]);
			*((DWORD *)&m_pmx_data.s_pPmxVertex[i].AddUV[j][0]) = *((DWORD *)&mem[4]);
			*((DWORD *)&m_pmx_data.s_pPmxVertex[i].AddUV[j][0]) = *((DWORD *)&mem[8]);
			*((DWORD *)&m_pmx_data.s_pPmxVertex[i].AddUV[j][0]) = *((DWORD *)&mem[12]);
			mem += 16;
		}

		m_pmx_data.s_pPmxVertex[i].WeightType = mem[0];
		mem++;

		m_pmx_data.s_pPmxVertex[i].BoneIndex[0] = -1;
		m_pmx_data.s_pPmxVertex[i].BoneIndex[1] = -1;
		m_pmx_data.s_pPmxVertex[i].BoneIndex[2] = -1;
		m_pmx_data.s_pPmxVertex[i].BoneIndex[3] = -1;
		switch (m_pmx_data.s_pPmxVertex[i].WeightType)
		{
		case 0:	// BDEF1
			m_pmx_data.s_pPmxVertex[i].BoneIndex[0] = MV1LoadModelToPMX_GetInt(&mem, m_pmx_data.s_PmxInfo.BoneIndexSize);
			m_pmx_data.s_pPmxVertex[i].BoneWeight[0] = 1.0f;
			break;

		case 1:	// BDEF2
			m_pmx_data.s_pPmxVertex[i].BoneIndex[0] = MV1LoadModelToPMX_GetInt(&mem, m_pmx_data.s_PmxInfo.BoneIndexSize);
			m_pmx_data.s_pPmxVertex[i].BoneIndex[1] = MV1LoadModelToPMX_GetInt(&mem, m_pmx_data.s_PmxInfo.BoneIndexSize);
			*((DWORD *)&m_pmx_data.s_pPmxVertex[i].BoneWeight[0]) = *((DWORD *)mem);
			mem += 4;
			m_pmx_data.s_pPmxVertex[i].BoneWeight[1] = 1.0f - m_pmx_data.s_pPmxVertex[i].BoneWeight[0];
			break;

		case 2:	// BDEF4
			m_pmx_data.s_pPmxVertex[i].BoneIndex[0] = MV1LoadModelToPMX_GetInt(&mem, m_pmx_data.s_PmxInfo.BoneIndexSize);
			m_pmx_data.s_pPmxVertex[i].BoneIndex[1] = MV1LoadModelToPMX_GetInt(&mem, m_pmx_data.s_PmxInfo.BoneIndexSize);
			m_pmx_data.s_pPmxVertex[i].BoneIndex[2] = MV1LoadModelToPMX_GetInt(&mem, m_pmx_data.s_PmxInfo.BoneIndexSize);
			m_pmx_data.s_pPmxVertex[i].BoneIndex[3] = MV1LoadModelToPMX_GetInt(&mem, m_pmx_data.s_PmxInfo.BoneIndexSize);
			*((DWORD *)&m_pmx_data.s_pPmxVertex[i].BoneWeight[0]) = *((DWORD *)&mem[0]);
			*((DWORD *)&m_pmx_data.s_pPmxVertex[i].BoneWeight[1]) = *((DWORD *)&mem[4]);
			*((DWORD *)&m_pmx_data.s_pPmxVertex[i].BoneWeight[2]) = *((DWORD *)&mem[8]);
			*((DWORD *)&m_pmx_data.s_pPmxVertex[i].BoneWeight[3]) = *((DWORD *)&mem[12]);
			mem += 16;
			break;

		case 3:	// SDEF
			m_pmx_data.s_pPmxVertex[i].BoneIndex[0] = MV1LoadModelToPMX_GetInt(&mem, m_pmx_data.s_PmxInfo.BoneIndexSize);
			m_pmx_data.s_pPmxVertex[i].BoneIndex[1] = MV1LoadModelToPMX_GetInt(&mem, m_pmx_data.s_PmxInfo.BoneIndexSize);
			*((DWORD *)&m_pmx_data.s_pPmxVertex[i].BoneWeight[0]) = *((DWORD *)mem);
			mem += 4;
			m_pmx_data.s_pPmxVertex[i].BoneWeight[1] = 1.0f - m_pmx_data.s_pPmxVertex[i].BoneWeight[0];
			*((DWORD *)&m_pmx_data.s_pPmxVertex[i].SDEF_C[0]) = *((DWORD *)&mem[0]);
			*((DWORD *)&m_pmx_data.s_pPmxVertex[i].SDEF_C[1]) = *((DWORD *)&mem[4]);
			*((DWORD *)&m_pmx_data.s_pPmxVertex[i].SDEF_C[2]) = *((DWORD *)&mem[8]);
			*((DWORD *)&m_pmx_data.s_pPmxVertex[i].SDEF_R0[0]) = *((DWORD *)&mem[12]);
			*((DWORD *)&m_pmx_data.s_pPmxVertex[i].SDEF_R0[1]) = *((DWORD *)&mem[16]);
			*((DWORD *)&m_pmx_data.s_pPmxVertex[i].SDEF_R0[2]) = *((DWORD *)&mem[20]);
			*((DWORD *)&m_pmx_data.s_pPmxVertex[i].SDEF_R1[0]) = *((DWORD *)&mem[24]);
			*((DWORD *)&m_pmx_data.s_pPmxVertex[i].SDEF_R1[1]) = *((DWORD *)&mem[28]);
			*((DWORD *)&m_pmx_data.s_pPmxVertex[i].SDEF_R1[2]) = *((DWORD *)&mem[32]);
			mem += 36;
			break;
		}

		*((DWORD *)&m_pmx_data.s_pPmxVertex[i].ToonEdgeScale) = *((DWORD *)&mem[0]);
		mem += 4;
	}

	// 面の数を取得
	m_pmx_data.s_PmxFaceNum = *((DWORD *)mem) / 3;
	mem += 4;

	// 面データを格納するメモリ領域の確保
	m_pmx_data.s_pPmxFace = (PMX_FACE *)malloc(sizeof(PMX_FACE) * m_pmx_data.s_PmxFaceNum);
	if (m_pmx_data.s_pPmxFace == NULL)
	{
		printf("PMX Load Error : 面データの一時記憶領域の確保に失敗しました\n");
		OutputDebugString("\nPMX Load Error : 面データの一時記憶領域の確保に失敗しました\n");
		goto ENDLABEL;
	}

	// 面を構成する頂点インデックスの取得
	switch (m_pmx_data.s_PmxInfo.VertexIndexSize)
	{
	case 1:
		for (i = 0; (DWORD)i < m_pmx_data.s_PmxFaceNum; i++)
		{
			m_pmx_data.s_pPmxFace[i].VertexIndex[0] = (int)(mem[0]);
			m_pmx_data.s_pPmxFace[i].VertexIndex[1] = (int)(mem[1]);
			m_pmx_data.s_pPmxFace[i].VertexIndex[2] = (int)(mem[2]);
			mem += 3;
		}
		break;

	case 2:
		for (i = 0; (DWORD)i < m_pmx_data.s_PmxFaceNum; i++)
		{
			m_pmx_data.s_pPmxFace[i].VertexIndex[0] = (int)(*((WORD *)&mem[0]));
			m_pmx_data.s_pPmxFace[i].VertexIndex[1] = (int)(*((WORD *)&mem[2]));
			m_pmx_data.s_pPmxFace[i].VertexIndex[2] = (int)(*((WORD *)&mem[4]));
			mem += 6;
		}
		break;

	case 4:
		for (i = 0; (DWORD)i < m_pmx_data.s_PmxFaceNum; i++)
		{
			m_pmx_data.s_pPmxFace[i].VertexIndex[0] = *((int *)&mem[0]);
			m_pmx_data.s_pPmxFace[i].VertexIndex[1] = *((int *)&mem[4]);
			m_pmx_data.s_pPmxFace[i].VertexIndex[2] = *((int *)&mem[8]);
			mem += 12;
		}
		break;
	}


	// テクスチャの数を取得
	m_pmx_data.s_PmxTextureNum = *((DWORD *)mem);
	mem += 4;

	// テクスチャデータを格納するメモリ領域の確保
	m_pmx_data.s_pPmxTexture = (PMX_TEXTURE *)malloc(sizeof(PMX_TEXTURE) * m_pmx_data.s_PmxTextureNum);
	if (m_pmx_data.s_pPmxTexture == NULL)
	{
		printf("PMX Load Error : テクスチャデータの一時記憶領域の確保に失敗しました\n");
		OutputDebugString("\nPMX Load Error : テクスチャデータの一時記憶領域の確保に失敗しました\n");
		goto ENDLABEL;
	}

	// テクスチャの情報を取得
	for (i = 0; (DWORD)i < m_pmx_data.s_PmxTextureNum; i++)
	{
		MV1LoadModelToPMX_GetString(&mem, m_pmx_data.s_pPmxTexture[i].TexturePath, m_pmx_data.s_PmxInfo.EncodeType);
	}

	// マテリアルの数を取得
	m_pmx_data.s_PmxMaterialNum = *((DWORD *)mem);
	mem += 4;

	// マテリアルデータを格納するメモリ領域の確保
	m_pmx_data.s_pPmxMaterial = (PMX_MATERIAL *)malloc(sizeof(PMX_MATERIAL) * m_pmx_data.s_PmxMaterialNum);
	if (m_pmx_data.s_pPmxMaterial == NULL)
	{
		printf("PMX Load Error : マテリアルデータの一時記憶領域の確保に失敗しました\n");
		OutputDebugString("\nPMX Load Error : マテリアルデータの一時記憶領域の確保に失敗しました\n");
		goto ENDLABEL;
	}

	// マテリアルの読みこみ
	for (i = 0; (DWORD)i < m_pmx_data.s_PmxMaterialNum; i++)
	{
		// 材質名の取得
		MV1LoadModelToPMX_GetString(&mem, m_pmx_data.s_pPmxMaterial[i].Name, m_pmx_data.s_PmxInfo.EncodeType);
		if (m_pmx_data.s_pPmxMaterial[i].Name[0] == '\0')
		{
			MV1LoadModelToPMX_GetString(&mem, m_pmx_data.s_pPmxMaterial[i].Name, m_pmx_data.s_PmxInfo.EncodeType);
			if (m_pmx_data.s_pPmxMaterial[i].Name[0] == '\0')
			{
				sprintf_s(m_pmx_data.s_pPmxMaterial[i].Name, "Mat_%d", i);
			}
		}
		else
		{
			mem += *((DWORD *)mem) + 4;
		}

		*((DWORD *)&m_pmx_data.s_pPmxMaterial[i].Diffuse[0]) = *((DWORD *)&mem[0]);
		*((DWORD *)&m_pmx_data.s_pPmxMaterial[i].Diffuse[1]) = *((DWORD *)&mem[4]);
		*((DWORD *)&m_pmx_data.s_pPmxMaterial[i].Diffuse[2]) = *((DWORD *)&mem[8]);
		*((DWORD *)&m_pmx_data.s_pPmxMaterial[i].Diffuse[3]) = *((DWORD *)&mem[12]);
		mem += 16;

		*((DWORD *)&m_pmx_data.s_pPmxMaterial[i].Specular[0]) = *((DWORD *)&mem[0]);
		*((DWORD *)&m_pmx_data.s_pPmxMaterial[i].Specular[1]) = *((DWORD *)&mem[4]);
		*((DWORD *)&m_pmx_data.s_pPmxMaterial[i].Specular[2]) = *((DWORD *)&mem[8]);
		mem += 12;

		*((DWORD *)&m_pmx_data.s_pPmxMaterial[i].SpecularPower) = *((DWORD *)&mem[0]);
		mem += 4;

		*((DWORD *)&m_pmx_data.s_pPmxMaterial[i].Ambient[0]) = *((DWORD *)&mem[0]);
		*((DWORD *)&m_pmx_data.s_pPmxMaterial[i].Ambient[1]) = *((DWORD *)&mem[4]);
		*((DWORD *)&m_pmx_data.s_pPmxMaterial[i].Ambient[2]) = *((DWORD *)&mem[8]);
		mem += 12;

		m_pmx_data.s_pPmxMaterial[i].CullingOff = (*mem & 0x01) ? 1 : 0;
		m_pmx_data.s_pPmxMaterial[i].GroundShadow = (*mem & 0x02) ? 1 : 0;
		m_pmx_data.s_pPmxMaterial[i].SelfShadowMap = (*mem & 0x04) ? 1 : 0;
		m_pmx_data.s_pPmxMaterial[i].SelfShadowDraw = (*mem & 0x08) ? 1 : 0;
		m_pmx_data.s_pPmxMaterial[i].EdgeDraw = (*mem & 0x10) ? 1 : 0;
		mem++;

		*((DWORD *)&m_pmx_data.s_pPmxMaterial[i].EdgeColor[0]) = *((DWORD *)&mem[0]);
		*((DWORD *)&m_pmx_data.s_pPmxMaterial[i].EdgeColor[1]) = *((DWORD *)&mem[4]);
		*((DWORD *)&m_pmx_data.s_pPmxMaterial[i].EdgeColor[2]) = *((DWORD *)&mem[8]);
		*((DWORD *)&m_pmx_data.s_pPmxMaterial[i].EdgeColor[3]) = *((DWORD *)&mem[12]);
		mem += 16;

		*((DWORD *)&m_pmx_data.s_pPmxMaterial[i].EdgeSize) = *((DWORD *)mem);
		mem += 4;

		m_pmx_data.s_pPmxMaterial[i].TextureIndex = MV1LoadModelToPMX_GetInt(&mem, m_pmx_data.s_PmxInfo.TextureIndexSize);
		m_pmx_data.s_pPmxMaterial[i].SphereTextureIndex = MV1LoadModelToPMX_GetInt(&mem, m_pmx_data.s_PmxInfo.TextureIndexSize);

		m_pmx_data.s_pPmxMaterial[i].SphereMode = *mem;
		mem++;

		m_pmx_data.s_pPmxMaterial[i].ToonFlag = *mem;
		mem++;
		if (m_pmx_data.s_pPmxMaterial[i].ToonFlag == 0)
		{
			m_pmx_data.s_pPmxMaterial[i].ToonTextureIndex = MV1LoadModelToPMX_GetInt(&mem, m_pmx_data.s_PmxInfo.TextureIndexSize);
		}
		else
		{
			m_pmx_data.s_pPmxMaterial[i].ToonTextureIndex = *mem;
			mem++;
		}

		// メモはスキップ
		mem += *((DWORD *)mem) + 4;

		m_pmx_data.s_pPmxMaterial[i].MaterialFaceNum = *((int *)mem);
		mem += 4;
	}


	// ボーンの数を取得
	m_pmx_data.s_PmxBoneNum = *((DWORD *)mem);
	mem += 4;

	// ボーンデータを格納するメモリ領域の確保
	m_pmx_data.s_pPmxBone = (PMX_BONE *)malloc(sizeof(PMX_BONE) * m_pmx_data.s_PmxBoneNum);
	if (m_pmx_data.s_pPmxBone == NULL)
	{
		printf("PMX Load Error : ボーンデータの一時記憶領域の確保に失敗しました\n");
		OutputDebugString("\nPMX Load Error : ボーンデータの一時記憶領域の確保に失敗しました\n");
		goto ENDLABEL;
	}

	// ボーン情報の取得
	m_pmx_data.s_PmxIKNum = 0;
	for (i = 0; (DWORD)i < m_pmx_data.s_PmxBoneNum; i++)
	{
		MV1LoadModelToPMX_GetString(&mem, m_pmx_data.s_pPmxBone[i].Name, m_pmx_data.s_PmxInfo.EncodeType);
		if (m_pmx_data.s_pPmxBone[i].Name[0] == '\0')
		{
			MV1LoadModelToPMX_GetString(&mem, m_pmx_data.s_pPmxBone[i].Name, m_pmx_data.s_PmxInfo.EncodeType);
		}
		else
		{
			mem += *((DWORD *)mem) + 4;
		}

		*((DWORD *)&m_pmx_data.s_pPmxBone[i].Position[0]) = *((DWORD *)&mem[0]);
		*((DWORD *)&m_pmx_data.s_pPmxBone[i].Position[1]) = *((DWORD *)&mem[4]);
		*((DWORD *)&m_pmx_data.s_pPmxBone[i].Position[2]) = *((DWORD *)&mem[8]);
		mem += 12;

		m_pmx_data.s_pPmxBone[i].ParentBoneIndex = MV1LoadModelToPMX_GetInt(&mem, m_pmx_data.s_PmxInfo.BoneIndexSize);
		m_pmx_data.s_pPmxBone[i].TransformLayer = *((int *)mem);
		mem += 4;

		WORD Flag = *((WORD *)mem);
		mem += 2;
		m_pmx_data.s_pPmxBone[i].Flag_LinkDest = (Flag & 0x0001) != 0 ? 1 : 0;
		m_pmx_data.s_pPmxBone[i].Flag_EnableRot = (Flag & 0x0002) != 0 ? 1 : 0;
		m_pmx_data.s_pPmxBone[i].Flag_EnableMov = (Flag & 0x0004) != 0 ? 1 : 0;
		m_pmx_data.s_pPmxBone[i].Flag_Disp = (Flag & 0x0008) != 0 ? 1 : 0;
		m_pmx_data.s_pPmxBone[i].Flag_EnableControl = (Flag & 0x0010) != 0 ? 1 : 0;
		m_pmx_data.s_pPmxBone[i].Flag_IK = (Flag & 0x0020) != 0 ? 1 : 0;
		m_pmx_data.s_pPmxBone[i].Flag_AddRot = (Flag & 0x0100) != 0 ? 1 : 0;
		m_pmx_data.s_pPmxBone[i].Flag_AddMov = (Flag & 0x0200) != 0 ? 1 : 0;
		m_pmx_data.s_pPmxBone[i].Flag_LockAxis = (Flag & 0x0400) != 0 ? 1 : 0;
		m_pmx_data.s_pPmxBone[i].Flag_LocalAxis = (Flag & 0x0800) != 0 ? 1 : 0;
		m_pmx_data.s_pPmxBone[i].Flag_AfterPhysicsTransform = (Flag & 0x1000) != 0 ? 1 : 0;
		m_pmx_data.s_pPmxBone[i].Flag_OutParentTransform = (Flag & 0x2000) != 0 ? 1 : 0;

		if (m_pmx_data.s_pPmxBone[i].Flag_LinkDest == 0)
		{
			*((DWORD *)&m_pmx_data.s_pPmxBone[i].OffsetPosition[0]) = *((DWORD *)&mem[0]);
			*((DWORD *)&m_pmx_data.s_pPmxBone[i].OffsetPosition[1]) = *((DWORD *)&mem[4]);
			*((DWORD *)&m_pmx_data.s_pPmxBone[i].OffsetPosition[2]) = *((DWORD *)&mem[8]);
			mem += 12;
		}
		else
		{
			m_pmx_data.s_pPmxBone[i].LinkBoneIndex = MV1LoadModelToPMX_GetInt(&mem, m_pmx_data.s_PmxInfo.BoneIndexSize);
		}

		if (m_pmx_data.s_pPmxBone[i].Flag_AddRot == 1 || m_pmx_data.s_pPmxBone[i].Flag_AddMov == 1)
		{
			m_pmx_data.s_pPmxBone[i].AddParentBoneIndex = MV1LoadModelToPMX_GetInt(&mem, m_pmx_data.s_PmxInfo.BoneIndexSize);
			*((DWORD *)&m_pmx_data.s_pPmxBone[i].AddRatio) = *((DWORD *)mem);
			mem += 4;
		}

		if (m_pmx_data.s_pPmxBone[i].Flag_LockAxis == 1)
		{
			*((DWORD *)&m_pmx_data.s_pPmxBone[i].LockAxisVector[0]) = *((DWORD *)&mem[0]);
			*((DWORD *)&m_pmx_data.s_pPmxBone[i].LockAxisVector[1]) = *((DWORD *)&mem[4]);
			*((DWORD *)&m_pmx_data.s_pPmxBone[i].LockAxisVector[2]) = *((DWORD *)&mem[8]);
			mem += 12;
		}

		if (m_pmx_data.s_pPmxBone[i].Flag_LocalAxis == 1)
		{
			*((DWORD *)&m_pmx_data.s_pPmxBone[i].LocalAxisXVector[0]) = *((DWORD *)&mem[0]);
			*((DWORD *)&m_pmx_data.s_pPmxBone[i].LocalAxisXVector[1]) = *((DWORD *)&mem[4]);
			*((DWORD *)&m_pmx_data.s_pPmxBone[i].LocalAxisXVector[2]) = *((DWORD *)&mem[8]);
			mem += 12;

			*((DWORD *)&m_pmx_data.s_pPmxBone[i].LocalAxisZVector[0]) = *((DWORD *)&mem[0]);
			*((DWORD *)&m_pmx_data.s_pPmxBone[i].LocalAxisZVector[1]) = *((DWORD *)&mem[4]);
			*((DWORD *)&m_pmx_data.s_pPmxBone[i].LocalAxisZVector[2]) = *((DWORD *)&mem[8]);
			mem += 12;
		}

		if (m_pmx_data.s_pPmxBone[i].Flag_OutParentTransform == 1)
		{
			m_pmx_data.s_pPmxBone[i].OutParentTransformKey = *((int *)mem);
			mem += 4;
		}

		if (m_pmx_data.s_pPmxBone[i].Flag_IK == 1)
		{
			m_pmx_data.s_PmxIKNum++;

			m_pmx_data.s_pPmxBone[i].IKInfo.TargetBoneIndex = MV1LoadModelToPMX_GetInt(&mem, m_pmx_data.s_PmxInfo.BoneIndexSize);
			m_pmx_data.s_pPmxBone[i].IKInfo.LoopNum = *((int *)mem);
			mem += 4;

			*((DWORD *)&m_pmx_data.s_pPmxBone[i].IKInfo.RotLimit) = *((DWORD *)mem);
			mem += 4;

			m_pmx_data.s_pPmxBone[i].IKInfo.LinkNum = *((int *)mem);
			mem += 4;
			if (m_pmx_data.s_pPmxBone[i].IKInfo.LinkNum >= PMX_MAX_IKLINKNUM)
			{
				printf("PMX Load Error : ＩＫリンクの数が対応数を超えています\n");
				OutputDebugString("\nPMX Load Error : ＩＫリンクの数が対応数を超えています\n");
				goto ENDLABEL;
			}

			for (j = 0; j < m_pmx_data.s_pPmxBone[i].IKInfo.LinkNum; j++)
			{
				m_pmx_data.s_pPmxBone[i].IKInfo.Link[j].BoneIndex = MV1LoadModelToPMX_GetInt(&mem, m_pmx_data.s_PmxInfo.BoneIndexSize);
				m_pmx_data.s_pPmxBone[i].IKInfo.Link[j].RotLockFlag = mem[0];
				mem++;

				if (m_pmx_data.s_pPmxBone[i].IKInfo.Link[j].RotLockFlag == 1)
				{
					*((DWORD *)&m_pmx_data.s_pPmxBone[i].IKInfo.Link[j].RotLockMin[0]) = *((DWORD *)&mem[0]);
					*((DWORD *)&m_pmx_data.s_pPmxBone[i].IKInfo.Link[j].RotLockMin[1]) = *((DWORD *)&mem[4]);
					*((DWORD *)&m_pmx_data.s_pPmxBone[i].IKInfo.Link[j].RotLockMin[2]) = *((DWORD *)&mem[8]);
					mem += 12;

					*((DWORD *)&m_pmx_data.s_pPmxBone[i].IKInfo.Link[j].RotLockMax[0]) = *((DWORD *)&mem[0]);
					*((DWORD *)&m_pmx_data.s_pPmxBone[i].IKInfo.Link[j].RotLockMax[1]) = *((DWORD *)&mem[4]);
					*((DWORD *)&m_pmx_data.s_pPmxBone[i].IKInfo.Link[j].RotLockMax[2]) = *((DWORD *)&mem[8]);
					mem += 12;
				}
			}
		}
	}


	// モーフ情報の数を取得
	m_pmx_data.s_PmxMorphNum = *((DWORD *)mem);
	mem += 4;

	// モーフデータを格納するメモリ領域の確保
	m_pmx_data.s_pPmxMorph = (PMX_MORPH *)malloc(sizeof(PMX_MORPH) * m_pmx_data.s_PmxMorphNum);
	if (m_pmx_data.s_pPmxMorph == NULL)
	{
		printf("PMX Load Error : モーフデータの一時記憶領域の確保に失敗しました\n");
		OutputDebugString("\nPMX Load Error : モーフデータの一時記憶領域の確保に失敗しました\n");
		goto ENDLABEL;
	}

	// モーフ情報の読み込み
	m_pmx_data.s_PmxSkinNum = 0;
	for (i = 0; (DWORD)i < m_pmx_data.s_PmxMorphNum; i++)
	{
		MV1LoadModelToPMX_GetString(&mem, m_pmx_data.s_pPmxMorph[i].Name, m_pmx_data.s_PmxInfo.EncodeType);
		if (m_pmx_data.s_pPmxMorph[i].Name[0] == '\0')
		{
			MV1LoadModelToPMX_GetString(&mem, m_pmx_data.s_pPmxMorph[i].Name, m_pmx_data.s_PmxInfo.EncodeType);
		}
		else
		{
			mem += *((DWORD *)mem) + 4;
		}

		m_pmx_data.s_pPmxMorph[i].ControlPanel = mem[0];
		m_pmx_data.s_pPmxMorph[i].Type = mem[1];
		m_pmx_data.s_pPmxMorph[i].DataNum = *((int *)&mem[2]);
		mem += 6;

		switch (m_pmx_data.s_pPmxMorph[i].Type)
		{
		case 0:	// グループモーフ
			m_pmx_data.s_pPmxMorph[i].Group = (PMX_MORPH_GROUP *)malloc(sizeof(PMX_MORPH_GROUP) * m_pmx_data.s_pPmxMorph[i].DataNum);
			if (m_pmx_data.s_pPmxMorph[i].Group == NULL)
			{
				printf("PMX Load Error : グループモーフデータの一時記憶領域の確保に失敗しました\n");
				OutputDebugString("\nPMX Load Error : グループモーフデータの一時記憶領域の確保に失敗しました\n");
					
				goto ENDLABEL;
			}

			for (j = 0; j < m_pmx_data.s_pPmxMorph[i].DataNum; j++)
			{
				m_pmx_data.s_pPmxMorph[i].Group[j].Index = MV1LoadModelToPMX_GetInt(&mem, m_pmx_data.s_PmxInfo.MorphIndexSize);
				*((DWORD *)&m_pmx_data.s_pPmxMorph[i].Group[j].Ratio) = *((DWORD *)mem);
				mem += 4;
			}
			break;

		case 1:	// 頂点
			m_pmx_data.s_PmxSkinNum++;
			m_pmx_data.s_pPmxMorph[i].Vertex = (PMX_MORPH_VERTEX *)malloc(sizeof(PMX_MORPH_VERTEX) * m_pmx_data.s_pPmxMorph[i].DataNum);
			if (m_pmx_data.s_pPmxMorph[i].Vertex == NULL)
			{
				printf("PMX Load Error : 頂点モーフデータの一時記憶領域の確保に失敗しました\n");
				OutputDebugString("\nPMX Load Error : 頂点モーフデータの一時記憶領域の確保に失敗しました\n");
				goto ENDLABEL;
			}

			for (j = 0; j < m_pmx_data.s_pPmxMorph[i].DataNum; j++)
			{
				m_pmx_data.s_pPmxMorph[i].Vertex[j].Index = MV1LoadModelToPMX_GetUInt(&mem, m_pmx_data.s_PmxInfo.VertexIndexSize);
				*((DWORD *)&m_pmx_data.s_pPmxMorph[i].Vertex[j].Offset[0]) = *((DWORD *)&mem[0]);
				*((DWORD *)&m_pmx_data.s_pPmxMorph[i].Vertex[j].Offset[1]) = *((DWORD *)&mem[4]);
				*((DWORD *)&m_pmx_data.s_pPmxMorph[i].Vertex[j].Offset[2]) = *((DWORD *)&mem[8]);
				mem += 12;
			}
			break;

		case 2:	// ボーンモーフ
			m_pmx_data.s_pPmxMorph[i].Bone = (PMX_MORPH_BONE *)malloc(sizeof(PMX_MORPH_BONE) * m_pmx_data.s_pPmxMorph[i].DataNum);
			if (m_pmx_data.s_pPmxMorph[i].Bone == NULL)
			{
				printf("PMX Load Error : ボーンモーフデータの一時記憶領域の確保に失敗しました\n");
				OutputDebugString("\nPMX Load Error : ボーンモーフデータの一時記憶領域の確保に失敗しました\n");
				goto ENDLABEL;
			}

			for (j = 0; j < m_pmx_data.s_pPmxMorph[i].DataNum; j++)
			{
				m_pmx_data.s_pPmxMorph[i].Bone[j].Index = MV1LoadModelToPMX_GetInt(&mem, m_pmx_data.s_PmxInfo.BoneIndexSize);
				*((DWORD *)&m_pmx_data.s_pPmxMorph[i].Bone[j].Offset[0]) = *((DWORD *)&mem[0]);
				*((DWORD *)&m_pmx_data.s_pPmxMorph[i].Bone[j].Offset[1]) = *((DWORD *)&mem[4]);
				*((DWORD *)&m_pmx_data.s_pPmxMorph[i].Bone[j].Offset[2]) = *((DWORD *)&mem[8]);
				mem += 12;

				*((DWORD *)&m_pmx_data.s_pPmxMorph[i].Bone[j].Quat[0]) = *((DWORD *)&mem[0]);
				*((DWORD *)&m_pmx_data.s_pPmxMorph[i].Bone[j].Quat[1]) = *((DWORD *)&mem[4]);
				*((DWORD *)&m_pmx_data.s_pPmxMorph[i].Bone[j].Quat[2]) = *((DWORD *)&mem[8]);
				*((DWORD *)&m_pmx_data.s_pPmxMorph[i].Bone[j].Quat[3]) = *((DWORD *)&mem[12]);
				mem += 16;
			}
			break;

		case 3:	// UVモーフ
		case 4:	// 追加UV1モーフ
		case 5:	// 追加UV2モーフ
		case 6:	// 追加UV3モーフ
		case 7:	// 追加UV4モーフ
			m_pmx_data.s_pPmxMorph[i].UV = (PMX_MORPH_UV *)malloc(sizeof(PMX_MORPH_UV) * m_pmx_data.s_pPmxMorph[i].DataNum);
			if (m_pmx_data.s_pPmxMorph[i].UV == NULL)
			{
				printf("PMX Load Error : UVモーフデータの一時記憶領域の確保に失敗しました\n");
				OutputDebugString("\nPMX Load Error : UVモーフデータの一時記憶領域の確保に失敗しました\n");
				goto ENDLABEL;
			}

			for (j = 0; j < m_pmx_data.s_pPmxMorph[i].DataNum; j++)
			{
				m_pmx_data.s_pPmxMorph[i].UV[j].Index = MV1LoadModelToPMX_GetUInt(&mem, m_pmx_data.s_PmxInfo.VertexIndexSize);
				*((DWORD *)&m_pmx_data.s_pPmxMorph[i].UV[j].Offset.x) = *((DWORD *)&mem[0]);
				*((DWORD *)&m_pmx_data.s_pPmxMorph[i].UV[j].Offset.y) = *((DWORD *)&mem[4]);
				*((DWORD *)&m_pmx_data.s_pPmxMorph[i].UV[j].Offset.z) = *((DWORD *)&mem[8]);
				*((DWORD *)&m_pmx_data.s_pPmxMorph[i].UV[j].Offset.w) = *((DWORD *)&mem[12]);
				mem += 16;
			}
			break;

		case 8:	// 材質モーフ
			m_pmx_data.s_pPmxMorph[i].Material = (PMX_MORPH_MATERIAL *)malloc(sizeof(PMX_MORPH_MATERIAL) * m_pmx_data.s_pPmxMorph[i].DataNum);
			if (m_pmx_data.s_pPmxMorph[i].Material == NULL)
			{
				printf("PMX Load Error : マテリアルモーフデータの一時記憶領域の確保に失敗しました\n");
				OutputDebugString("\nPMX Load Error : マテリアルモーフデータの一時記憶領域の確保に失敗しました\n");
				goto ENDLABEL;
			}

			for (j = 0; j < m_pmx_data.s_pPmxMorph[i].DataNum; j++)
			{
				m_pmx_data.s_pPmxMorph[i].Material[j].Index = MV1LoadModelToPMX_GetInt(&mem, m_pmx_data.s_PmxInfo.MaterialIndexSize);
				m_pmx_data.s_pPmxMorph[i].Material[j].CalcType = mem[0];
				mem++;

				*((DWORD *)&m_pmx_data.s_pPmxMorph[i].Material[j].Diffuse[0]) = *((DWORD *)&mem[0]);
				*((DWORD *)&m_pmx_data.s_pPmxMorph[i].Material[j].Diffuse[1]) = *((DWORD *)&mem[4]);
				*((DWORD *)&m_pmx_data.s_pPmxMorph[i].Material[j].Diffuse[2]) = *((DWORD *)&mem[8]);
				*((DWORD *)&m_pmx_data.s_pPmxMorph[i].Material[j].Diffuse[3]) = *((DWORD *)&mem[12]);
				mem += 16;

				*((DWORD *)&m_pmx_data.s_pPmxMorph[i].Material[j].Specular[0]) = *((DWORD *)&mem[0]);
				*((DWORD *)&m_pmx_data.s_pPmxMorph[i].Material[j].Specular[1]) = *((DWORD *)&mem[4]);
				*((DWORD *)&m_pmx_data.s_pPmxMorph[i].Material[j].Specular[2]) = *((DWORD *)&mem[8]);
				mem += 12;

				*((DWORD *)&m_pmx_data.s_pPmxMorph[i].Material[j].SpecularPower) = *((DWORD *)&mem[0]);
				mem += 4;

				*((DWORD *)&m_pmx_data.s_pPmxMorph[i].Material[j].Ambient[0]) = *((DWORD *)&mem[0]);
				*((DWORD *)&m_pmx_data.s_pPmxMorph[i].Material[j].Ambient[1]) = *((DWORD *)&mem[4]);
				*((DWORD *)&m_pmx_data.s_pPmxMorph[i].Material[j].Ambient[2]) = *((DWORD *)&mem[8]);
				mem += 12;

				*((DWORD *)&m_pmx_data.s_pPmxMorph[i].Material[j].EdgeColor[0]) = *((DWORD *)&mem[0]);
				*((DWORD *)&m_pmx_data.s_pPmxMorph[i].Material[j].EdgeColor[1]) = *((DWORD *)&mem[4]);
				*((DWORD *)&m_pmx_data.s_pPmxMorph[i].Material[j].EdgeColor[2]) = *((DWORD *)&mem[8]);
				*((DWORD *)&m_pmx_data.s_pPmxMorph[i].Material[j].EdgeColor[3]) = *((DWORD *)&mem[12]);
				mem += 16;

				*((DWORD *)&m_pmx_data.s_pPmxMorph[i].Material[j].EdgeSize) = *((DWORD *)&mem[0]);
				mem += 4;

				*((DWORD *)&m_pmx_data.s_pPmxMorph[i].Material[j].TextureScale[0]) = *((DWORD *)&mem[0]);
				*((DWORD *)&m_pmx_data.s_pPmxMorph[i].Material[j].TextureScale[1]) = *((DWORD *)&mem[4]);
				*((DWORD *)&m_pmx_data.s_pPmxMorph[i].Material[j].TextureScale[2]) = *((DWORD *)&mem[8]);
				*((DWORD *)&m_pmx_data.s_pPmxMorph[i].Material[j].TextureScale[3]) = *((DWORD *)&mem[12]);
				mem += 16;

				*((DWORD *)&m_pmx_data.s_pPmxMorph[i].Material[j].SphereTextureScale[0]) = *((DWORD *)&mem[0]);
				*((DWORD *)&m_pmx_data.s_pPmxMorph[i].Material[j].SphereTextureScale[1]) = *((DWORD *)&mem[4]);
				*((DWORD *)&m_pmx_data.s_pPmxMorph[i].Material[j].SphereTextureScale[2]) = *((DWORD *)&mem[8]);
				*((DWORD *)&m_pmx_data.s_pPmxMorph[i].Material[j].SphereTextureScale[3]) = *((DWORD *)&mem[12]);
				mem += 16;

				*((DWORD *)&m_pmx_data.s_pPmxMorph[i].Material[j].ToonTextureScale[0]) = *((DWORD *)&mem[0]);
				*((DWORD *)&m_pmx_data.s_pPmxMorph[i].Material[j].ToonTextureScale[1]) = *((DWORD *)&mem[4]);
				*((DWORD *)&m_pmx_data.s_pPmxMorph[i].Material[j].ToonTextureScale[2]) = *((DWORD *)&mem[8]);
				*((DWORD *)&m_pmx_data.s_pPmxMorph[i].Material[j].ToonTextureScale[3]) = *((DWORD *)&mem[12]);
				mem += 16;
			}
			break;
		}
	}


	// 表示枠データの読み込みはスキップ
	{
		DWORD PmxDispFrameNum;
		DWORD Num;
		BYTE Target;

		// 表示枠データの数を取得
		PmxDispFrameNum = *((DWORD *)mem);
		mem += 4;

		// 枠の数だけ繰り返し
		for (i = 0; (DWORD)i < PmxDispFrameNum; i++)
		{
			// 名前読み飛ばし
			mem += *((DWORD *)mem) + 4;
			mem += *((DWORD *)mem) + 4;

			// 特殊枠フラグ読み飛ばし
			mem++;

			// 枠内要素の数を取得
			Num = *((DWORD *)mem);
			mem += 4;

			// 枠内要素の読み飛ばし
			for (j = 0; (DWORD)j < Num; j++)
			{
				// 要素対象を取得
				Target = mem[0];
				mem++;

				// 要素対象によって処理を分岐
				switch (Target)
				{
				case 0:	// ボーンがターゲットの場合
					MV1LoadModelToPMX_GetInt(&mem, m_pmx_data.s_PmxInfo.BoneIndexSize);
					break;

				case 1:	// モーフがターゲットの場合
					MV1LoadModelToPMX_GetInt(&mem, m_pmx_data.s_PmxInfo.MorphIndexSize);
					break;
				}
			}
		}
	}


	// 剛体情報の数を取得
	m_pmx_data.s_PmxRigidbodyNum = *((DWORD *)mem);
	mem += 4;

	// 剛体データを格納するメモリ領域の確保
	m_pmx_data.s_pPmxRigidbody = (PMX_RIGIDBODY *)malloc(sizeof(PMX_RIGIDBODY) * m_pmx_data.s_PmxRigidbodyNum);
	if (m_pmx_data.s_pPmxRigidbody == NULL)
	{
		printf("PMX Load Error : 剛体データの一時記憶領域の確保に失敗しました\n");
		OutputDebugString("\nPMX Load Error : 剛体データの一時記憶領域の確保に失敗しました\n");
		goto ENDLABEL;
	}

	// 剛体情報の読み込み
	for (i = 0; (DWORD)i < m_pmx_data.s_PmxRigidbodyNum; i++)
	{
		MV1LoadModelToPMX_GetString(&mem, m_pmx_data.s_pPmxRigidbody[i].Name, m_pmx_data.s_PmxInfo.EncodeType);
		if (m_pmx_data.s_pPmxRigidbody[i].Name[0] == '\0')
		{
			MV1LoadModelToPMX_GetString(&mem, m_pmx_data.s_pPmxRigidbody[i].Name, m_pmx_data.s_PmxInfo.EncodeType);
		}
		else
		{
			mem += *((DWORD *)mem) + 4;
		}

		m_pmx_data.s_pPmxRigidbody[i].BoneIndex = MV1LoadModelToPMX_GetInt(&mem, m_pmx_data.s_PmxInfo.BoneIndexSize);

		m_pmx_data.s_pPmxRigidbody[i].RigidBodyGroupIndex = mem[0];
		mem++;

		*((WORD *)&m_pmx_data.s_pPmxRigidbody[i].RigidBodyGroupTarget) = *((WORD *)&mem[0]);
		mem += 2;

		m_pmx_data.s_pPmxRigidbody[i].ShapeType = mem[0];
		mem++;

		*((DWORD *)&m_pmx_data.s_pPmxRigidbody[i].ShapeW) = *((DWORD *)&mem[0]);
		*((DWORD *)&m_pmx_data.s_pPmxRigidbody[i].ShapeH) = *((DWORD *)&mem[4]);
		*((DWORD *)&m_pmx_data.s_pPmxRigidbody[i].ShapeD) = *((DWORD *)&mem[8]);
		mem += 12;

		*((DWORD *)&m_pmx_data.s_pPmxRigidbody[i].Position[0]) = *((DWORD *)&mem[0]);
		*((DWORD *)&m_pmx_data.s_pPmxRigidbody[i].Position[1]) = *((DWORD *)&mem[4]);
		*((DWORD *)&m_pmx_data.s_pPmxRigidbody[i].Position[2]) = *((DWORD *)&mem[8]);
		mem += 12;

		*((DWORD *)&m_pmx_data.s_pPmxRigidbody[i].Rotation[0]) = *((DWORD *)&mem[0]);
		*((DWORD *)&m_pmx_data.s_pPmxRigidbody[i].Rotation[1]) = *((DWORD *)&mem[4]);
		*((DWORD *)&m_pmx_data.s_pPmxRigidbody[i].Rotation[2]) = *((DWORD *)&mem[8]);
		mem += 12;

		*((DWORD *)&m_pmx_data.s_pPmxRigidbody[i].RigidBodyWeight) = *((DWORD *)&mem[0]);
		*((DWORD *)&m_pmx_data.s_pPmxRigidbody[i].RigidBodyPosDim) = *((DWORD *)&mem[4]);
		*((DWORD *)&m_pmx_data.s_pPmxRigidbody[i].RigidBodyRotDim) = *((DWORD *)&mem[8]);
		*((DWORD *)&m_pmx_data.s_pPmxRigidbody[i].RigidBodyRecoil) = *((DWORD *)&mem[12]);
		*((DWORD *)&m_pmx_data.s_pPmxRigidbody[i].RigidBodyFriction) = *((DWORD *)&mem[16]);
		mem += 20;

		m_pmx_data.s_pPmxRigidbody[i].RigidBodyType = mem[0];
		mem++;
	}


	// ジョイント情報の数を取得
	m_pmx_data.s_PmxJointNum = *((DWORD *)mem);
	mem += 4;

	// ジョイントデータを格納するメモリ領域の確保
	if (m_pmx_data.s_PmxJointNum != 0)
	{
		m_pmx_data.s_pPmxJoint = (PMX_JOINT *)malloc(sizeof(PMX_JOINT) * m_pmx_data.s_PmxJointNum);
		if (m_pmx_data.s_pPmxJoint == NULL)
		{
			printf("PMX Load Error : ジョイントデータの一時記憶領域の確保に失敗しました\n");
			OutputDebugString("\nPMX Load Error : ジョイントデータの一時記憶領域の確保に失敗しました\n");
			goto ENDLABEL;
		}

		// ジョイント情報の読み込み
		for (i = 0; (DWORD)i < m_pmx_data.s_PmxJointNum; i++)
		{
			MV1LoadModelToPMX_GetString(&mem, m_pmx_data.s_pPmxJoint[i].Name, m_pmx_data.s_PmxInfo.EncodeType);
			if (m_pmx_data.s_pPmxJoint[i].Name[0] == '\0')
			{
				MV1LoadModelToPMX_GetString(&mem, m_pmx_data.s_pPmxJoint[i].Name, m_pmx_data.s_PmxInfo.EncodeType);
			}
			else
			{
				mem += *((DWORD *)mem) + 4;
			}

			m_pmx_data.s_pPmxJoint[i].Type = mem[0];
			mem++;

			m_pmx_data.s_pPmxJoint[i].RigidBodyAIndex = MV1LoadModelToPMX_GetInt(&mem, m_pmx_data.s_PmxInfo.RigidIndexSize);
			m_pmx_data.s_pPmxJoint[i].RigidBodyBIndex = MV1LoadModelToPMX_GetInt(&mem, m_pmx_data.s_PmxInfo.RigidIndexSize);

			*((DWORD *)&m_pmx_data.s_pPmxJoint[i].Position[0]) = *((DWORD *)&mem[0]);
			*((DWORD *)&m_pmx_data.s_pPmxJoint[i].Position[1]) = *((DWORD *)&mem[4]);
			*((DWORD *)&m_pmx_data.s_pPmxJoint[i].Position[2]) = *((DWORD *)&mem[8]);
			mem += 12;

			*((DWORD *)&m_pmx_data.s_pPmxJoint[i].Rotation[0]) = *((DWORD *)&mem[0]);
			*((DWORD *)&m_pmx_data.s_pPmxJoint[i].Rotation[1]) = *((DWORD *)&mem[4]);
			*((DWORD *)&m_pmx_data.s_pPmxJoint[i].Rotation[2]) = *((DWORD *)&mem[8]);
			mem += 12;

			*((DWORD *)&m_pmx_data.s_pPmxJoint[i].ConstrainPositionMin[0]) = *((DWORD *)&mem[0]);
			*((DWORD *)&m_pmx_data.s_pPmxJoint[i].ConstrainPositionMin[1]) = *((DWORD *)&mem[4]);
			*((DWORD *)&m_pmx_data.s_pPmxJoint[i].ConstrainPositionMin[2]) = *((DWORD *)&mem[8]);
			mem += 12;

			*((DWORD *)&m_pmx_data.s_pPmxJoint[i].ConstrainPositionMax[0]) = *((DWORD *)&mem[0]);
			*((DWORD *)&m_pmx_data.s_pPmxJoint[i].ConstrainPositionMax[1]) = *((DWORD *)&mem[4]);
			*((DWORD *)&m_pmx_data.s_pPmxJoint[i].ConstrainPositionMax[2]) = *((DWORD *)&mem[8]);
			mem += 12;

			*((DWORD *)&m_pmx_data.s_pPmxJoint[i].ConstrainRotationMin[0]) = *((DWORD *)&mem[0]);
			*((DWORD *)&m_pmx_data.s_pPmxJoint[i].ConstrainRotationMin[1]) = *((DWORD *)&mem[4]);
			*((DWORD *)&m_pmx_data.s_pPmxJoint[i].ConstrainRotationMin[2]) = *((DWORD *)&mem[8]);
			mem += 12;

			*((DWORD *)&m_pmx_data.s_pPmxJoint[i].ConstrainRotationMax[0]) = *((DWORD *)&mem[0]);
			*((DWORD *)&m_pmx_data.s_pPmxJoint[i].ConstrainRotationMax[1]) = *((DWORD *)&mem[4]);
			*((DWORD *)&m_pmx_data.s_pPmxJoint[i].ConstrainRotationMax[2]) = *((DWORD *)&mem[8]);
			mem += 12;

			*((DWORD *)&m_pmx_data.s_pPmxJoint[i].SpringPosition[0]) = *((DWORD *)&mem[0]);
			*((DWORD *)&m_pmx_data.s_pPmxJoint[i].SpringPosition[1]) = *((DWORD *)&mem[4]);
			*((DWORD *)&m_pmx_data.s_pPmxJoint[i].SpringPosition[2]) = *((DWORD *)&mem[8]);
			mem += 12;

			*((DWORD *)&m_pmx_data.s_pPmxJoint[i].SpringRotation[0]) = *((DWORD *)&mem[0]);
			*((DWORD *)&m_pmx_data.s_pPmxJoint[i].SpringRotation[1]) = *((DWORD *)&mem[4]);
			*((DWORD *)&m_pmx_data.s_pPmxJoint[i].SpringRotation[2]) = *((DWORD *)&mem[8]);
			mem += 12;
		}
	}

	free(mem_head);//解放
	OutputDebugString("\n正常にデータ格納が出来ました。\n\n");
	return true;
ENDLABEL://確保に失敗したら通る
	free(mem_head);
	OutputDebugString("\n正常に達成できませんでした上記が理由です。\n");
	return false;
}