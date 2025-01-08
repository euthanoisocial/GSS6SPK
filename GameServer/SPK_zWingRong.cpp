#include "StdAfx.h"
#if ACTIVE_VKR
#include "SPK_zWingRong.h"
#include "MemScript.h"
#include "Message.h"
#include "Notice.h"
#include "Util.h"
#include "ItemManager.h"
#include "CustomStartItemDame.h"

ITEM12 GITEM12;

void ITEM12::Load(char* FilePath) // OK
{
	pugi::xml_document file;
	pugi::xml_parse_result res = file.load_file(FilePath);
	if (res.status != pugi::status_ok)
	{
		ErrorMessageBox("File %s load fail. Error: %s", FilePath, res.description());
		return;
	}
	pugi::xml_node CustomVuKhiRong = file.child("SPK");
	this->m_ITEM12.Enabled = CustomVuKhiRong.attribute("Enable").as_int();

	pugi::xml_node SlotVuKhi = CustomVuKhiRong.child("ItemRong");
	this->m_ITEM12.IsITEM12[0] = SlotVuKhi.attribute("Level_Item1").as_int();
	this->m_ITEM12.IsITEM12[1] = SlotVuKhi.attribute("Level_Item2").as_int();
	this->m_ITEM12.IsITEM12[2] = SlotVuKhi.attribute("Level_Item3").as_int();
}

void ITEM12::Option(LPOBJ lpObj, bool flag)
{
	if (!this->m_ITEM12.Enabled) return;

	if (flag != 0){	return;	}

	int VuKhiRong = 0;

	for (int x = 0; x < 3; x++)
	{
		if (this->CountItem(lpObj, this->m_ITEM12.IsITEM12[x]) >= 1)
		{
			VuKhiRong = x + 1;
		}
	}

	PMSG_ITEM12_REQ pRequest;
	pRequest.Head.set(0xFB, 0x0C, sizeof(pRequest));
	pRequest.ItemRong = VuKhiRong;

	DataSend(lpObj->Index, (LPBYTE)&pRequest, sizeof(pRequest));
}

int ITEM12::CountItem(LPOBJ lpObj, int level)
{
	int count = 0;
	CItem* Inventory = &lpObj->Inventory[7];

	if ( lpObj->Inventory[7].m_Index >= GET_ITEM(12, 00) && lpObj->Inventory[7].m_Index <= GET_ITEM(12, 400))
	{
		if (Inventory->m_Level >= level && Inventory->m_NewOption)
		{
			count++;;
		}
	}
	return count;
}
#endif