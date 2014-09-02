#include "Canvas.h"
#include "windows.h"
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

using namespace std;

int main()
{
	Canvas newCanvas;
	newCanvas.tools();
	newCanvas.load("Maps\\green.dat");

	COORD brushSize = {1,1};

	newCanvas.setCursorPos(0,0);

	char brushAtt[3];
	bool gotFG;
	bool gotBG;
	bool gotTile;

	// Set the info for hiding the flashing cursor
	CONSOLE_CURSOR_INFO cursor;
	cursor.bVisible = 0;
	cursor.dwSize = sizeof(CONSOLE_CURSOR_INFO); // Get current size
	SetConsoleCursorInfo(newCanvas.outHnd, &cursor);


	while (0 < 1)
	{
		POINT coord;
		GetCursorPos(&coord);
		ScreenToClient(newCanvas.hWnd, &coord);

		// write the mouse coords
		newCanvas.setCursorPos(0,47);
		Sleep(2);
		cout << "Mouse X: " << coord.x/12 << " Y: " << coord.y/16 << "    ";

		INPUT_RECORD key;
		DWORD numRead;
		PeekConsoleInput (newCanvas.inHnd, &key, 1, &numRead);
		if (numRead > 0)
		{
			ReadConsoleInput (newCanvas.inHnd, &key, 1, &numRead);
		
			if ((key.EventType == KEY_EVENT) && (key.Event.KeyEvent.bKeyDown))
			{
				if(key.Event.KeyEvent.uChar.AsciiChar == 's')
				{
					//cout << key.Event.KeyEvent.uChar.AsciiChar << endl;
					//cout << newCanvas.brush();


					CHAR_INFO brush[1];
					COORD brushSize = {1,1};
					COORD brushPos = {0,0};
					SMALL_RECT brushRead = {74, 41, 74, 41};

					ReadConsoleOutputA(Canvas::outHnd, brush, brushSize, brushPos, &brushRead);

					SMALL_RECT brushWrite = {coord.x/12, coord.y/16, coord.x/12, coord.y/16};

					if (coord.y/16 < 40)
						WriteConsoleOutputA(newCanvas.outHnd, brush, brushSize, brushPos, &brushWrite);
				}
				else if(key.Event.KeyEvent.uChar.AsciiChar == 'a')
				{
					//cout << key.Event.KeyEvent.uChar.AsciiChar << endl;
					//cout << newCanvas.brush();


					CHAR_INFO brush[1];
					CHAR_INFO bigbrush[9];
					COORD brushSize = {3,3};
					COORD brushPos = {0,0};
					SMALL_RECT brushRead = {74, 41, 74, 41};

					ReadConsoleOutputA(Canvas::outHnd, brush, brushSize, brushPos, &brushRead);
					
					for( int i = 0; i < 9; ++i)
						bigbrush[i] = brush[0];

					SMALL_RECT brushWrite = {(coord.x/12)-1, (coord.y/16)-1, (coord.x/12)+1, (coord.y/16)+1};

					if (coord.y/16 < 39)
						WriteConsoleOutputA(newCanvas.outHnd, bigbrush, brushSize, brushPos, &brushWrite);
				}
				else if(key.Event.KeyEvent.uChar.AsciiChar == 'c')
				{
					CHAR_INFO brush[1];
					COORD brushSize = {1,1};
					COORD brushPos = {0,0};
					SMALL_RECT brushRead = {coord.x/12, coord.y/16, coord.x/12, coord.y/16};

					ReadConsoleOutputA(Canvas::outHnd, brush, brushSize, brushPos, &brushRead);

					SMALL_RECT brushWrite = {74, 41, 74, 41};
					WriteConsoleOutputA(newCanvas.outHnd, brush, brushSize, brushPos, &brushWrite);
				}
				else if(key.Event.KeyEvent.uChar.AsciiChar == 't')
				{		
					CHAR_INFO brushCharAtt[1];
					COORD brushSize = {1,1};
					COORD brushPos = {0,0};
					SMALL_RECT tileRead = {coord.x/12, coord.y/16, coord.x/12, coord.y/16};
					
					ReadConsoleOutputA(Canvas::outHnd, brushCharAtt, brushSize, brushPos, &tileRead);

					brushAtt[2] = brushCharAtt[0].Char.AsciiChar;

					gotTile = true;
					newCanvas.setCursorPos(0,49);
					cout << "Now Please Select Your Foreground with [f]";
				}
				else if(key.Event.KeyEvent.uChar.AsciiChar == 'b')
				{
					CHAR_INFO brushBGAtt[1];
					COORD brushSize = {1,1};
					COORD brushPos = {0,0};
					SMALL_RECT tileRead = {coord.x/12, coord.y/16, coord.x/12, coord.y/16};
					
					ReadConsoleOutputA(Canvas::outHnd, brushBGAtt, brushSize, brushPos, &tileRead);

					brushAtt[0] = brushBGAtt[0].Attributes;

					gotBG = true;
					newCanvas.setCursorPos(0,49);
					cout << "Now Please Select Your Foreground with [f]";
				}
				else if(key.Event.KeyEvent.uChar.AsciiChar == 'f')
				{
					CHAR_INFO brushFGAtt[1];
					COORD brushSize = {1,1};
					COORD brushPos = {0,0};
					SMALL_RECT tileRead = {coord.x/12, coord.y/16, coord.x/12, coord.y/16};

					ReadConsoleOutputA(Canvas::outHnd, brushFGAtt, brushSize, brushPos, &tileRead);

					brushAtt[1] = brushFGAtt[0].Attributes;

					gotFG = true;
					newCanvas.setCursorPos(0,49);
					cout << "Now Please Select Your Background with [b]";
				}
				else if(key.Event.KeyEvent.uChar.AsciiChar == 'i')
				{
					newCanvas.setCursorPos(0,48);
					cout << "Load file: ";
					stringstream in;
					string sin;
					getline(cin, sin);
					in << "Maps\\" << sin << ".dat";

					newCanvas.setCursorPos(0,48);
					if (newCanvas.load(in.str()))
						cout << "Loaded...             ";
					else
						cout << "Failed...             ";

					Sleep(1000);
					newCanvas.setCursorPos(0,48);
					cout << "                      ";
					newCanvas.setCursorPos(0,48);
				}
				else if(key.Event.KeyEvent.uChar.AsciiChar == 'o')
				{
					newCanvas.setCursorPos(0,48);
					cout << "Save as: ";
					stringstream out;
					string sout;
					getline(cin, sout);
					out << "Maps\\" << sout << ".dat";

					// Set up the positions
					CHAR_INFO buffer[40 * 80];
					COORD charBufSize = {80,40};
					COORD characterPos = {0,0};
					SMALL_RECT readArea = {0,0,80-1,40-1}; 

					// Read the characters
					ReadConsoleOutputA(Canvas::outHnd, buffer, charBufSize, characterPos, &readArea);

					newCanvas.setCursorPos(0,48);
					if (newCanvas.save(buffer, out.str()))
						cout << "Saved...             ";
					else
						cout << "Failed...             ";

					Sleep(1000);
					newCanvas.setCursorPos(0,48);
					cout << "                      ";
					newCanvas.setCursorPos(0,48);

				}

			}
		}

		if(gotFG && gotBG && gotTile)
		{
			newCanvas.setCursorPos(0,49);
			cout << "                                                               ";
			gotTile = false;
			gotFG = false;
			gotBG = false;

			CHAR_INFO brush[1];
			COORD brushSize = {1,1};
			COORD brushPos = {0,0};
			//SMALL_RECT brushRead = {74, 41, 74, 41};
			//SMALL_RECT tileRead = {coord.x/12, coord.y/16, coord.x/12, coord.y/16};

			brush[0].Char.AsciiChar = brushAtt[2];
			brush[0].Attributes = brushAtt[0] << 4 | brushAtt[1];

			//ReadConsoleOutputA(Canvas::outHnd, brush, brushSize, brushPos, &brushRead);

			SMALL_RECT brushWrite = {74, 41, 74, 41};
			WriteConsoleOutputA(newCanvas.outHnd, brush, brushSize, brushPos, &brushWrite);
		};

		Sleep(10);
	}

	return 0;
}

