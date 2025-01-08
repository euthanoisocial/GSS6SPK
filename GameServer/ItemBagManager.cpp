// ItemBagManager.cpp: implementation of the CItemBagManager class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ItemBagManager.h"
#include "MemScript.h"
#include "Path.h"
#include "Util.h"
#include "ItemManager.h"
#include "DropReward.h"
#include "Notice.h"
#include "Message.h"
#include "Log.h"

CItemBagManager gItemBagManager;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CItemBagManager::CItemBagManager() // OK
{
	this->m_ItemBagManagerInfo.clear();
}

CItemBagManager::~CItemBagManager() // OK
{

}

void CItemBagManager::Load(char* path) // OK
{
	CMemScript* lpMemScript = new CMemScript;

	if(lpMemScript == 0)
	{
		ErrorMessageBox(MEM_SCRIPT_ALLOC_ERROR,path);
		return;
	}

	if(lpMemScript->SetBuffer(path) == 0)
	{
		ErrorMessageBox(lpMemScript->GetLastError());
		delete lpMemScript;
		return;
	}

	this->m_ItemBagManagerInfo.clear();

	try
	{
		while(true)
		{
			if(lpMemScript->GetToken() == TOKEN_END)
			{
				break;
			}

			if(strcmp("end",lpMemScript->GetString()) == 0)
			{
				break;
			}

			ITEM_BAG_MANAGER_INFO info;

			info.Index = lpMemScript->GetNumber();

			info.ItemIndex = lpMemScript->GetAsNumber();

			info.ItemLevel = lpMemScript->GetAsNumber();

			info.MonsterClass = lpMemScript->GetAsNumber();

			info.SpecialValue = lpMemScript->GetAsNumber();

			info.DropType = lpMemScript->GetAsNumber();

			this->m_ItemBagManagerInfo.insert(std::pair<int,ITEM_BAG_MANAGER_INFO>(info.Index,info));
		}
	}
	catch(...)
	{
		ErrorMessageBox(lpMemScript->GetLastError());
	}

	delete lpMemScript;
}

