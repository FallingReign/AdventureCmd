#include "game.h"
#include "Character.h"
#include "Map.h"
#include "Window.h"
#include "Interface.h"
#include "Objects.h"

// Network Includes
#include <UdpSocket.hpp>
#include <NetBuffer.hpp>
#include <NetAddress.hpp>
#include "GameClient.hpp"
#include "Timer.hpp"
#include <unordered_map>

// Create an array of vectors which point to objects[map][Y][X]
vector<Character*> monsters[2][4][6];
vector<Item*> loot[3][4][6];
vector<NPC*> npc[3][4][6];
vector<Objects*> objects[3][4][6];

void actionKeyPress(Map nw, Window con, Character& p, Interface& bag, Interface& shop);
void loadObjects(Map nw, Window console);

// define an array the hold all the general help information
const int helptipsSize = 10;
string helptips[helptipsSize] = {
	"Instructions",
	"To use an item press the corresponding number key.",
	"Weapons will be eqquiped, Consumables will be",
	"consumed, and other items can be used only at ",
	"certain areas in the game.",
	" ",
	"                 DROPPING ITEMS                   ",
	"To drop an item, press [D] followed by the item",
	"number, the item will appear in the world and you",
	"can always come back and pick it up later"
};

/*
	help.addRow("Movement:   [W, A, S, D] or Arrow keys to move the character      ");
	help.addRow("Action Key: [E] key near characters and objects to interact");
	help.addRow("Tooltips:   [ALT] Display the tooltips when in the inventory");
	help.addRow("Inventory:  [B, I] keys to access your inventory");
	help.addRow("            [1-9] keys to use the corresponding item"); */



const int helpSize = 20;
string helpMenu[helpSize] = {
	"                                                                      ",
	"  KEY BINDINGS                                                        ",
	"                                                                      ",
	"  Movement   [W, A, S, D] or Arrow keys to move the character.        ",
	"  Action Key [E] The action key changes its functionality depending on",
	"             each situation. pressing it near a monster will attack.  ",
	"             If you're near an item it will loot.                     ",
	"  Inventory  [B, I] keys to access your inventory.                    ",
	"             [1-9] keys to use the corresponding item.                ",
	"             [ALT] Displays the tooltips when in the inventory.       ",
	"             [ESC] Pauses the game and shows the title menu           ",
	"                                                                      ",
	"  COMBAT                                                              ",
	"                                                                      ",
	"  Status     HP Is your Health, you're wounded when you reach 0.      ",
	"             SP Is your Speed, you can attack when this reaches 100%  ",
	"  Attack     [E] Attacks the enemy with your equipped weapon.         ",
	"  Run Away   [R] enemies restore 100% when you run from the fight.    ",
	"             If you die fighting an enemy it restores 50% HP.         ",
	"             [1-9] keys to use the corresponding inventory item.      ",
	

};

// define array with items: name, type, drop %, qty random modifier
const int tooltipsSize = 10;
string tooltips[tooltipsSize][2] = {
	{"HP Potion", "Restores 100% health when not wounded"},
	{"Chapati", "Restores 50% health when not wounded"},
	{"Life Potion", "Restores 100% health when you're wounded"},
	{"Dynamite", "Mostly used by miners to collapse caves"},
	{"Rusty Key", "Looks like it might be useful for a rusty gate"},
	{"Sword", "A basic sword"},
	{"Longsword", "A strong sword"},
	{"Bloodsword", "A brutal sword made for bloodshed"},
	{"Soulbreaker", "A powerful sword enchanted with souls from the dead"},
	{"Pick Axe", "A tool given to us by Craver"}
};

// define array with items: name, type, drop %, qty random modifier, exclusive monster drop
const int lootDropRows = 27;
string lootDrops[lootDropRows][5] = {
	{"Gold", "gold", "50", "1"},
	{"Chapati", "consumable", "25", "2"},

	{"Sword", "weapon", "10", "2"},
	{"Longsword", "weapon", "5", "8"},
	{"Bloodsword", "weapon", "2", "20"},
	{"Soultaker", "weapon", "1", "40"},

	{"Bow", "weapon", "10", "2"},
	{"Longbow", "weapon", "5", "8"},
	{"Crossbow", "weapon", "2", "20"},
	{"Lichbane", "weapon", "1", "40"},

	{"Axe", "weapon", "10", "2"},
	{"Great Axe", "weapon", "5", "8"},
	{"Vorpal Axe", "weapon", "2", "20"},
	{"Interceptor", "weapon", "1", "40"},

	{"Mace", "weapon", "10", "2"},
	{"Iron Mace", "weapon", "5", "8"},
	{"Chrono Mace", "weapon", "2", "20"},
	{"Eliminator", "weapon", "1", "40"},

	{"Staff", "weapon", "10", "2"},
	{"Glowstaff", "weapon", "5", "8"},
	{"Battle Staff", "weapon", "2", "20"},
	{"Inferno", "weapon", "1", "40"},

	{"Wand", "weapon", "10", "2"},
	{"Spiral Wand", "weapon", "5", "8"},
	{"Scepter", "weapon", "2", "20"},
	{"Starfall", "weapon", "1", "40"},

	{"Skybeak", "normal", "100", "1", "Skyhawk"}
};

// define array with fusion names
const int fusionRows = 18;
string fusion[fusionRows][2] = {
	{"Sword", "Longsword"},
	{"Longsword", "Bloodsword"},
	{"Bloodsword", "Soultaker"},

	{"Bow", "Longbow"},
	{"Longbow", "Crossbow"},
	{"Crossbow", "Lichbane"},

	{"Axe", "Great Axe"},
	{"Great Axe", "Vorpal Axe"},
	{"Vorpal Axe", "Interceptor"},

	{"Mace", "Iron Mace"},
	{"Iron Mace", "Chrono Mace"},
	{"Chrono Mace", "Eliminator"},

	{"Staff", "Glowstaff"},
	{"Glowstaff", "Battle Staff"},
	{"Battle Staff", "Inferno"},

	{"Wand", "Spiral Wand"},
	{"Spiral Wand", "Scepter"},
	{"Scepter", "Starfall"}
};

const int shopSize = 4;
string shopMenu[shopSize] = {
	" 10 Gold   HP Potion     100% HP when not wounded    ",
	" 6 Gold    Chapati       50% HP when not wounded     ",
	" 20 Gold   Life Potion   100% HP when you're wounded ",
	" LEAVE SHOP"
};


// Network globals
anet::UdpSocket clientSock;
anet::NetAddress serverAddress("127.0.0.1", 33309);
static const anet::UInt16 PROTOCOL_ID = 50322;
std::unordered_map<unsigned int, Character> clientList;
unsigned int timeAccumulator = 0;

enum class MessageType
	: anet::UInt8
{
	Connection,
	Disconnection,
	ClientListing,
	Position,
	Room
};

void SendHeartbeat(const Character& player)
{
    anet::NetBuffer posBuffer;
    posBuffer << PROTOCOL_ID << (anet::UInt8)MessageType::Position << player.loc.X << player.loc.Y;
    clientSock.send(posBuffer, serverAddress);
}

