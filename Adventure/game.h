#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <windows.h>
#include <cstdlib>
#include <ctime>

using namespace std;

#pragma once

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

enum ItemIcon {
	Gold = 36,
	Sword = 25,
	Face = 2,
	Body = 197,
	Legs = 94,
	Star = 15,
	Hash = 35,
	Axe = 194,
	Rock = 254,
	Dynamite = 161,
	Heart = 3,
	Chisel = 16
};