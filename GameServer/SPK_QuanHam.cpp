#include "stdafx.h"
#if QUAN_HAM_NEW
#include "SPK_QuanHam.h"
#include "DSProtocol.h"
#include "EffectManager.h"
#include "Map.h"
#include "MemScript.h"
#include "Notice.h"
#include "Util.h"
#include "ItemOptionRate.h"
#include "ObjectManager.h"
#include "Path.h"
#include "ItemLevel.h"
#include "CustomRankUser.h"
#include "CustomJewelBank.h"
SPKQuanHam gSPKQuanHam;

SPKQuanHam::SPKQuanHam()
{
	this->Enable = false;
	this->ThongBao = false;
	this->mDataConfigQuanHam.clear();
}

SPKQuanHam::~SPKQuanHam()
{

}

void SPKQuanHam::LoadConfig(char* FilePath)
{
	this->mDataConfigQuanHam.clear();
	this->Enable = false;
	this->ThongBao = false;

	pugi::xml_document file;
	pugi::xml_parse_result res = file.load_file(FilePath);
	if (res.status != pugi::status_ok) {
		ErrorMessageBox("File %s load fail. Error: %s", FilePath, res.description());
		return;
	}

	pugi::xml_node oQuanHam = file.child("QuanHam");

	this->Enable	= oQuanHam.attribute("Enable").as_int();
	this->ThongBao	= oQuanHam.attribute("Enable").as_int();
	this->ThoiGian	= oQuanHam.attribute("ThoiGian").as_int();
	this->GioiHan	= oQuanHam.attribute("GioiHan").as_int();

	pugi::xml_node Message = oQuanHam.child("Message");
	for (pugi::xml_node msg = Message.child("Msg"); msg; msg = msg.next_sibling())
	{
		MESSAGE_INFO_QUANHAM info;
		info.Index = msg.attribute("Index").as_int();
		strcpy_s(info.Message, msg.attribute("Text").as_string());
		this->m_MessageInfoBP.insert(std::pair<int, MESSAGE_INFO_QUANHAM>(info.Index, info));
	}
	pugi::xml_node ConfigQuanHam	= oQuanHam.child("ConfigQuanHam");
	for (pugi::xml_node CapDo		= ConfigQuanHam.child("CapDo"); CapDo; CapDo = CapDo.next_sibling())
	{
		CONFIDATA_QUANHAM InfoQuanHam;
		InfoQuanHam.LvQuanHam = CapDo.attribute("LvQuanHam").as_int();
		InfoQuanHam.YCItemSL	= CapDo.attribute("YCItemSL").as_int();
		InfoQuanHam.YCItemID	= CapDo.attribute("YCItemID").as_int();
		InfoQuanHam.YCItemLv	= CapDo.attribute("YCItemLv").as_int();
		InfoQuanHam.YCWC		= CapDo.attribute("YCWC").as_int();
		InfoQuanHam.YCWP		= CapDo.attribute("YCWP").as_int();
		InfoQuanHam.YCGP		= CapDo.attribute("YCGP").as_int();
		InfoQuanHam.TangMau		= CapDo.attribute("TangMau").as_int();
		InfoQuanHam.TangSD		= CapDo.attribute("TangSD").as_int();
		InfoQuanHam.TangST		= CapDo.attribute("TangST").as_int();
		InfoQuanHam.TangPT		= CapDo.attribute("TangPT").as_int();
		this->mDataConfigQuanHam.insert(std::pair<int, CONFIDATA_QUANHAM>(InfoQuanHam.LvQuanHam, InfoQuanHam));
	}
}

char* SPKQuanHam::GetMessage(int index)
{
	std::map<int, MESSAGE_INFO_QUANHAM>::iterator it = this->m_MessageInfoBP.find(index);
	if (it == this->m_MessageInfoBP.end())
	{
		char Error[256];
		wsprintf(Error, "Could not find message %d!", index);
		return Error;
	}
	else
	{
		return it->second.Message;
	}
}

CONFIDATA_QUANHAM* SPKQuanHam::GetConfigLvQuanHam(int LvQuanHam)
{
	std::map<int, CONFIDATA_QUANHAM>::iterator it = this->mDataConfigQuanHam.find(LvQuanHam);
	if (it == this->mDataConfigQuanHam.end())
	{
		return 0;
	}
	else
	{
		return &it->second;
	}
}
void SPKQuanHam::SendInfoClient(int aIndex) // OK
{
	if (!this->Enable)
	{
		gNotice.GCNoticeSend(aIndex, 1, 0, 0, 0, 0, 0, this->GetMessage(0));
		return;
	}
	if (OBJECT_RANGE(aIndex) == 0)
	{
		return;
	}
	if (gObjIsConnected(aIndex) == false)
	{
		return;
	}
	
	LPOBJ lpObj = &gObj[aIndex];
	BYTE send[4096];
	CSEV_COUNTLIST pMsg;
	pMsg.header.set(0xD5, 0x04, 0);

	int size = sizeof(pMsg);
	pMsg.Count = 0;

	for (int i = 0; i < this->mDataConfigQuanHam.size(); i++)
	{
		if (size + sizeof(this->mDataConfigQuanHam[i]) > 4096)
		{
			LogAdd(LOG_RED, "(%s %d) Data qua dai !!", __FILE__, __LINE__);
			return;
		}
		memcpy(&send[size], &this->mDataConfigQuanHam[i], sizeof(this->mDataConfigQuanHam[i]));
		size += sizeof(this->mDataConfigQuanHam[i]);

		pMsg.Count++;
	}
	pMsg.header.size[0] = SET_NUMBERHB(size);
	pMsg.header.size[1] = SET_NUMBERLB(size);
	memcpy(send, &pMsg, sizeof(pMsg));
	DataSend(aIndex, send, size);
}

