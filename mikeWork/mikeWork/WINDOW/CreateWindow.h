#pragma once
#include "../BASE/BASE.h"

#define APP_NAME "èAêEçÏïi"
class Window
{
public:
	//Data
	HWND m_hWnd;

	//Method
	HRESULT InitWindow(HINSTANCE, INT, INT, INT, INT, LPCSTR);
	LRESULT MsgProc(HWND, UINT, WPARAM, LPARAM);
};
