#pragma once
#include "../PMX/CPmx.h"

class CMainchar :public CPmx
{
public:
	void move();
	void charDraw();

	CMainchar(DrawingType type):CPmx(type)
	{
		
	}

	CMainchar() :CPmx()
	{

	}

	~CMainchar()
	{

	}
};