// CustomMonster.cpp: implementation of the CCustomMonster class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CustomMonster.h"
#include "DSProtocol.h"
#include "MemScript.h"
#include "Message.h"
#include "Monster.h"
#include "Notice.h"
#include "Util.h"
#include "CashShop.h"
#include "GameMain.h"

CCustomMonster gCustomMonster;

CCustomMonster::CCustomMonster() // OK
{
	this->m_CustomMonsterInfo.clear();
}

void CCustomMonster::Load(char* path) // OK
{
	CMemScript* lpMemScript = new CMemScript;

	if(lpMemScript == 0)
	{
		ErrorMessageBox(MEM_SCRIPT_ALLOC_ERROR,path);
		return;
	}

	if(lpMemScript->SetBuffer(path) == 0)
	{
		ErrorMessageBox(lpMemScript->GetLastError());
		delete lpMemScript;
		return;
	}

	this->m_CustomMonsterInfo.clear();

	try
	{
		while(true)
		{
			if(lpMemScript->GetToken() == TOKEN_END)
			{
				break;
			}

			if(strcmp("end",lpMemScript->GetString()) == 0)
			{
				break;
			}

			CUSTOM_MONSTER_INFO info;

			info.Index = lpMemScript->GetNumber();

			info.MapNumber = lpMemScript->GetAsNumber();

			info.MaxLife = lpMemScript->GetAsNumber();

			info.DamageMin = lpMemScript->GetAsNumber();

			info.DamageMax = lpMemScript->GetAsNumber();

			info.Defense = lpMemScript->GetAsNumber();

			info.AttackRate = lpMemScript->GetAsNumber();

			info.DefenseRate = lpMemScript->GetAsNumber();

			info.ExperienceRate = lpMemScript->GetAsNumber();

			info.KillMessage = lpMemScript->GetAsNumber();

			info.InfoMessage = lpMemScript->GetAsNumber();

			info.RewardValue1 = lpMemScript->GetAsNumber();

			info.RewardValue2 = lpMemScript->GetAsNumber();

			info.RewardValue3 = lpMemScript->GetAsNumber();

			info.RewardValue4 = lpMemScript->GetAsNumber();

			info.SummonMonster = lpMemScript->GetAsNumber();

			info.SummonMonsterCount = lpMemScript->GetAsNumber();

			info.DrawClient = lpMemScript->GetAsNumber();

			info.KillCountBoss = lpMemScript->GetAsNumber();

			this->m_CustomMonsterInfo.push_back(info);
		}
	}
	catch(...)
	{
		ErrorMessageBox(lpMemScript->GetLastError());
	}

	delete lpMemScript;
}

void CCustomMonster::SetCustomMonsterInfo(LPOBJ lpObj) // OK
{
	CUSTOM_MONSTER_INFO CustomMonsterInfo;

	if(this->GetCustomMonsterInfo(lpObj->Class,lpObj->Map,&CustomMonsterInfo) == 0)
	{
		return;
	}

	lpObj->Life = (float)((__int64)lpObj->Life*((CustomMonsterInfo.MaxLife==-1)?100:CustomMonsterInfo.MaxLife))/100;

	lpObj->MaxLife = (float)((__int64)lpObj->MaxLife*((CustomMonsterInfo.MaxLife==-1)?100:CustomMonsterInfo.MaxLife))/100;

	lpObj->ScriptMaxLife = (float)((__int64)lpObj->ScriptMaxLife*((CustomMonsterInfo.MaxLife==-1)?100:CustomMonsterInfo.MaxLife))/100;

	lpObj->PhysiDamageMin = ((__int64)lpObj->PhysiDamageMin*((CustomMonsterInfo.DamageMin==-1)?100:CustomMonsterInfo.DamageMin))/100;

	lpObj->PhysiDamageMax = ((__int64)lpObj->PhysiDamageMax*((CustomMonsterInfo.DamageMax==-1)?100:CustomMonsterInfo.DamageMax))/100;

	lpObj->Defense = ((__int64)lpObj->Defense*((CustomMonsterInfo.Defense==-1)?100:CustomMonsterInfo.Defense))/100;

	lpObj->AttackSuccessRate = ((__int64)lpObj->AttackSuccessRate*((CustomMonsterInfo.AttackRate==-1)?100:CustomMonsterInfo.AttackRate))/100;

	lpObj->DefenseSuccessRate = ((__int64)lpObj->DefenseSuccessRate*((CustomMonsterInfo.DefenseRate==-1)?100:CustomMonsterInfo.DefenseRate))/100;
}

