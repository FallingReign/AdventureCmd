#pragma once
#include "game.h"
#include "Map.h"
#include "Window.h"
#include "Item.h"
#include "Objects.h"

class Interface;
extern vector<Objects*> objects[3][4][6];

class Character
{
protected:
	HANDLE outHnd;	// Write Handle
	HANDLE inHnd;	// Read Handle
	Window console;

public:
	Map& world;
	string name, weapon;
	int health, mana, attack, defence, maxhealth, map, xp, xpmax, level, gold, weapondmg, capacity;
	double dex, maxdex;
	bool player;
	COORD loc;
	CHAR_INFO oldTile[1]; // holds the information for the old tile
	
	Character(Window w, Map& newWorld, string name, COORD loc, int hp, int def, int att, int dex); // contructor with values
	Character(Window w, Map& newWorld, string name, COORD loc);
	~Character();

	bool Move(char d);
	void BattleStatUpdate(Character& character, double timer, COORD pos);
	bool Battle(Character& enemy, Interface& bag);

};

class NPC : public Character
{
public:
	NPC(Window w, Map& newWorld, string name, COORD loc, CHAR_INFO icon, string needItem = "", string rewardItem = "", string rewardType = "", int rewardQty = 1);

	CHAR_INFO icon;
	vector<string> talk;
	int talkPos;
	string needItem, rewardItem, rewardType;
	int rewardQty;
};