#include "stdafx.h"

#include "SPK_TuChan.h"
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
#if TU_LUYEN_NEW
SPKTuLuyen gSPKTuLuyen;

SPKTuLuyen::SPKTuLuyen()
{
	this->Enable = false;
	this->ThongBao = false;
	this->mDataConfigTuLuyen.clear();
}

SPKTuLuyen::~SPKTuLuyen()
{

}

void SPKTuLuyen::LoadConfig(char* FilePath)
{
	this->mDataConfigTuLuyen.clear();
	this->Enable = false;
	this->ThongBao = false;

	pugi::xml_document file;
	pugi::xml_parse_result res = file.load_file(FilePath);
	if (res.status != pugi::status_ok) {
		ErrorMessageBox("File %s load fail. Error: %s", FilePath, res.description());
		return;
	}

	pugi::xml_node oTuLuyen = file.child("TuLuyen");

	this->Enable	= oTuLuyen.attribute("Enable").as_int();
	this->ThongBao	= oTuLuyen.attribute("Enable").as_int();
	this->ThoiGian	= oTuLuyen.attribute("ThoiGian").as_int();
	this->GioiHan	= oTuLuyen.attribute("GioiHan").as_int();

	pugi::xml_node Message = oTuLuyen.child("Message");
	for (pugi::xml_node msg = Message.child("Msg"); msg; msg = msg.next_sibling())
	{
		MESSAGE_INFO_TULUYEN info;
		info.Index = msg.attribute("Index").as_int();
		strcpy_s(info.Message, msg.attribute("Text").as_string());
		this->m_MessageInfoBP.insert(std::pair<int, MESSAGE_INFO_TULUYEN>(info.Index, info));
	}
	pugi::xml_node ConfigTuLuyen	= oTuLuyen.child("ConfigTuLuyen");
	for (pugi::xml_node CapDo		= ConfigTuLuyen.child("CapDo"); CapDo; CapDo = CapDo.next_sibling())
	{
		CONFIDATA_TULUYEN InfoTuLuyen;
		InfoTuLuyen.LvTuLuyen = CapDo.attribute("LvTuLuyen").as_int();
		InfoTuLuyen.YCItemSL	= CapDo.attribute("YCItemSL").as_int();
		InfoTuLuyen.YCItemID	= CapDo.attribute("YCItemID").as_int();
		InfoTuLuyen.YCItemLv	= CapDo.attribute("YCItemLv").as_int();
		InfoTuLuyen.YCWC		= CapDo.attribute("YCWC").as_int();
		InfoTuLuyen.YCWP		= CapDo.attribute("YCWP").as_int();
		InfoTuLuyen.YCGP		= CapDo.attribute("YCGP").as_int();
		InfoTuLuyen.TangMau	= CapDo.attribute("TangMau").as_int();
		InfoTuLuyen.TangSD		= CapDo.attribute("TangSD").as_int();
		InfoTuLuyen.TangST		= CapDo.attribute("TangST").as_int();
		InfoTuLuyen.TangPT		= CapDo.attribute("TangPT").as_int();
		this->mDataConfigTuLuyen.insert(std::pair<int, CONFIDATA_TULUYEN>(InfoTuLuyen.LvTuLuyen, InfoTuLuyen));
	}
}

char* SPKTuLuyen::GetMessage(int index)
{
	std::map<int, MESSAGE_INFO_TULUYEN>::iterator it = this->m_MessageInfoBP.find(index);
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

CONFIDATA_TULUYEN* SPKTuLuyen::GetConfigLvTuLuyen(int LvTuLuyen)
{
	std::map<int, CONFIDATA_TULUYEN>::iterator it = this->mDataConfigTuLuyen.find(LvTuLuyen);
	if (it == this->mDataConfigTuLuyen.end())
	{
		return 0;
	}
	else
	{
		return &it->second;
	}
}
void SPKTuLuyen::SendInfoClient(int aIndex) // OK
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
	pMsg.header.set(0xD5, 0x02, 0);

	int size = sizeof(pMsg);
	pMsg.Count = 0;

	for (int i = 0; i < this->mDataConfigTuLuyen.size(); i++)
	{
		if (size + sizeof(this->mDataConfigTuLuyen[i]) > 4096)
		{
			LogAdd(LOG_RED, "(%s %d) Data qua dai !!", __FILE__, __LINE__);
			return;
		}
		memcpy(&send[size], &this->mDataConfigTuLuyen[i], sizeof(this->mDataConfigTuLuyen[i]));
		size += sizeof(this->mDataConfigTuLuyen[i]);

		pMsg.Count++;
	}
	pMsg.header.size[0] = SET_NUMBERHB(size);
	pMsg.header.size[1] = SET_NUMBERLB(size);
	memcpy(send, &pMsg, sizeof(pMsg));
	DataSend(aIndex, send, size);
}

