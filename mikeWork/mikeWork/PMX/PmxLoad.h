#pragma once

#include "PmxStruct.h"

using namespace PmxStruct;
//============================================================
//!	@class	CLoadPMX
//!	@brief	PMXファイルロードクラス
//============================================================
class CLoadPMX
{
protected:
	PMX_DATA m_pmx_data;
public:
	CLoadPMX() {
		m_pmx_data.s_pPmxVertex = NULL;
		m_pmx_data.s_pPmxFace = NULL;
		m_pmx_data.s_pPmxTexture = NULL;
		m_pmx_data.s_pPmxMaterial = NULL;
		m_pmx_data.s_pPmxBone = NULL;
		m_pmx_data.s_pPmxMorph = NULL;
		m_pmx_data.s_pPmxRigidbody = NULL;
		m_pmx_data.s_pPmxJoint = NULL;
	};
	virtual ~CLoadPMX() {
		//全部開放
		if (m_pmx_data.s_pPmxVertex)
		{
			free(m_pmx_data.s_pPmxVertex);
		}
		if (m_pmx_data.s_pPmxFace)
		{
			free(m_pmx_data.s_pPmxFace);
		}
		if (m_pmx_data.s_pPmxTexture)
		{
			free(m_pmx_data.s_pPmxTexture);
		}
		if (m_pmx_data.s_pPmxMaterial)
		{
			free(m_pmx_data.s_pPmxMaterial);
		}
		if (m_pmx_data.s_pPmxBone)
		{
			free(m_pmx_data.s_pPmxBone);
		}

		for (int i = 0; (DWORD)i < m_pmx_data.s_PmxMorphNum; i++)
		{
			switch (m_pmx_data.s_pPmxMorph[i].Type)
			{
			case 0:	// グループモーフ
				free(m_pmx_data.s_pPmxMorph[i].Group);
				break;
			case 1:	// 頂点
				free(m_pmx_data.s_pPmxMorph[i].Vertex);
				break;
			case 2:	// ボーンモーフ
				free(m_pmx_data.s_pPmxMorph[i].Bone);
				break;
			case 3:	// UVモーフ
			case 4:	// 追加UV1モーフ
			case 5:	// 追加UV2モーフ
			case 6:	// 追加UV3モーフ
			case 7:	// 追加UV4モーフ
				free(m_pmx_data.s_pPmxMorph[i].UV);
				break;
			case 8:	// 材質モーフ
				free(m_pmx_data.s_pPmxMorph[i].Material);
				break;
			}
		}
		if (m_pmx_data.s_pPmxMorph)
		{
			free(m_pmx_data.s_pPmxMorph);
		}
		if (m_pmx_data.s_pPmxRigidbody)
		{
			free(m_pmx_data.s_pPmxRigidbody);
		}
		if (m_pmx_data.s_pPmxJoint)
		{

			free(m_pmx_data.s_pPmxJoint);
		}
	};

public:
	bool LoadPMXFile(LPCSTR _src);
	PMX_DATA* GetPmxData(void) {
		return &m_pmx_data;
	}
};