void SPKQuanHam::GetOptionQuanHam(LPOBJ lpObj, bool flag)
{
	if (flag != 0 || lpObj->CB_Status1 != 0 || !this->Enable || OBJECT_RANGE(lpObj->Index) == 0 || gObjIsConnected(lpObj->Index) == false)
	{
		return;
	}
	CONFIDATA_QUANHAM* BGetDataQuanHam = this->GetConfigLvQuanHam(lpObj->rQuanHam);
	if(BGetDataQuanHam <= 0)
	{
		return;
	}
	lpObj->MagicDamageMin		+= BGetDataQuanHam->TangST;
	lpObj->MagicDamageMax		+= BGetDataQuanHam->TangST;
	lpObj->PhysiDamageMinLeft	+= BGetDataQuanHam->TangST;
	lpObj->PhysiDamageMinRight	+= BGetDataQuanHam->TangST;
	lpObj->PhysiDamageMaxLeft	+= BGetDataQuanHam->TangST;
	lpObj->PhysiDamageMaxRight	+= BGetDataQuanHam->TangST;
	lpObj->Defense				+= BGetDataQuanHam->TangPT;
	lpObj->AddLife				+= BGetDataQuanHam->TangMau;
	lpObj->AddShield			+= BGetDataQuanHam->TangSD;
}

bool SPKQuanHam::NangCapQuanHam(int aIndex)
{
	if (!this->Enable)
	{
		gNotice.GCNoticeSend(aIndex, 1, 0, 0, 0, 0, 0, this->GetMessage(0));
		return 0;
	}
	if (OBJECT_RANGE(aIndex) == 0 || gObjIsConnected(aIndex) == false)
	{
		return 0;
	}
	LPOBJ lpObj = &gObj[aIndex];
	CONFIDATA_QUANHAM* GetDataQuanHam = this->GetConfigLvQuanHam(lpObj->rQuanHam  + 1);
	if (!GetDataQuanHam)
	{
		return 0;
	}
	#if MOVE_ITEMBANK_DIR
	if (GetDataQuanHam->YCItemSL > 0)
	{
		lpObj->ChaosLock = 1;
		int count = gItemManager.GetInventoryItemCount(lpObj, GetDataQuanHam->YCItemID, GetDataQuanHam->YCItemLv);
		if (count < GetDataQuanHam->YCItemSL)
		{
			lpObj->ChaosLock = 0;
			gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, this->GetMessage(1), GetDataQuanHam->YCItemSL, gItemLevel.GetItemName(GetDataQuanHam->YCItemID, GetDataQuanHam->YCItemLv));
			return false;
		}		
	}
	#endif
	int CheckWC = GetDataQuanHam->YCWC;
	int CheckWP = GetDataQuanHam->YCWP;
	int CheckGP = GetDataQuanHam->YCGP;
	if (CheckWC > lpObj->Coin1)
	{
		gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, this->GetMessage(2), CheckWC, "WC");
		return false;
	}
	if (CheckWP > lpObj->Coin2)
	{
		gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, this->GetMessage(2), CheckWP, "WP");
		return false;
	}
	if (CheckGP > lpObj->Coin3)
	{
		gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, this->GetMessage(2), CheckGP, "GP");
		return false;
	}
	if ((GetTickCount() - lpObj->ClickClientSend) < this->ThoiGian * 1000)
	{
		gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, this->GetMessage(6), this->ThoiGian);
		return false;
	}
	if(lpObj->rQuanHam == this->GioiHan )
	{
		gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, this->GetMessage(5), this->GioiHan);
		return false;
	}
	if (CheckWC > 0 || CheckWP > 0 || CheckGP > 0)
	{
		GDSetCoinSend(lpObj->Index, (CheckWC > 0 ? -CheckWC : 0), (CheckWP > 0 ? -CheckWP : 0), (CheckGP > 0 ? -CheckGP : 0), 0, 0, "rQuanHam");
	}
	
	#if MOVE_ITEMBANK_DIR
	gItemManager.DeleteInventoryItemCount(lpObj, GetDataQuanHam->YCItemID, GetDataQuanHam->YCItemLv, GetDataQuanHam->YCItemSL);
	lpObj->ChaosLock = 0;
	#else
	if (lpObj->ItemBank[GetDataQuanHam->YCItemID] <= 0 || lpObj->ItemBank[GetDataQuanHam->YCItemID] < GetDataQuanHam->YCItemSL)
	{
		gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, "Ngân hàng không đủ ngọc");
		return false;
	}
	gCustomJewelBank.GDCustomJewelBankDelJewel(lpObj, GetDataQuanHam->YCItemID, GetDataQuanHam->YCItemSL);
	#endif

	lpObj->rQuanHam++;
	if (this->ThongBao)
	{
		gNotice.GCNoticeSendToAll(0, 0, 0, 0, 0, 0, this->GetMessage(4), lpObj->Name, lpObj->rQuanHam);
	}
	gNotice.GCNoticeSend(aIndex,1, 0, 0, 0, 0, 0, this->GetMessage(3), lpObj->rQuanHam);
	gCustomRankUser.GCReqRankLevelUser(lpObj->Index, lpObj->Index);
	gObjectManager.CharacterCalcAttribute(lpObj->Index);
	GDCharacterInfoSaveSend(lpObj->Index);
	lpObj->ClickClientSend = GetTickCount();
	return 1;
}
#endif