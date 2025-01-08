#include "stdafx.h"
#if(HT_THUE_FLAG)
#include "ThueFlag.h"
#include "MemScript.h"
#include "Util.h"
#include "ObjectManager.h"
#include "Notice.h"
#include "Viewport.h"
#include "DSProtocol.h"
#include "Message.h"
#include "ItemManager.h"
#include "CustomRankUser.h"
#include "CashShop.h"
#include "GameMain.h"
cThueFlag ThueFlag;

cThueFlag::cThueFlag()
{
}

cThueFlag::~cThueFlag()
{
}

void cThueFlag::Read(char* path)
{
	CMemScript* lpMemScript = new CMemScript;

	if (lpMemScript == 0)
	{
		ErrorMessageBox(MEM_SCRIPT_ALLOC_ERROR, path);
		return;
	}

	if (lpMemScript->SetBuffer(path) == 0)
	{
		ErrorMessageBox(lpMemScript->GetLastError());
		delete lpMemScript;
		return;
	}

	this->m_Data.clear();

	int type = -1;

	int sNum = 1;
	int lNum = 1;

	try
	{
		while (true)
		{
			if (lpMemScript->GetToken() == TOKEN_END)
			{
				break;
			}

			int section = lpMemScript->GetNumber();

			while (true)
			{
				if (section == 0)
				{
					if (strcmp("end", lpMemScript->GetAsString()) == 0)
					{
						break;
					}
					this->TypeBonus = lpMemScript->GetNumber();;
				}
				else if (section == 1)
				{
					if (strcmp("end", lpMemScript->GetAsString()) == 0)
					{
						break;
					}

					THUE_FLAG_DATA skin;

					skin.Id = this->m_Data.size();

					skin.Skin = lpMemScript->GetNumber();

					skin.Time = lpMemScript->GetAsNumber();

					skin.Price = lpMemScript->GetAsNumber();

					skin.Value1 = lpMemScript->GetAsNumber();

					skin.Value2 = lpMemScript->GetAsNumber();

					skin.Value3 = lpMemScript->GetAsNumber();

					skin.Value4 = lpMemScript->GetAsNumber();

					skin.Value7 = lpMemScript->GetAsNumber();

					skin.Value8 = lpMemScript->GetAsNumber();

					skin.Value9 = lpMemScript->GetAsNumber();

					this->m_Data.push_back(skin);
				}
				else
				{
					break;
				}
			}
		}
	}
	catch (...)
	{
		ErrorMessageBox(lpMemScript->GetLastError());
	}

	delete lpMemScript;
}

void cThueFlag::CGRequestRecv(BYTE* a, int aIndex)
{
	if (OBJECT_RANGE(aIndex) == 0)
	{
		return;
	}

	PMSG_THUE_FLAG_RECV* Data = (PMSG_THUE_FLAG_RECV*)a;

	switch (Data->Type)
	{
		case 0x0: this->GCInfoSend(Data->Value, aIndex);			break;
		case 0x1: this->BuyProc(Data->Value, aIndex);				break;
		case 0x2: gObjViewportListProtocolCreate(&gObj[aIndex]);	break;
		case 0x3: this->SkinState(Data->Value, aIndex);			break;
		case 0x4:
				  gObjViewportListProtocolCreate(&gObj[aIndex]);
				  gObjClearViewport(&gObj[aIndex]);
		default:
		break;
	}
}

