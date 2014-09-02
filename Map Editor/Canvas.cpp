#include "Canvas.h"

HANDLE Canvas::outHnd;	// Write Handle
HANDLE Canvas::inHnd;	// Read Handle
HWND Canvas::hWnd;	// Window Handle

Canvas::Canvas(int pwidth, int pheight)
{
	cWidth = pwidth;
	cHeight = pheight;
	width = pwidth;
	height = pheight-10;

	// Setup the handles
	outHnd = GetStdHandle(STD_OUTPUT_HANDLE);
	inHnd = GetStdHandle(STD_INPUT_HANDLE);
	hWnd = GetConsoleWindow();

	// Set console window size
	COORD bufferSize = {cWidth, cHeight};
	SMALL_RECT windowSize = {0, 0, cWidth-1, cHeight-1};
	
	// Set canvas size
	COORD cBufSize = {80,40};
	COORD cPos = {0,0};
	SMALL_RECT cArea = {0,0,80-1,40-1};

	// Fill the canvas buffer with characters
	CHAR_INFO cBuffer[80 * 40];
	for (int y = 0; y < 40; ++y) 
	{	
		for (int x = 0; x < 80; ++x) 
		{
			cBuffer[x + 80 * y].Char.AsciiChar = 255;						
			cBuffer[x + 80 * y].Attributes = Green << BG;
		}
	}

	CONSOLE_FONT_INFOEX font;
 
	// set the size of the CONSOLE_FONT_INFOEX
	font.cbSize = sizeof(CONSOLE_FONT_INFOEX);
 
	// get the current value
	GetCurrentConsoleFontEx(outHnd, false, &font);
 
	// set size to be 3x5; 5x8; 6x12; 8x16; 8x18; 10x20;
	font.dwFontSize.X = 8;
	font.dwFontSize.Y = 16;
 
	// submit the settings
	SetConsoleTitle(TEXT("Map Editor"));
	SetConsoleWindowInfo(outHnd, TRUE, &windowSize);
	SetConsoleScreenBufferSize(outHnd, bufferSize);
	SetCurrentConsoleFontEx(outHnd, false, &font); 

	// Write the characters
	WriteConsoleOutputA(outHnd, cBuffer, cBufSize, cPos, &cArea);
}

bool Canvas::save(CHAR_INFO buffer[40 * 80], string fileName)
{
    ofstream fOut(fileName.c_str());
    char singleAtt;
    char singleChar;


    for (int y = 0; y < 40; ++y) 
	{	
		for (int x = 0; x < 80; ++x) 
		{
			singleAtt = buffer[x + 80 * y].Attributes;
			singleChar = buffer[x + 80 * y].Char.AsciiChar;
			fOut << singleChar << " " << singleAtt << " ";
		}
		//fOut << "\n";
    }
        return true;
}

bool Canvas::load(string fileName)
{
    ifstream fin(fileName.c_str());
    
	CHAR_INFO screenBuffer[40 * 80];

	char singleAtt;
    char singleChar;

	for (int y = 0; y < 40; ++y) 
	{	
		for (int x = 0; x < 80; ++x) 
		{
			fin >> singleChar;
			fin >> singleAtt;

			screenBuffer[x + 80 * y].Char.AsciiChar = singleChar;						
			screenBuffer[x + 80 * y].Attributes = singleAtt;
		}
	}

	// Set up the positions:
	COORD charBufSize = {80,40};
	COORD characterPos = {0,0};
	SMALL_RECT writeArea = {0,0,80-1,40-1}; 

	// Write the character:
	WriteConsoleOutputA(Canvas::outHnd, screenBuffer, charBufSize, characterPos, &writeArea);

	return 1;
}

CHAR_INFO brush(int x, int y)
{
	COORD dwRead = {x, y};
	CHAR_INFO brush[1];
	LPDWORD read;

	LPWORD brushAtt;
	LPWSTR brushChar;

	ReadConsoleOutputAttribute(Canvas::outHnd, brushAtt, 1, dwRead, read);
	ReadConsoleOutputCharacter(Canvas::outHnd, brushChar, 1, dwRead, read);

	brush[1].Char.AsciiChar = char(brushAtt);						
	brush[1].Attributes = char(brushChar);

	return brush[1];
}

void Canvas::tools()
{
	char charInfo[25][1] = {
		{255},
		{2},
		{4},
		{5},
		{8},
		{176}, 
		{177}, 
		{178}, 
		{179}, 
		{185},
		{191},
		{192},
		{194},
		{196},
		{197},
		{215},
		{217},
		{206},
		{218},
		{219},
		{247},
		{94},
		{95},
		{46},
		{254}
	};

	int Colours[16] = {
		Black,
		Grey,
		Blue,
		Green,
		Cyan,
		Red,
		Purple,
		LightGrey,
		LightBlue,
		LightGreen,
		LightCyan,
		LightRed,
		LightPurple,
		Brown,
		Yellow,
		White
	};

	CHAR_INFO screenBuffer[16];
	Canvas::setCursorPos(0, 40);
	cout << "--------------------------------------------------------------------------------"
		<< "Colours:                                                            Brush: " << endl
		<< "  Tiles:" << endl << endl
		<< "[t] Create Tile \t [c] Copy Selected Tile \t [s] Paint Brush" << endl
		<< " ^ Hover over the character you would like to copy and press [t]";

	for(int x = 0; x < 16; ++x) 
	{
		screenBuffer[x].Char.AsciiChar = 219;						
		screenBuffer[x].Attributes = Colours[x];
	}

	HANDLE outHnd = GetStdHandle(STD_OUTPUT_HANDLE);

	// Set up the positions
	COORD charBufSize = {16,1};
	COORD characterPos = {0,0};
	SMALL_RECT writeArea = {8,41,80-1,50-1}; 

	// Write the character:
	WriteConsoleOutputA(outHnd, screenBuffer, charBufSize, characterPos, &writeArea);
	CHAR_INFO screenBuffer2[sizeof(charInfo)];
	// Set up the positions
	COORD charBufSize2 = {sizeof(charInfo),1};
	COORD characterPos2 = {0,0};
	SMALL_RECT writeArea2 = {8,42,80-1,50-1}; 

	for(int x = 0; x < 25; ++x) 
	{
		screenBuffer2[x].Char.AsciiChar = charInfo[x][0];						
		screenBuffer2[x].Attributes = White | Black << BG;
	}

	WriteConsoleOutputA(outHnd, screenBuffer2, charBufSize2, characterPos2, &writeArea2);

}

void Canvas::setCursorPos(int x, int y) // Set the position of the cursor
{
	COORD loc = {x, y};
    SetConsoleCursorPosition(outHnd, loc);
}

COORD Canvas::getCursorPos() // Get the position of the cursor
{
	CONSOLE_SCREEN_BUFFER_INFO mapinfo;

	GetConsoleScreenBufferInfo (outHnd, &mapinfo);
    return mapinfo.dwCursorPosition;
}