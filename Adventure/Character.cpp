#include "Character.h"
#include "Interface.h"

Character::Character(Window w, Map& newWorld, string name, COORD loc, int hp, int def, int att, int dex)
	: world(newWorld), outHnd(w.outHnd), console(w), name(name), loc(loc), health(hp), maxhealth(hp), defence(def), attack(att), dex(dex), maxdex(dex), xp(0), xpmax(10), level(1)
{
	oldTile[0].Attributes = Green << BG;
	oldTile[0].Char.AsciiChar = 255;
	gold = 0;
	weapon = "Fist";
	weapondmg = 0;
	capacity = 6;
}


Character::Character(Window w, Map& newWorld, string name, COORD loc)
	: world(newWorld), outHnd(w.outHnd), console(w), name(name), loc(loc)
{

}

Character::~Character()
{
}

bool Character::Move(char d)
{
	COORD oldLoc = loc;

	char walkableTiles[13] = {
		4, 95, 179, 191, 192, 193, 194, 195, 196, 217, 218, 255
	};

	//set character
	switch (d) 
	{
		case 'u' :
			loc.Y--;
			break;
		case 'd':
			loc.Y++;
			break;
		case 'l':
			loc.X--;
			break;
		case 'r':
			loc.X++;
			break;
	}

	// IF u, d, l, r
	// TODO: Add position packet

	//check if moving to next zone
	bool newMap = 0; // assume not new world
	COORD tmp = world.zone;
	if (loc.X > 79)
	{
		tmp.X++;
		if (world.load(world.world, tmp))
		{
			loc.X = 0;
			newMap = 1;
			world.zone.X++;
		}
	}
	else if (loc.X < 0)
	{
		tmp.X--;
		if (world.load(world.world, tmp))
		{
			loc.X = 79;
			newMap = 1;
			world.zone.X--;
		}
	} 
	else if (loc.Y > 39)
	{
		tmp.Y--;
		if (world.load(world.world, tmp))
		{
			loc.Y = 0;
			newMap = 1;
			world.zone.Y--;
		}
	}
	else if (loc.Y < 0)
	{
		tmp.Y++;
		if (world.load(world.world, tmp))
		{
			loc.Y = 39;
			newMap = 1;
			world.zone.Y++;
		}
	}

	// check to see if we're walking onto a waypoint
	for (unsigned int i = 0; i < objects[world.world][world.zone.Y][world.zone.X].size(); ++i)
	{

		if (!objects[world.world][world.zone.Y][world.zone.X][i] )
			continue;
		if (objects[world.world][world.zone.Y][world.zone.X][i]->getToMap() == 99)
			break;

		// get the waypoint position
		Objects& waypt = *objects[world.world][world.zone.Y][world.zone.X][i];
		SMALL_RECT tPos = waypt.getPos();

		// check if we're inside the waypoint
		if ((loc.X >= tPos.Left && loc.X <= tPos.Right) && (loc.Y >= tPos.Top && loc.Y <= tPos.Bottom))
		{
			// if new waypoint set character position pos
			loc = waypt.getToPos();	

			// set change to a new world and zone
			world.world = waypt.getToMap();
			world.zone = waypt.getToZone();	

			// load the new world map & zone
			console.setCursorPos(0,0);
			for (int t = 0; t < console.height; ++t)
			{
				cout << "                                                                                ";
				Sleep(10);
			}
			world.load(world.world, world.zone);
			newMap = 1;
		}

	}

	//setup the buffer and location of tiles
	CHAR_INFO newTile[1];
	COORD buffSize = {1,1};
	COORD buffPos = {0,0};
	SMALL_RECT newBuffArea = {loc.X, loc.Y, loc.X, loc.Y};
	SMALL_RECT oldBuffArea = {oldLoc.X, oldLoc.Y, oldLoc.X, oldLoc.Y};

	//store newTile info
	ReadConsoleOutputA(outHnd, newTile, buffSize, buffPos, &newBuffArea);

	//check if newTile is valid
	bool valid = 0; // assume not valid
	for (int i=0; i < sizeof(walkableTiles); i++)
	{
		if (newTile[0].Char.AsciiChar == walkableTiles[i])
			valid = 1;
	}

	if (!valid)
	{
		loc = oldLoc;
		return 0;
	}

	//restore oldTile information if not new world
	if (!newMap)
		WriteConsoleOutputA(outHnd, oldTile, buffSize, buffPos, &oldBuffArea);

	//newTile becomes oldTile
	oldTile[0] = newTile[0];

	//get bg of newTile and set character/attributes
	int bg = (newTile[0].Attributes & Blue << BG ) | (newTile[0].Attributes & Red << BG) | (newTile[0].Attributes & Green << BG) | (newTile[0].Attributes & BACKGROUND_INTENSITY);
	newTile[0].Char.AsciiChar = 2;
	if (d == 'c')
		newTile[0].Attributes = Yellow | bg;
	else
		newTile[0].Attributes = White | bg;

	//place character on newTile
	WriteConsoleOutputA(outHnd, newTile, buffSize, buffPos, &newBuffArea);

	if (newMap)
		return 1;
	return 0;

}

