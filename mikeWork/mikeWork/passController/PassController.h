#pragma once
#include <Windows.h>
#include <string>

using namespace std;

//============================================================
//!	@class	CPathControll
//!	@brief	パス操作用クラスですしおすし
//============================================================
class CPathControll
{
private:
	CPathControll(void) {
	};
public:
	//シングルトンでの使用
	static CPathControll& Instance(void) {
		static CPathControll inst;
		return inst;
	}

	string GetAlternatePath(LPCSTR _src)//同じ場所に
	{
		int path_found = 0;
		string path = _src;
		string slash = "/";
		string newpath;
		for (unsigned int i = 0; i < path.length(); ++i) //textureのある場所にするため　アドレスを一階層前にする　（よってtextureはPMXと同じところに置こう（ファイルでも同じ）
		{
			if (path.at(i) == slash.at(0))//文字列内にslashがあるなら
			{
				path_found = i;//最後のslashになるように調整
			}
		}
		if (path_found != 0)//最初がslashじゃないなら
		{
			newpath = path.substr(0, path_found + 1);//slashまでの部分を返す
			return newpath;
		}
		else
		{
			return path;//受け取ったものをそのまま返す
		}
	}

};
//EOF