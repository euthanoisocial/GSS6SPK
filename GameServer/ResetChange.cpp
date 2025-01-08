#include "stdafx.h"
#include "CashShop.h"
#include "ResetChange.h"
#include "DSProtocol.h"
#include "JSProtocol.h"
#include "Util.h"
#include "MemScript.h"
#include "Message.h"
#include "Notice.h"
#include "ObjectManager.h"
#include "ServerInfo.h"
#include "CustomRankUser.h"
#include "Util.h"
#include "ItemBagManager.h"
#include "SPK_Message.h"

CResetChange gResetChange;

CResetChange::CResetChange() // OK
{
	this->Init();
}

void CResetChange::Init() // OK
{
	for (int n = 0; n < MAX_RESET_CHANGE; n++)
	{
		this->m_CustomResetChangeInfo[n].Index = -1;
	}
}

void CResetChange::Load(char* path) // OK
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

	this->Init();

	try
	{
		while (true)
		{
			if (lpMemScript->GetToken() == TOKEN_END)
			{
				break;
			}
			
			if (strcmp("end", lpMemScript->GetString()) == 0)
			{
				break;
			}
			RESETCHANGE_INFO info;
			memset(&info, 0, sizeof(info));

			info.Index			= lpMemScript->GetNumber();
			info.ReqReset		= lpMemScript->GetAsNumber();
			info.Coin1			= lpMemScript->GetAsNumber();
			info.LevelUpPoint	= lpMemScript->GetAsNumber();
			info.TangQua		= lpMemScript->GetAsNumber();

			this->SetInfo(info);
		}
	}
	catch (...)
	{
		printf(lpMemScript->GetLastError());
	}
	delete lpMemScript;
}

void CResetChange::SetInfo(RESETCHANGE_INFO info) // OK
{
	if (info.Index < 0 || info.Index >= MAX_RESET_CHANGE)
	{
		return;
	}
	this->m_CustomResetChangeInfo[info.Index] = info;
}

RESETCHANGE_INFO* CResetChange::GetInfo(int index) // OK
{
	if (index < 0 || index >= MAX_RESET_CHANGE)
	{
		return 0;
	}

	if (this->m_CustomResetChangeInfo[index].Index != index)
	{
		return 0;
	}

	return &this->m_CustomResetChangeInfo[index];
}

void CResetChange::ResetChange(int aIndex, RESETCHANGE_REQ* lpMsg)
{
	LPOBJ lpObj = &gObj[aIndex];
	if ((GetTickCount() - lpObj->ClickClientSend) < 2000)
	{
		gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, gSPKMessage.GetMessage(10));
		return;
	}
	if (gObjIsConnectedGP(aIndex) == 0)
	{
		return;
	}

	if (lpMsg->numchange < 1 || lpMsg->numchange > 5)
	{
		return;
	}
	
	if (lpObj->Reset < m_CustomResetChangeInfo[lpMsg->numchange - 1].ReqReset)
	{
		this->SoLieuReset = m_CustomResetChangeInfo[lpMsg->numchange - 1].ReqReset - lpObj->Reset;
		gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, gSPKMessage.GetMessage(11),SoLieuReset);
		return;
	}
	
	if (lpObj->LevelUpPoint < m_CustomResetChangeInfo[lpMsg->numchange - 1].LevelUpPoint)
	{
		this->SoLieuPoint = m_CustomResetChangeInfo[lpMsg->numchange - 1].LevelUpPoint - lpObj->LevelUpPoint;
		gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, gSPKMessage.GetMessage(12),SoLieuPoint);
		return;
	}
	
	lpObj->Reset			= (lpObj->Reset - m_CustomResetChangeInfo[lpMsg->numchange - 1].ReqReset);
	lpObj->LevelUpPoint		= (lpObj->LevelUpPoint - m_CustomResetChangeInfo[lpMsg->numchange - 1].LevelUpPoint);
	lpObj->TangQua			= (lpObj->TangQua - m_CustomResetChangeInfo[lpMsg->numchange - 1].TangQua);

	//tue add
	lpObj->Strength = 32;
	lpObj->Dexterity = 32;
	lpObj->Vitality = 32;
	lpObj->Energy = 32;
	//end add


	GDCreateItemSend(lpObj->Index, 0xEB, 0, 0, (m_CustomResetChangeInfo[lpMsg->numchange - 1].TangQua), 0, 1, 0, 0, 0,0, 0, 0, 0, 0, 0, 0, 0);

	GDSetCoinSend(lpObj->Index, +(m_CustomResetChangeInfo[lpMsg->numchange - 1].Coin1), 0, 0, 0, 0, "ResetChange");
	gObjectManager.CharacterCalcAttribute(lpObj->Index);
	
	GCNewCharacterInfoSend(lpObj);
	GDCharacterInfoSaveSend(lpObj->Index);
	GDResetInfoSaveSend(lpObj->Index, 0, 0, 0);
	gCashShop.CGCashShopPointRecv(lpObj->Index);
	if (gServerInfo.m_CustomRankUserType == 0)
	{
		gCustomRankUser.CheckUpdate(lpObj);
	}
	gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, gSPKMessage.GetMessage(13));
	lpObj->ClickClientSend = GetTickCount();
}
