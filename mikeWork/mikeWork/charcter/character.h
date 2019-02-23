#pragma once
#include "../PMX/CPmx.h"

class CMainchar :public CPmx
{
public:
	void move();
	void charDraw(XMFLOAT3);

	CMainchar(DrawingType type):CPmx(type)
	{
		
	}

	CMainchar(DrawingType type,UINT morphNum) :CPmx(type,morphNum)
	{

	}


	CMainchar() :CPmx()
	{

	}

	~CMainchar()
	{

	}
};