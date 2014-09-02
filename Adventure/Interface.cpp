#include "Interface.h"

Interface::Interface(Window console, COORD sPos, SMALL_RECT sSize)
	: pos(sPos), size(sSize), console(console)
{

}

Interface::~Interface()
{

}

void Interface::addRow(string sItem)
{
	rows.push_back(sItem);
}

void Interface::setRowText(int row, string text)
{
	rows[row] = text;
}

string Interface::getRow(int i)
{
	return rows[i];
}

int Interface::getRowCount()
{
	return rows.size();
}

void Interface::setTitle(string t)
{
	title = t;
}

string Interface::getTitle()
{
	return title;
}

void Interface::setPos(int x, int y)
{
	pos.X = x;
	pos.Y = y;
}

COORD Interface::getPos()
{
	return pos;
}

bool Interface::useItem(Character& player, bool inBattle)
{
	COORD textPos = {pos.X+1,console.height-2};
	string preword = "";

	do {
		for (int i = 0; i < items.size(); ++i)
		{

			if (GetAsyncKeyState(0x44) & 0x8000)				// Key Detection drop item [D]
			{
				if (inBattle)
				{
					console.setCursorPos(pos.X+1, pos.Y+16);
					cout << "Not Allowed Here ";
				}
				else
				{
					console.setCursorPos(pos.X+1, pos.Y+16);
					cout << "Drop which item? ";
					preword = "Dropped";
				}
				while (GetAsyncKeyState(0x44) & 0x8000); // wait for key up

			}

			if ((GetAsyncKeyState(0x31+i) & 0x8000) || (GetAsyncKeyState(0x61+i) & 0x8000))				// Key Detection for each item
			{		
				int qty = items[i]->getItemQty();
				if(qty > 0 && preword != "Dropped")
				{
					if (items[i]->getItemType() == "weapon")
					{
						player.weapon = items[i]->getItemName();
						player.weapondmg = items[i]->getItemQty();
						preword = "Equipped";
						display(player, 1);
					}
					
					console.setCursorPos(textPos.X, textPos.Y);
					if (items[i]->getItemName() == "HP Potion")
					{
						if (player.health == player.maxhealth)
						{
							cout << "Already Full HP!   ";
							return 0;
						}
						else if (player.health > 0)
						{
							player.health = player.maxhealth;
							preword = "Used";
						}
						else
						{
							cout << "You're too Wounded ";
							return 0;
						}
					}
					else if (items[i]->getItemName() == "Chapati")
					{
						if (player.health == player.maxhealth)
						{
							cout << "Already Full HP!   ";
							return 0;
						}
						else if (player.health > 0)
						{
							player.health += player.maxhealth/2;
							if (player.health > player.maxhealth)
								player.health = player.maxhealth;

							preword = "Used";
						}
						else
						{
							cout << "You're too Wounded ";
							return 0;
						}
					}
					else if (items[i]->getItemName() == "Life Potion")
					{
						if (player.health < 1)
						{
							player.health = player.maxhealth;
							preword = "Used";
						}
						else
						{
							cout << "You don't need this";
							return 0;
						}
					}
					else if(items[i]->getItemName() == "Dynamite")
					{
						COORD tLoc = player.loc;
						if (tLoc.Y >= console.height-1)
							tLoc.Y--;
						else
							tLoc.Y++;

						CHAR_INFO charInfo[1];
						int bg = (player.oldTile[0].Attributes & Blue << BG ) | (player.oldTile[0].Attributes & Red << BG) | (player.oldTile[0].Attributes & Green << BG) | (player.oldTile[0].Attributes & BACKGROUND_INTENSITY);
						charInfo[0].Char.AsciiChar = Star;
						charInfo[0].Attributes = Yellow | bg;

						// Check if the player is near water
						CHAR_INFO cInfo[9];
						COORD cSize = {3,3};
						COORD cPos = {0,0};
						SMALL_RECT cRead = {player.loc.X-1, player.loc.Y-1, player.loc.X+1, player.loc.Y+1};

						ReadConsoleOutputA(console.outHnd, cInfo, cSize, cPos, &cRead);
						for (int c = 0; c < 9; ++c)
						{
							int bg = (cInfo[c].Attributes & Blue << BG ) | (cInfo[c].Attributes & Red << BG) | (cInfo[c].Attributes & Green << BG) | (cInfo[c].Attributes & BACKGROUND_INTENSITY);
							if (bg == Blue << BG)
							{
								loot[player.world.world][player.world.zone.Y][player.world.zone.X].push_back(new Item("Fish", "normal", 1, charInfo[0], tLoc));
								message("\n The dynamite explodes and some fish fly out of \n the lake. Wakefield is gonna like this.", "BOOM!", 1);
								
								items[i]->setItemQty(qty-1);
								isVisible = 0;
								return 1;
							}
						}

						// Check if player is near the Dragon
						if((player.loc.X >= 2 && player.loc.X <= 13) && (player.loc.Y >= 4 && player.loc.Y <= 10) && player.world.world == 0 && player.world.zone.X == 3 && player.world.zone.Y == 0)
						{
							player.loc.Y = 10;
							CHAR_INFO tIcon;
							SMALL_RECT tPos = {2, 4, 13, 9};
							tIcon.Attributes = LightGrey | Grey << BG;
							tIcon.Char.AsciiChar = char(247);
							objects[0][0][3].push_back(new Trigger(tPos, "Rubble", "UNDEFINED", tIcon, "\n\n Looks like the cave entrance is blocked now.",  "UNDEFINED"));
							
							charInfo[0].Char.AsciiChar = Star;
							charInfo[0].Attributes = Red | Brown << BG;
							tLoc.X = 15, tLoc.Y = 12;
							loot[player.world.world][player.world.zone.Y][player.world.zone.X].push_back(new Item("Supplies", "normal", 1, charInfo[0], tLoc));

							charInfo[0].Char.AsciiChar = Gold;
							charInfo[0].Attributes = Yellow | Brown << BG;
							tLoc.X = 9, tLoc.Y = 14;
							loot[player.world.world][player.world.zone.Y][player.world.zone.X].push_back(new Item("Gold", "gold", 1, charInfo[0], tLoc));
							tLoc.X = 14, tLoc.Y = 16;
							loot[player.world.world][player.world.zone.Y][player.world.zone.X].push_back(new Item("Gold", "gold", 1, charInfo[0], tLoc));
							tLoc.X = 2, tLoc.Y = 13;
							loot[player.world.world][player.world.zone.Y][player.world.zone.X].push_back(new Item("Gold", "gold", 1, charInfo[0], tLoc));
							tLoc.X = 4, tLoc.Y = 17;
							loot[player.world.world][player.world.zone.Y][player.world.zone.X].push_back(new Item("Gold", "gold", 1, charInfo[0], tLoc));
							message("\n The dynamite explodes and rubble starts falling \n over the cave entrance. \n Items fly everywhere...", "BOOM!", 1);

							items[i]->setItemQty(qty-1);
							isVisible = 0;
							return 1;

						}

					}

					while ((GetAsyncKeyState(0x31+i) & 0x8000) || (GetAsyncKeyState(0x61+i) & 0x8000)); //wait for key up

					// Check if the player is near a trigger
					bool triggerFound = 0;
					for (unsigned int t = 0; t < objects[player.world.world][player.world.zone.Y][player.world.zone.X].size(); ++t)
					{
						// Check if the trigger still exists and is correct type
						if (!objects[player.world.world][player.world.zone.Y][player.world.zone.X][t] && objects[player.world.world][player.world.zone.Y][player.world.zone.X][t]->getToMap() == 99) 
							continue;

						// get the trigger position
						SMALL_RECT tLoc = objects[player.world.world][player.world.zone.Y][player.world.zone.X][t]->getPos();

						// check if we're in range of the trigger
						if ((player.loc.X >= tLoc.Left-1 && player.loc.X <= tLoc.Right+1) && (player.loc.Y >= tLoc.Top-1 && player.loc.Y <= tLoc.Bottom+1))
						{
							string iName = objects[player.world.world][player.world.zone.Y][player.world.zone.X][t]->getName();
							stringstream msg;
							msg << objects[player.world.world][player.world.zone.Y][player.world.zone.X][t]->getMsg();

							// did we use the correct item
							if (items[i]->getItemName() == objects[player.world.world][player.world.zone.Y][player.world.zone.X][t]->getKey())
							{
								msg.str(std::string());
								msg << objects[player.world.world][player.world.zone.Y][player.world.zone.X][t]->getWinMsg();

								// delete the trigger from the world
								delete objects[player.world.world][player.world.zone.Y][player.world.zone.X][t];
								objects[player.world.world][player.world.zone.Y][player.world.zone.X].erase(objects[player.world.world][player.world.zone.Y][player.world.zone.X].begin() + t);

								triggerFound = 1;
							}

							// output the correct message
							if (triggerFound)
							{
								message(msg.str(), iName, 1);
								isVisible = 0;
								return 1;
							}
						}
					}

					//store item name incase we delete it;
					string tmpItemName = items[i]->getItemName();

					// if consumable decrease item count, if 0 then delete
					if (items[i]->getItemType() == "consumable" && preword == "Used")
					{
						items[i]->setItemQty(items[i]->getItemQty()-1);
						if (items[i]->getItemQty() < 1)
						{
							delete items[i];
							items.erase(items.begin() + i);
						}

					}

					display(player, 1);
					console.setCursorPos(textPos.X, textPos.Y);
					cout << preword << " " << tmpItemName;
					if (preword == "Used")
						return 1;
					else
						return 0;
				}
				else
				{
					if (preword == "Dropped")
					{
						COORD tLoc = player.loc;
						if (tLoc.Y >= console.height-1)
							tLoc.Y--;
						else
							tLoc.Y++;

						CHAR_INFO charInfo[1];
						charInfo[0].Char.AsciiChar = Star;

						//extract the background from the tile
						int bg = (player.oldTile[0].Attributes & Blue << BG ) | (player.oldTile[0].Attributes & Red << BG) | (player.oldTile[0].Attributes & Green << BG) | (player.oldTile[0].Attributes & BACKGROUND_INTENSITY);
						charInfo[0].Attributes = Red | bg;

						// place the item in the world
						loot[player.world.world][player.world.zone.Y][player.world.zone.X].push_back(new Item(items[i]->getItemName(), items[i]->getItemType(), items[i]->getItemQty(), charInfo[0], tLoc));
						console.setCursorPos(pos.X+1, pos.Y+16);

						// display message to the user
						cout << preword << " " << items[i]->getItemName();

						// if item is equiped, reset to fist
						if (player.weapon == items[i]->getItemName())
						{
							player.weapon = "Fist";
							player.weapondmg = 0;
						}

						// delete the item from the heap
						delete items[i];
						items.erase(items.begin() + i);
						display(player, 1);

						while ((GetAsyncKeyState(0x31+i) & 0x8000) || (GetAsyncKeyState(0x61+i) & 0x8000)); //wait for key up
						return 0;
					}
					else
					{
						console.setCursorPos(textPos.X, textPos.Y);
						cout << "No " << items[i]->getItemName() << "'s";
					}
				}
					
				while ((GetAsyncKeyState(0x31+i) & 0x8000) || (GetAsyncKeyState(0x61+i) & 0x8000)); //wait for key up
				break;
			}
		} 
		if ((GetAsyncKeyState(0x42) & 0x8000) || (GetAsyncKeyState(0x49) & 0x8000))	// if user presses I while dropping it bails out.
			preword = "";

	} while (preword == "Dropped");
	return 0;
}

