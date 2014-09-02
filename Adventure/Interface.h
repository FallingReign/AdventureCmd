#pragma once
#include "game.h"
#include "Item.h"
#include "Character.h"
#include "Window.h"

extern vector<Item*> loot[3][4][6];
extern vector<Objects*> objects[3][4][6];

class Interface
{
public:
	Interface(Window console, COORD sPos, SMALL_RECT sSize);
	~Interface();

	void addRow(string sItem);
	void setRowText(int row, string text);
	string getRow(int i);
	int getRowCount();
	void setTitle(string t);
	string getTitle();
	void setPos(int x, int y);
	COORD getPos();
	bool useItem(Character& player, bool inBattle = 0);
	void message(string msg, string title = "", int pause = 0); // sends a message to the default message box
	void display(Character& player, bool fullload);
	void display(Character& player);

	Window console;
	bool isVisible;
	vector<Item*> items;
	SMALL_RECT size;
private:
	string title;
	COORD pos;
	vector<string> rows;
};