int main()
{
	//clientSock.send()
	WSAData data;
	WSAStartup(MAKEWORD(2, 2), &data);

	//temporary location variable used in main
	COORD tLoc = {25,5};

	// create the console window
	Window console;

	// create the world
	Map newWorld(console);
	newWorld.world = 0;
	COORD zone = { 1, 0 };  // X, Y
	newWorld.zone = zone;
	
	// Create the player character and set to default position
	Character player(console, newWorld, "Rodney", tLoc, 5, 1, 2, 5);
	//tLoc.X++;
	//Character ally(console, newWorld, "Ally", tLoc, 5, 1, 2, 5);

	// create title menu map/background
	Map title(console);
	COORD tmp = {0,0};
	
	// create the start menu
	SMALL_RECT mSize = {0,0,18,10};
	COORD mPos = {(console.width/2) - (mSize.Right/2),17};
	Interface menu(console, mPos, mSize);
	menu.addRow("Play Game");
	menu.addRow("Multiplayer");
	menu.addRow("Show Help");
	menu.addRow("Quit Game");

	// create the instuctions menu
	SMALL_RECT hSize = {0,0,74,helpSize+10};
	COORD hPos = {(console.width/2)-(hSize.Right/2),7};
	Interface help(console, hPos, hSize);
	help.setTitle("Instructions");
	for (int i = 0; i < helpSize; ++i)
		help.addRow(helpMenu[i]);

	// create the inventory menu
	SMALL_RECT iSize = {0,0,21,console.height};
	COORD iPos = {0,0}; //(console.width/2)-(iSize.Right/2)
	Interface inventory(console, iPos, iSize);
	inventory.setTitle("Inventory");
	inventory.items.push_back(new Item("HP Potion", "consumable", 1));
	inventory.isVisible = 0; // sets the inventory to hidden

	// create the shop menu
	SMALL_RECT sSize = {0,0,57,20};
	COORD sPos = {0,0}; //(console.width/2)-(iSize.Right/2)
	Interface shop(console, sPos, sSize);
	shop.setTitle("Shop");
	for (int i = 0; i < shopSize; ++i)
		shop.addRow(shopMenu[i]);

	// Create a objects on map  eg. monsters[0][1][0] = map 0 on the 1, 0 zone
	// Define All Monsters
	// north of the town
	tLoc.X = 8; tLoc.Y = 8;
	monsters[0][1][1].push_back(new Character(console, newWorld, "Frog", tLoc, 2, 1, 2, 4));
	tLoc.X = 33; tLoc.Y = 33;
	monsters[0][1][1].push_back(new Character(console, newWorld, "Frog", tLoc, 2, 1, 2, 4));
	tLoc.X = 36; tLoc.Y = 5;
	monsters[0][1][1].push_back(new Character(console, newWorld, "Frog", tLoc, 2, 1, 2, 4));
	tLoc.X = 72; tLoc.Y = 37;
	monsters[0][1][1].push_back(new Character(console, newWorld, "Frog", tLoc, 2, 1, 2, 4));
	tLoc.X = 17; tLoc.Y = 2;
	monsters[0][1][1].push_back(new Character(console, newWorld, "Toad", tLoc, 4, 2, 2, 3));
	tLoc.X = 29; tLoc.Y = 21;
	monsters[0][1][1].push_back(new Character(console, newWorld, "Toad", tLoc, 4, 2, 2, 3));
	tLoc.X = 14; tLoc.Y = 30;
	monsters[0][1][1].push_back(new Character(console, newWorld, "Toad", tLoc, 4, 2, 2, 3));

	// bear cave entrance
	tLoc.X = 38; tLoc.Y = 4;
	monsters[0][1][0].push_back(new Character(console, newWorld, "  Bear  ", tLoc, 8, 3, 4, 3)); 

	// player enters bear cave
	tLoc.X = 16; tLoc.Y = 7;
	monsters[1][0][0].push_back(new Character(console, newWorld, " Bear  ", tLoc, 8, 3, 4, 3));
	tLoc.X = 3; tLoc.Y = 20;
	monsters[1][0][0].push_back(new Character(console, newWorld, "Spider", tLoc, 8, 2, 3, 4));

	tLoc.X = 13; tLoc.Y = 25;
	monsters[1][0][1].push_back(new Character(console, newWorld, "Bat", tLoc, 5, 1, 1, 7));
	tLoc.X = 37; tLoc.Y = 10;
	monsters[1][0][1].push_back(new Character(console, newWorld, "Snake", tLoc, 10, 2, 3, 5));  

	tLoc.X = 14; tLoc.Y = 9;
	monsters[1][1][0].push_back(new Character(console, newWorld, "Bat", tLoc, 5, 1, 1, 7));
	tLoc.X = 20; tLoc.Y = 25;
	monsters[1][1][0].push_back(new Character(console, newWorld, "Snake", tLoc, 10, 2, 3, 5));

	tLoc.X = 68; tLoc.Y = 29;
	monsters[1][1][1].push_back(new Character(console, newWorld, "Bat", tLoc, 5, 1, 1, 7)); 
	tLoc.X = 10; tLoc.Y = 2;
	monsters[1][1][1].push_back(new Character(console, newWorld, "Bat", tLoc, 5, 1, 1, 7)); 
	tLoc.X = 65; tLoc.Y = 15;
	monsters[1][1][1].push_back(new Character(console, newWorld, "Bat", tLoc, 5, 1, 1, 7)); 
	
	// dragon cave entrance
	tLoc.X = 2; tLoc.Y = 9;
	monsters[0][0][3].push_back(new Character(console, newWorld, "   Dragon   ", tLoc, 20, 4, 6, 7));

	// secret dragon caves
	tLoc.X = 69; tLoc.Y = 9;
	monsters[1][0][2].push_back(new Character(console, newWorld, "Snake", tLoc, 10, 2, 3, 5));
	tLoc.X = 8; tLoc.Y = 7;
	monsters[1][1][2].push_back(new Character(console, newWorld, "Snake", tLoc, 10, 2, 3, 5));
	tLoc.X = 58; tLoc.Y = 17;

	monsters[1][1][2].push_back(new Character(console, newWorld, "  Hatchling  ", tLoc, 10, 3, 5, 6)); 
	tLoc.X = 61; tLoc.Y = 21;
	monsters[1][1][2].push_back(new Character(console, newWorld, "  Hatchling  ", tLoc, 10, 3, 5, 6)); 

	tLoc.X = 48; tLoc.Y = 14;
	monsters[1][2][0].push_back(new Character(console, newWorld, "Bat", tLoc, 5, 1, 1, 7));
	tLoc.X = 13; tLoc.Y = 6;
	monsters[1][2][0].push_back(new Character(console, newWorld, "Spider", tLoc, 8, 2, 3, 4));
	tLoc.X = 5; tLoc.Y = 35;
	monsters[1][2][0].push_back(new Character(console, newWorld, "Spider", tLoc, 8, 2, 3, 4));

	tLoc.X = 64; tLoc.Y = 5;
	monsters[1][2][1].push_back(new Character(console, newWorld, "Bat", tLoc, 5, 1, 1, 7)); 
	tLoc.X = 15; tLoc.Y = 10;
	monsters[1][2][1].push_back(new Character(console, newWorld, "Bat", tLoc, 5, 1, 1, 7));

	tLoc.X = 25; tLoc.Y = 4;
	monsters[1][3][0].push_back(new Character(console, newWorld, "Grizzly", tLoc, 8, 3, 5, 4));
	tLoc.X = 26; tLoc.Y = 6;
	monsters[1][3][0].push_back(new Character(console, newWorld, "Bear", tLoc, 8, 3, 4, 3));

	tLoc.X = 55; tLoc.Y = 15;
	monsters[1][3][1].push_back(new Character(console, newWorld, "Snake", tLoc, 10, 2, 3, 5));
	
	//the mazes core
	tLoc.X = 10; tLoc.Y = 1;
	monsters[0][0][4].push_back(new Character(console, newWorld, "Fox", tLoc, 11, 3, 6, 7));
	tLoc.X = 26; tLoc.Y = 35;
	monsters[0][0][4].push_back(new Character(console, newWorld, "Fox", tLoc, 11, 3, 6, 7));
	tLoc.X = 25; tLoc.Y = 17;
	monsters[0][0][4].push_back(new Character(console, newWorld, "Skunk", tLoc, 9, 6, 5, 5));
	tLoc.X = 49; tLoc.Y = 1;
	monsters[0][0][4].push_back(new Character(console, newWorld, "Spider", tLoc, 8, 2, 3, 4));

	// the maze fish
	tLoc.X = 45; tLoc.Y = 18;
	monsters[0][0][5].push_back(new Character(console, newWorld, "Skunk", tLoc, 9, 6, 5, 5));


	// maze exit YAY
	tLoc.X = 3; tLoc.Y = 29;
	monsters[0][1][4].push_back(new Character(console, newWorld, "Fox", tLoc, 11, 3, 6, 7));

	// Skyhawk lake
	tLoc.X = 50; tLoc.Y = 33;
	monsters[0][1][5].push_back(new Character(console, newWorld, "Skyhawk", tLoc, 11, 4, 5, 10));

	// Define all Waypoint Objects
	CHAR_INFO tIcon;
	tIcon.Attributes = Black | Black << BG;
	tIcon.Char.AsciiChar = char(255);

	// Bears cave entrance enter
	SMALL_RECT tPos = {38, 3, 45, 3};	// position of the waypoint & size
	COORD tZone = {1, 1};				// zone we're traveling to (Y, X)
	tLoc.X = 55; tLoc.Y = 38;			// coord we're traveling to
	objects[0][1][0].push_back(new Waypoint(tPos, 1, tZone, tLoc, tIcon));

	// Bears cave exit
	tPos.Left = 79; tPos.Top = 26; tPos.Right = 79; tPos.Bottom = 30;
	tZone.X = 0; tZone.Y = 1;
	tLoc.X = 25; tLoc.Y = 30;
	objects[1][0][1].push_back(new Waypoint(tPos, 0, tZone, tLoc, tIcon));

	// Bears cave entrance return
	tPos.Left = 53; tPos.Top = 39; tPos.Right = 57; tPos.Bottom = 39;
	tZone.X = 0; tZone.Y = 1;
	tLoc.X = 41; tLoc.Y = 5;
	objects[1][1][1].push_back(new Waypoint(tPos, 0, tZone, tLoc, tIcon));
	
	// Secret Cave passage enter
	tPos.Left = 59; tPos.Top = 1; tPos.Right = 60; tPos.Bottom = 1;
	tZone.X = 1; tZone.Y = 3;
	tLoc.X = 59; tLoc.Y = 38;
	objects[1][2][1].push_back(new Waypoint(tPos, 1, tZone, tLoc, tIcon));

	// Secret Cave passage return
	tPos.Left = 59; tPos.Top = 39; tPos.Right = 60; tPos.Bottom = 39;
	tZone.X = 1; tZone.Y = 2;
	tLoc.X = 59; tLoc.Y = 2;
	objects[1][3][1].push_back(new Waypoint(tPos, 1, tZone, tLoc, tIcon));

	// Top Left House in Town exit
	tPos.Left = 22; tPos.Top = 35; tPos.Right = 26; tPos.Bottom = 35;
	tZone.X = 1; tZone.Y = 0;
	tLoc.X = 9; tLoc.Y = 8;
	objects[2][0][0].push_back(new Waypoint(tPos, 0, tZone, tLoc, tIcon));

	// Top Left House in Town entrance
	tIcon.Attributes = LightGrey | LightGrey << BG;
	tPos.Left = 9; tPos.Top = 7; tPos.Right = 10; tPos.Bottom = 7;
	tZone.X = 0; tZone.Y = 0;
	tLoc.X = 24; tLoc.Y = 33;
	objects[0][0][1].push_back(new Waypoint(tPos, 2, tZone, tLoc, tIcon));

	// Secret Dragon cave entrance
	tIcon.Attributes = Black | Black << BG;
	tPos.Left = 3; tPos.Top = 7; tPos.Right = 12; tPos.Bottom = 7;
	tZone.X = 2; tZone.Y = 0;
	tLoc.X = 15; tLoc.Y = 38;
	objects[0][0][3].push_back(new Waypoint(tPos, 1, tZone, tLoc, tIcon));

	// Secret Dragon cave entrance return
	tPos.Left = 12; tPos.Top = 39; tPos.Right = 18; tPos.Bottom = 39;
	tZone.X = 3; tZone.Y = 0;
	tLoc.X = 7; tLoc.Y = 9;
	objects[1][0][2].push_back(new Waypoint(tPos, 0, tZone, tLoc, tIcon));

	// Secret Dragon cave exit
	tPos.Left = 28; tPos.Top = 0; tPos.Right = 34; tPos.Bottom = 0;
	tZone.X = 2; tZone.Y = 1;
	tLoc.X = 54; tLoc.Y = 20;
	objects[1][1][2].push_back(new Waypoint(tPos, 0, tZone, tLoc, tIcon));	
	
	// Secret Dragon cave exit return
	tIcon.Char.AsciiChar = char(95); // change the waypoint colour
	tIcon.Attributes = Black | Green << BG;
	tPos.Left = 52; tPos.Top = 21; tPos.Right = 56; tPos.Bottom = 21;
	tZone.X = 2; tZone.Y = 1;
	tLoc.X = 31; tLoc.Y = 1;
	objects[0][1][2].push_back(new Waypoint(tPos, 1, tZone, tLoc, tIcon));
	tIcon.Char.AsciiChar = char(255); // rest to normal

	// Define all Trigger Objects
	// Town Boulders
	//CHAR_INFO tIcon;
	tPos.Left = 12; tPos.Top = 23; tPos.Right = 12; tPos.Bottom = 25;
	tIcon.Attributes = Grey | Green << BG;
	tIcon.Char.AsciiChar = char(Rock);
	objects[0][0][2].push_back(new Trigger(tPos, "Boulders", "Pick Axe", tIcon, "\n These boulders are too heavy to move.           \n I'll need to break them into smaller pieces.",  "\n That should do it!                             \n Now I can move the smaller peices out of the way"));
	
	// Maze gates
	tPos.Left = 47; tPos.Top = 29; tPos.Right = 47; tPos.Bottom = 30;
	tIcon.Attributes = LightCyan | Green << BG;
	tIcon.Char.AsciiChar = char(Hash);
	//																                     |                                                 |                                                 |                                                 |                                                 |                                                 |
	objects[0][0][3].push_back(new Trigger(tPos, "Maze Entrance", "Rusty Key", tIcon, " \n The gate says \"Wakefields Maze\"              \n This must be what that old man is always on    \n about... His \"Greatest Creation\"        ",  "\n The Rusty Key has unlocked the gate!"));
	

	// Define all loot
	//player ventures around town area
	tLoc.X = 57; tLoc.Y = 2;
	tIcon.Attributes = Yellow | Green << BG;
	tIcon.Char.AsciiChar = char(Gold);
	loot[0][1][0].push_back(new Item("Gold", "gold", 1,tIcon,tLoc));
	tLoc.X = 76; tLoc.Y = 11;
	loot[0][1][0].push_back(new Item("Gold", "gold", 1,tIcon,tLoc));
	tLoc.X = 55; tLoc.Y = 32;
	loot[0][2][0].push_back(new Item("Gold", "gold", 1,tIcon,tLoc));
	tLoc.X = 26; tLoc.Y = 30;
	loot[0][2][0].push_back(new Item("Gold", "gold", 1,tIcon,tLoc));

	tLoc.X = 41; tLoc.Y = 34;
	tIcon.Attributes = Red | Green << BG;
	tIcon.Char.AsciiChar = char(Heart);
	loot[0][2][0].push_back(new Item("HP Potion", "consumable", 2,tIcon,tLoc));

	tLoc.X = 59; tLoc.Y = 35;
	tIcon.Attributes = LightGrey | Green << BG;
	tIcon.Char.AsciiChar = char(Sword);
	loot[0][2][0].push_back(new Item("Shiv", "weapon", 1,tIcon,tLoc));

	// player goes into the caves
	tLoc.X = 7; tLoc.Y = 12;
	tIcon.Attributes = Yellow | Black << BG;
	tIcon.Char.AsciiChar = char(Gold);
	loot[1][1][1].push_back(new Item("Gold", "gold", 1,tIcon,tLoc));
	tLoc.X = 20; tLoc.Y = 34;
	loot[1][2][1].push_back(new Item("Gold", "gold", 1,tIcon,tLoc));
	tLoc.X = 18; tLoc.Y = 35;
	loot[1][2][1].push_back(new Item("Gold", "gold", 1,tIcon,tLoc));
	tLoc.X = 77; tLoc.Y = 11;
	loot[1][2][0].push_back(new Item("Gold", "gold", 1,tIcon,tLoc));
	tLoc.X = 28; tLoc.Y = 12;
	loot[1][2][0].push_back(new Item("Gold", "gold", 1,tIcon,tLoc));
	tLoc.X = 7; tLoc.Y = 33;
	loot[1][2][0].push_back(new Item("Gold", "gold", 1,tIcon,tLoc));
	tLoc.X = 11; tLoc.Y = 33;
	loot[1][2][0].push_back(new Item("Gold", "gold", 1,tIcon,tLoc));

	// player finds secret area in cave
	tLoc.X = 16; tLoc.Y = 26;
	tIcon.Attributes = Red | Black << BG;
	tIcon.Char.AsciiChar = char(Heart);
	loot[1][3][1].push_back(new Item("HP Potion", "consumable", 3,tIcon,tLoc));

	tLoc.X = 10; tLoc.Y = 26;
	tIcon.Attributes = Yellow | Black << BG;
	tIcon.Char.AsciiChar = char(Gold);
	loot[1][3][1].push_back(new Item("Gold", "gold", 1,tIcon,tLoc));
	tLoc.X = 15; tLoc.Y = 30;
	loot[1][3][1].push_back(new Item("Gold", "gold", 1,tIcon,tLoc));
	tLoc.X = 3; tLoc.Y = 31;
	loot[1][3][1].push_back(new Item("Gold", "gold", 1,tIcon,tLoc));
	tLoc.X = 8; tLoc.Y = 31;
	loot[1][3][1].push_back(new Item("Gold", "gold", 1,tIcon,tLoc));
	tLoc.X = 11; tLoc.Y = 34;
	loot[1][3][1].push_back(new Item("Gold", "gold", 1,tIcon,tLoc));
	tLoc.X = 17; tLoc.Y = 34;
	loot[1][3][1].push_back(new Item("Gold", "gold", 1,tIcon,tLoc));

	//player returns from secret area in cave
	tIcon.Attributes = Yellow | Black << BG;
	tIcon.Char.AsciiChar = char(Gold);
	tLoc.X = 35; tLoc.Y = 4;
	loot[1][0][0].push_back(new Item("Gold", "gold", 1,tIcon,tLoc));
	tLoc.X = 61; tLoc.Y = 22;
	loot[1][0][0].push_back(new Item("Gold", "gold", 1,tIcon,tLoc));

	tLoc.X = 17; tLoc.Y = 4;
	tIcon.Attributes = Red | Black << BG;
	tIcon.Char.AsciiChar = char(Heart);
	loot[1][0][0].push_back(new Item("HP Potion", "consumable", 1,tIcon,tLoc));

	tLoc.X = 4; tLoc.Y = 35;
	tIcon.Attributes = LightGrey | Black << BG;
	tIcon.Char.AsciiChar = char(Chisel);
	loot[1][0][1].push_back(new Item("Chisel", "normal", 1,tIcon,tLoc));




	// player ventures into the canyon
	tLoc.X = 31; tLoc.Y = 22;
	tIcon.Attributes = LightRed | Brown << BG;
	tIcon.Char.AsciiChar = char(Dynamite);
	loot[0][0][3].push_back(new Item("Dynamite", "consumable", 3, tIcon, tLoc));




	// player enters hidden dragon cave
	tLoc.X = 40; tLoc.Y = 33;
	tIcon.Attributes = Yellow | Black << BG;
	tIcon.Char.AsciiChar = char(Gold);
	loot[1][0][2].push_back(new Item("Gold", "gold", 1,tIcon,tLoc));
	tLoc.X = 40; tLoc.Y = 33;
	loot[1][0][2].push_back(new Item("Gold", "gold", 1,tIcon,tLoc));
	tLoc.X = 69; tLoc.Y = 23;
	loot[1][1][2].push_back(new Item("Gold", "gold", 1,tIcon,tLoc));
	tLoc.X = 66; tLoc.Y = 25;
	loot[1][1][2].push_back(new Item("Gold", "gold", 1,tIcon,tLoc));
	tLoc.X = 74; tLoc.Y = 25;
	loot[1][1][2].push_back(new Item("Gold", "gold", 1,tIcon,tLoc));
	tLoc.X = 63; tLoc.Y = 26;
	loot[1][1][2].push_back(new Item("Gold", "gold", 1,tIcon,tLoc));
	tLoc.X = 70; tLoc.Y = 26;
	loot[1][1][2].push_back(new Item("Gold", "gold", 1,tIcon,tLoc));
	tLoc.X = 63; tLoc.Y = 29;
	loot[1][1][2].push_back(new Item("Gold", "gold", 1,tIcon,tLoc));
	tLoc.X = 67; tLoc.Y = 29;
	loot[1][1][2].push_back(new Item("Gold", "gold", 1,tIcon,tLoc));
	tLoc.X = 73; tLoc.Y = 29;
	loot[1][1][2].push_back(new Item("Gold", "gold", 1,tIcon,tLoc));
	tLoc.X = 69; tLoc.Y = 30;
	loot[1][1][2].push_back(new Item("Gold", "gold", 1,tIcon,tLoc));
	tLoc.X = 46; tLoc.Y = 36;
	loot[1][1][2].push_back(new Item("Gold", "gold", 1,tIcon,tLoc));





	// player is inside toad cove
	tLoc.X = 54; tLoc.Y = 15;
	tIcon.Attributes = LightGrey | Green << BG;
	tIcon.Char.AsciiChar = char(Sword);
	loot[0][1][2].push_back(new Item("Bloodsword", "weapon", 6,tIcon,tLoc));

	tLoc.X = 49; tLoc.Y = 11;
	tIcon.Attributes = Blue | Green << BG;
	tIcon.Char.AsciiChar = char(Heart);
	loot[0][1][2].push_back(new Item("Life Potion", "consumable", 4,tIcon,tLoc));

	tLoc.X = 30; tLoc.Y = 28;
	tIcon.Attributes = Red | Green << BG;
	tIcon.Char.AsciiChar = char(Heart);
	loot[0][1][2].push_back(new Item("HP Potion", "consumable", 2,tIcon,tLoc));

	tLoc.X = 61; tLoc.Y = 15;
	tIcon.Attributes = Yellow | Green << BG;
	tIcon.Char.AsciiChar = char(Gold);
	loot[0][1][2].push_back(new Item("Gold", "gold", 1,tIcon,tLoc));
	tLoc.X = 58; tLoc.Y = 17;
	loot[0][1][2].push_back(new Item("Gold", "gold", 1,tIcon,tLoc));
	tLoc.X = 13; tLoc.Y = 28;
	loot[0][1][2].push_back(new Item("Gold", "gold", 1,tIcon,tLoc));


	//player enters the maze


	//player is inside the maze core
	tLoc.X = 20; tLoc.Y = 37;
	loot[0][0][4].push_back(new Item("Gold", "gold", 1,tIcon,tLoc));
	tLoc.X = 10; tLoc.Y = 38;
	loot[0][0][4].push_back(new Item("Gold", "gold", 1,tIcon,tLoc));
	tLoc.X = 75; tLoc.Y = 2;
	loot[0][0][4].push_back(new Item("Gold", "gold", 1,tIcon,tLoc));

	// the maze fish
	tLoc.X = 13; tLoc.Y = 26;
	loot[0][0][5].push_back(new Item("Gold", "gold", 1,tIcon,tLoc));
	tLoc.X = 13; tLoc.Y = 27;
	loot[0][0][5].push_back(new Item("Gold", "gold", 1,tIcon,tLoc));
	tLoc.X = 13; tLoc.Y = 27;
	loot[0][0][5].push_back(new Item("Gold", "gold", 1,tIcon,tLoc));
	tLoc.X = 13; tLoc.Y = 27;
	loot[0][0][5].push_back(new Item("Gold", "gold", 1,tIcon,tLoc));
	tLoc.X = 13; tLoc.Y = 28;
	loot[0][0][5].push_back(new Item("Gold", "gold", 1,tIcon,tLoc));
	tLoc.X = 12; tLoc.Y = 27;
	loot[0][0][5].push_back(new Item("Gold", "gold", 1,tIcon,tLoc));
	tLoc.X = 14; tLoc.Y = 27;
	loot[0][0][5].push_back(new Item("Gold", "gold", 1,tIcon,tLoc));


	// maze exit YAY
	tLoc.X = 18; tLoc.Y = 31;
	tIcon.Attributes = Red | Green << BG;
	tIcon.Char.AsciiChar = char(Heart);
	loot[0][1][4].push_back(new Item("HP Potion", "consumable", 2,tIcon,tLoc));

	// Define All Characters
	tIcon.Char.AsciiChar = char(Face);

	// Define Father Character
	tLoc.X = 20; tLoc.Y = 5;
	tIcon.Attributes = White | Green << BG;
	npc[0][0][1].push_back(new NPC(console, newWorld, "Father", tLoc, tIcon));
	//								|                                                 |                                                 |                                                 |                                                 |                                                 |
	npc[0][0][1][0]->talk.push_back(" Wow! You're really leaving already, huh?       \n It will be sad to see you go but venturing off \n into the world of exploration is an essential  \n part of growing up...");
	npc[0][0][1][0]->talk.push_back(" You should head north just above the town, there \n are heaps of frogs and toads that need culling.\n That way you can hone your combat skills without\n too many consequences.");
	npc[0][0][1][0]->talk.push_back("\n If you find yourself too injured remember, you \n can allways come home to rest. I'm sure your   \n Mother would love to look after you.");
	npc[0][0][1][0]->talk.push_back("END");

	// Define Patchie Character
	tLoc.X = 60; tLoc.Y = 30;
	tIcon.Attributes = LightGrey | Green << BG;
	npc[0][0][1].push_back(new NPC(console, newWorld, "Patchie", tLoc, tIcon, "", "", "", 1));
	//								|                                                 |                                                 |                                                 |                                                 |                                                 |
	npc[0][0][1][1]->talk.push_back("\n Hey there, care to join me? Im just doing some \n fishing... You know I saw a couple of miners   \n doing some \"blast fishing\" the other day.");
	npc[0][0][1][1]->talk.push_back("\n If you ask me fishing with dynamite is crazy!  \n I still prefer my trusty fishing rod");

	// Define Mother Character
	tLoc.X = 57; tLoc.Y = 27;
	tIcon.Attributes = Purple | LightGrey << BG;
	npc[2][0][0].push_back(new NPC(console, newWorld, "Mother", tLoc, tIcon));
	//								|                                                 |                                                 |                                                 |                                                 |                                                 |
	npc[2][0][0][0]->talk.push_back("\n Oh honey! its lovely to see you.               \n You look tired, go get some rest and I'll fix \n you up something nice for the road.");
	npc[2][0][0][0]->talk.push_back("\n Always remember to stop and drink some water \n by the river. It will keep you strong and \n healthy...");

	// Define Jesca Character
	tLoc.X = 20; tLoc.Y = 20;
	tIcon.Attributes = Yellow | Green << BG;
	npc[0][1][1].push_back(new NPC(console, newWorld, "Jesca", tLoc, tIcon, "", "", "", 1));
	//								|                                                 |                                                 |                                                 |                                                 |                                                 |
	npc[0][1][1][0]->talk.push_back("\n Did you know that when you place two weapons of\n the same type together they auto-magically fuse \n and create a more powerful weapon?");
	npc[0][1][1][0]->talk.push_back("\n I just found a sword earlier and presto!       \n Now I can cut down my enemies in half the time.");

	// Define Wakefield Character
	tLoc.X = 10; tLoc.Y = 30;
	tIcon.Attributes = Yellow | Green << BG;
	npc[0][0][0].push_back(new NPC(console, newWorld, "Wakefield", tLoc, tIcon, "Fish", "Rusty Key", "normal", 1));
	//								|                                                 |                                                 |                                                 |                                                 |                                                 |
	npc[0][0][0][0]->talk.push_back("\n Howdy there partner. You wouldn't have here a  \n something I could eat would ya? I'd blow up the\n Lake for some of them fish right there, Ahh Huh!");
	npc[0][0][0][0]->talk.push_back(" I tell ya what!                                \n You go and get me some o' them Fish, an I'll   \n give you the key to the greatest creation there\n ever was, Ahh Huh. I aint just being bias, it  \n really is something!");
	npc[0][0][0][0]->talk.push_back("ITEM");
	npc[0][0][0][0]->talk.push_back("END");
	
	// Define Kael Character
	tLoc.X = 35; tLoc.Y = 8;
	tIcon.Attributes = LightGrey | Brown << BG;
	npc[0][0][3].push_back(new NPC(console, newWorld, "Kael", tLoc, tIcon, "Supplies", "INVENTORY", "normal", 9));
	//								|                                                 |                                                 |                                                 |                                                 |                                                 |
	npc[0][0][3][0]->talk.push_back("\n Help us! that big mean Dragon injured my sister \n and took our bag of medical supplies.");
	npc[0][0][3][0]->talk.push_back("\n Can you please help?                           \n I would go myself but I dont want to leave my \n sister alone.");
	npc[0][0][3][0]->talk.push_back("\n He is really strong, so maybe find something   \n that has a bit of BOOM to it. That should take \n care of the dumb Dragon!");
	npc[0][0][3][0]->talk.push_back("ITEM");
	npc[0][0][3][0]->talk.push_back("DELETE");
	
	// Define Zara Character
	tLoc.X = 38; tLoc.Y = 9;
	tIcon.Attributes = LightCyan | Brown << BG;
	npc[0][0][3].push_back(new NPC(console, newWorld, "Zara", tLoc, tIcon, "", "", "Kael", 1));
	//								|                                                 |                                                 |                                                 |                                                 |                                                 |
	npc[0][0][3][1]->talk.push_back(" Argh! Tell Kael I'm fine. Look at the smile on \n my face. Do I look injured to you?             \n I still would love it if you could get me back \n my supplies.");
	npc[0][0][3][1]->talk.push_back("\n You're awesome, when you get them, Kael will   \n take them off your hands and give you a reward!");
	npc[0][0][3][1]->talk.push_back("DELETEIF");


	// Define Craver Character
	tLoc.X = 64; tLoc.Y = 4;
	tIcon.Attributes = Cyan | Black << BG;
	npc[1][1][0].push_back(new NPC(console, newWorld, "Craver", tLoc, tIcon, "Chisel", "Pick Axe", "normal", 1));
	//								|                                                 |                                                 |                                                 |                                                 |                                                 |
	npc[1][1][0][0]->talk.push_back("\n Oh Hello there, the name's Craver.             \n I dont see too many people around these parts  \n you know with the cave critters and all.");
	npc[1][1][0][0]->talk.push_back("\n It's actually great timing. I was working in the\n southern area of the cave and I left my chisel \n behind. Do you think you could get it for me?");
	npc[1][1][0][0]->talk.push_back("ITEM");
	npc[1][1][0][0]->talk.push_back(" What a difference it makes when you have the   \n right tools. I didn't think I would ever get   \n this project done... It helps that I took a  \n few days off work though, HaHa!");
	npc[1][1][0][0]->talk.push_back("END");

	// Define Quinto Character
	tLoc.X = 68; tLoc.Y = 2;
	tIcon.Attributes = LightBlue | Green << BG;
	npc[0][0][4].push_back(new NPC(console, newWorld, "Quinto", tLoc, tIcon, "Compass", "Life Potion", "consumable", 3));
	//								|                                                 |                                                 |                                                 |                                                 |                                                 |
	npc[0][0][4][0]->talk.push_back(" I think I'm lost!?! I was in here with my      \n brother but he's gone missing with my compass. \n I have no clue where to go without it. ");
	npc[0][0][4][0]->talk.push_back("\n If you get it back I'm sure I can spare some of\n my potions as a reward. ");
	npc[0][0][4][0]->talk.push_back("ITEM");
	npc[0][0][4][0]->talk.push_back("\n So he was really just going to leave me in here.\n Lesson learned, dont trust my Brother... ");
	npc[0][0][4][0]->talk.push_back("END");

	// Define Eliude Character
	tLoc.X = 59; tLoc.Y = 31;
	tIcon.Attributes = LightBlue | Green << BG;
	npc[0][1][4].push_back(new NPC(console, newWorld, "Eliude", tLoc, tIcon, "Skybeak", "Compass", "normal", 1));
	//								|                                                 |                                                 |                                                 |                                                 |                                                 |
	npc[0][1][4][0]->talk.push_back("\n Oh you're here for the compass aren't you?  \n Did my brother send you?");
	npc[0][1][4][0]->talk.push_back("\n I thought I would leave him in the maze to see \n if he could fend for himself. He relies on this\n compass way too much for his own good.");
	npc[0][1][4][0]->talk.push_back("\n I'll tell you what, there's a rare bird called a\n Skyhawk. If you can brink me its beak I will  \n trade you this compass.");
	npc[0][1][4][0]->talk.push_back("ITEM");
	npc[0][1][4][0]->talk.push_back("\n Best of luck to ya! \n I hope Quinto's reward is worth it.");
	npc[0][1][4][0]->talk.push_back("END");
	
	bool gameover = 0;
	bool firstrun = 1;
	bool showmenu = 1;

	// get the handle of the window
	HWND hWnd =  GetConsoleWindow();


	// Main Game Loop
	bool multiplayer = false;
    Timer timer;
	while (!gameover) 
	{
		if (showmenu)
		{
			// display the title menu
			title.load('t', tmp);
			menu.display(player);
			bool displayHelp = 0;

			bool option = 0;

			// Start Main Menu Loop
			do
			{
				if ((GetAsyncKeyState(0x31) || GetAsyncKeyState(VK_NUMPAD1)) || (GetAsyncKeyState(VK_ESCAPE) & 0x8000))			// Key 1 detection (Play Game)
				{
					option = 1;
				}
				else if (GetAsyncKeyState(0x32) || GetAsyncKeyState(VK_NUMPAD3))	// Key 2 detection (Multiplayer)
				{
					if (clientSock.bind(0) == false)
					{
						option = 0;
						std::cout << WSAGetLastError();
					}

                    // Ask for a connection.
					/*anet::NetBuffer connBuffer;
					connBuffer << PROTOCOL_ID << (anet::UInt8)MessageType::Connection;
					clientSock.send(connBuffer, serverAddress);*/

                    // Broadcast connection request.
                    int useBroadcast = 1;
                    ::setsockopt(clientSock.getSocketID(), SOL_SOCKET, SO_BROADCAST, (char*)useBroadcast, sizeof(useBroadcast));

                    anet::NetBuffer broadcastBuffer;
                    broadcastBuffer << PROTOCOL_ID << (anet::UInt8)MessageType::Connection;
                    anet::NetAddress broadcastAddr("255.255.255.255", 33309);
                    clientSock.send(broadcastBuffer, broadcastAddr);

                    useBroadcast = 0;
                    ::setsockopt(clientSock.getSocketID(), SOL_SOCKET, SO_BROADCAST, (char*)useBroadcast, sizeof(useBroadcast));

                    clientSock.setBlocking(false);

					// add multiplayer code
					option = 1;
					multiplayer = true;
				}
				else if (GetAsyncKeyState(0x33) || GetAsyncKeyState(VK_NUMPAD2))	// Key 3 detection (Instructions)
				{
					if (!displayHelp)
					{
						help.display(player);
						displayHelp = 1;
					}
					else
					{
						title.load('t', tmp);
						menu.display(player);
						displayHelp = 0;
					}
				}
				else if (GetAsyncKeyState(0x34) || GetAsyncKeyState(VK_NUMPAD3))	// Key 4 detection (Quit)
				{
					gameover = 1;
					option = 1;
				}
				Sleep(100);
			} while (!option);
			showmenu = 0;

			//close the menu & load the new world, objects and characters
			if (!gameover)
			{
				menu.setRowText(0, "Resume Game");
				newWorld.load(newWorld.world, newWorld.zone);
				loadObjects(newWorld, console);
				player.Move('x');
				firstrun = 0;

			}
		}

		//wait if window not active
	   // while (GetForegroundWindow() != hWnd) { Sleep(100);}

		bool hasMoved = false;
		bool mapChange = false;
		if (GetForegroundWindow() == hWnd)
		{
			// Detect each key state 
			if (GetAsyncKeyState(VK_ESCAPE) & 0x8000)
			{
				showmenu = 1;
				while (GetAsyncKeyState(VK_ESCAPE) & 0x8000); // wait for key up
			}

			if (GetAsyncKeyState(VK_UP) || GetAsyncKeyState(0x57))		// W & UP Detection
			{
				if (player.Move('u')) //returns true when character changes map
				{
					loadObjects(newWorld, console);
					mapChange = true;
				}

				hasMoved = true;
			}
			if (GetAsyncKeyState(VK_DOWN) || GetAsyncKeyState(0x53))	// S & DOWN Detection
			{
				if (player.Move('d')) //returns true when character changes map
				{
					loadObjects(newWorld, console);
					mapChange = true;
				}
				hasMoved = true;
			}
			if (GetAsyncKeyState(VK_LEFT) || GetAsyncKeyState(0x41))	// A & LEFT Detection
			{
				if (player.Move('l')) //returns true when character changes map
				{
					loadObjects(newWorld, console);
					mapChange = true;
				}
				hasMoved = true;
			}
			if (GetAsyncKeyState(VK_RIGHT) || GetAsyncKeyState(0x44))	// D & RIGHT Detection
			{
				if (player.Move('r')) //returns true when character changes map
				{
					loadObjects(newWorld, console);
					mapChange = true;
				}
				hasMoved = true;
			}
		}
        // player.world.world = WORLD ID. (INT32)
        // player.world.zone.X = ZONE X (INT16)
        // player.world.zone.Y = ZONE Y (INT16)
        // player.loc = PLAYER LOCATION. (INT16)

		// Send 
        timeAccumulator += timer.Restart();

		if ((hasMoved || timeAccumulator >= 10) && multiplayer)
		{
            SendHeartbeat(player);

            timeAccumulator = 0;
		}

        if (mapChange)
        {
            anet::NetBuffer changeBuffer;
            changeBuffer << PROTOCOL_ID << (anet::UInt8)MessageType::Room << newWorld.world << newWorld.zone.X << newWorld.zone.Y;
            clientSock.send(changeBuffer, serverAddress);

            clientList.clear();
        }

        // Recv
        anet::NetBuffer recvBuffer;
        anet::NetAddress recvAddr;
        int bytesRecv = clientSock.receive(recvBuffer, recvAddr);

        if (bytesRecv > 0 && recvAddr == serverAddress)
        {
            anet::UInt16 protID;
            recvBuffer >> protID;

            if (protID == PROTOCOL_ID)
            {
                anet::UInt8 mid;
                recvBuffer >> mid;
                MessageType type = static_cast<MessageType>(mid);

                switch (type)
                {
                case MessageType::ClientListing: // Gotta add a new client to our own list.
                {
                    // [HASH] [X] [Y] [ROOM]
                    unsigned int hash;
                    short x, y, zoneX, zoneY;
					COORD cLoc;
                    int worldID;

                    recvBuffer >> hash >> x >> y >> worldID >> zoneX >> zoneY;
					
					COORD loc;
					loc.X = x;
					loc.Y = y;

                    Character client(console, newWorld, "Client", loc);
                    //client.x = x;
                    //client.y = y;
                    client.worldid = worldID;
                    client.zoneX = zoneX;
                    client.zoneY = zoneY;

                    // If they exist in our space then add them. Otherwise we just ignore this listing.
                    if (client.worldid == player.world.world && client.zoneX == player.world.zone.X && client.zoneY == player.world.zone.Y)
                        clientList.insert(std::pair<unsigned int, Character>(hash, client));
                    break;
                }
                case MessageType::Position:
                {
                    unsigned int hash;
                    short x, y;

                    recvBuffer >> hash >> x >> y;

                    auto& cit = clientList.find(hash);

                    if (cit != clientList.end())
                    {
                        auto& client = cit->second;
						//client.x = x;
                        //client.y = y;
						client.loc.X = x;
						client.loc.Y = y;
                    }
                    break;
                }
                case MessageType::Room:
                {
                    // Someone changed rooms. If they are not in ours then get rid of them else add them or ignore.
                    anet::UInt32 hash;
                    anet::Int32 worldID;
                    anet::Int16 zoneX, zoneY;
                    
                    recvBuffer >> hash >> worldID >> zoneX >> zoneY;

                    auto& cit = clientList.find(hash);
                    auto& world = player.world;

                    bool accept = (worldID == world.world) && (zoneX == world.zone.X) && (zoneY == world.zone.Y);

                    //std::cout << worldID << " " << zoneX << " " << zoneY;

                    if (cit != clientList.end())
                    {
                        // Exists. Compare. Remove if no longer in same location.
                        if (!accept)
                        {
                            clientList.erase(cit);
                        }
                    }
                    else if (accept) // Add to our list if in our location.
                    {

						COORD loc;
						loc.X = 0;
						loc.Y = 0;

						Character client(console, newWorld, "Client", loc);
                        client.worldid = worldID;
                        client.zoneX = zoneX;
                        client.zoneY = zoneY;
                        clientList.insert(std::pair<unsigned int, Character>(hash, client));
                    }

                    break;
                }
                case MessageType::Disconnection:
                {
                    anet::UInt8 code;
                    recvBuffer >> code;

                    if (code == 0) // We were disconnected.
                    {
                        multiplayer = false;
                        showmenu = true;
                    }
                    else if (code == 1) // Someone else disconnected.
                    {
                        unsigned int hash;
                        recvBuffer >> hash;

                        auto& c = clientList.find(hash);

                        if (c != clientList.end())
                        {
                            auto& client = c->second;
                            clientList.erase(c);

                            player.world.load(player.world.world, player.world.zone); 
                            loadObjects(newWorld, console);
                            player.Move('x');
                        }
                    }

                    break;
                }
                }
            }
        }

        // Clients
        for (auto& c : clientList)
        {
            player.world.load(player.world.world, player.world.zone);
            loadObjects(newWorld, console);
            auto& client = c.second;

            client.Move('c');
            player.Move('x');
        }

        ////////////
		
		if (GetAsyncKeyState(0x45) & 0x8000)					// E detection for Action button
		{
			actionKeyPress(newWorld, console, player, inventory, shop);
			while (GetAsyncKeyState(0x45) & 0x8000); // wait for key up
		}
		if ((GetAsyncKeyState(0x42) & 0x8000) || (GetAsyncKeyState(0x49) & 0x8000))		// I or B Detection for inventory
		{	
			if (!inventory.isVisible)
			{
				inventory.isVisible = 1;
				inventory.display(player, 1);
				// wait until user lets go of keys
				while ((GetAsyncKeyState(0x42) & 0x8000) || (GetAsyncKeyState(0x49) & 0x8000) || (GetAsyncKeyState(0x44) & 0x8000));
			}
		}

		// Loop while inventory is open
		while (inventory.isVisible)
		{
            SendHeartbeat(player);

			// close the inventoy
			if ((GetAsyncKeyState(0x42) & 0x8000) || (GetAsyncKeyState(0x49) & 0x8000))
			{	
				inventory.isVisible = 0;
				newWorld.load(newWorld.world, newWorld.zone);
				loadObjects(newWorld, console);
				player.Move('x');		// Show the player character
				
				// wait until user lets go of keys
				while ((GetAsyncKeyState(0x42) & 0x8000) || (GetAsyncKeyState(0x49) & 0x8000));
			}

			//run through a loop of all items and check each keystate
			if (inventory.useItem(player))
			{
				// reload the world
				newWorld.load(newWorld.world, newWorld.zone);
				loadObjects(newWorld, console);
				player.Move('x');		// Show the player character
				if (inventory.isVisible)
					inventory.display(player, 1);
			}

			// if the alt key is held display the tooltips while in menu
			if (GetAsyncKeyState(VK_MENU) & 0x8000)
			{
				COORD ttPos = inventory.getPos();

				// output the box for all the item information
				ttPos.Y = 20;
				if (ttPos.X < console.width / 2)
				{
					// output item description
					int tmpy = 20;
					console.setCursorPos(ttPos.X + inventory.size.Right, tmpy);
					cout << "-------------------------------------------------- ";
					for (int s = 0; s < console.height - ttPos.Y-1; ++s)
					{
						console.setCursorPos(ttPos.X + inventory.size.Right, ++tmpy);
						cout << "                                                  |";
					}
					//output general information
					int helpy = ttPos.Y - (helptipsSize + 2);
					console.setCursorPos(ttPos.X + inventory.size.Right, helpy);
					cout << "-------------------------------------------------- ";
					for (int s = 0; s < helptipsSize; ++s)
					{
						console.setCursorPos(ttPos.X + inventory.size.Right, ++helpy);
						cout << "                                                  |";
						console.setCursorPos(ttPos.X + inventory.size.Right, helpy);
						cout << helptips[s];
					}

					console.setCursorPos(ttPos.X + inventory.size.Right, ++helpy);
					cout << "                                                  |";
				}
				else
				{
					int tmpy = 20;
					console.setCursorPos(ttPos.X - 51, tmpy);
					cout << " --------------------------------------------------";
					for (int s = 0; s < console.height - ttPos.Y-1; ++s)
					{
						console.setCursorPos(ttPos.X -51, ++tmpy);
						cout << "|                                                  ";
					}
					//output general information
					int helpy = ttPos.Y - (helptipsSize + 2);
					console.setCursorPos(ttPos.X - 51, helpy);
					cout << " --------------------------------------------------";
					for (int s = 0; s < helptipsSize; ++s)
					{
						console.setCursorPos(ttPos.X - 51, ++helpy);
						cout << "|                                                  ";
						console.setCursorPos(ttPos.X - 50, helpy);
						cout << helptips[s];
					}

					console.setCursorPos(ttPos.X - 51, ++helpy);
					cout << "|                                                  ";
				}

				//output the description for each item
				ttPos.Y = 21;
				for (int t = 0; t < inventory.items.size(); ++t)
				{

					for (int tt = 0; tt < tooltipsSize; ++tt)
						if (tooltips[tt][0] == inventory.items[t]->getItemName())
						{
							if (ttPos.X < console.width / 2)
							{
								console.setCursorPos(ttPos.X + inventory.size.Right, ttPos.Y);
								cout << "- " << tooltips[tt][1];
							}
							else
							{
								console.setCursorPos(ttPos.X-tooltips[tt][1].length()-2, ttPos.Y);
								cout << tooltips[tt][1] << " -";
							}

						}
						ttPos.Y++;
				}
				
                while (GetAsyncKeyState(VK_MENU) & 0x8000) { if(multiplayer) SendHeartbeat(player); } //wait for key up
				newWorld.load(newWorld.world, newWorld.zone);
				loadObjects(newWorld, console);
				inventory.display(player, 1);
				player.Move('x');		// Show the player character
			}
		}

		// check to see if we have leveled
		if (player.xp >= player.xpmax)
		{
			player.health = ++player.maxhealth;
			player.level++;
			player.xp = player.xp - player.xpmax;
			player.xpmax += ((player.xpmax/1.5)*1.7);
			inventory.display(player, 1);
			inventory.message("\n You have reached a new level \n Where will you place the first point? \n\n 1) Attack  2) Defence  3) Health  4) Dexterity", "Congratulations!", -1);
			int points = 2;
			bool msg2 = 1;
			while (points > 0)
			{
				if (points == 1 && msg2)
				{
					inventory.display(player, 1);
					inventory.message("\n You have placed your first skill point \n Where will you place the second point? \n\n 1) Attack  2) Defence  3) Health  4) Dexterity", "Well Done!", -1);
					msg2 = 0;
				}
				if ((GetAsyncKeyState(0x31) & 0x8000) || (GetAsyncKeyState(VK_NUMPAD1) & 0x8000))		// Key 1 detection (Attack+)
				{
					player.attack++;
					points --;
					while ((GetAsyncKeyState(0x31) & 0x8000) || (GetAsyncKeyState(VK_NUMPAD1) & 0x8000)); //wait for key up
				}
				else if ((GetAsyncKeyState(0x32) & 0x8000) || (GetAsyncKeyState(VK_NUMPAD2) & 0x8000))	// Key 2 detection (Defence+)
				{
					player.defence++;
					points --;
					while ((GetAsyncKeyState(0x32) & 0x8000) || (GetAsyncKeyState(VK_NUMPAD2) & 0x8000)); //wait for key up
				}
				else if ((GetAsyncKeyState(0x33) & 0x8000) || (GetAsyncKeyState(VK_NUMPAD3) & 0x8000))	// Key 3 detection (Health+)
				{
					player.health = player.maxhealth+=2;
					points --;
					while ((GetAsyncKeyState(0x33) & 0x8000) || (GetAsyncKeyState(VK_NUMPAD3) & 0x8000)); //wait for key up
				}
				else if ((GetAsyncKeyState(0x34) & 0x8000) || (GetAsyncKeyState(VK_NUMPAD4) & 0x8000))	// Key 4 detection (Dexterity+)
				{
					player.maxdex++;
					points --;
					while ((GetAsyncKeyState(0x34) & 0x8000) || (GetAsyncKeyState(VK_NUMPAD4) & 0x8000)); //wait for key up
				}
			}
			inventory.isVisible = 0;
			newWorld.load(newWorld.world, newWorld.zone);
			loadObjects(newWorld, console);
			player.Move('x');		// Show the player character
		}

		Sleep(60);
	}


	// cleanup the mess we made in the heap for monsters
	for ( unsigned int i = 0; i < monsters[newWorld.world][newWorld.zone.Y][newWorld.zone.X].size(); ++i)
	{
		if (monsters[newWorld.world][newWorld.zone.Y][newWorld.zone.X][i])
		{
			delete monsters[newWorld.world][newWorld.zone.Y][newWorld.zone.X][i];
		}
	}
	// clean up for loot
	for ( unsigned int i = 0; i < loot[newWorld.world][newWorld.zone.Y][newWorld.zone.X].size(); ++i)
	{
		if (loot[newWorld.world][newWorld.zone.Y][newWorld.zone.X][i])
		{
			delete loot[newWorld.world][newWorld.zone.Y][newWorld.zone.X][i];
		}
	}
	// clean up for objects
	for ( unsigned int i = 0; i < objects[newWorld.world][newWorld.zone.Y][newWorld.zone.X].size(); ++i)
	{
		if (objects[newWorld.world][newWorld.zone.Y][newWorld.zone.X][i])
		{
			delete objects[newWorld.world][newWorld.zone.Y][newWorld.zone.X][i];
		}
	}
	// clean up the items vector
	for ( unsigned int i = 0; i < inventory.items.size(); ++i)
		if (inventory.items[i])
		{
			delete inventory.items[i];
		}

	clientSock.unBind();
	WSACleanup();
	return 0;
}

