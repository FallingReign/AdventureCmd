#pragma once
#include "game.h"
#include <random>

class Window
{
public:
	Window(int pwidth = 80, int pheight = 40);
	
	void setCursorPos(int x, int y);
	COORD getCursorPos();
	int random(int low, int high);

	HANDLE outHnd; // Write Handle
	HANDLE inHnd; // Read Handle
	int width, height;

	//typedef std::mt19937 uniEng;
	//uniEng rng;
};