void SPKTuLuyen::GetOptionTuLuyen(LPOBJ lpObj, bool flag)
{
	if (flag != 0 || lpObj->CB_Status1 != 0 || !this->Enable || OBJECT_RANGE(lpObj->Index) == 0 || gObjIsConnected(lpObj->Index) == false)
	{
		return;
	}
	CONFIDATA_TULUYEN* BGetDataTuLuyen = this->GetConfigLvTuLuyen(lpObj->rTuLuyen);
	if(BGetDataTuLuyen <= 0)
	{
		return;
	}
	lpObj->MagicDamageMin		+= BGetDataTuLuyen->TangST;
	lpObj->MagicDamageMax		+= BGetDataTuLuyen->TangST;
	lpObj->PhysiDamageMinLeft	+= BGetDataTuLuyen->TangST;
	lpObj->PhysiDamageMinRight	+= BGetDataTuLuyen->TangST;
	lpObj->PhysiDamageMaxLeft	+= BGetDataTuLuyen->TangST;
	lpObj->PhysiDamageMaxRight	+= BGetDataTuLuyen->TangST;
	lpObj->Defense				+= BGetDataTuLuyen->TangPT;
	lpObj->AddLife				+= BGetDataTuLuyen->TangMau;
	lpObj->AddShield			+= BGetDataTuLuyen->TangSD;
}

bool SPKTuLuyen::NangCapTuLuyen(int aIndex)
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
	CONFIDATA_TULUYEN* GetDataTuLuyen = this->GetConfigLvTuLuyen(lpObj->rTuLuyen + 1);
	if (!GetDataTuLuyen)
	{
		return 0;
	}
	#if MOVE_ITEMBANK_DIR
	if (GetDataTuLuyen->YCItemSL > 0)
	{
		lpObj->ChaosLock = 1;
		int count = gItemManager.GetInventoryItemCount(lpObj, GetDataTuLuyen->YCItemID, GetDataTuLuyen->YCItemLv);
		if (count < GetDataTuLuyen->YCItemSL)
		{
			lpObj->ChaosLock = 0;
			gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, this->GetMessage(1), GetDataTuLuyen->YCItemSL, gItemLevel.GetItemName(GetDataTuLuyen->YCItemID, GetDataTuLuyen->YCItemLv));
			return false;
		}		
	}
	#endif
	int CheckWC = GetDataTuLuyen->YCWC;
	int CheckWP = GetDataTuLuyen->YCWP;
	int CheckGP = GetDataTuLuyen->YCGP;
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
	if(lpObj->rTuLuyen == this->GioiHan )
	{
		gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, this->GetMessage(5), this->GioiHan);
		return false;
	}
	if (CheckWC > 0 || CheckWP > 0 || CheckGP > 0)
	{
		GDSetCoinSend(lpObj->Index, (CheckWC > 0 ? -CheckWC : 0), (CheckWP > 0 ? -CheckWP : 0), (CheckGP > 0 ? -CheckGP : 0), 0, 0, "rTuCHan");
	}
	
	#if MOVE_ITEMBANK_DIR
	gItemManager.DeleteInventoryItemCount(lpObj, GetDataTuLuyen->YCItemID, GetDataTuLuyen->YCItemLv, GetDataTuLuyen->YCItemSL);
	lpObj->ChaosLock = 0;
	#else
	if (lpObj->ItemBank[GetDataTuLuyen->YCItemID] <= 0 || lpObj->ItemBank[GetDataTuLuyen->YCItemID] < GetDataTuLuyen->YCItemSL)
	{
		gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, "Ngân hàng không đủ ngọc");
		return false;
	}
	 
	gCustomJewelBank.GDCustomJewelBankDelJewel(lpObj, GetDataTuLuyen->YCItemID, GetDataTuLuyen->YCItemSL);
	#endif
	lpObj->rTuLuyen++;
	if (this->ThongBao)
	{
		gNotice.GCNoticeSendToAll(0, 0, 0, 0, 0, 0, this->GetMessage(4), lpObj->Name, lpObj->rTuLuyen);
	}
	gNotice.GCNoticeSend(aIndex,1, 0, 0, 0, 0, 0, this->GetMessage(3), lpObj->rTuLuyen);
	gCustomRankUser.GCReqRankLevelUser(lpObj->Index, lpObj->Index);
	gObjectManager.CharacterCalcAttribute(lpObj->Index);
	GDCharacterInfoSaveSend(lpObj->Index);
	lpObj->ClickClientSend = GetTickCount();
	return 1;
}
#endif