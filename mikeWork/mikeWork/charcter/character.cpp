#include "character.h"
#include "../GameMain/GameUse/input.h"
void CMainchar::move()
{

	//11/15 ƒLƒƒƒ‰‚Ì“®‚«‚µ‚Á‚©‚è‚İ‚é ok ƒJƒƒ‰‚ªˆ«‚©‚Á‚½

	/*if (GetKeyboardPress(DIK_A))
	{
		Rotetation.y++;
	}
	else if (GetKeyboardPress(DIK_D))
	{
		Rotetation.y--;
	}
	if (GetKeyboardPress(DIK_W))
	{
		Rotetation.x++;
	}
	else if (GetKeyboardPress(DIK_S))
	{
		Rotetation.x--;
	}*/
}

void CMainchar::charDraw(XMFLOAT3 lightDir)
{
	move();
	ProcessingCalc(lightDir);
	Position = XMFLOAT3(0, 0, 0);
}