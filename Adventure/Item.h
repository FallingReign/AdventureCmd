#pragma once
#include "game.h"

class Item
{
public:
	Item(string item, string type, int qty);
	Item(string item, string type, int qty, CHAR_INFO icon, COORD loc);
	string getItemName();
	int getItemQty();
	string getItemType();
	void setItemQty(int q);
	void setItemName(string n);
	
	COORD loc;
	CHAR_INFO icon;
private:
	string name, type; // type is normal, consumable, weapon or gold
	int qty;
};