void Character::BattleStatUpdate(Character& character, double timer, COORD pos)
{
	double hp = 0;
	
	// get the current health percent
	if (character.health > 0)
		hp = ((character.health + 0.0) / character.maxhealth) * 10;
	else
		character.health = 0;

	// setup the buff area for the stats
	CHAR_INFO eStats[20];
	COORD buffSize = {10,2};
	COORD buffPos = {0,0};
	SMALL_RECT BuffArea = {pos.X+3, pos.Y+1, pos.X+13, pos.Y+2};

	// output the status text
	console.setCursorPos(pos.X, pos.Y+=1);
	cout << "HP";
	console.setCursorPos(pos.X+14, pos.Y);
	cout << character.health << "/" << character.maxhealth << "  ";

	console.setCursorPos(pos.X, pos.Y+=1);
	cout << "SP";
	console.setCursorPos(pos.X+14, pos.Y);
	cout << int((timer > 10 ? 10 : timer)*10) << "%  ";

	console.setCursorPos(pos.X, pos.Y+=2);
	cout << "Attack:     " << character.attack + character.weapondmg;
	console.setCursorPos(pos.X, pos.Y+=1);
	cout << "Defence:    " << character.defence;
	console.setCursorPos(pos.X, pos.Y+=1);
	cout << "Dexterity:  " << character.maxdex;

	// output the status bars
	for (int i = 0; i < 10; ++i)
	{
		if (i < hp)
		{
			eStats[i].Char.AsciiChar = 255;
			eStats[i].Attributes =  Red | Red << BG;
		}
		else
		{
			eStats[i].Char.AsciiChar = 255;
			eStats[i].Attributes = Grey | Grey << BG;
		}

		if (i < timer)
		{
			eStats[i + 10].Char.AsciiChar = 255;
			eStats[i + 10].Attributes =  Blue | Blue << BG;
		}
		else
		{
			eStats[i + 10].Char.AsciiChar = 255;
			eStats[i + 10].Attributes = Grey | Grey << BG;
		}
	}

	WriteConsoleOutputA(console.outHnd, eStats, buffSize, buffPos, &BuffArea);
}

bool Character::Battle(Character& enemy, Interface& bag)
{

	// setup attack timer variables for player and enemy
	double etimer = 0;
	double ptimer = 0;

	// get the enemy name without whitespace
	stringstream nm;
	for (int i = 0; i < enemy.name.length(); ++i)	// put temporary name into monster name
		if (enemy.name[i] != ' ')					// do not include spaces
			nm << enemy.name[i];
	string ename = nm.str();
	
	console.setCursorPos(28,17);
	cout << "You are fighting a " << ename << "!";

	// create the stream for the text output
	while (health > 0 && enemy.health > 0) {

		if ( ptimer < 10 || etimer < 10)
		{
			Sleep(100);
			ptimer += maxdex/10;
			etimer += enemy.maxdex/10;
		}

		// update stat display for enemy
		COORD pos = {28,23};
		BattleStatUpdate(enemy, etimer, pos);

		// update stat display for player
		COORD ppos = {55,23};
		BattleStatUpdate(*this, ptimer, ppos);

		if (ptimer >= 10)
		{
			console.setCursorPos(28,33);
			cout << "Awaiting Orders!                           ";

			while(ptimer)
			{
				console.setCursorPos(28,17);
				if (GetAsyncKeyState(0x45) & 0x8000)	// detect the users action
				{
					//check if the enemy takes tamage or dodges
					int dodge = (enemy.maxdex/maxdex)*10;
					if (console.random(1,100) < dodge)
						cout << ename << " has dodged                     ";            
					else
					{
						int mod = console.random(-1, level);
						if (((attack + weapondmg)/enemy.defence) + mod > 0)
							enemy.health -= ((attack + weapondmg)/enemy.defence) + mod;

						if (mod > level / 2)
							cout << "You damage " << ename << " for " << ((attack + weapondmg)/enemy.defence) + mod << " critical!       ";
						else if (((attack + weapondmg)/enemy.defence) + mod > 0)
							cout << "You damage " << ename << " for " << ((attack + weapondmg)/enemy.defence) + mod << "              ";
						else
							cout << "You missed!                             ";
					}
					ptimer = 0;
				}
				else if (GetAsyncKeyState(0x52) & 0x8000)
				{
					cout << "You flee from the fight, " << ename << "'s HP is restored";
					enemy.health = enemy.maxhealth;
					Sleep(2000);
					return 0;
				}

				if (bag.useItem(*this, 1))
					ptimer = 0;
			}
			console.setCursorPos(28,33);
			cout << "                            ";
			Sleep(1000);
		}
		else if (etimer >= 10)
		{
			console.setCursorPos(28,17);

			//check if you dodge or take the damage
			int dodge = (maxdex/enemy.maxdex)*10;
			if (console.random(1,100) < dodge)
				cout << "You dodged the " << ename << "              ";
			else
			{

				int mod = console.random(-1, level);
					if ((enemy.attack/defence) + mod > 0)
						health -= (enemy.attack/defence + mod);

				if (mod > level / 2)
					cout << ename << " damages you for " << (enemy.attack/defence) + mod << " critical!       ";
				else if ((enemy.attack/defence) + mod > 0)
					cout << ename << " damages you for " << (enemy.attack/defence) + mod << "               ";
				else
					cout << ename << " misses                           ";
			}

			etimer = 0;
		}
		
		bag.display(*this, 0); // update inventory when player gets attacked
	}

	if (health > 0)
	{
		console.setCursorPos(28,17);
		// add required experience points
		xp += enemy.maxhealth + (enemy.defence + enemy.attack)/2;
		cout << "You have defeated the " << ename << " and gained " << enemy.maxhealth + (enemy.defence + enemy.attack)/2 << " XP!";
		Sleep(2000);
		return 1;
	}
	else
	{
		Sleep(1000);
		console.setCursorPos(28,17);
		cout << "You have been defeated by the " << ename;
		Sleep(3000);
		return 0;
	}
}