void CCustomMonster::MonsterDieProc(LPOBJ lpObj,LPOBJ lpTarget) // OK
{
	CUSTOM_MONSTER_INFO CustomMonsterInfo;

	if(this->GetCustomMonsterInfo(lpObj->Class,lpObj->Map,&CustomMonsterInfo) == 0)
	{
		return;
	}

	int aIndex = gObjMonsterGetTopHitDamageUser(lpObj);

	if(OBJECT_RANGE(aIndex) != 0)
	{
		lpTarget = &gObj[aIndex];
	}

	if(CustomMonsterInfo.KillMessage != -1)
	{
		gNotice.GCNoticeSendToAll(0,0,0,0,0,0,gMessage.GetMessage(CustomMonsterInfo.KillMessage),lpTarget->Name);
	}

	if (CustomMonsterInfo.InfoMessage != -1)
	{
		if (CustomMonsterInfo.RewardValue1 > 0)
		{
			gNotice.GCNoticeSend(lpTarget->Index, 1, 0, 0, 0, 0, 0, "Bạn nhận được %d WCoiC", CustomMonsterInfo.RewardValue1);
		}
		if (CustomMonsterInfo.RewardValue2 > 0)
		{
			gNotice.GCNoticeSend(lpTarget->Index, 1, 0, 0, 0, 0, 0, "Bạn nhận được %d WCoiP", CustomMonsterInfo.RewardValue2);
		}
		if (CustomMonsterInfo.RewardValue3 > 0)
		{
			gNotice.GCNoticeSend(lpTarget->Index, 1, 0, 0, 0, 0, 0, "Bạn nhận được %d PCPoin", CustomMonsterInfo.RewardValue3);
		}
		if (CustomMonsterInfo.RewardValue4 > 0)
		{
			gNotice.GCNoticeSend(lpTarget->Index, 1, 0, 0, 0, 0, 0, "Bạn nhận được %d Ruud", CustomMonsterInfo.RewardValue4);
		}
	}
	if(CustomMonsterInfo.RewardValue1 != -1 && CustomMonsterInfo.RewardValue2 != -1 && CustomMonsterInfo.RewardValue3 != -1 && CustomMonsterInfo.RewardValue4 != -1)
	{
		GDCustomMonsterRewardSaveSend(lpTarget->Index,lpObj->Class,lpObj->Map,CustomMonsterInfo.RewardValue1, CustomMonsterInfo.RewardValue2, CustomMonsterInfo.RewardValue3,CustomMonsterInfo.RewardValue4);
		//-- Ruud Update
		gCashShop.CGCashShopPointRecv(lpTarget->Index);
	}

	if(CustomMonsterInfo.SummonMonster > 0)
	{
		int qtd = (CustomMonsterInfo.SummonMonsterCount > 0) ? CustomMonsterInfo.SummonMonsterCount : 1;

		for(int n=0;n < qtd;n++)
		{
			int index = gObjAddMonster(lpObj->Map);

			if(OBJECT_RANGE(index) == 0)
			{
				return;
			}

			LPOBJ lpMonster = &gObj[index];

			int px = lpObj->X;
			int py = lpObj->Y;

			if(gObjGetRandomFreeLocation(lpObj->Map,&px,&py,3,3,50) == 0)
			{
				return;
			}

			lpMonster->PosNum = -1;
			lpMonster->X = px;
			lpMonster->Y = py;
			lpMonster->TX = px;
			lpMonster->TY = py;
			lpMonster->OldX = px;
			lpMonster->OldY = py;
			lpMonster->StartX = px;
			lpMonster->StartY = py;
			lpMonster->Dir = 1;
			lpMonster->Map = lpObj->Map;
			lpMonster->MonsterDeleteTime = GetTickCount()+1800000;

			if(gObjSetMonster(index,CustomMonsterInfo.SummonMonster) == 0)
			{
				gObjDel(index);
			}
		}
	}
}

