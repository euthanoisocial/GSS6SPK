#include "StdAfx.h"
#include "CustomLucChien.h"
#include "DSProtocol.h"
#include "EffectManager.h"
#include "Map.h"
#include "MemScript.h"
#include "Notice.h"
#include "Util.h"
#include "ItemOptionRate.h"
#include "ObjectManager.h"
#include "Guild.h"
#include "Move.h"
#include "Monster.h"
#include "ItemBagManager.h"
#include "Party.h"

CustomLucChien gCustomLucChien;

CustomLucChien::CustomLucChien()
{
	this->Init();
}


CustomLucChien::~CustomLucChien()
{
}

void CustomLucChien::Init()
{
	this->mLucChienDataChar.clear();
	this->mLucChienDataItem.clear();

}

void CustomLucChien::LoadFileXML(char* FilePath)
{
	this->Init();
	pugi::xml_document file;
	pugi::xml_parse_result res = file.load_file(FilePath);
	if (res.status != pugi::status_ok) {
		ErrorMessageBox("File %s load fail. Error: %s", FilePath, res.description());
		return;
	}
	//--
	pugi::xml_node oCutomLucChien = file.child("CutomLucChien");
	this->Enable = oCutomLucChien.attribute("Enable").as_int();

	pugi::xml_node CharLucChien = oCutomLucChien.child("CharLucChien");
	this->mLucChienDataChar.LCLevel = CharLucChien.attribute("LCLevel").as_int();
	this->mLucChienDataChar.LCStr = CharLucChien.attribute("LCStr").as_int();
	this->mLucChienDataChar.LCAgi = CharLucChien.attribute("LCAgi").as_int();
	this->mLucChienDataChar.LCVit = CharLucChien.attribute("LCVit").as_int();
	this->mLucChienDataChar.LCEne = CharLucChien.attribute("LCEne").as_int();
	this->mLucChienDataChar.LCCmd = CharLucChien.attribute("LCCmd").as_int();
	this->mLucChienDataChar.LCLvMaster = CharLucChien.attribute("LCLvMaster").as_int();
	this->mLucChienDataChar.LCAddSkillMT = CharLucChien.attribute("LCAddSkillMT").as_int();

	pugi::xml_node ItemLucChien = oCutomLucChien.child("ItemLucChien");
	for (pugi::xml_node Item = ItemLucChien.child("Item"); Item; Item = Item.next_sibling())
	{
		ITEMDATA_LUCCHIEN info = { 0 };
		info.IndexMin = Item.attribute("IndexMin").as_int();
		info.IndexMax = Item.attribute("IndexMax").as_int();
		info.LCCap = Item.attribute("LCCap").as_int();
		info.LCGoc = Item.attribute("LCGoc").as_int();
		info.LCLevel = Item.attribute("LCLevel").as_int();
		info.LCOptSkill = Item.attribute("LCOptSkill").as_int();
		info.LCOptLuck = Item.attribute("LCOptLuck").as_int();
		info.LCOptLife = Item.attribute("LCOptLife").as_int();
		info.LCOptExl = Item.attribute("LCOptExl").as_int();
		info.LCOptHarmony = Item.attribute("LCOptHarmony").as_int();
		info.LCOpt380 = Item.attribute("LCOpt380").as_int();
		info.LCOptAnc = Item.attribute("LCOptAnc").as_int();
		info.LCOptSocket = Item.attribute("LCOptSocket").as_int();
		this->mLucChienDataItem.push_back(info);
	}

	//LogAdd(LOG_BLUE, "[LucChien] Enable(%d), ItemData (%d)", this->Enable, this->mLucChienDataItem.size());
}
int CustomLucChien::GetLucChienItem(CItem* Item)
{
	DWORD TinhToanLucChien = 0;
	for (std::vector<ITEMDATA_LUCCHIEN>::iterator it = this->mLucChienDataItem.begin(); it != this->mLucChienDataItem.end(); it++)
	{
		if (Item->m_Index >= it->IndexMin && Item->m_Index <= it->IndexMax)
		{

			TinhToanLucChien += it->LCGoc;
			TinhToanLucChien += (it->LCLevel * Item->m_Level);
			TinhToanLucChien += it->LCCap * gItemManager.GetItemLevel(Item->m_Index);

			if (Item->m_Option1)
			{
				TinhToanLucChien += it->LCOptSkill;
			}
			if (Item->m_Option2)
			{
				TinhToanLucChien += it->LCOptLuck;
			}
			if (Item->m_Option3)
			{
				TinhToanLucChien += (it->LCOptLife * Item->m_Option3);
			}
			if (Item->IsExcItem())
			{
				TinhToanLucChien += (it->LCOptExl * GetNewOptionCount(Item->m_NewOption));
			}
			if (Item->IsJewelOfHarmonyItem())
			{
				TinhToanLucChien += it->LCOptHarmony;
			}
			if (Item->Is380Item())
			{
				TinhToanLucChien += it->LCOpt380;
			}
			if (Item->IsSetItem())
			{
				TinhToanLucChien += it->LCOptAnc;
			}
			if (Item->IsSocketItem())
			{
				TinhToanLucChien += (it->LCOptSocket * GetSocketOptionCount(Item->m_SocketOption));
			}
	
		}

	}
	//LogAdd(LOG_RED, "[LucChien Gew] %ld", TinhToanLucChien);
	return TinhToanLucChien;
}

void CustomLucChien::CalcLucChien(int aIndex)
{
	if (!this->Enable)
	{
		return;
	}
	if (OBJECT_RANGE(aIndex) == 0)
	{
		return;
	}
	LPOBJ lpObj = &gObj[aIndex];

	DWORD TinhToanLucChien = 0;
	TinhToanLucChien += this->mLucChienDataChar.LCLevel * lpObj->Level;
	TinhToanLucChien += this->mLucChienDataChar.LCStr * (lpObj->Strength + lpObj->AddStrength);
	TinhToanLucChien += this->mLucChienDataChar.LCAgi * (lpObj->Dexterity + lpObj->AddDexterity);
	TinhToanLucChien += this->mLucChienDataChar.LCVit * (lpObj->Vitality + lpObj->AddVitality);
	TinhToanLucChien += this->mLucChienDataChar.LCEne * (lpObj->Energy + lpObj->AddEnergy);
	TinhToanLucChien += this->mLucChienDataChar.LCCmd * (lpObj->Leadership+lpObj->AddLeadership);
	if (lpObj->ChangeUp >= 2)
	{
		TinhToanLucChien += this->mLucChienDataChar.LCLvMaster * (lpObj->MasterLevel);
		TinhToanLucChien += this->mLucChienDataChar.LCAddSkillMT * (lpObj->MasterPoint);
	}

	for (int n = 0; n < 12; n++)
	{
		if (!lpObj->Inventory[n].IsItem())
		{
			continue;
		}

		TinhToanLucChien += this->GetLucChienItem(&lpObj->Inventory[n]);	
	}

	if (lpObj->mLucChien != TinhToanLucChien)
	{
		lpObj->mLucChien = TinhToanLucChien;
		//LogAdd(LOG_RED, "[LucChien] %s (%ld)", lpObj->Name, lpObj->mLucChien);
		//====
		DATA_LUCCHIEN pMsg;
		pMsg.header.set(0xD3, 0x23, sizeof(pMsg));
		pMsg.LucChien = lpObj->mLucChien;	//Clear Item Cache O Client
		DataSend(lpObj->Index, (BYTE*)& pMsg, pMsg.header.size);
	}
}
