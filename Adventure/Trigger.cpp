#include "Objects.h"

Trigger::Trigger(SMALL_RECT pos, string name, string key, CHAR_INFO icon, string msg, string winMsg)
	: Objects(pos, name), key(key), icon(icon), msg(msg), winMsg(winMsg)
{

}

CHAR_INFO Trigger::getIcon()
{
	return icon;
}
string Trigger::getKey()
{
	return key;
}
string Trigger::getMsg()
{
	return msg;
}
string Trigger::getWinMsg()
{
	return winMsg;
}
