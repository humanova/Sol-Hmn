#include <iostream>

#include "SolHook.h"

int main()
{
	SolHook::Settings settings;
	settings.stabilizer = 0;

	SolHook Sol(settings);
	
	
	while (1)
	{
		Sol.Aimbot();
		Sol.RefreshVal();
		Sol.DebugSomething();
		Sol.RefreshEnemy();
		Sol.CheckEvents();
		
		Sleep(1);
	}

	return 0;
}