long CCustomMonster::GetCustomMonsterExperienceRate(int index,int map) // OK
{
	CUSTOM_MONSTER_INFO CustomMonsterInfo;

	if(this->GetCustomMonsterInfo(index,map,&CustomMonsterInfo) == 0)
	{
		return 100;
	}
	else
	{
		return ((CustomMonsterInfo.ExperienceRate==-1)?100:CustomMonsterInfo.ExperienceRate);
	}
}

long CCustomMonster::GetCustomMonsterMasterExperienceRate(int index,int map) // OK
{
	CUSTOM_MONSTER_INFO CustomMonsterInfo;

	if(this->GetCustomMonsterInfo(index,map,&CustomMonsterInfo) == 0)
	{
		return 100;
	}
	else
	{
		return ((CustomMonsterInfo.ExperienceRate==-1)?100:CustomMonsterInfo.ExperienceRate);
	}
}

bool CCustomMonster::GetCustomMonsterInfo(int index,int map,CUSTOM_MONSTER_INFO* lpInfo) // OK
{
	for(std::vector<CUSTOM_MONSTER_INFO>::iterator it=this->m_CustomMonsterInfo.begin();it != this->m_CustomMonsterInfo.end();it++)
	{
		if(it->Index == index && (it->MapNumber == -1 || it->MapNumber == map))
		{
			(*lpInfo) = (*it);
			return 1;
		}
	}

	return 0;
}
bool CCustomMonster::GetCustomMonsterDrawBXH(int index,int map,CUSTOM_MONSTER_INFO* lpInfo) // OK
{
	for(std::vector<CUSTOM_MONSTER_INFO>::iterator it=this->m_CustomMonsterInfo.begin();it != this->m_CustomMonsterInfo.end();it++)
	{
		if(it->Index == index && (it->MapNumber == -1 || it->MapNumber == map ) && it->DrawClient == 1)
		{

			(*lpInfo) = (*it);
			return 1;
		}
	}

	return 0;
}

void CCustomMonster::RewardKill(LPOBJ lpObj, LPOBJ lpTarget)
{
	for (std::vector<CUSTOM_MONSTER_INFO>::iterator it = this->m_CustomMonsterInfo.begin(); it != this->m_CustomMonsterInfo.end(); it++)
	{
		if (it->Index == lpObj->Class)
		{
			this->GDRankingBosSaveSend(lpTarget->Index, it->KillCountBoss);
		}
	}
}
struct SDHP_RANKING_BOS_SAVE_SEND
{
	PBMSG_HEAD header; // C1:3D
	WORD index;
	char account[11];
	char name[11];
	DWORD score;
};
void CCustomMonster::GDRankingBosSaveSend(int aIndex, DWORD Score)
{
	LPOBJ lpObj = &gObj[aIndex];
	SDHP_RANKING_BOS_SAVE_SEND pMsg;
	pMsg.header.set(0x3B, sizeof(pMsg));
	pMsg.index = aIndex;
	memcpy(pMsg.account, lpObj->Account, sizeof(pMsg.account));
	memcpy(pMsg.name, lpObj->Name, sizeof(pMsg.name));
	pMsg.score = Score;
	gDataServerConnection.DataSend((BYTE*)&pMsg, pMsg.header.size);
}