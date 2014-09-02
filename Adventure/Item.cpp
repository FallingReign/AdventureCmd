#include "Item.h"


Item::Item(string item, string type, int qty)
	:name(item), type(type), qty(qty)
{
}

Item::Item(string item, string type, int qty, CHAR_INFO icon, COORD loc)
	:name(item), type(type), qty(qty), icon(icon), loc(loc)
{
}

string Item::getItemName()
{
	return name;
}

int Item::getItemQty()
{
	return qty;
}

string Item::getItemType()
{
	return type;
}

void Item::setItemQty(int q)
{
	qty = q;
}
void Item::setItemName(string n)
{
	name = n;
}