void Interface::message(string msg, string title, int pause)
{
	COORD coord = {0, 33};
	console.setCursorPos(coord.X, coord.Y);             
	cout << "------------------------------------------------- \n";
	for (int i = 0; i < 5; ++i)
		cout << "                                                 |\n";
	if (pause)
		cout << "------------------------------------ Press [E] -- ";
	else
		cout << "------------------------------------------------- ";
	if (title != "")
	{
		console.setCursorPos(coord.X+2, coord.Y);
		cout << " "<< title << " ";
	}

	console.setCursorPos(coord.X, ++coord.Y);
	cout << msg;

	if(pause == 1)
	{
		while (GetAsyncKeyState(0x45) & 0x8000);
		while(!(GetAsyncKeyState(0x45) & 0x8000));
	}
	else if (pause == 0)
		Sleep(1000);
}

void Interface::display(Character& player, bool fullload)
{
	// set up the inventory position based on player location
	if (player.loc.X > console.width / 2) 
		pos.X = 0;
	else
		pos.X = console.width - size.Right;

	if (fullload)
	{
		bool hasTitle = 0;
		if (getTitle().length() > 1)
			hasTitle = 1;

		int vPadMenu = (size.Bottom-items.size()-hasTitle)/2;
		if (title == "Inventory")
			vPadMenu = 19;

		COORD tmp = pos;
		console.setCursorPos(tmp.X, tmp.Y);
	
		// Output padding on top
		for (int i = 0; i < vPadMenu; ++i)
		{
			for (int x = 0; x < size.Right; x++)
				cout << " ";
			console.setCursorPos(tmp.X, tmp.Y += 1);
		}

		// Output menu title if exists
		if (hasTitle)
		{
			cout << " " << getTitle();
			for (int s = 0; s < (size.Right-getTitle().length()-1); ++s)
				cout << " ";

			console.setCursorPos(tmp.X, tmp.Y += 1);
			for (int i = 0; i < size.Right; ++i)
				cout << "-";
			console.setCursorPos(tmp.X, tmp.Y += 1);
		}
	
		// Output menu items
		if (!items.size())
		{
			cout << " No items";
			for (int s = 0; s < (size.Right-9); ++s)
				cout << " ";
			console.setCursorPos(tmp.X, tmp.Y += 1);
		}
		else
		{
			for (int i = 0; i < items.size(); ++i)
			{
				cout << " " << i+1 << ") " << items[i]->getItemName();
				for (int s = 0; s < (size.Right-items[i]->getItemName().length()-4); ++s)
					cout << " ";
				console.setCursorPos(tmp.X+size.Right-5, tmp.Y);
				if (items[i]->getItemType() == "weapon")
					cout << " +" << items[i]->getItemQty();
				else
					cout << " x" << items[i]->getItemQty();
				console.setCursorPos(tmp.X, tmp.Y += 1);
			}
		}

		// setup the buff area for the inventory based on position and size of inventory
		CHAR_INFO* charInfo = new CHAR_INFO[(size.Right-size.Left)*(console.height-tmp.Y)];
		COORD buffSize = {size.Right-size.Left,console.height-tmp.Y};
		COORD buffPos = {0,0};
		SMALL_RECT BuffArea = {tmp.X, tmp.Y, pos.X+size.Right, console.height};

		for (int s = 0; s < (size.Right-size.Left)*(console.height-tmp.Y); ++s)
		{
			charInfo[s].Attributes = Black | Black << BG;
		}

		// Output padding on bottom
		WriteConsoleOutputA(console.outHnd, charInfo, buffSize, buffPos, &BuffArea);
		
		delete charInfo;
	}

	if (title == "Inventory") {

		// setup the buff area for the stats
		CHAR_INFO stats[10];
		COORD buffSize = {10,1};
		COORD buffPos = {0,0};
		SMALL_RECT BuffArea = {pos.X+4, pos.Y+1, pos.X+14, pos.Y+1};

		// get the current players health percent
		double pHP = 0;

		if (player.health > 0)
			pHP = ((player.health + 0.0) / player.maxhealth) * 10;
		else
			player.health = 0;
		
		// output the status text
		console.setCursorPos(pos.X+1, pos.Y+1);
		cout << "HP";
		console.setCursorPos(pos.X+15, pos.Y+1);
		cout << player.health << "/" << player.maxhealth << " ";

		console.setCursorPos(pos.X+1, pos.Y+3);
		cout << "Attack:       " << player.attack + player.weapondmg;
		console.setCursorPos(pos.X+1, pos.Y+4);
		cout << "Defence:      " << player.defence;
		console.setCursorPos(pos.X+1, pos.Y+5);
		cout << "Dexterity:    " << player.maxdex;
		console.setCursorPos(pos.X+1, pos.Y+6);
		cout << "Damage (DPS): " << (player.maxdex/10)*(player.attack+player.weapondmg);

		console.setCursorPos(pos.X+1, pos.Y+8);
		cout << "Level " << player.level;
		console.setCursorPos(pos.X+1, pos.Y+9);
		cout << "Experience: " << player.xp << "/" << player.xpmax;

		console.setCursorPos(pos.X+1, pos.Y+11);
		cout << "Gold: " << player.gold;

		console.setCursorPos(pos.X+1, pos.Y+13);
		cout << "Weapon Equipped: ";
		console.setCursorPos(pos.X+1, pos.Y+14);
		cout << player.weapon << " +" << player.weapondmg;

		console.setCursorPos(pos.X+1, pos.Y+16);
		cout << "Drop Item [D]       ";

		console.setCursorPos(pos.X+size.Right-5, pos.Y+19);
		cout << items.size() << "/" << player.capacity;

		// output the status bars
		for (int i = 0; i < 10; ++i)
		{
			if (i < pHP)
			{
				stats[i].Char.AsciiChar = 255;
				stats[i].Attributes =  Red | Red << BG;
			}
			else
			{
				stats[i].Char.AsciiChar = 255;
				stats[i].Attributes = Grey | Grey << BG;
			}
		}

		WriteConsoleOutputA(console.outHnd, stats, buffSize, buffPos, &BuffArea);
	}
}

