#include "stdafx.h"
#include "DSProtocol.h"
#include "Util.h"
#include "Message.h"
#include "User.h"
#include "Path.h"
#include "ServerInfo.h"
#include "MemScript.h"
#include "CustomRankUser.h"
#include "Notice.h"
#include "ResetChange.h"

CCustomRankUser gCustomRankUser;

void CCustomRankUser::Load(char* path){

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

	for (int n = 0; n < MAX_RANK_USER; n++)
	{
		this->m_Data[n].Clear();
	}

	this->m_count = 0;

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

					int index = lpMemScript->GetNumber();

					strcpy_s(this->m_Data[index].m_Name, lpMemScript->GetAsString());
					this->m_Data[index].m_iResetMin = lpMemScript->GetAsNumber();
					this->m_Data[index].m_iResetMax = lpMemScript->GetAsNumber();
					this->m_Data[index].m_Coin1 = lpMemScript->GetAsNumber();
					this->m_Data[index].m_Coin2 = lpMemScript->GetAsNumber();
					this->m_Data[index].m_Coin3 = lpMemScript->GetAsNumber();

					this->m_count++;
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

void CCustomRankUser::GCReqRankLevelUser(int aIndex, int aTargetIndex)
{
	if (gServerInfo.m_CustomRankUserSwitch == 0)
	{
		return;
	}
	//tue add
	if (gObj[aTargetIndex].Type != OBJECT_USER && gObj[aTargetIndex].Type != OBJECT_BOTS || gObj[aIndex].Type != OBJECT_USER && gObj[aIndex].Type != OBJECT_BOTS) {
		return;
	}
	//-----
	if (gObjIsConnected(aIndex) == false || gObj[aTargetIndex].IsBot >= 1)
	{
		return;
	}
	PMSG_CUSTOM_RANKUSER pMsg = { 0 };
	pMsg.h.set(0xF3, 0xE5, sizeof(pMsg));
	int iRankIndex = this->GetRankIndex(aTargetIndex);
	pMsg.iIndex = aTargetIndex;
	pMsg.iLevel = gObj[aTargetIndex].Level;
	#if DANH_HIEU_NEW
	pMsg.m_RankTitle1 = gObj[aTargetIndex].rDanhHieu;
	#endif
	#if QUAN_HAM_NEW
	pMsg.m_RankTitle2 = gObj[aTargetIndex].rQuanHam;
	#endif
	#if TU_LUYEN_NEW
	pMsg.m_RankTitle3 = gObj[aTargetIndex].rTuLuyen;
	#endif
	#if NEW_VIP_NEW
	pMsg.m_RankTitle4 = gObj[aTargetIndex].rNewVip;
	#endif
	#if(B_HON_HOAN)
	pMsg.m_RankTitle5 = gObj[aTargetIndex].m_RankTitle5;
	#endif
	#if(HT_THUE_FLAG)
	pMsg.m_RankTitle6 = gObj[aTargetIndex].m_RankTitle6;
	#endif
	#if(BAN_RS)
	for (int i = 0; i < MAX_NUM_RSCHANGE; i++)
	{
		pMsg.ReqResetChange[i]= gResetChange.m_CustomResetChangeInfo[i].ReqReset;
		pMsg.ReqResetCoin[i] = gResetChange.m_CustomResetChangeInfo[i].Coin1;
		pMsg.ReqResetUpPoint[i] = gResetChange.m_CustomResetChangeInfo[i].LevelUpPoint;
	}
	#endif
	pMsg.iVipIcon = gObj[aTargetIndex].AccountLevel;
	// tue add ---
	OBJECTSTRUCT * gObj2 = &gObj[aTargetIndex];
	//pMsg.iReset = gObj[aTargetIndex].Reset;
	pMsg.iReset = this->GetUserCRankIndex(aTargetIndex);
	pMsg.iMReset = gObj2->MasterReset;
	pMsg.iKills = gObj2->Kills;
	pMsg.iDeads = gObj2->Deads;
	//pMsg.iLevel = gObj[aTargetIndex].Level;
	//pMsg.iIndex = aTargetIndex;
	pMsg.iFor = gObj2->Strength;
	pMsg.iAgi = gObj2->Dexterity;
	pMsg.iEne = gObj2->Energy;
	pMsg.iVit = gObj2->Vitality;
	pMsg.iCom = gObj2->Leadership;
	pMsg.iRank = iRankIndex;
	pMsg.iClasse = gObj2->DBClass;
	// ---
	pMsg.iReset = gObj[aTargetIndex].Reset;
	pMsg.iMReset = gObj[aTargetIndex].MasterReset;
	pMsg.iKills = gObj[aTargetIndex].Kills;
	pMsg.iDeads = gObj[aTargetIndex].Deads;
	pMsg.iLevel = gObj[aTargetIndex].Level;
	pMsg.iIndex = aTargetIndex;
	pMsg.iFor = gObj[aTargetIndex].Strength;
	pMsg.iAgi = gObj[aTargetIndex].Dexterity;
	pMsg.iEne = gObj[aTargetIndex].Energy;
	pMsg.iVit = gObj[aTargetIndex].Vitality;
	pMsg.iCom = gObj[aTargetIndex].Leadership;
	pMsg.iRank = iRankIndex;
	//pMsg.iClasse = gObj[aTargetIndex].DBClass;
	pMsg.iClasse = gObj[aTargetIndex].Class;
	
	//memcpy(pMsg.szName, this->m_Data[iRankIndex].m_Name, sizeof(pMsg.szName));
	//memcpy(pMsg.iVip, gMessage.GetMessage(32 + gObj[aTargetIndex].AccountLevel), sizeof(pMsg.iVip));
	// end add ---
	//test
	
	//end test
	memcpy(pMsg.szName, this->m_Data[iRankIndex].m_Name, sizeof(pMsg.szName));
	memcpy(pMsg.iVip, gMessage.GetMessage(32 + gObj[aTargetIndex].AccountLevel), sizeof(pMsg.iVip));

	memcpy(pMsg.szName, this->m_Data[iRankIndex].m_Name, sizeof(pMsg.szName));
	
	// ---
	DataSend(aIndex, (LPBYTE)&pMsg, pMsg.h.size);
	DataSend(aTargetIndex, (LPBYTE)&pMsg, pMsg.h.size);
} 


int CCustomRankUser::GetRankIndex(int aIndex)
{
	int Valor;

	if (gServerInfo.m_CustomRankUserType == 1)
	{
		Valor = gObj[aIndex].MasterReset;
	}
	else if (gServerInfo.m_CustomRankUserType == 2)
	{
		Valor = (gObj[aIndex].Level + gObj[aIndex].MasterLevel);
	}
	else
	{
		Valor = gObj[aIndex].Reset;
	}

	for (int i = 0; i < this->m_count; i++)
	{
		if (Valor >= this->m_Data[i].m_iResetMin && Valor <= this->m_Data[i].m_iResetMax ||
			Valor >= this->m_Data[i].m_iResetMin && this->m_Data[i].m_iResetMax == -1)
		{
			return i;
		}
	}
	return -1;
}

int CCustomRankUser::GetUserCRankIndex(int aIndex)
{
	int Valor;
		Valor = gObj[aIndex].Reset;
	
	return Valor;
}

void CCustomRankUser::CheckUpdate(LPOBJ lpObj)
{
#if (GAMESERVER_CLIENTE_UPDATE >= 4)

	if (gServerInfo.m_CustomRankUserRewardSwitch == 0)
	{
		return;
	}

	int Valor;

	if (gServerInfo.m_CustomRankUserType == 1)
	{
		Valor = lpObj->MasterReset;
	}
	else if (gServerInfo.m_CustomRankUserType == 2)
	{
		Valor = lpObj->Level + lpObj->MasterLevel;
	}
	else
	{
		Valor = lpObj->Reset;
	}

	for (int i = 0; i < this->m_count; i++)
	{
		if (this->m_Data[i].m_iResetMin == Valor)
		{
			if (gServerInfo.m_CustomRankUserNoticeToAll == 1)
			{
				gNotice.GCNoticeSendToAll(0, 0, 0, 0, 0, 0, gMessage.GetMessage(738), lpObj->Name, this->m_Data[i].m_Name);
			}
			if (gServerInfo.m_CustomRankUserNoticeToUser == 1)
			{
				gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, gMessage.GetMessage(739), this->m_Data[i].m_Name);
			}
			GDSetCoinSend(lpObj->Index, this->m_Data[i].m_Coin1, this->m_Data[i].m_Coin2, this->m_Data[i].m_Coin3, 0, 0, "RankUser");
			this->GCReqRankLevelUser(lpObj->Index, lpObj->Index);

			return;
		}
	}
	return;

#endif
}