void CItemBagManager::LoadEventItemBag() // OK
{
	std::map<int,int> LoadEventItemBag;

	char wildcard_path[MAX_PATH];

	wsprintf(wildcard_path,"%s*",gPath.GetFullPath("EventItemBag\\"));

	WIN32_FIND_DATA data;

	HANDLE file = FindFirstFile(wildcard_path,&data);

	if(file == INVALID_HANDLE_VALUE)
	{
		return;
	}

	do
	{
		if((data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0)
		{
			if(isdigit(data.cFileName[0]) != 0 && isdigit(data.cFileName[1]) != 0 && isdigit(data.cFileName[2]) != 0)
			{
				if(data.cFileName[3] == ' ' && data.cFileName[4] == '-' && data.cFileName[5] == ' ')
				{
					std::map<int,ITEM_BAG_MANAGER_INFO>::iterator it = this->m_ItemBagManagerInfo.find(atoi(data.cFileName));

					if(it != this->m_ItemBagManagerInfo.end())
					{
						if(LoadEventItemBag.find(it->first) == LoadEventItemBag.end())
						{
							char path[MAX_PATH];

							wsprintf(path,"EventItemBag\\%s",data. cFileName);

							it->second.ItemBag.Load(gPath.GetFullPath(path));

							LoadEventItemBag.insert(std::pair<int,int>(it->first,1));
						}
					}
				}
			}
		}
	}
	while(FindNextFile(file,&data) != 0);
}

bool CItemBagManager::GetItemByItemIndex(int ItemIndex,int ItemLevel,LPOBJ lpObj,CItem* lpItem) // OK
{
	for(std::map<int,ITEM_BAG_MANAGER_INFO>::iterator it=this->m_ItemBagManagerInfo.begin();it != this->m_ItemBagManagerInfo.end();it++)
	{
		if(it->second.ItemIndex != -1 && it->second.ItemIndex == ItemIndex && (it->second.ItemLevel == -1 || it->second.ItemLevel == ItemLevel))
		{
			return it->second.ItemBag.GetItem(lpObj,lpItem);
		}
	}

	return 0;
}

bool CItemBagManager::GetItemByMonsterClass(int MonsterClass,LPOBJ lpObj,CItem* lpItem) // OK
{
	for(std::map<int,ITEM_BAG_MANAGER_INFO>::iterator it=this->m_ItemBagManagerInfo.begin();it != this->m_ItemBagManagerInfo.end();it++)
	{
		if(it->second.MonsterClass != -1 && it->second.MonsterClass == MonsterClass)
		{
			return it->second.ItemBag.GetItem(lpObj,lpItem);
		}
	}

	return 0;
}

bool CItemBagManager::GetItemBySpecialValue(int SpecialValue,LPOBJ lpObj,CItem* lpItem) // OK
{
	for(std::map<int,ITEM_BAG_MANAGER_INFO>::iterator it=this->m_ItemBagManagerInfo.begin();it != this->m_ItemBagManagerInfo.end();it++)
	{
		if(it->second.SpecialValue != -1 && it->second.SpecialValue == SpecialValue)
		{
			return it->second.ItemBag.GetItem(lpObj,lpItem);
		}
	}

	return 0;
}

int CItemBagManager::DropItemByItemIndex(int ItemIndex,int ItemLevel,LPOBJ lpObj,int map,int x,int y) // OK
{
	if(gDropReward.DropRewardByItemIndex(ItemIndex,lpObj))
	{
		return 1;
	}

	for(std::map<int,ITEM_BAG_MANAGER_INFO>::iterator it=this->m_ItemBagManagerInfo.begin(); it != this->m_ItemBagManagerInfo.end(); it++)
	{
		if(it->second.ItemIndex != -1 && it->second.ItemIndex == ItemIndex && (it->second.ItemLevel == -1 || it->second.ItemLevel == ItemLevel))
		{
			#if BAG_EVENT_DROP
			gLog.Output(LOG_ITEMBAGDROP, "[DropItemByItemIndex] Char [%s] [%d](%d/%d) User Item [%d]+%d Drop", lpObj->Name, map, x, y, ItemIndex, ItemLevel);			
			#endif
			if (lpObj->IsFakeOnline == false)
			{
				if (it->second.DropType == 1)
				{
					return (it->second.ItemBag.DropInventoryItem(lpObj, map, x, y)) ? 1 : 2;
				}
				else
				{
					return (it->second.ItemBag.DropItem(lpObj, map, x, y)) ? 1 : 0;
				}
			}
			else if (lpObj->IsFakeOnline == true)
			{
				return (it->second.ItemBag.DropItem(lpObj, map, x, y)) ? 1 : 0;
			}
			
		}
	}
	return 0;
}
#if BAG_EVENT_DROP
char *MapSPKBag[] ={
	"Lorencia",					//0
	"Dungeon",					//1
	"Davias",					//2
	"Noria",					//3
	"LostTower",				//4
	"Exile",					//5
	"Arena",					//6
	"Atlans",					//7
	"Tarkan",					//8
	"Devil Square",				//9
	"Icarus",					//10
	"Blood Castle 1",			//11
	"Blood Castle 2",			//12
	"Blood Castle 3",			//13
	"Blood Castle 4",			//14
	"Blood Castle 5",			//15
	"Blood Castle 6",			//16
	"Blood Castle 7",			//17
	"Chaos Castle 1",			//18
	"Chaos Castle 2",			//19
	"Chaos Castle 3",			//20
	"Chaos Castle 4",			//21
	"Chaos Castle 5",			//22
	"Chaos Castle 6",			//23
	"Kalima 1",					//24
	"Kalima 2",					//25
	"Kalima 3",					//26
	"Kalima 4",					//27
	"Kalima 5",					//28
	"Kalima 6",					//29
	"Valley of Loren",			//30
	"Land of Trials",			//31
	"Devil Square",				//32
	"Aida",						//33
	"Crywolf Fortress",			//34
	"NULL35",					//35
	"Kalima 7",					//36
	"Kantru",					//37
	"Kantru K4",				//38
	"Kantru 3",					//39
	"Silent Map",				//40
	"Doanh Trại Barack",		//41
	"Balgass Refuge",			//42
	"NULL43",					//43
	"NULL44",					//44
	"Illusion Temple 1",		//45
	"Illusion Temple 2",		//46
	"Illusion Temple 3",		//47
	"Illusion Temple 4",		//48
	"Illusion Temple 5",		//49
	"Illusion Temple 6",		//50
	"Elbeland",					//51
	"Blood Castle 8",			//52
	"Chaos Castle 7",			//53
	"NULL54",					//54
	"NULL55",					//55
	"Swamp of Calmness",		//56
	"Raklion",					//57
	"Raklion Boss",				//58
	"NULL59",					//59
	"NULL60",					//60
	"NULL61",					//61
	"Village's Santa",			//62
	"Vulcanus",					//63
	"Duel Arena",				//64
	"Doppelganger 1",			//65
	"Doppelganger 2",			//66
	"Doppelganger 3",			//67
	"Doppelganger 4",			//68
	"Empire Fortress 1",		//69
	"Empire Fortress 2",		//70
	"Empire Fortress 3",		//71
	"Empire Fortress 4",		//72
	"NULL73",					//73
	"NULL74",					//74
	"NULL75",					//75
	"NULL76",					//76
	"NULL77",					//77
	"NULL78",					//78
	"NULL79",					//79
	"Karuntan 1",				//80
	"Karuntan 2",				//81
	"NULL82",					//82
	"NULL83",					//83
	"NULL84",					//84
	"NULL85",					//85
	"Nars",					//86
	"NULL87",					//87
	"NULL88",					//88
	"Acheron",					//89
	"Debanter",					//90
	"NULL91",					//91
	"NULL92",					//92
	"NULL93",					//93
	"NULL94",					//94
	"NULL95",					//95
	"Thần Ma Chiến",			//96
	"Chiến Trường Cổ",			//97
	"NULL98",					//98
	"NULL99",					//99
	"NULL100"};					//100
#include "MonsterManager.h"
#endif
bool CItemBagManager::DropItemByMonsterClass(int MonsterClass,LPOBJ lpObj,int map,int x,int y) // OK
{
	for(std::map<int,ITEM_BAG_MANAGER_INFO>::iterator it=this->m_ItemBagManagerInfo.begin();it != this->m_ItemBagManagerInfo.end();it++)
	{
		if(it->second.MonsterClass != -1 && it->second.MonsterClass == MonsterClass)
		{
			#if BAG_EVENT_DROP
			gLog.Output(LOG_ITEMBAGDROP, "[CustomMonster] %s: %s: %d/%d - ID Monster: [%d] %s", lpObj->Name, MapSPKBag[map], x, y,MonsterClass,gMonsterManager.GetMonsterName(MonsterClass));
			#endif
			return it->second.ItemBag.DropItem(lpObj,map,x,y);
		}
	}

	return 0;
}

bool CItemBagManager::DropItemBySpecialValue(int SpecialValue,LPOBJ lpObj,int map,int x,int y) // OK
{
	for(std::map<int,ITEM_BAG_MANAGER_INFO>::iterator it=this->m_ItemBagManagerInfo.begin();it != this->m_ItemBagManagerInfo.end();it++)
	{
		if(it->second.SpecialValue != -1 && it->second.SpecialValue == SpecialValue)
		{

			if( it->second.DropType == 1 )
			{
				return ( it->second.ItemBag.DropInventoryItem(lpObj, map, x, y) ) ? 1 : 2;
			}
			else
			{
				return it->second.ItemBag.DropItem(lpObj,map,x,y);
			}
		}
	}

	return 0;
}
bool CItemBagManager::DropItemByIndexItemBag(int IndexItemBag,LPOBJ lpObj) // OK
{
	for(std::map<int,ITEM_BAG_MANAGER_INFO>::iterator it=this->m_ItemBagManagerInfo.begin();it != this->m_ItemBagManagerInfo.end();it++)
	{
		if(it->second.Index != -1 && it->second.Index  == IndexItemBag)
		{
			if( it->second.DropType == 1 )
			{
				return ( it->second.ItemBag.DropInventoryItem(lpObj,lpObj->Map,lpObj->X,lpObj->Y) ) ? 1 : 2;

			}
			else
			{
				return it->second.ItemBag.DropItem(lpObj,lpObj->Map,lpObj->X,lpObj->Y);
			}
		}
	}

	return 0;
}