#include "StdAfx.h"
#if ACTIVE_VKR
#include "SPK_zGangRong.h"
#include "MemScript.h"
#include "Message.h"
#include "Notice.h"
#include "Util.h"
#include "ItemManager.h"
#include "CustomStartItemDame.h"

ITEM10 GITEM10;

void ITEM10::Load(char* FilePath) // OK
{
	pugi::xml_document file;
	pugi::xml_parse_result res = file.load_file(FilePath);
	if (res.status != pugi::status_ok)
	{
		ErrorMessageBox("File %s load fail. Error: %s", FilePath, res.description());
		return;
	}
	pugi::xml_node CustomVuKhiRong = file.child("SPK");
	this->m_ITEM10.Enabled = CustomVuKhiRong.attribute("Enable").as_int();

	pugi::xml_node SlotVuKhi = CustomVuKhiRong.child("ItemRong");
	this->m_ITEM10.IsITEM10[0] = SlotVuKhi.attribute("Level_Item1").as_int();
	this->m_ITEM10.IsITEM10[1] = SlotVuKhi.attribute("Level_Item2").as_int();
	this->m_ITEM10.IsITEM10[2] = SlotVuKhi.attribute("Level_Item3").as_int();
}

void ITEM10::Option(LPOBJ lpObj, bool flag)
{
	if (!this->m_ITEM10.Enabled) return;

	if (flag != 0){	return;	}

	int VuKhiRong = 0;

	for (int x = 0; x < 3; x++)
	{
		if (this->CountItem(lpObj, this->m_ITEM10.IsITEM10[x]) >= 1)
		{
			VuKhiRong = x + 1;
		}
	}

	PMSG_ITEM10_REQ pRequest;
	pRequest.Head.set(0xFB, 0x0A, sizeof(pRequest));
	pRequest.ItemRong = VuKhiRong;

	DataSend(lpObj->Index, (LPBYTE)&pRequest, sizeof(pRequest));
}

int ITEM10::CountItem(LPOBJ lpObj, int level)
{
	int count = 0;
	CItem* Inventory = &lpObj->Inventory[5];

	if (   lpObj->Inventory[5].m_Index >= GET_ITEM(10, 00) && lpObj->Inventory[5].m_Index <= GET_ITEM(10, 400))
	{
		if (Inventory->m_Level >= level && Inventory->m_NewOption)
		{
			count++;;
		}
	}
	return count;
}
#endif