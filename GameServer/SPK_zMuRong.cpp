#include "StdAfx.h"
#if ACTIVE_VKR
#include "SPK_zMuRong.h"
#include "MemScript.h"
#include "Message.h"
#include "Notice.h"
#include "Util.h"
#include "ItemManager.h"
#include "CustomStartItemDame.h"

ITEM07 GITEM07;

void ITEM07::Load(char* FilePath) // OK
{
	pugi::xml_document file;
	pugi::xml_parse_result res = file.load_file(FilePath);
	if (res.status != pugi::status_ok)
	{
		ErrorMessageBox("File %s load fail. Error: %s", FilePath, res.description());
		return;
	}
	pugi::xml_node CustomVuKhiRong = file.child("SPK");
	this->m_ITEM07.Enabled = CustomVuKhiRong.attribute("Enable").as_int();

	pugi::xml_node SlotVuKhi = CustomVuKhiRong.child("ItemRong");
	this->m_ITEM07.IsITEM07[0] = SlotVuKhi.attribute("Level_Item1").as_int();
	this->m_ITEM07.IsITEM07[1] = SlotVuKhi.attribute("Level_Item2").as_int();
	this->m_ITEM07.IsITEM07[2] = SlotVuKhi.attribute("Level_Item3").as_int();
}

void ITEM07::Option(LPOBJ lpObj, bool flag)
{
	if (!this->m_ITEM07.Enabled) return;

	if (flag != 0){	return;	}

	int VuKhiRong = 0;

	for (int x = 0; x < 3; x++)
	{
		if (this->CountItem(lpObj, this->m_ITEM07.IsITEM07[x]) >= 1)
		{
			VuKhiRong = x + 1;
		}
	}

	PMSG_ITEM07_REQ pRequest;
	pRequest.Head.set(0xFB, 0x07, sizeof(pRequest));
	pRequest.ItemRong = VuKhiRong;

	DataSend(lpObj->Index, (LPBYTE)&pRequest, sizeof(pRequest));
}

int ITEM07::CountItem(LPOBJ lpObj, int level)
{
	int count = 0;
	CItem* Inventory = &lpObj->Inventory[2];

	if (   lpObj->Inventory[2].m_Index >= GET_ITEM(7, 00) && lpObj->Inventory[2].m_Index <= GET_ITEM(7, 400))
	{
		if (Inventory->m_Level >= level && Inventory->m_NewOption)
		{
			count++;;
		}
	}
	return count;
}
#endif