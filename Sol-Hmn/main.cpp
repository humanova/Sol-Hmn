#include "SolHook.h"

int main()
{
	SolHook::Settings settings;
	settings.stabilizer = 0;

	SolHook Sol(settings);
	
	while (1)
	{
		Sol.RefreshVal();
		if (GetKeyState(VK_MENU) & 1)
		{		
			printf("%f - %f\n", Sol.val.playerPos.x, Sol.val.playerPos.y);
		}
		Sol.RefreshEnemy();
		//Sol.RefreshBullet();
		//Sol.GetSelfBullet();
		//Sol.MagicBullet();
		Sol.CheckEvents();
		Sleep(1);
	}

	return 0;
}