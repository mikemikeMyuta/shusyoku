#include <windows.h>
#include "../Director/Director.h"

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, INT)
{
	Director* pDirector = new Director;
	if (pDirector == NULL)
	{
		MessageBox(0, "�N���X�������s�@�A�v�����I�����܂�", NULL, MB_OK);
		return 0;
	}
	//�i�s�̓f�B���N�^�[��
	pDirector->Run(hInstance);

	//�A�v���I��
	delete pDirector;

	return 0;
}