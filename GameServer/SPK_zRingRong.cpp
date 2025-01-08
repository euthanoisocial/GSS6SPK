#include "StdAfx.h"
#if ACTIVE_VKR
#include "SPK_zRingRong.h"
#include "MemScript.h"
#include "Message.h"
#include "Notice.h"
#include "Util.h"
#include "ItemManager.h"
#include "CustomStartItemDame.h"

ITEM13 GITEM13;

void ITEM13::Load(char* FilePath) // OK
{
	pugi::xml_document file;
	pugi::xml_parse_result res = file.load_file(FilePath);
	if (res.status != pugi::status_ok)
	{
		ErrorMessageBox("File %s load fail. Error: %s", FilePath, res.description());
		return;
	}
	pugi::xml_node CustomVuKhiRong = file.child("SPK");
	this->m_ITEM13.Enabled = CustomVuKhiRong.attribute("Enable").as_int();

	pugi::xml_node SlotVuKhi = CustomVuKhiRong.child("ItemRong");
	this->m_ITEM13.IsITEM13[0] = SlotVuKhi.attribute("Level_Item1").as_int();
	this->m_ITEM13.IsITEM13[1] = SlotVuKhi.attribute("Level_Item2").as_int();
	this->m_ITEM13.IsITEM13[2] = SlotVuKhi.attribute("Level_Item3").as_int();
}

void ITEM13::Option(LPOBJ lpObj, bool flag)
{
	if (!this->m_ITEM13.Enabled) return;

	if (flag != 0) { return; }

	int VuKhiRong = 0;

	for (int x = 0; x < 3; x++)
	{
		if (this->CountItem(lpObj, this->m_ITEM13.IsITEM13[x]) >= 1)
		{
			VuKhiRong = x + 1;
		}
	}

	PMSG_ITEM13_REQ pRequest;
	pRequest.Head.set(0xFB, 0x0D, sizeof(pRequest));
	pRequest.ItemRong = VuKhiRong;

	DataSend(lpObj->Index, (LPBYTE)&pRequest, sizeof(pRequest));
}

int ITEM13::CountItem(LPOBJ lpObj, int level)
{
	int count = 0;

	CItem* Inventory09 = &lpObj->Inventory[9];
	CItem* Inventory10 = &lpObj->Inventory[10];
	CItem* Inventory11 = &lpObj->Inventory[11];

	if (lpObj->Inventory[9].m_Index >= GET_ITEM(13, 00) && lpObj->Inventory[9].m_Index <= GET_ITEM(13, 400))
	{
		if (Inventory09->m_Level >= level && Inventory09->m_NewOption)
		{
			count++;
		}
	}
	if (lpObj->Inventory[10].m_Index >= GET_ITEM(13, 00) && lpObj->Inventory[10].m_Index <= GET_ITEM(13, 400))
	{
		if (Inventory10->m_Level >= level && Inventory10->m_NewOption)
		{
			count++;
		}
	}
	if (lpObj->Inventory[11].m_Index >= GET_ITEM(13, 00) && lpObj->Inventory[11].m_Index <= GET_ITEM(13, 400))
	{
		if (Inventory11->m_Level >= level && Inventory11->m_NewOption)
		{
			count++;
		}
	}
	return count;
}
#endif