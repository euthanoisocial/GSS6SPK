#include "stdafx.h"
#include "SPK_HonHoan.h"
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
#include "Log.h"
#include "SPK_Message.h"
#include "CustomJewelBank.h"
#if(B_HON_HOAN)
CBHonHoan gBHonHoan;

CBHonHoan::CBHonHoan()
{
	this->Enable = false;
	this->ThongBao = false;
	gBHonHoan.mDataConfigHonHoan.clear();
}

CBHonHoan::~CBHonHoan()
{

}


void CBHonHoan::LoadConfig(char* FilePath)
{
	gBHonHoan.mDataConfigHonHoan.clear();
	this->Enable = false;
	this->ThongBao = false;

	pugi::xml_document file;
	pugi::xml_parse_result res = file.load_file(FilePath);
	if (res.status != pugi::status_ok) {
		ErrorMessageBox("File %s load fail. Error: %s", FilePath, res.description());
		return;
	}
	//--
	pugi::xml_node oHonHoan = file.child("HonHoan");

	this->Enable	= oHonHoan.attribute("Enable").as_int();
	this->ThongBao	= oHonHoan.attribute("Enable").as_int();
	this->ThoiGian	= oHonHoan.attribute("ThoiGian").as_int();
	this->GioiHan	= oHonHoan.attribute("GioiHan").as_int();
	//= Mess Load
	pugi::xml_node Message = oHonHoan.child("Message");
	for (pugi::xml_node msg = Message.child("Msg"); msg; msg = msg.next_sibling())
	{
		MESSAGE_INFO_HONHOAN info;

		info.Index = msg.attribute("Index").as_int();

		strcpy_s(info.Message, msg.attribute("Text").as_string());

		this->m_MessageInfoBP.insert(std::pair<int, MESSAGE_INFO_HONHOAN>(info.Index, info));
	}
	//==Load Config
	pugi::xml_node ConfigHonHoan = oHonHoan.child("ConfigHonHoan");
	for (pugi::xml_node CapDo = ConfigHonHoan.child("CapDo"); CapDo; CapDo = CapDo.next_sibling())
	{
		CONFIDATA_HONHOAN InfoHonHoan;
		InfoHonHoan.LvHonHoan	= CapDo.attribute("LvHonHoan").as_int();
		InfoHonHoan.YCItemSL	= CapDo.attribute("YCItemSL").as_int();
		InfoHonHoan.YCItemID	= CapDo.attribute("YCItemID").as_int();
		InfoHonHoan.YCItemLv	= CapDo.attribute("YCItemLv").as_int();
		InfoHonHoan.YCWC		= CapDo.attribute("YCWC").as_int();
		InfoHonHoan.YCWP		= CapDo.attribute("YCWP").as_int();
		InfoHonHoan.YCGP		= CapDo.attribute("YCGP").as_int();
		InfoHonHoan.TangMau		= CapDo.attribute("TangMau").as_int();
		InfoHonHoan.TangSD		= CapDo.attribute("TangSD").as_int();
		InfoHonHoan.TangST		= CapDo.attribute("TangST").as_int();
		InfoHonHoan.TangPT		= CapDo.attribute("TangPT").as_int();
		InfoHonHoan.ItemReward	= CapDo.attribute("ItemReward").as_int();
		this->mDataConfigHonHoan.insert(std::pair<int, CONFIDATA_HONHOAN>(InfoHonHoan.LvHonHoan, InfoHonHoan));
	}
}

char* CBHonHoan::GetMessage(int index) // OK
{
	std::map<int, MESSAGE_INFO_HONHOAN>::iterator it = this->m_MessageInfoBP.find(index);

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

CONFIDATA_HONHOAN* CBHonHoan::GetConfigLvHonHoan(int LvHonHoan)
{
	std::map<int, CONFIDATA_HONHOAN>::iterator it = this->mDataConfigHonHoan.find(LvHonHoan);

	if (it == this->mDataConfigHonHoan.end())
	{
		return 0;
	}
	else
	{
		return &it->second;
	}
}


void CBHonHoan::SendInfoClient(int aIndex) // OK
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

	pMsg.header.set(0xD5, 0x01, 0);

	int size = sizeof(pMsg);

	pMsg.Count = 0;

	for (int i = 0; i < this->mDataConfigHonHoan.size(); i++)
	{
		if (size + sizeof(this->mDataConfigHonHoan[i]) > 4096)
		{
			LogAdd(LOG_RED, "(%s %d) Data qua dai !!", __FILE__, __LINE__);
			return;
		}
		memcpy(&send[size], &this->mDataConfigHonHoan[i], sizeof(this->mDataConfigHonHoan[i]));
		size += sizeof(this->mDataConfigHonHoan[i]);

		pMsg.Count++;
	}
	pMsg.header.size[0] = SET_NUMBERHB(size);

	pMsg.header.size[1] = SET_NUMBERLB(size);

	memcpy(send, &pMsg, sizeof(pMsg));
	DataSend(aIndex, send, size);
}