void cThueFlag::GDGetListBuy(int aIndex)
{
	if (gObjIsConnected(aIndex) == false)
	{
		return;
	}
	GSSENDDS_GETLISTISBUYFLAG pMsg;
	pMsg.header.set(0xD9, 0x03, sizeof(pMsg));
	memcpy(pMsg.NameChar, gObj[aIndex].Name, sizeof(pMsg.NameChar));
	pMsg.aIndex = aIndex;
	gDataServerConnection.DataSend((BYTE*)&pMsg, pMsg.header.size);
}
void cThueFlag::GDSaveFlagBuy(int aIndex, int FlagID, int Time)
{

	if (gObjIsConnected(aIndex) == false)
	{
		return;
	}

	GSSENDDS_SAVEBUYFLAG pMsg;
	pMsg.header.set(0xD9, 0x04, sizeof(pMsg));
	memcpy(pMsg.NameChar, gObj[aIndex].Name, sizeof(pMsg.NameChar));
	pMsg.FlagID = FlagID;
	pMsg.Time = Time;
	gDataServerConnection.DataSend((BYTE*)&pMsg, pMsg.header.size);
}
void cThueFlag::RecvGDGetListBuy(DATAFLAG_LOAD_COUNT* lpMsg)
{
	if (gObjIsConnected(lpMsg->aIndex) == false)
	{
		return;
	}
	LPOBJ lpObj = &gObj[lpMsg->aIndex];

	for (int n = 0; n < lpMsg->count; n++)
	{
		BCUSTOM_FLAGBUY_DATA* lpInfo = (BCUSTOM_FLAGBUY_DATA*)(((BYTE*)lpMsg) + sizeof(DATAFLAG_LOAD_COUNT) + (sizeof(BCUSTOM_FLAGBUY_DATA) * n));

		this->UpdateListFlagBuy(lpMsg->aIndex, lpInfo->FlagID, lpInfo->Time);
	}
	this->OnLogin(lpMsg->aIndex);
}
bool cThueFlag::UpdateListFlagBuy(int aIndex, int FlagID, int Time)
{
	if (gObjIsConnected(aIndex) == false)
	{
		return 0;
	}
	LPOBJ lpObj = &gObj[aIndex];
	for (int n = 0; n < MAX_BUYFLAG_CB; n++)
	{
		if (lpObj->mInfoBuyFlag[n].FlagID == -1)
		{
			lpObj->mInfoBuyFlag[n].AddListBuyFlag(FlagID, Time);
			return 1;
		}
	}
	return 0;
}
int cThueFlag::CheckFlagIsBuy(int aIndex, int FlagID)
{
	if (gObjIsConnected(aIndex) == false)
	{
		return 0;
	}

	LPOBJ lpObj = &gObj[aIndex];
	for (int n = 0; n < MAX_BUYFLAG_CB; n++)
	{
		if (lpObj->mInfoBuyFlag[n].IsBuy(FlagID) != -1)
		{
			return lpObj->mInfoBuyFlag[n].IsBuy(FlagID);
		}
	}
	return -1;
}
void cThueFlag::GCInfoSend(int a, int aIndex, bool changeskin)
{
	LPOBJ lpObj = &gObj[aIndex];
	PMSG_THUE_FLAG_SEND rSend;
	rSend.h.set((LPBYTE)&rSend, 0xFB, 0x1A, sizeof(rSend));

	if (!OBJECT_USER_RANGE(aIndex))
	{
		return;
	}

	rSend.Skin = lpObj->m_RankTitle6;

	if (a == -1)
	{
		rSend.Data.Id = -1;
	}
	else
	{

		if (a < 0 || a >= this->m_Data.size())
		{
			rSend.Data.Id = -1;
		}
		else
		{
			THUE_FLAG_DATA* skin = &this->m_Data[a];

			rSend.Data.Id = skin->Id;
			rSend.Data.Skin = skin->Skin; //FlagID
			rSend.Data.Time = skin->Time;
			rSend.Data.Price = skin->Price;
			rSend.Data.Value1 = skin->Value1;
			rSend.Data.Value2 = skin->Value2;
			rSend.Data.Value3 = skin->Value3;
			rSend.Data.Value4 = skin->Value4;
			rSend.Data.Value5 = skin->Value5;
			rSend.Data.Value6 = skin->Value6;
			rSend.Data.Value7 = skin->Value7;
			rSend.Data.Value8 = skin->Value8;
			rSend.Data.Value9 = skin->Value9;
			rSend.Data.Value10 = this->CheckFlagIsBuy(aIndex, skin->Skin); //Status Flag da so huu
		}
	}
	DataSend(aIndex, (LPBYTE)&rSend, sizeof(rSend));
}