void Interface::display(Character& player)
{
	COORD crd = pos;
	console.setCursorPos(crd.X, crd.Y);
	for (int i = 0; i < size.Bottom - size.Top-1; ++i)
	{
		for (int x = 0; x < size.Right; x++)
			cout << " ";
		console.setCursorPos(crd.X, crd.Y += 1);
	}
	
	crd = pos;
	console.setCursorPos(pos.X, pos.Y);
	int vPadMenu = (size.Bottom-rows.size()-(title == "" ? 0 : 1))/2;

	// Output padding on top
	for (int i = 0; i < vPadMenu; ++i)
	{
		for (int x = 0; x < size.Right; x++)
			cout << " ";
		console.setCursorPos(crd.X, crd.Y += 1);
	}

	if (title != "")
	{
		cout << " " << title;
		for (int s = 0; s < (size.Right-title.length()-1); ++s)
			cout << " ";

		console.setCursorPos(crd.X, crd.Y += 1);
		for (int i = 0; i < size.Right; ++i)
			cout << "-";
		console.setCursorPos(crd.X, crd.Y += 1);
	}

	for (int i = 0; i < rows.size(); ++i)
	{
		if (title == "Instructions")
			cout << " " << rows[i];
		else
			cout << " " << i+1 << ") " << rows[i];
		for (int s = 0; s < (size.Right-rows[i].length()-4); ++s)
			cout << " ";
		console.setCursorPos(crd.X, crd.Y += 1);
	}



//	return;
//	bool hasTitle = 0;
//	if (getTitle().length() > 1)
//		hasTitle = 1;
//
////	int vPadMenu = (size.Bottom-rows.size()-hasTitle)/2;
//	if (title == "Inventory")
//		vPadMenu = 19;
//	COORD tmp = pos;
//	console.setCursorPos(tmp.X, tmp.Y);
//	
//
//	// Output padding on top
//	for (int i = 0; i < vPadMenu; ++i)
//	{
//		for (int x = 0; x < size.Right; x++)
//			cout << " ";
//		console.setCursorPos(tmp.X, tmp.Y += 1);
//	}
//
//	// Output menu title if exists
//	if (hasTitle)
//	{
//		cout << " " << getTitle();
//		for (int s = 0; s < (size.Right-getTitle().length()-1); ++s)
//			cout << " ";
//
//		console.setCursorPos(tmp.X, tmp.Y += 1);
//		for (int i = 0; i < size.Right; ++i)
//			cout << "-";
//		console.setCursorPos(tmp.X, tmp.Y += 1);
//	}
//	
//	// Output menu items
//	if (!rows.size())
//	{
//		cout << " No items";
//		for (int s = 0; s < (size.Right-9); ++s)
//			cout << " ";
//		console.setCursorPos(tmp.X, tmp.Y += 1);
//	}
//	else
//	{
//		for (int i = 0; i < rows.size(); ++i)
//		{
//			if (title == "Instructions")
//				cout << " " << rows[i];
//			else
//				cout << " " << i+1 << ") " << rows[i];
//			for (int s = 0; s < (size.Right-rows[i].length()-4); ++s)
//				cout << " ";
//			console.setCursorPos(tmp.X, tmp.Y += 1);
//		}
//	}
//
//	// Output padding on bottom
//	for (int i = 0; i < vPadMenu; ++i)
//	{
//		for (int x = 0; x < size.Right && tmp.Y < console.height; ++x)
//			cout << " ";
//		if (i < vPadMenu-1)
//			console.setCursorPos(tmp.X, tmp.Y += 1);
//	}
}
