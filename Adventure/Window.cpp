#include "Window.h"

Window::Window(int pwidth, int pheight)
{
	height = pheight;
	width = pwidth;

	// Setup the handles
	outHnd = GetStdHandle(STD_OUTPUT_HANDLE);
	inHnd = GetStdHandle(STD_INPUT_HANDLE);

	// Set console window size
	COORD bufferSize = {pwidth, pheight};
	SMALL_RECT windowSize = {0, 0, pwidth-1, pheight-1};
	
	// set the size of the CONSOLE_FONT_INFOEX
	CONSOLE_FONT_INFOEX font;
	font.cbSize = sizeof(CONSOLE_FONT_INFOEX);
	GetCurrentConsoleFontEx(outHnd, false, &font); // Get current size
 
	// set size to be 3x5; 5x8; 6x12; 8x16; 8x18; 10x20;
	font.dwFontSize.X = 8;
	font.dwFontSize.Y = 16;
	 
	// Set the info for hiding the flashing cursor
	CONSOLE_CURSOR_INFO cursor;
	cursor.bVisible = 0;
	cursor.dwSize = sizeof(CONSOLE_CURSOR_INFO); // Get current size
	
	// Submit the settings
	SetConsoleTitle(TEXT("Adventure CMD!"));
	SetConsoleWindowInfo(outHnd, TRUE, &windowSize);
	SetConsoleScreenBufferSize(outHnd, bufferSize);
	SetCurrentConsoleFontEx(outHnd, false, &font);
	SetConsoleCursorInfo(outHnd, &cursor);
	srand(time(0));

}

void Window::setCursorPos(int x, int y) // Set the position of the cursor
{
	COORD loc = {x, y};
    SetConsoleCursorPosition(outHnd, loc);
}

COORD Window::getCursorPos() // Get the position of the cursor
{
	CONSOLE_SCREEN_BUFFER_INFO mapinfo;

	GetConsoleScreenBufferInfo (outHnd, &mapinfo);
    return mapinfo.dwCursorPosition;
}

int Window::random(int low, int high)
{
	//std::uniform_real_distribution<double> result(low, high);
	//std::uniform_int<int> result(low, high);
	//int res = result(rng);
	return ((double) rand() / (RAND_MAX+1)) * (high-low+1) + low;
}