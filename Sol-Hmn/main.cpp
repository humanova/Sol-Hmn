/*Emir Erbasan (humanova) 2018

*============*
	Sol-Hmn is a fun project that I'm working on to gain some knowledge on reverse engineering and game hacking.
	And I worked on reversing a 2D shooter game called "Soldat" to improve myself on reverse engineering.
	I don't know open sourcing this project will cause a problem with the devs(because they currently work an a  
	huge update on this month (December 2018) and the game will get released to the Steam) If a problem occurs 
	I can delete/set private this repository.

	Before diving into the code you need to know that this project is only for 'Soldat 1.7.1'.
	Memory addresses, structures, functions can differ version to version.
	Even a small change on the game's source code may cause this project to not to work.
*============*

== Notes about some methods and structures on this file ==

Line : SolHook::Settings settings; :
	
	SolHook::Settings is a structure to store launch settings of the hack.
	All settings are set to 'true' by default. If a setting is
	set to 'false', even if you toggled that cheat(or something else)
	it won't run/call any function(related to that cheat)

Line : settings.stabilizer = 0; : 

	Creating an instance of SolHook using the constructor that takes
	launch settings. Default constructor doesn't take any arguments
	and uses default settings.
	Use 'SolHook::SetSettings()' to change settings manually (after construction ofc).

		
Method : SolHook::RefreshVal() :

	Refreshes 'val' which is a gameVal structure that contains all necessary game values

Method : SolHook::CheckEvents() :

	Checks events, such as toggling and activated cheats (to call functions)

*/

#include "SolHook.h"

int main()
{
	SolHook::Settings settings;
	settings.stabilizer = 0;

	SolHook Sol(settings);
	
	while (1)
	{
		Sol.RefreshVal();
		Sol.CheckEvents();

		Sleep(1);
	}

	return 0;
}