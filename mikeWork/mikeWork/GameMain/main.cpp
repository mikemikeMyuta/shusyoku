#include <windows.h>
#include "../Director/Director.h"

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, INT)
{
	Director* pDirector = new Director;
	if (pDirector == NULL)
	{
		MessageBox(0, "クラス生成失敗　アプリを終了します", NULL, MB_OK);
		return 0;
	}
	//進行はディレクターに
	pDirector->Run(hInstance);

	//アプリ終了
	delete pDirector;

	return 0;
}