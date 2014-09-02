#include "Character.h"

NPC::NPC(Window w, Map& newWorld, string name, COORD loc, CHAR_INFO icon, string needItem, string rewardItem, string rewardType, int rewardQty) 
	: Character(w, newWorld, name, loc), icon(icon), needItem(needItem), rewardItem(rewardItem), rewardType(rewardType), rewardQty(rewardQty)
{
	talkPos = 0;
}