void CBHonHoan::GetOptionHonHoan(LPOBJ lpObj, bool flag)
{
	if (flag != 0 || lpObj->CB_Status1 != 0 || !this->Enable || OBJECT_RANGE(lpObj->Index) == 0 || gObjIsConnected(lpObj->Index) == false)
	{
		return;
	}
	//CONFIDATA_HONHOAN* BGetDataHonHoan = this->GetConfigLvHonHoan(lpObj->m_RankTitle5);
	CONFIDATA_HONHOAN* BGetDataHonHoan = this->GetConfigLvHonHoan(lpObj->m_RankTitle5);
	if(BGetDataHonHoan <= 0)
	{
		return;
	}
	lpObj->MagicDamageMin		+= BGetDataHonHoan->TangST;
	lpObj->MagicDamageMax		+= BGetDataHonHoan->TangST;
	lpObj->PhysiDamageMinLeft	+= BGetDataHonHoan->TangST;
	lpObj->PhysiDamageMinRight	+= BGetDataHonHoan->TangST;
	lpObj->PhysiDamageMaxLeft	+= BGetDataHonHoan->TangST;
	lpObj->PhysiDamageMaxRight	+= BGetDataHonHoan->TangST;
	lpObj->Defense += BGetDataHonHoan->TangPT;
	lpObj->AddLife += BGetDataHonHoan->TangMau;
	lpObj->AddShield += BGetDataHonHoan->TangSD;
}

bool CBHonHoan::NangCapHonHoan(int aIndex) // OK
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

	CONFIDATA_HONHOAN* GetDataHonHoan = this->GetConfigLvHonHoan(lpObj->m_RankTitle5 + 1);

	if (!GetDataHonHoan)
	{
		return 0;
	}	
	#if MOVE_ITEMBANK_DIR
	if (GetDataHonHoan->YCItemSL > 0)
	{
		lpObj->ChaosLock = 1;
		int count = gItemManager.GetInventoryItemCount(lpObj, GetDataHonHoan->YCItemID, GetDataHonHoan->YCItemLv);

		if (count < GetDataHonHoan->YCItemSL)
		{
			lpObj->ChaosLock = 0;
			gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, this->GetMessage(1), GetDataHonHoan->YCItemSL, gItemLevel.GetItemName(GetDataHonHoan->YCItemID, GetDataHonHoan->YCItemLv));
			return false;
		}		
	}
	#endif
	int CheckWC = GetDataHonHoan->YCWC;
	int CheckWP = GetDataHonHoan->YCWP;
	int CheckGP = GetDataHonHoan->YCGP;

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
	if(lpObj->m_RankTitle5 == this->GioiHan )
	{
		gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, this->GetMessage(5), this->GioiHan);
		return false;
	}
	if (CheckWC > 0 || CheckWP > 0 || CheckGP > 0)
	{
		GDSetCoinSend(lpObj->Index, (CheckWC > 0 ? -CheckWC : 0), (CheckWP > 0 ? -CheckWP : 0), (CheckGP > 0 ? -CheckGP : 0), 0, 0, "rHonHoan");
	}
	
	#if MOVE_ITEMBANK_DIR
	gItemManager.DeleteInventoryItemCount(lpObj, GetDataHonHoan->YCItemID, GetDataHonHoan->YCItemLv, GetDataHonHoan->YCItemSL);
	lpObj->ChaosLock = 0;
	#else
	if (lpObj->ItemBank[GetDataHonHoan->YCItemID] <= 0 || lpObj->ItemBank[GetDataHonHoan->YCItemID] < GetDataHonHoan->YCItemSL)
	{
		gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, "Ngân hàng không đủ ngọc");
		return false;
	}
	gCustomJewelBank.GDCustomJewelBankDelJewel(lpObj, GetDataHonHoan->YCItemID, GetDataHonHoan->YCItemSL);
	#endif

	lpObj->m_RankTitle5++;
	// 
	int IsOk = GetDataHonHoan->ItemReward;
	if (this->ThongBao)
	{
		gNotice.GCNoticeSendToAll(0, 0, 0, 0, 0, 0, this->GetMessage(4), lpObj->Name, lpObj->m_RankTitle5);
	}
	if (lpObj->m_RankTitle5 == 5)
	{
		GDCreateItemSend(lpObj->Index, 0xEB, 0, 0, IsOk, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
	}
	gNotice.GCNoticeSend(aIndex,1, 0, 0, 0, 0, 0, this->GetMessage(3), lpObj->m_RankTitle5);
	gCustomRankUser.GCReqRankLevelUser(lpObj->Index, lpObj->Index);
	gObjectManager.CharacterCalcAttribute(lpObj->Index);

	GDCharacterInfoSaveSend(lpObj->Index);

	LogAdd(LOG_RED, gSPKMessage.GetMessage(25), lpObj->Name, GetDataHonHoan->LvHonHoan, gItemLevel.GetItemName(GetDataHonHoan->ItemReward, 0));
	gLog.Output(LOG_GENERAL, gSPKMessage.GetMessage(25), lpObj->Name, GetDataHonHoan->LvHonHoan, gItemLevel.GetItemName(GetDataHonHoan->ItemReward, 0));

	lpObj->ClickClientSend = GetTickCount();
	return 1;

}
#endif