void cThueFlag::SkinState(bool state, int aIndex)
{
	LPOBJ lpObj = &gObj[aIndex];

	if (lpObj->m_RankTitle6 == 0)
	{
		return;
	}

	THUE_FLAG_DATA* skin = this->FindSkin(abs(lpObj->m_RankTitle6));

	if (skin == 0)
	{
		return;
	}
	lpObj->m_RankTitle6 *= -1;

	gObjViewportListProtocolCreate(lpObj);
	gObjectManager.CharacterUpdateMapEffect(lpObj);
	gCustomRankUser.GCReqRankLevelUser(lpObj->Index, lpObj->Index);
	this->GCInfoSend(skin->Id, aIndex);
	gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, "Bạn Đã Hủy Sử Dụng Flag");
}

void cThueFlag::BuyProc(int a, int aIndex)
{
	if (this->TypeBonus == 0) return;
	LPOBJ lpObj = &gObj[aIndex];

	if (a < 0 || a >= this->m_Data.size())
	{
		gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, "Thông Tin Không Hợp Lệ");
		return;
	}

	THUE_FLAG_DATA* skin = &this->m_Data[a];

	if (skin->Skin == abs(lpObj->m_RankTitle6))
	{
		gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, "Bạn Đang Sử dụng Flag Này");
		return;
	}

	if (this->CheckFlagIsBuy(aIndex, skin->Skin) != -1)
	{
		gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, "Đổi Flag Thành Công");
		lpObj->m_RankTitle6 = skin->Skin;

		gObjViewportListProtocolCreate(lpObj);
		gObjectManager.CharacterUpdateMapEffect(lpObj);
		gObjectManager.CharacterCalcAttribute(aIndex);
		gCustomRankUser.GCReqRankLevelUser(lpObj->Index, lpObj->Index);
		this->GCInfoSend(skin->Id, aIndex);
	}
	else
	{
		if (skin->Value7 > 0)
		{
			if (gItemManager.GetInventoryItemCount(lpObj, GET_ITEM(skin->Value8, skin->Value9), 0) < skin->Value7)
			{
				gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, "Bạn cần có x%d [%s]", skin->Value7 - gItemManager.GetInventoryItemCount(lpObj, GET_ITEM(skin->Value8, skin->Value9), 0), gItemManager.GetItemName(GET_ITEM(skin->Value8, skin->Value9)));
				return;
			}
		}

		if (lpObj->Coin1 - skin->Price < 0) //check tien te skin->Price;
		{
			gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, "Bạn Không Đủ WC");
			return;
		}

		GDSetCoinSend(lpObj->Index, -skin->Price, 0, 0, 0, 0, "Flag");
		gItemManager.DeleteInventoryItemCount(lpObj, GET_ITEM(skin->Value8, skin->Value9), 0, skin->Value7);

		lpObj->m_RankTitle6 = skin->Skin;

		this->UpdateListFlagBuy(aIndex, skin->Skin, (skin->Time > 0 ? (skin->Time * 60) + time(0) : 0)); //=== them vao list da mua
		this->GDSaveFlagBuy(aIndex, skin->Skin, (skin->Time > 0 ? (skin->Time * 60) + time(0) : 0)); //==save sql
		gObjViewportListProtocolCreate(lpObj);
		gObjectManager.CharacterUpdateMapEffect(lpObj);
		gCustomRankUser.GCReqRankLevelUser(lpObj->Index, lpObj->Index);
		gObjectManager.CharacterCalcAttribute(aIndex);
		this->GCInfoSend(skin->Id, aIndex);
		gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, "Bạn Đã Mua Flag Thành Công");
	}
}

