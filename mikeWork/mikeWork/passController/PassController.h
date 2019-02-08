#pragma once
#include <Windows.h>
#include <string>

using namespace std;

//============================================================
//!	@class	CPathControll
//!	@brief	�p�X����p�N���X�ł���������
//============================================================
class CPathControll
{
private:
	CPathControll(void) {
	};
public:
	//�V���O���g���ł̎g�p
	static CPathControll& Instance(void) {
		static CPathControll inst;
		return inst;
	}

	string GetAlternatePath(LPCSTR _src)//�����ꏊ��
	{
		int path_found = 0;
		string path = _src;
		string slash = "/";
		string newpath;
		for (unsigned int i = 0; i < path.length(); ++i) //texture�̂���ꏊ�ɂ��邽�߁@�A�h���X����K�w�O�ɂ���@�i�����texture��PMX�Ɠ����Ƃ���ɒu�����i�t�@�C���ł������j
		{
			if (path.at(i) == slash.at(0))//���������slash������Ȃ�
			{
				path_found = i;//�Ō��slash�ɂȂ�悤�ɒ���
			}
		}
		if (path_found != 0)//�ŏ���slash����Ȃ��Ȃ�
		{
			newpath = path.substr(0, path_found + 1);//slash�܂ł̕�����Ԃ�
			return newpath;
		}
		else
		{
			return path;//�󂯎�������̂����̂܂ܕԂ�
		}
	}

};
//EOF