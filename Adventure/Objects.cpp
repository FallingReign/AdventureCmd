#include "Objects.h"


Objects::Objects(SMALL_RECT pos, string name)
	: pos(pos), name(name)
{
}
SMALL_RECT Objects::getPos()
{
	return pos;
}

string Objects::getName()
{
	return name;
}