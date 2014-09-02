#include "Objects.h"

Waypoint::Waypoint(SMALL_RECT pos, int toMap, COORD toZone, COORD toPos, CHAR_INFO icon)
	: Objects(pos), toMap(toMap), toZone(toZone), toPos(toPos), icon(icon)
{

}
int Waypoint::getToMap()
{
	return toMap;
}

COORD Waypoint::getToZone()
{
	return toZone;
}

COORD Waypoint::getToPos()
{
	return toPos;
}
CHAR_INFO Waypoint::getIcon()
{
	return icon;
}