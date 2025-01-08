#include "StdAfx.h"
#if ACTIVE_VKR
#include "SPK_zVuKhiRong.h"
#include "MemScript.h"
#include "Message.h"
#include "Notice.h"
#include "Util.h"
#include "ItemManager.h"
#include "CustomStartItemDame.h"

ITEM00 GITEM00;

void ITEM00::Load(char* FilePath) // OK
{
	pugi::xml_document file;
	pugi::xml_parse_result res = file.load_file(FilePath);
	if (res.status != pugi::status_ok)
	{
		ErrorMessageBox("File %s load fail. Error: %s", FilePath, res.description());
		return;
	}
	pugi::xml_node CustomVuKhiRong = file.child("SPK");
	this->m_ITEM00.Enabled = CustomVuKhiRong.attribute("Enable").as_int();

	pugi::xml_node SlotVuKhi = CustomVuKhiRong.child("ItemRong");
	this->m_ITEM00.IsITEM00[0] = SlotVuKhi.attribute("Level_Item1").as_int();
	this->m_ITEM00.IsITEM00[1] = SlotVuKhi.attribute("Level_Item2").as_int();
	this->m_ITEM00.IsITEM00[2] = SlotVuKhi.attribute("Level_Item3").as_int();
}

void ITEM00::Option(LPOBJ lpObj, bool flag)
{
	if (!this->m_ITEM00.Enabled) return;

	if (flag != 0){	return;	}

	int VuKhiRong = 0;

	for (int x = 0; x < 3; x++)
	{
		if (this->CountItem(lpObj, this->m_ITEM00.IsITEM00[x]) >= 1)
		{
			VuKhiRong = x + 1;
		}
	}

	PMSG_ITEM00_REQ pRequest;
	pRequest.Head.set(0xFB, 0x00, sizeof(pRequest));
	pRequest.ItemRong = VuKhiRong;

	DataSend(lpObj->Index, (LPBYTE)&pRequest, sizeof(pRequest));
}

int ITEM00::CountItem(LPOBJ lpObj, int level)
{
	int count = 0;
	CItem* Inventory0 = &lpObj->Inventory[0];
	CItem* Inventory1 = &lpObj->Inventory[1];
	if (   lpObj->Inventory[0].m_Index >= GET_ITEM(0, 00) && lpObj->Inventory[0].m_Index <= GET_ITEM(0, 500)
		|| lpObj->Inventory[0].m_Index >= GET_ITEM(1, 00) && lpObj->Inventory[0].m_Index <= GET_ITEM(1, 500)
		|| lpObj->Inventory[0].m_Index >= GET_ITEM(2, 00) && lpObj->Inventory[0].m_Index <= GET_ITEM(2, 500)
		|| lpObj->Inventory[0].m_Index >= GET_ITEM(3, 00) && lpObj->Inventory[0].m_Index <= GET_ITEM(3, 500)
		|| lpObj->Inventory[0].m_Index >= GET_ITEM(4, 00) && lpObj->Inventory[0].m_Index <= GET_ITEM(4, 500)
		|| lpObj->Inventory[0].m_Index >= GET_ITEM(5, 00) && lpObj->Inventory[0].m_Index <= GET_ITEM(5, 500)

		|| lpObj->Inventory[1].m_Index >= GET_ITEM(0, 00) && lpObj->Inventory[1].m_Index <= GET_ITEM(0, 500)
		|| lpObj->Inventory[1].m_Index >= GET_ITEM(1, 00) && lpObj->Inventory[1].m_Index <= GET_ITEM(1, 500)
		|| lpObj->Inventory[1].m_Index >= GET_ITEM(2, 00) && lpObj->Inventory[1].m_Index <= GET_ITEM(2, 500)
		|| lpObj->Inventory[1].m_Index >= GET_ITEM(3, 00) && lpObj->Inventory[1].m_Index <= GET_ITEM(3, 500)
		|| lpObj->Inventory[1].m_Index >= GET_ITEM(4, 00) && lpObj->Inventory[1].m_Index <= GET_ITEM(4, 500)
		|| lpObj->Inventory[1].m_Index >= GET_ITEM(5, 00) && lpObj->Inventory[1].m_Index <= GET_ITEM(5, 500)
		
	)
	{
		if (Inventory0->m_Level >= level && Inventory0->m_NewOption)
		{
			count++;;
		}

		if (Inventory1->m_Level >= level && Inventory1->m_NewOption)
		{
			count++;;
		}
	}
	return count;
}
#endif