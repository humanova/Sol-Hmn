#pragma once
#include "proc.h"

enum SoldatOffset
{
	playerOffset = 0xC5D0,

	cameraX = 0x8CBFE8,
	cameraY = cameraX + 0x4,
	camID = cameraX + 0x8,

	playerX = 0x767914,
	playerY = playerX + 0x4,

	playerVelX = 0x768A94,
	playerVelY = 0x768A98,

	playerWeapon = 0x8D7FBE,
	playerWeaponVel = 0x8E45C4,

	playerHealth = 0x8CC004,

	playerTeam = 0x8D8180,

	playerAlive = 0x8D8017,

	playerOnline = 0x8CBFF4,

	playerCursorX = 0x5EC370,
	playerCursorY = playerCursorX + 0x4,

	playerCount = 0xCF270C,

	barretBink = 0x5EB4C0,
	barretMovAcc = 0x5EB4BC,
	barretStartupTime = 0x5EB4D0,

	scoreToggle = 0x5EC764,

	bulletOffset = 0x56,
	bulletOwner = 0xA579FA,
	bulletOwnerWeapon = 0xA579FB,
	bulletActive = 0xA579F4,
	bulletX = 0x76ED1C,
	bulletY = bulletX + 0x4,
	bulletVelX = 0x76FE9C,
	bulletVelY = bulletVelX + 0x4,
	

	WindowWidth = 0x5E0C40,
	WindowHeight = WindowWidth + 0x4
};


enum GostekOffset
{
	exactPos = 0,
	torso = -11,
};