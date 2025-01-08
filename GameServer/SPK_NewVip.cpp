#include "stdafx.h"
#if NEW_VIP_NEW
#include "SPK_NewVip.h"
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
#include "JSProtocol.h"
SPKNewVip gSPKNewVip;

SPKNewVip::SPKNewVip()
{
	this->Enable = false;
	this->ThongBao = false;
	this->mDataConfigNewVip.clear();
}

SPKNewVip::~SPKNewVip()
{

}

void SPKNewVip::LoadConfig(char* FilePath)
{
	this->mDataConfigNewVip.clear();
	this->Enable = false;
	this->ThongBao = false;

	pugi::xml_document file;
	pugi::xml_parse_result res = file.load_file(FilePath);
	if (res.status != pugi::status_ok) {
		ErrorMessageBox("File %s load fail. Error: %s", FilePath, res.description());
		return;
	}

	pugi::xml_node oNewVip = file.child("NewVip");

	this->Enable	= oNewVip.attribute("Enable").as_int();
	this->ThongBao	= oNewVip.attribute("Enable").as_int();
	this->ThoiGian	= oNewVip.attribute("ThoiGian").as_int();
	this->GioiHan	= oNewVip.attribute("GioiHan").as_int();

	pugi::xml_node Message = oNewVip.child("Message");
	for (pugi::xml_node msg = Message.child("Msg"); msg; msg = msg.next_sibling())
	{
		MESSAGE_INFO_NEWVIP info;
		info.Index = msg.attribute("Index").as_int();
		strcpy_s(info.Message, msg.attribute("Text").as_string());
		this->m_MessageInfoBP.insert(std::pair<int, MESSAGE_INFO_NEWVIP>(info.Index, info));
	}
	pugi::xml_node ConfigNewVip	= oNewVip.child("ConfigNewVip");
	for (pugi::xml_node CapDo		= ConfigNewVip.child("CapDo"); CapDo; CapDo = CapDo.next_sibling())
	{
		CONFIDATA_NEWVIP InfoNewVip;
		InfoNewVip.LvNewVip		= CapDo.attribute("LvNewVip").as_int();
		InfoNewVip.YCWC			= CapDo.attribute("YCWC").as_int();
		InfoNewVip.ExpThuong	= CapDo.attribute("ExpThuong").as_int();
		InfoNewVip.ExpMaster	= CapDo.attribute("ExpMaster").as_int();
		InfoNewVip.Days			= CapDo.attribute("Days").as_int();

		this->mDataConfigNewVip.insert(std::pair<int, CONFIDATA_NEWVIP>(InfoNewVip.LvNewVip, InfoNewVip));
	}
}

char* SPKNewVip::GetMessage(int index)
{
	std::map<int, MESSAGE_INFO_NEWVIP>::iterator it = this->m_MessageInfoBP.find(index);
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

CONFIDATA_NEWVIP* SPKNewVip::GetConfigLvNewVip(int LvNewVip)
{
	std::map<int, CONFIDATA_NEWVIP>::iterator it = this->mDataConfigNewVip.find(LvNewVip);
	if (it == this->mDataConfigNewVip.end())
	{
		return 0;
	}
	else
	{
		return &it->second;
	}
}
void SPKNewVip::SendInfoClient(int aIndex) // OK
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
	pMsg.header.set(0xD5, 0x05, 0);

	int size = sizeof(pMsg);
	pMsg.Count = 0;

	for (int i = 0; i < this->mDataConfigNewVip.size(); i++)
	{
		if (size + sizeof(this->mDataConfigNewVip[i]) > 4096)
		{
			LogAdd(LOG_RED, "(%s %d) Data qua dai !!", __FILE__, __LINE__);
			return;
		}
		memcpy(&send[size], &this->mDataConfigNewVip[i], sizeof(this->mDataConfigNewVip[i]));
		size += sizeof(this->mDataConfigNewVip[i]);

		pMsg.Count++;
	}
	pMsg.header.size[0] = SET_NUMBERHB(size);
	pMsg.header.size[1] = SET_NUMBERLB(size);
	memcpy(send, &pMsg, sizeof(pMsg));
	DataSend(aIndex, send, size);
}

void SPKNewVip::GetOptionNewVip(LPOBJ lpObj, bool flag)
{
	if (flag != 0 || lpObj->CB_Status1 != 0 || !this->Enable || OBJECT_RANGE(lpObj->Index) == 0 || gObjIsConnected(lpObj->Index) == false)
	{
		return;
	}
	CONFIDATA_NEWVIP* BGetDataNewVip = this->GetConfigLvNewVip(lpObj->rNewVip);
	if(BGetDataNewVip <= 0)
	{
		return;
	}
	lpObj->ExperienceRate		= lpObj->ExperienceRate + BGetDataNewVip->ExpThuong;
	lpObj->MasterExperienceRate = lpObj->MasterExperienceRate + BGetDataNewVip->ExpMaster;
}

bool SPKNewVip::NangCapNewVip(int aIndex)
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
	CONFIDATA_NEWVIP* GetDataNewVip = this->GetConfigLvNewVip(lpObj->rNewVip + 1);
	if (!GetDataNewVip)
	{
		return 0;
	}

	int CheckWC = GetDataNewVip->YCWC;
	if (CheckWC > lpObj->Coin1)
	{
		gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, this->GetMessage(2), CheckWC, "WC");
		return false;
	}
	
	if ((GetTickCount() - lpObj->ClickClientSend) < this->ThoiGian * 1000)
	{
		gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, this->GetMessage(6), this->ThoiGian);
		return false;
	}
	if(lpObj->rNewVip >= this->GioiHan )
	{
		gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, this->GetMessage(5), this->GioiHan);
		return false;
	}
	if (CheckWC > 0 )
	{
		GDSetCoinSend(lpObj->Index, (CheckWC > 0 ? -CheckWC : 0),0, 0,0,0, "rNewVip");
	}
	
	lpObj->rNewVip++;
	if (this->ThongBao)
	{
		gNotice.GCNoticeSendToAll(0, 0, 0, 0, 0, 0, this->GetMessage(4), lpObj->Name, lpObj->rNewVip);
	}
	gNotice.GCNoticeSend(aIndex,1, 0, 0, 0, 0, 0, this->GetMessage(3), lpObj->rNewVip);

	int UpVip = lpObj->rNewVip;
	if (UpVip >= 3)
	{
		UpVip = 3;
	}

	GJAccountLevelSaveSend(lpObj->Index, UpVip, GetDataNewVip->Days * 86400);
	GJAccountLevelSend(lpObj->Index);

	gCustomRankUser.GCReqRankLevelUser(lpObj->Index, lpObj->Index);
	gObjectManager.CharacterCalcAttribute(lpObj->Index);
	GDCharacterInfoSaveSend(lpObj->Index);
	lpObj->ClickClientSend = GetTickCount();
	return 1;
}
#endif