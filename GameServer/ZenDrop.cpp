#include "stdafx.h"
#include "ZenDrop.h"
#include "Util.h"

cZenDrop gZenDrop;

void cZenDrop::Load(char* path)
{
	pugi::xml_document file;
	pugi::xml_parse_result res = file.load_file(path);

	if (res.status != pugi::status_ok)
	{
		ErrorMessageBox("Failed to load %s file (%s)", path, res.description());
		return;
	}

	this->m_ZenDrop.clear();

	pugi::xml_node main = file.child("ZenDropSystem");

	this->m_ZenDropEnable = main.attribute("Enable").as_bool();
	for (pugi::xml_node map = main.child("Map"); map; map = map.next_sibling())
	{
		ZEN_DROP ZenDrop;

		ZenDrop.MapNumber = map.attribute("Number").as_int();
		ZenDrop.MinMoney = map.attribute("MinMoneyCount").as_int();
		ZenDrop.MaxMoney = map.attribute("MaxMoneyCount").as_int();

		this->m_ZenDrop.insert(std::pair<int, ZEN_DROP>(ZenDrop.MapNumber, ZenDrop));
	}
}

bool cZenDrop::IsZenDropActive(BYTE Map)
{
	if (this->m_ZenDropEnable == false)
	{
		return false;
	}

	if (this->m_ZenDrop.find(Map) == this->m_ZenDrop.end())
	{
		return false;
	}

	return true;
}

DWORD cZenDrop::GetZenAmount(BYTE MapNumber, WORD MonsterLevel)
{
	if (this->m_ZenDropEnable == false)
	{
		return 0;
	}

	std::map<int, ZEN_DROP>::iterator It = this->m_ZenDrop.find(MapNumber);

	if (It == this->m_ZenDrop.end())
	{
		return 0;
	}
	DWORD dwMoney = 0;
	DWORD dwSub = It->second.MinMoney;

	if (dwSub < 0)
	{
		return 0;
	}
	dwMoney = (It->second.MinMoney + rand()%(dwSub+1));
	return dwMoney;
}
