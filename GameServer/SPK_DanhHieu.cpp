#include "stdafx.h"
#if DANH_HIEU_NEW
#include "SPK_DanhHieu.h"
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

SPKDanhHieu gSPKDanhHieu;

SPKDanhHieu::SPKDanhHieu()
{
	this->Enable = false;
	this->ThongBao = false;
	this->mDataConfigDanhHieu.clear();
}

SPKDanhHieu::~SPKDanhHieu()
{

}

void SPKDanhHieu::LoadConfig(char* FilePath)
{
	this->mDataConfigDanhHieu.clear();
	this->Enable = false;
	this->ThongBao = false;

	pugi::xml_document file;
	pugi::xml_parse_result res = file.load_file(FilePath);
	if (res.status != pugi::status_ok) {
		ErrorMessageBox("File %s load fail. Error: %s", FilePath, res.description());
		return;
	}

	pugi::xml_node oDanhHieu = file.child("DanhHieu");

	this->Enable	= oDanhHieu.attribute("Enable").as_int();
	this->ThongBao	= oDanhHieu.attribute("Enable").as_int();
	this->ThoiGian	= oDanhHieu.attribute("ThoiGian").as_int();
	this->GioiHan	= oDanhHieu.attribute("GioiHan").as_int();

	pugi::xml_node Message = oDanhHieu.child("Message");
	for (pugi::xml_node msg = Message.child("Msg"); msg; msg = msg.next_sibling())
	{
		MESSAGE_INFO_DANHHIEU info;
		info.Index = msg.attribute("Index").as_int();
		strcpy_s(info.Message, msg.attribute("Text").as_string());
		this->m_MessageInfoBP.insert(std::pair<int, MESSAGE_INFO_DANHHIEU>(info.Index, info));
	}
	pugi::xml_node ConfigDanhHieu	= oDanhHieu.child("ConfigDanhHieu");
	for (pugi::xml_node CapDo		= ConfigDanhHieu.child("CapDo"); CapDo; CapDo = CapDo.next_sibling())
	{
		CONFIDATA_DANHHIEU InfoDanhHieu;
		InfoDanhHieu.LvDanhHieu = CapDo.attribute("LvDanhHieu").as_int();
		InfoDanhHieu.YCItemSL	= CapDo.attribute("YCItemSL").as_int();
		InfoDanhHieu.YCItemID	= CapDo.attribute("YCItemID").as_int();
		InfoDanhHieu.YCItemLv	= CapDo.attribute("YCItemLv").as_int();
		InfoDanhHieu.YCWC		= CapDo.attribute("YCWC").as_int();
		InfoDanhHieu.YCWP		= CapDo.attribute("YCWP").as_int();
		InfoDanhHieu.YCGP		= CapDo.attribute("YCGP").as_int();
		InfoDanhHieu.TangMau	= CapDo.attribute("TangMau").as_int();
		InfoDanhHieu.TangSD		= CapDo.attribute("TangSD").as_int();
		InfoDanhHieu.TangST		= CapDo.attribute("TangST").as_int();
		InfoDanhHieu.TangPT		= CapDo.attribute("TangPT").as_int();
		this->mDataConfigDanhHieu.insert(std::pair<int, CONFIDATA_DANHHIEU>(InfoDanhHieu.LvDanhHieu, InfoDanhHieu));
	}
}

char* SPKDanhHieu::GetMessage(int index)
{
	std::map<int, MESSAGE_INFO_DANHHIEU>::iterator it = this->m_MessageInfoBP.find(index);
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

CONFIDATA_DANHHIEU* SPKDanhHieu::GetConfigLvDanhHieu(int LvDanhHieu)
{
	std::map<int, CONFIDATA_DANHHIEU>::iterator it = this->mDataConfigDanhHieu.find(LvDanhHieu);
	if (it == this->mDataConfigDanhHieu.end())
	{
		return 0;
	}
	else
	{
		return &it->second;
	}
}
void SPKDanhHieu::SendInfoClient(int aIndex) // OK
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
	pMsg.header.set(0xD5, 0x03, 0);

	int size = sizeof(pMsg);
	pMsg.Count = 0;

	for (int i = 0; i < this->mDataConfigDanhHieu.size(); i++)
	{
		if (size + sizeof(this->mDataConfigDanhHieu[i]) > 4096)
		{
			LogAdd(LOG_RED, "(%s %d) Data qua dai !!", __FILE__, __LINE__);
			return;
		}
		memcpy(&send[size], &this->mDataConfigDanhHieu[i], sizeof(this->mDataConfigDanhHieu[i]));
		size += sizeof(this->mDataConfigDanhHieu[i]);

		pMsg.Count++;
	}
	pMsg.header.size[0] = SET_NUMBERHB(size);
	pMsg.header.size[1] = SET_NUMBERLB(size);
	memcpy(send, &pMsg, sizeof(pMsg));
	DataSend(aIndex, send, size);
}

