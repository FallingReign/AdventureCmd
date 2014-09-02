#pragma once

#include "windows.h"
#include <fstream>
#include <string>
#include <iostream>

// set size to be 3x5; 5x8; 6x12; 8x16; 8x18; 10x20;
const COORD charSize = {8,16};

using namespace std;

enum Colour {
	Black       = 0,
    Grey        = FOREGROUND_INTENSITY,
    Blue        = FOREGROUND_BLUE,
    Green       = FOREGROUND_GREEN,
    Cyan        = FOREGROUND_GREEN | FOREGROUND_BLUE,
    Red         = FOREGROUND_RED,
    Purple      = FOREGROUND_RED   | FOREGROUND_BLUE,
    LightGrey   = FOREGROUND_RED   | FOREGROUND_GREEN | FOREGROUND_BLUE,
    LightBlue   = FOREGROUND_BLUE  | FOREGROUND_INTENSITY,
    LightGreen  = FOREGROUND_GREEN | FOREGROUND_INTENSITY,
    LightCyan   = FOREGROUND_GREEN | FOREGROUND_BLUE  | FOREGROUND_INTENSITY,
    LightRed    = FOREGROUND_RED   | FOREGROUND_INTENSITY,
    LightPurple = FOREGROUND_RED   | FOREGROUND_BLUE  | FOREGROUND_INTENSITY,
    Brown       = FOREGROUND_RED   | FOREGROUND_GREEN,
    Yellow      = FOREGROUND_RED   | FOREGROUND_GREEN | FOREGROUND_INTENSITY,
    White       = FOREGROUND_RED   | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY,
	BG			= 4,
};

class Canvas
{
public:
	Canvas(int pwidth = 80, int pheight = 50);

	static HANDLE outHnd;	// Write Handle
	static HANDLE inHnd;	// Read Handle
	static HWND hWnd;		// Window Handle

	bool save(CHAR_INFO buffer[40 * 80], string fileName);
	bool load(string fileName);
	CHAR_INFO brush(int x = 74, int y = 41);
	void tools();
	void setCursorPos(int x, int y);
	COORD getCursorPos();

private:
	COORD brushSize;
	char brushBG, brushFG, brushChar;
	int width, height;	// Size of the canvas
	int cWidth, cHeight; // Size of the consol
	
};

