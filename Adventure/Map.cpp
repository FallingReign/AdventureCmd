
#include "Map.h"

Map::Map(Window w)
	: outHnd(w.outHnd), height(w.height), width(w.width)
{
}

bool Map::load(int world, COORD n)
{
	// load correct map from coord

	stringstream fileName;
	if (world == 't')
		fileName << "Maps/title.dat";
	else if (world == 'b')
		fileName << "Maps/battle.dat";
	else
		fileName << "Maps/map" << world << n.Y << n.X << ".dat";

    ifstream fin(fileName.str());
	if (!fin.is_open())
		return 0;
    
	CHAR_INFO screenBuffer[40 * 80];

	char singleAtt;
    char singleChar;

	for (int y = 0; y < height; ++y) 
	{	
		for (int x = 0; x < width; ++x) 
		{
			fin >> singleChar;
			fin >> singleAtt;

			screenBuffer[x + width * y].Char.AsciiChar = singleChar;						
			screenBuffer[x + width * y].Attributes = singleAtt;
		}
	}

	fin.close();

	// Set up the positions:
	COORD charBufSize = {width,height};
	COORD characterPos = {0,0};
	SMALL_RECT writeArea = {0,0,width-1,height-1}; 

	// Write the character:
	WriteConsoleOutputA(outHnd, screenBuffer, charBufSize, characterPos, &writeArea);

	return 1;
}