void SPKDanhHieu::GetOptionDanhHieu(LPOBJ lpObj, bool flag)
{
	if (flag != 0 || lpObj->CB_Status1 != 0 || !this->Enable || OBJECT_RANGE(lpObj->Index) == 0 || gObjIsConnected(lpObj->Index) == false)
	{
		return;
	}
	CONFIDATA_DANHHIEU* BGetDataDanhHieu = this->GetConfigLvDanhHieu(lpObj->rDanhHieu);
	if(BGetDataDanhHieu <= 0)
	{
		return;
	}
	lpObj->MagicDamageMin		+= BGetDataDanhHieu->TangST;
	lpObj->MagicDamageMax		+= BGetDataDanhHieu->TangST;
	lpObj->PhysiDamageMinLeft	+= BGetDataDanhHieu->TangST;
	lpObj->PhysiDamageMinRight	+= BGetDataDanhHieu->TangST;
	lpObj->PhysiDamageMaxLeft	+= BGetDataDanhHieu->TangST;
	lpObj->PhysiDamageMaxRight	+= BGetDataDanhHieu->TangST;
	lpObj->Defense				+= BGetDataDanhHieu->TangPT;
	lpObj->AddLife				+= BGetDataDanhHieu->TangMau;
	lpObj->AddShield			+= BGetDataDanhHieu->TangSD;
}
bool SPKDanhHieu::NangCapDanhHieu(int aIndex)
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
	CONFIDATA_DANHHIEU* GetDataDanhHieu = this->GetConfigLvDanhHieu(lpObj->rDanhHieu + 1);
	if (!GetDataDanhHieu)
	{
		return 0;
	}
	#if MOVE_ITEMBANK_DIR
	if (GetDataDanhHieu->YCItemSL > 0)
	{
		lpObj->ChaosLock = 1;
		int count = gItemManager.GetInventoryItemCount(lpObj, GetDataDanhHieu->YCItemID, GetDataDanhHieu->YCItemLv);
		if (count < GetDataDanhHieu->YCItemSL)
		{
			lpObj->ChaosLock = 0;
			gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, this->GetMessage(1), GetDataDanhHieu->YCItemSL, gItemLevel.GetItemName(GetDataDanhHieu->YCItemID, GetDataDanhHieu->YCItemLv));
			return false;
		}
	}
	#endif
	int CheckWC = GetDataDanhHieu->YCWC;
	int CheckWP = GetDataDanhHieu->YCWP;
	int CheckGP = GetDataDanhHieu->YCGP;
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
	if(lpObj->rDanhHieu == this->GioiHan )
	{
		gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, this->GetMessage(5), this->GioiHan);
		return false;
	}
	if (CheckWC > 0 || CheckWP > 0 || CheckGP > 0)
	{
		GDSetCoinSend(lpObj->Index, (CheckWC > 0 ? -CheckWC : 0), (CheckWP > 0 ? -CheckWP : 0), (CheckGP > 0 ? -CheckGP : 0), 0, 0, "rDanhHieu");
	}

	#if MOVE_ITEMBANK_DIR
	gItemManager.DeleteInventoryItemCount(lpObj, GetDataDanhHieu->YCItemID, GetDataDanhHieu->YCItemLv, GetDataDanhHieu->YCItemSL);
	lpObj->ChaosLock = 0;
	#else
	if (lpObj->ItemBank[GetDataDanhHieu->YCItemID] <= 0 || lpObj->ItemBank[GetDataDanhHieu->YCItemID] < GetDataDanhHieu->YCItemSL)
	{
		gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, "Ngân hàng không đủ ngọc");
		return false;
	}
	 
	gCustomJewelBank.GDCustomJewelBankDelJewel(lpObj, GetDataDanhHieu->YCItemID, GetDataDanhHieu->YCItemSL);
	#endif

	lpObj->rDanhHieu++;
	if (this->ThongBao)
	{
		gNotice.GCNoticeSendToAll(0, 0, 0, 0, 0, 0, this->GetMessage(4), lpObj->Name, lpObj->rDanhHieu);
	}
	gNotice.GCNoticeSend(aIndex,1, 0, 0, 0, 0, 0, this->GetMessage(3), lpObj->rDanhHieu);
	gCustomRankUser.GCReqRankLevelUser(lpObj->Index, lpObj->Index);
	gObjectManager.CharacterCalcAttribute(lpObj->Index);
	GDCharacterInfoSaveSend(lpObj->Index);
	lpObj->ClickClientSend = GetTickCount();
	return 1;
}
#endif