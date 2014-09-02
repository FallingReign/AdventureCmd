#pragma once
#include "game.h"

class Objects
{
public:
	Objects(SMALL_RECT pos, string name = "");
	SMALL_RECT getPos();
	string getName();

	virtual CHAR_INFO getIcon() = 0;

	// Waypoint Functions
	virtual int getToMap() = 0;
	virtual COORD getToZone() = 0;
	virtual COORD getToPos() = 0;	
	
	// Trigger Functions
	virtual string getKey() = 0;
	virtual string getMsg() = 0;
	virtual string getWinMsg() = 0;


protected:
	SMALL_RECT pos;
	string name;
};

class Waypoint : public Objects
{
public:
	Waypoint(SMALL_RECT pos, int toMap, COORD toZone, COORD toPos, CHAR_INFO icon);
	int getToMap();
	COORD getToZone();
	COORD getToPos();
	CHAR_INFO getIcon();

	// Trigger functions
	virtual string getKey() { return "s"; }
	virtual string getMsg() { return "s"; }
	virtual string getWinMsg() { return "s"; }

private:
	int toMap;
	COORD toZone, toPos;
	CHAR_INFO icon;
};

class Trigger : public Objects
{
public:
	Trigger(SMALL_RECT pos, string name, string key, CHAR_INFO icon, string msg, string winMsg);

	CHAR_INFO getIcon();
	string getKey();
	string getMsg();
	string getWinMsg();

	//waypoint functions
	virtual int getToMap() { return 99;}
	virtual COORD getToZone() {COORD t = {0,0}; return t;}
	virtual COORD getToPos() {COORD t = {0,0}; return t;}

private:
	CHAR_INFO icon;
	string key, msg, winMsg;
};