void loadObjects(Map nw, Window console)
{

	//set each map to their own default BG colour
	int colourMap[2];
	colourMap[0] = White | Green << BG;
	colourMap[1] = White | Black << BG;

	// set each zone to their own defualt BG colour (if false then functions use map default)
	
	static int colourZone[2][4][6] = { { {0} } };

	colourZone[0][0][3] = White | Black << BG;
	colourZone[0][1][0] = White | Black << BG;

	//colourZone[0][1][1] = White | Green << BG;

	// load each monster into the map
	for ( unsigned int i = 0; i < monsters[nw.world][nw.zone.Y][nw.zone.X].size(); ++i)
	{
		if (monsters[nw.world][nw.zone.Y][nw.zone.X][i])
		{
			if (colourZone[nw.world][nw.zone.Y][nw.zone.X] > 0) // check if zone has a default BG
				SetConsoleTextAttribute(console.outHnd,colourZone[nw.world][nw.zone.Y][nw.zone.X]);
			else if (colourZone[nw.world] > 0) //check if map has a default BG
				SetConsoleTextAttribute(console.outHnd, colourMap[nw.world]);
			else // default to black
				SetConsoleTextAttribute(console.outHnd, White | Black << BG);

			console.setCursorPos(monsters[nw.world][nw.zone.Y][nw.zone.X][i]->loc.X, monsters[nw.world][nw.zone.Y][nw.zone.X][i]->loc.Y);
			cout << monsters[nw.world][nw.zone.Y][nw.zone.X][i]->name;
		}
	}

	// load each loot item into the map
	for ( unsigned int i = 0; i < loot[nw.world][nw.zone.Y][nw.zone.X].size(); ++i)
	{
		if (loot[nw.world][nw.zone.Y][nw.zone.X][i])
		{
			console.setCursorPos(loot[nw.world][nw.zone.Y][nw.zone.X][i]->loc.X, loot[nw.world][nw.zone.Y][nw.zone.X][i]->loc.Y);
			SetConsoleTextAttribute(console.outHnd, loot[nw.world][nw.zone.Y][nw.zone.X][i]->icon.Attributes);
			cout << loot[nw.world][nw.zone.Y][nw.zone.X][i]->icon.Char.AsciiChar;
		}
	}

	// load each npc into the map
	for ( unsigned int i = 0; i < npc[nw.world][nw.zone.Y][nw.zone.X].size(); ++i)
	{
		if (npc[nw.world][nw.zone.Y][nw.zone.X][i])
		{
			console.setCursorPos(npc[nw.world][nw.zone.Y][nw.zone.X][i]->loc.X, npc[nw.world][nw.zone.Y][nw.zone.X][i]->loc.Y);
			SetConsoleTextAttribute(console.outHnd, npc[nw.world][nw.zone.Y][nw.zone.X][i]->icon.Attributes);
			cout << npc[nw.world][nw.zone.Y][nw.zone.X][i]->icon.Char.AsciiChar;

			if (npc[nw.world][nw.zone.Y][nw.zone.X][i]->icon.Char.AsciiChar == Face)
			{
				console.setCursorPos(npc[nw.world][nw.zone.Y][nw.zone.X][i]->loc.X, npc[nw.world][nw.zone.Y][nw.zone.X][i]->loc.Y+1);
				cout << char(Body);
				console.setCursorPos(npc[nw.world][nw.zone.Y][nw.zone.X][i]->loc.X, npc[nw.world][nw.zone.Y][nw.zone.X][i]->loc.Y+2);
				cout << char(Legs);
			}
		}
	}

	// load each trigger into the map
	for ( unsigned int i = 0; i < objects[nw.world][nw.zone.Y][nw.zone.X].size(); ++i)
	{
		if (objects[nw.world][nw.zone.Y][nw.zone.X][i])
		{
			// get the info from the trigger object
			CHAR_INFO tIcon = objects[nw.world][nw.zone.Y][nw.zone.X][i]->getIcon();
			SMALL_RECT tPos = objects[nw.world][nw.zone.Y][nw.zone.X][i]->getPos();

			// setup the write buffer
			int tArea = (tPos.Right-tPos.Left+1) * (tPos.Bottom-tPos.Top+1);
			CHAR_INFO* tIcons = new CHAR_INFO[tArea];
			COORD buffSize = {tPos.Right-tPos.Left+1, tPos.Bottom-tPos.Top+1};
			COORD buffPos = {0,0};

			// insert the object icons into the array
			for (int i = 0; i < tArea; ++i)
				tIcons[i] = tIcon;

			// output the trigger
			WriteConsoleOutputA(console.outHnd, tIcons, buffSize, buffPos, &tPos);

		}
	}
	
	// restore the console text attributes
	CHAR_INFO tmp;
	tmp.Attributes = White | Black << BG;
	SetConsoleTextAttribute(console.outHnd, tmp.Attributes);

}

