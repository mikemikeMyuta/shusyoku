#include "BASE.h"
#pragma warning (disable:4996)

Base::Base()
{
	ZeroMemory(this, sizeof(Base));
	for (int i = 0;i < 1124;i++)
	{
		szRootPath[i] = 0;
	}
}

void Base::InitDirectory(char* root)
{
	strcpy(szRootPath, root);
}

//
//
//
void Base::SetRootDirectory()
{
	SetCurrentDirectory(szRootPath);
}
//
//
//
void Base::SetSoundDirectory()
{
	char tmp[512] = { 0 };
	strcpy(tmp, szRootPath);
	strcat(tmp, SOUND_DIRECTORY);
	SetCurrentDirectory(tmp);
}
//
//
//
void Base::SetTexDirectory()
{
	char tmp[512] = { 0 };
	strcpy(tmp, szRootPath);
	strcat(tmp, TEX_DIRECTORY);
	SetCurrentDirectory(tmp);
}
//
//
//
void Base::SetModelDirectory()
{
	char tmp[512] = { 0 };
	strcpy(tmp, szRootPath);
	strcat(tmp, MODEL_DIRECTORY);
	SetCurrentDirectory(tmp);
}
//
//
//
void Base::SetShaderDirectory()
{
	char tmp[512] = { 0 };
	strcpy(tmp, szRootPath);
	strcat(tmp, SHADER_DIRECTORY);
	SetCurrentDirectory(tmp);
}