THUE_FLAG_DATA* cThueFlag::FindSkin(int id)
{
	for (int i = 0; i < this->m_Data.size(); i++)
	{
		if (this->m_Data[i].Skin == id)
		{
			return &this->m_Data[i];
		}
	}
	return 0;
}
void cThueFlag::MainProc(int aIndex)
{
	if (this->TypeBonus == 0) return;

	if (!OBJECT_USER_RANGE(aIndex))
	{
		return;
	}
	LPOBJ lpObj = &gObj[aIndex];
	THUE_FLAG_DATA* data = this->FindSkin(abs(lpObj->m_RankTitle6));

	if (!data)return;

	if (this->CheckFlagIsBuy(aIndex, lpObj->m_RankTitle6) == -1 || data == 0)
	{
		//gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, "Flag bạn dùng đã Đã Hết Hạn!!");
		lpObj->m_RankTitle6 = 0;
		gCustomRankUser.GCReqRankLevelUser(lpObj->Index, lpObj->Index);
		gObjectManager.CharacterCalcAttribute(aIndex);
		gObjViewportListProtocolCreate(lpObj);
		gObjectManager.CharacterUpdateMapEffect(lpObj);

		return;
	}

}
void cThueFlag::OnLogin(int aIndex)
{
	if (this->TypeBonus == 0) return;
	LPOBJ lpObj = &gObj[aIndex];

	if (!OBJECT_USER_RANGE(aIndex))
	{
		return;
	}
	lpObj->CheckFlag = 1;
	THUE_FLAG_DATA* data = this->FindSkin(abs(lpObj->m_RankTitle6));
	if (data != 0)
	{
		gObjectManager.CharacterCalcAttribute(aIndex);
		gObjViewportListProtocolCreate(lpObj);
		gObjectManager.CharacterUpdateMapEffect(lpObj);

		return;
	}
}

void cThueFlag::CharacterCalc(int aIndex, bool flag)
{
	if (this->TypeBonus == 0) return;
	if (flag != 0)
	{
		return;
	}

	LPOBJ lpObj = &gObj[aIndex];

	if (!OBJECT_USER_RANGE(aIndex))
	{
		return;
	}

	THUE_FLAG_DATA* data = this->FindSkin(abs(lpObj->m_RankTitle6));
	if (data == 0)
	{
		return;
	}

	if (this->TypeBonus == 1)
	{
		for (std::vector<THUE_FLAG_DATA>::iterator it = ThueFlag.m_Data.begin(); it != ThueFlag.m_Data.end(); it++)
		{
			if (this->CheckFlagIsBuy(aIndex, it->Skin) == -1)
			{
				continue;
			}
			lpObj->PhysiDamageMinLeft += it->Value1;
			lpObj->PhysiDamageMinRight += it->Value1;
			lpObj->PhysiDamageMaxLeft += it->Value1;
			lpObj->PhysiDamageMaxRight += it->Value1;
			lpObj->CurseDamageMin += it->Value1;
			lpObj->CurseDamageMax += it->Value1;
			lpObj->MagicDamageMin += it->Value1;
			lpObj->MagicDamageMax += it->Value1;
			lpObj->Defense += it->Value2;
			lpObj->AddLife += it->Value3;
			lpObj->AddShield += it->Value4;
		}
	}
	else if (this->TypeBonus == 2)
	{
		if (this->CheckFlagIsBuy(aIndex, data->Skin) != -1)
		{
			lpObj->PhysiDamageMinLeft += data->Value1;
			lpObj->PhysiDamageMinRight += data->Value1;
			lpObj->PhysiDamageMaxLeft += data->Value1;
			lpObj->PhysiDamageMaxRight += data->Value1;
			lpObj->CurseDamageMin += data->Value1;
			lpObj->CurseDamageMax += data->Value1;
			lpObj->MagicDamageMin += data->Value1;
			lpObj->MagicDamageMax += data->Value1;
			lpObj->Defense += data->Value2;
			lpObj->AddLife += data->Value3;
			lpObj->AddShield += data->Value4;
		}
	}
}
#endif