void actionKeyPress(Map nw, Window con, Character& p, Interface& bag, Interface& shop)
{

	//define char info foor loot drops
	CHAR_INFO charInfo[1];
	charInfo[0].Char.AsciiChar = Star;

	//extract the background from the tile
	int bg = (p.oldTile[0].Attributes & Blue << BG ) | (p.oldTile[0].Attributes & Red << BG) | (p.oldTile[0].Attributes & Green << BG) | (p.oldTile[0].Attributes & BACKGROUND_INTENSITY);
	charInfo[0].Attributes = Yellow | bg;

	// Check if the player is near a trigger
	for (unsigned int i = 0; i < objects[nw.world][nw.zone.Y][nw.zone.X].size(); ++i)
	{
		// Check if the trigger still exists and is correct type
		if (!objects[nw.world][nw.zone.Y][nw.zone.X][i] && objects[nw.world][nw.zone.Y][nw.zone.X][i]->getToMap() == 99) 
			continue;

		// get the trigger position
		SMALL_RECT tLoc = objects[nw.world][nw.zone.Y][nw.zone.X][i]->getPos();

		// check if we're in range of the trigger
		if ((p.loc.X >= tLoc.Left-1 && p.loc.X <= tLoc.Right+1) && (p.loc.Y >= tLoc.Top-1 && p.loc.Y <= tLoc.Bottom+1))
		{
			string iName = objects[nw.world][nw.zone.Y][nw.zone.X][i]->getName();
			stringstream msg;
			msg << objects[nw.world][nw.zone.Y][nw.zone.X][i]->getMsg();

			// output the correct message
			bag.message(msg.str(), iName, 1);

			// reload the world
			nw.load(nw.world, nw.zone);
			loadObjects(nw, con);
			p.Move('x');		// Show the player characteri
			Sleep(500);
			return;
		}
	}

	// Check if the player is near a monster
	for (unsigned int i = 0; i < monsters[nw.world][nw.zone.Y][nw.zone.X].size(); ++i)
	{
		// Check if the monster still exists
		if (!monsters[nw.world][nw.zone.Y][nw.zone.X][i])
			continue;

		// get the monster position and size
		COORD tLoc = monsters[nw.world][nw.zone.Y][nw.zone.X][i]->loc;
		int tLen = monsters[nw.world][nw.zone.Y][nw.zone.X][i]->name.length();
		
		// check if an enemy is near by
		for (int l = 0; l < tLen; ++l)
		{
			if ((p.loc.X >= tLoc.X-1+l && p.loc.X <= tLoc.X+1+l) && (p.loc.Y >= tLoc.Y-1 && p.loc.Y <= tLoc.Y+1))
			{
				if (p.health > 0)
				{
					// We're near an enemy setup the combat interface
					con.setCursorPos(0,0);
					for (int t = 0; t < con.height; ++t)
					{
						cout << "                                                                                ";
						Sleep(10);
					}
					cout << "                      ";

					// load battle map/background
					Map battle(con);
					COORD tmp = {0,0};
					battle.load('b', tmp);
					
					// Temporary set player pos so inventory displays correctly
					int oldPloc = p.loc.X;
					p.loc.X = (con.width / 2) + 1;

					// set the new inventory position and display
					bag.display(p, 1);

					// display options and enemy name
					string tName = monsters[nw.world][nw.zone.Y][nw.zone.X][i]->name;
					stringstream mName;
					for (unsigned int n = 0; n < tName.length(); ++n)	// put temporary name into monster name
						if (tName[n] != ' ')					// do not include spaces
							mName << tName[n];

					con.setCursorPos(28, 35);
					cout << "[E] Attack \t[R] Run Away";
					con.setCursorPos(28, 22);
					cout << mName.str() << " Stats";
					con.setCursorPos(55, 22);
					cout << "Your Stats";

					// enter combat
					if (p.Battle(*monsters[nw.world][nw.zone.Y][nw.zone.X][i], bag)) // returns true when battle won
					{

						//drop loot if you win
						for (int r = 0; r < lootDropRows; ++r)
						{
							//convert strings to numbers
							int chance, qty;
							if (!(istringstream(lootDrops[r][2]) >> chance)) chance = 1;
							if (!(istringstream(lootDrops[r][3]) >> qty)) qty = 1;
							if (qty > 1)
								qty = con.random(qty/2,qty);

							// only drop loot if the loot is not exlusive or were fighting the required monster
							COORD lootLoc = {tLoc.X+(tLen/2), tLoc.Y};
							if (lootDrops[r][4] == "" || monsters[nw.world][nw.zone.Y][nw.zone.X][i]->name == lootDrops[r][4])
								if (con.random(1,100) < chance && p.level > qty * 1.5)
									loot[p.world.world][p.world.zone.Y][p.world.zone.X].push_back(new Item(lootDrops[r][0], lootDrops[r][1], qty, charInfo[0], lootLoc));
									
						}


						// delete the monster when it has been defeated
						delete monsters[nw.world][nw.zone.Y][nw.zone.X][i];
						monsters[nw.world][nw.zone.Y][nw.zone.X].erase(monsters[nw.world][nw.zone.Y][nw.zone.X].begin() + i);
						//monsters[nw.world][nw.zone.Y][nw.zone.X][i] = 0;
					}
					// reload the world
					nw.load(nw.world, nw.zone);
					loadObjects(nw, con);
					p.loc.X = oldPloc; // restore player pos
					p.Move('x');		// Show the player character
					return;
				}
				else
				{
					bag.message("\n\n You cannot battle when you are wounded","",1);
					//cout << "You cannot battle when you are wounded";
					nw.load(nw.world, nw.zone);
					loadObjects(nw, con);
					p.Move('x');
					return;
				}
			}
		}
	}


	// Check if the player is near a loot item
	for (unsigned int i = 0; i < loot[nw.world][nw.zone.Y][nw.zone.X].size(); i++)
	{
		// get the loot position
		Item& iloot = *loot[nw.world][nw.zone.Y][nw.zone.X][i];
		if (!loot[nw.world][nw.zone.Y][nw.zone.X][i]) // Check if the loot still exists
			continue;

		COORD tLoc = iloot.loc;

		// check if we're in range of the loot
		if ((p.loc.X >= tLoc.X-1 && p.loc.X <= tLoc.X+1) && (p.loc.Y >= tLoc.Y-1 && p.loc.Y <= tLoc.Y+1))
		{
			bool match = 0; // assume no item match exists
			int fusionFoundCount = 0; // assume no fusion is found
			bool fusionDone = 0;
			int itemArrayPosition;
			stringstream msg;
			// check if the item is gold
			if (iloot.getItemType() == "gold")
			{
				int rand = con.random(1, p.level)*2;
				p.gold += rand;
				iloot.setItemQty(rand);
			}
			else
			{
				
				string itemNameName = iloot.getItemName();
				string newWeaponName = "";
				int newDamageQty = iloot.getItemQty();
				// check if we already have a matching item
				for (unsigned int n = 0; n < bag.items.size(); n++)
				{
					if (itemNameName == bag.items[n]->getItemName())
					{
						match = 1; // found an item match
						if (iloot.getItemType() == "weapon")
						{
							// iterate through the fusions table
							for (int f = 0; f < fusionRows; ++f)
							{
								// make sure there is an evolution of the fusion.
								if (fusion[f][0] == itemNameName)
								{
									newWeaponName = fusion[f][1];
									fusionFoundCount += 1; // increment fusion counter
									itemArrayPosition = n; // save item position
									break;
								}
								if(f == fusionRows - 1)
									fusionDone = 1;

							}

							// if there is a fusion match then fuse items otherwise add item seperately to the inventory.
							if(fusionFoundCount && !fusionDone)
							{
								msg << " The two " << itemNameName << "'s fuse together... \n They make a " << newWeaponName <<" +" << (newDamageQty += bag.items[n]->getItemQty()) << "\n";
								
								p.weapon = "Fist";
								p.weapondmg = 0;

								// reset so we can check for a second fusion
								itemNameName = newWeaponName;
								newWeaponName = "";
								n = -1;
							}
							else 
								break;
						}
						else
							bag.items[n]->setItemQty(bag.items[n]->getItemQty()+newDamageQty);
					}
				}

				if(fusionFoundCount)
				{
					bag.items[itemArrayPosition]->setItemName(itemNameName);
					bag.items[itemArrayPosition]->setItemQty(newDamageQty);
					for(fusionFoundCount; fusionFoundCount > 1; fusionFoundCount--)
						for (int n = 0; n < bag.items.size(); n++)
							if (iloot.getItemName() == bag.items[n]->getItemName())
							{
								delete bag.items[n];
								bag.items.erase(bag.items.begin() + n);
							}
						
				}

				// if we dont have a matching item create new entry if we have room
				if (!match)
					if (bag.items.size() < p.capacity)
						bag.items.push_back(new Item(iloot.getItemName(), iloot.getItemType(), iloot.getItemQty()));
					else
					{
						bag.message("\n Inventory full. No room for " + iloot.getItemName() + " \n Drop an item to make room, or find a bigger bag.", "Inventory Full", 1);
						// reload the world
						nw.load(nw.world, nw.zone);
						loadObjects(nw, con);
						p.Move('x');		// Show the player character
						Sleep(200);
						return;
					}
			}
			// send a message to the screen
			if (iloot.getItemType() == "weapon" && !match)
				msg << "\n\n You have picked up the " << iloot.getItemName() << " +" << iloot.getItemQty();
			else if(!fusionFoundCount)
				msg << "\n\n You have picked up " << iloot.getItemQty() << " " << iloot.getItemName() << ((iloot.getItemQty() > 1 && iloot.getItemType() != "gold") ? "'s" : "");

			bag.message(msg.str(), "Loot", 1);

			// delete the object from the array
			delete loot[nw.world][nw.zone.Y][nw.zone.X][i];
			loot[nw.world][nw.zone.Y][nw.zone.X].erase(loot[nw.world][nw.zone.Y][nw.zone.X].begin() + i);
			
			// reload the world
			nw.load(nw.world, nw.zone);
			loadObjects(nw, con);
			p.Move('x');		// Show the player character
			Sleep(200);
			return;
		}
	}


	// Check if the player is near an npc
	for (unsigned int i = 0; i < npc[nw.world][nw.zone.Y][nw.zone.X].size(); ++i)
	{
		// get the npc position
		NPC& inpc = *npc[nw.world][nw.zone.Y][nw.zone.X][i];
		if (!npc[nw.world][nw.zone.Y][nw.zone.X][i]) // Check if the npc still exists
			continue;

		COORD tLoc = inpc.loc;

		// check if we're in range of the npc
		if ((p.loc.X >= tLoc.X-1 && p.loc.X <= tLoc.X+1) && (p.loc.Y >= tLoc.Y && p.loc.Y <= tLoc.Y+1))
		{
			// check if the NPC has already been spoken to
			if (inpc.talk[inpc.talkPos] == "END")
			{
				switch(con.random(1,6))
				{
					case 1:
						bag.message("\n\n I have nothing more to say.", inpc.name, 1);
						break;
					case 2:
						bag.message("\n\n What a beautiful day.", inpc.name, 1);
						break;
					case 3:
						bag.message("\n\n I thought you were leaving?", inpc.name, 1);
						break;
					case 4:
						bag.message("\n\n You have anything for me?... No?... Okay.", inpc.name, 1);
						break;
					case 5:
						bag.message("\n\n You should go home and rest.", inpc.name, 1);
						break;
					default:
						bag.message("\n\n Something wrong?", inpc.name, 1);
						break;
				}
			}
			else if (inpc.talk[inpc.talkPos] == "ITEM")
			{

				COORD tLoc = p.loc;
				if (tLoc.Y >= con.height-1)
					tLoc.Y--;
				else
					tLoc.Y++;

				stringstream msg;
				msg << "\n\n Did you find me the " << inpc.needItem << "?";
				// look through inventory for item needed inpc.needItem
				for (unsigned int n = 0; n < bag.items.size(); ++n)
					if (bag.items[n]->getItemName() == inpc.needItem)
					{
						msg.str(std::string());
						msg << "\n I see you found the " << inpc.needItem << "!\n Here, please take your reward";
						
						if( inpc.rewardItem == "INVENTORY")
						{
							msg.str(std::string());
							msg << "\n I see you found the " << inpc.needItem << "!\n Here, take this bag, you can now carry " << inpc.rewardQty << " items";
							p.capacity = inpc.rewardQty;
						}
						else
							loot[p.world.world][p.world.zone.Y][p.world.zone.X].push_back(new Item(inpc.rewardItem, inpc.rewardType, inpc.rewardQty, charInfo[0], tLoc));

						inpc.talkPos++;

						// delete the item from the heap
						delete bag.items[n];
						bag.items.erase(bag.items.begin() + n);
						break;
					}
				// output the correct message
				bag.message(msg.str(), inpc.name, 1);
			}

			// lets spark up a conversation
			for (unsigned int tp = inpc.talkPos; tp < inpc.talk.size(); ++tp)
			{
				if (inpc.talk[tp] == "END")
				{
					inpc.talkPos = tp;
					break;
				}
				else if (inpc.talk[tp] == "ITEM")
				{
					inpc.talkPos = tp;
					break;
				}
				else if (inpc.talk[tp] == "DELETEIF")
				{
					inpc.talkPos = tp;
					break;
				}
				else if (inpc.talk[tp] == "DELETE")
				{
					bag.message("\n\n Well, time to go. Thanks!", inpc.name, 1);
					for (int d = 0; d < npc[nw.world][nw.zone.Y][nw.zone.X].size(); d++)
					{
						if (npc[nw.world][nw.zone.Y][nw.zone.X][d]->rewardType == inpc.name)
						{
							delete npc[nw.world][nw.zone.Y][nw.zone.X][d];
							npc[nw.world][nw.zone.Y][nw.zone.X].erase(npc[nw.world][nw.zone.Y][nw.zone.X].begin() + d);
						}
					}

					cout << npc[nw.world][nw.zone.Y][nw.zone.X].size();
					delete npc[nw.world][nw.zone.Y][nw.zone.X][i];
					npc[nw.world][nw.zone.Y][nw.zone.X].erase(npc[nw.world][nw.zone.Y][nw.zone.X].begin() + i);
					break;
				}
				bag.message(inpc.talk[tp], inpc.name, 1);
			}

			// reload the world
			nw.load(nw.world, nw.zone);
			loadObjects(nw, con);
			p.Move('x');		// Show the player character
			Sleep(500);

			return;
		}
	}

	// Check if the player can restore health near an object
	if (nw.world == 2) // Check if the player is near bed
	{
		if((p.loc.X >= 53 && p.loc.X <= 63) && (p.loc.Y >= 9 && p.loc.Y <= 15))
		{
			bag.message("\n\n It's time to rest...", "Sleep Time");
			con.setCursorPos(0,0);
			for (int t = 0; t < con.height; ++t)
			{
				cout << "                                                                                ";
				Sleep(10);
			}
			Sleep(2000);
			bag.message("\n\n YAWN!... I feel much better now!", "Wakey Wakey!", 1);
			p.health = p.maxhealth;

			// reload the world
			nw.load(nw.world, nw.zone);
			loadObjects(nw, con);
			p.Move('x');		// Show the player character
			return;
		}
	}
	else // Check if the player is near water
	{
		CHAR_INFO cInfo[9];
		COORD cSize = {3,3};
		COORD cPos = {0,0};
		SMALL_RECT cRead = {p.loc.X-1, p.loc.Y-1, p.loc.X+1, p.loc.Y+1};

		ReadConsoleOutputA(con.outHnd, cInfo, cSize, cPos, &cRead);
		for (int c = 0; c < 9; ++c)
		{
			int bg = (cInfo[c].Attributes & Blue << BG ) | (cInfo[c].Attributes & Red << BG) | (cInfo[c].Attributes & Green << BG) | (cInfo[c].Attributes & BACKGROUND_INTENSITY);
			if (bg == Blue << BG)
			{
				if (p.health > 0)
				{
					bag.message("\n You drink some water. \n Your health is fully restored.");
					p.health = p.maxhealth;
				}
				else
					bag.message("\n You drink some water. \n Your wounds are too severe, you need to rest \n to heal.", "", 1);

				// reload the world
				nw.load(nw.world, nw.zone);
				loadObjects(nw, con);
				p.Move('x');		// Show the player character
				return;
			}
		}
	}

	// Check if the player is near the shop
	if (nw.world == 0)
	{
		if((p.loc.X >= 25 && p.loc.X <= 26) && (p.loc.Y >= 35 && p.loc.Y <= 35))
		{
			bag.message("\n\n Knock, Knock, Knock...");
			con.setCursorPos(0,0);
			for (int t = 0; t < con.height; ++t)
			{
				cout << "                                                                                ";
				Sleep(10);
			}

			bag.display(p, 1);
			shop.display(p);
		
			bool exit = 0, option = 0;
			int cost;
			string itemName = "";
			do
			{
				// Start option loop
				do
				{
					if (GetAsyncKeyState(0x31) || GetAsyncKeyState(VK_NUMPAD1))		// Key 1 detection
					{
						itemName = "HP Potion";
						cost = 10;
						option = 1;
					}
					else if (GetAsyncKeyState(0x32) || GetAsyncKeyState(VK_NUMPAD2))	// Key 2 detection 
					{
						itemName = "Chapati";
						cost = 6;
						option = 1;	
					}
					else if (GetAsyncKeyState(0x33) || GetAsyncKeyState(VK_NUMPAD3))	// Key 3 detection 
					{
						itemName = "Life Potion";
						cost = 20;
						option = 1;
					}
					else if ((GetAsyncKeyState(0x34) || GetAsyncKeyState(VK_NUMPAD4)) || (GetAsyncKeyState(VK_ESCAPE) & 0x8000))		// Key 4 detection (Quit) 
					{
						exit = 1;
						option = 1;
						itemName = "";
						cost = 0;
					}

				} while (!option);

				bool match = 0;
				stringstream msg;
				option = 0;

				if (p.gold - cost < 0)
				{
					itemName = "";
					bag.message("Not enough Gold", "");
				}
				else
				{
					p.gold -= cost;
				}
				// check if we already have a matching item
				for (unsigned int n = 0; n < bag.items.size(); n++)
				{
					if (itemName == bag.items[n]->getItemName())
					{
						match = 1; // found an item match
						bag.items[n]->setItemQty(bag.items[n]->getItemQty()+1);
						msg << "\n You purchased a " << itemName;
						bag.message(msg.str(), "");
					}
				}
				if (!match && itemName != "")
					if (bag.items.size() < p.capacity)
					{
						bag.items.push_back(new Item(itemName, "consumable", 1));
						msg << "\n You purchased a " << itemName;
						bag.message(msg.str(), "");
					}
					else
					{
						bag.message("\n Your inventory is full. \n Drop an item to make room, or find a bigger bag.", "Inventory Full", 1);
						// reload the world
						nw.load(nw.world, nw.zone);
						loadObjects(nw, con);
						p.Move('x');		// Show the player character
						Sleep(200);
						return;
					}

				con.setCursorPos(0,33);
				for (int t = 0; t < 7; ++t)
				{
					cout << "                                                                               ";
				}	
				bag.display(p, 1);
				shop.display(p);

			} while(!exit);
	//			" 2 Gold   HP Potion     Restores 100% health when not wounded ",
	//" 1 Gold   Chapati       Restores 50% health when not wounded",
	//" 3 Gold   Life Potion   Restores 100% health when you're wounded",
	//" LEAVE SHOP"

			// reload the world
			nw.load(nw.world, nw.zone);
			loadObjects(nw, con);
			p.Move('x');		// Show the player character
			return;
		}
	}
}
