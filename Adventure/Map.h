#pragma once
#include "game.h"
#include "Window.h"

class Map
{
public:
	Map(Window w);
	bool load(int world, COORD n);

	int world;
	COORD zone;
	HANDLE outHnd; // Write Handle
	int width, height;
	//static int width, height;
	//HANDLE getHandle(bool b = 0);

};
