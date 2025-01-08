#include "StdAfx.h"
#include "CustomRanking.h"
#include "GameMain.h"
#include "DSProtocol.h"
#include "user.h"
#include "protocol.h"
#include "Util.h"
#include "Log.h"
#include "ItemManager.h"
cRanking gRanking;

void cRanking::SendDS()
{
	BYTE send[4096];
	GDTop pMsg;
	pMsg.h.set(0x7A, 0x01, 0);
	int size = sizeof(pMsg);
	pMsg.Result = 1;
	pMsg.h.size[0] = SET_NUMBERHB(size);
	pMsg.h.size[1] = SET_NUMBERLB(size);
	memcpy(send, &pMsg, sizeof(pMsg));
	gDataServerConnection.DataSend(send, size);
}
void cRanking::RecvDS(DGCharTop *Recv)
{
	for (int i = 0; i < MAXTOP; i++)
	{
		strncpy(this->RankingChar[i].Name,					  Recv->tp[i].Name, 11);
				this->RankingChar[i].Is_Class				= Recv->tp[i].Is_Class;
				this->RankingChar[i].Is_cLevel				= Recv->tp[i].Is_cLevel;
				this->RankingChar[i].Is_ResetCount			= Recv->tp[i].Is_ResetCount;
				this->RankingChar[i].Is_MasterResetCount	= Recv->tp[i].Is_MasterResetCount;
				this->RankingChar[i].Is_ResetTime			= Recv->tp[i].Is_ResetTime;
				this->RankingChar[i].Is_TotalPoint			= Recv->tp[i].Is_TotalPoint;
				#if MAXTOP <= 40
				this->RankingChar[i].Is_Strength			= Recv->tp[i].Is_Strength;
				this->RankingChar[i].Is_Dexterity			= Recv->tp[i].Is_Dexterity;
				this->RankingChar[i].Is_Vitality			= Recv->tp[i].Is_Vitality;
				this->RankingChar[i].Is_Energy				= Recv->tp[i].Is_Energy;
				this->RankingChar[i].Is_Leadership			= Recv->tp[i].Is_Leadership;
				this->RankingChar[i].Is_Life				= Recv->tp[i].Is_Life;
				this->RankingChar[i].Is_MaxLife				= Recv->tp[i].Is_MaxLife;
				this->RankingChar[i].Is_Mana				= Recv->tp[i].Is_Mana;
				this->RankingChar[i].Is_MaxMana				= Recv->tp[i].Is_MaxMana;
				this->RankingChar[i].Is_BP					= Recv->tp[i].Is_BP;
				this->RankingChar[i].Is_MaxBP				= Recv->tp[i].Is_MaxBP;
				this->RankingChar[i].Is_Shield				= Recv->tp[i].Is_Shield;
				this->RankingChar[i].Is_MaxShield			= Recv->tp[i].Is_MaxShield;
				this->RankingChar[i].Is_LucChien			= Recv->tp[i].Is_LucChien;
				this->RankingChar[i].Is_PkCount				= Recv->tp[i].Is_PkCount;
				this->RankingChar[i].Is_Kills				= Recv->tp[i].Is_Kills;
				this->RankingChar[i].Is_Deads				= Recv->tp[i].Is_Deads;
				this->RankingChar[i].Is_WcoinC				= Recv->tp[i].Is_WcoinC;
				this->RankingChar[i].Is_CoinAtm				= Recv->tp[i].Is_CoinAtm;
				this->RankingChar[i].Is_ConnectStat			= Recv->tp[i].Is_ConnectStat;
				this->RankingChar[i].Is_OnlineHours			= Recv->tp[i].Is_OnlineHours;

				this->RankingChar[i].Is_BC					= Recv->tp[i].Is_BC;
				this->RankingChar[i].Is_CC					= Recv->tp[i].Is_CC;
				this->RankingChar[i].Is_DS					= Recv->tp[i].Is_DS;
				this->RankingChar[i].Is_Win					= Recv->tp[i].Is_Win;
				this->RankingChar[i].Is_Lose				= Recv->tp[i].Is_Lose;
				this->RankingChar[i].Is_Kboss				= Recv->tp[i].Is_Kboss;

		strncpy(this->RankingChar[i].Is_JoinDate,			  Recv->tp[i].Is_JoinDate, 22);
		strncpy(this->RankingChar[i].Is_LastIP,				  Recv->tp[i].Is_LastIP, 16);
				#endif
		strncpy(this->RankingChar[i].Is_SvSub,				  Recv->tp[i].Is_SvSub, 9);
	}
}
void cRanking::SendUser(LPOBJ lpObj)
{
	DGCharTop2	sClient;
	sClient.h.set(0x04, sizeof(sClient));
	for (int i = 0; i<MAXTOP; i++)
	{
		strncpy(sClient.tp[i].Name, this->RankingChar[i].Name, 11);

		sClient.tp[i].Is_Class				= this->RankingChar[i].Is_Class;
		sClient.tp[i].Is_cLevel				= this->RankingChar[i].Is_cLevel;
		sClient.tp[i].Is_ResetCount			= this->RankingChar[i].Is_ResetCount;
		sClient.tp[i].Is_MasterResetCount	= this->RankingChar[i].Is_MasterResetCount;
		sClient.tp[i].Is_ResetTime			= this->RankingChar[i].Is_ResetTime;
		sClient.tp[i].Is_TotalPoint			= this->RankingChar[i].Is_TotalPoint;

		#if MAXTOP <= 40
		sClient.tp[i].Is_Strength			= this->RankingChar[i].Is_Strength;
		sClient.tp[i].Is_Dexterity			= this->RankingChar[i].Is_Dexterity;
		sClient.tp[i].Is_Vitality			= this->RankingChar[i].Is_Vitality;
		sClient.tp[i].Is_Energy				= this->RankingChar[i].Is_Energy;
		sClient.tp[i].Is_Leadership			= this->RankingChar[i].Is_Leadership;
		sClient.tp[i].Is_Life				= this->RankingChar[i].Is_Life;
		sClient.tp[i].Is_MaxLife			= this->RankingChar[i].Is_MaxLife;
		sClient.tp[i].Is_Mana				= this->RankingChar[i].Is_Mana;
		sClient.tp[i].Is_MaxMana			= this->RankingChar[i].Is_MaxMana;
		sClient.tp[i].Is_BP					= this->RankingChar[i].Is_BP;
		sClient.tp[i].Is_MaxBP				= this->RankingChar[i].Is_MaxBP;
		sClient.tp[i].Is_Shield				= this->RankingChar[i].Is_Shield;
		sClient.tp[i].Is_MaxShield			= this->RankingChar[i].Is_MaxShield;
		sClient.tp[i].Is_LucChien			= this->RankingChar[i].Is_LucChien;
		sClient.tp[i].Is_PkCount			= this->RankingChar[i].Is_PkCount;
		sClient.tp[i].Is_Kills				= this->RankingChar[i].Is_Kills;
		sClient.tp[i].Is_Deads				= this->RankingChar[i].Is_Deads;
		sClient.tp[i].Is_WcoinC				= this->RankingChar[i].Is_WcoinC;
		sClient.tp[i].Is_CoinAtm			= this->RankingChar[i].Is_CoinAtm;
		sClient.tp[i].Is_ConnectStat		= this->RankingChar[i].Is_ConnectStat;
		sClient.tp[i].Is_OnlineHours		= this->RankingChar[i].Is_OnlineHours;

		sClient.tp[i].Is_BC					= this->RankingChar[i].Is_BC;
		sClient.tp[i].Is_CC					= this->RankingChar[i].Is_CC;
		sClient.tp[i].Is_DS					= this->RankingChar[i].Is_DS;
		sClient.tp[i].Is_Win				= this->RankingChar[i].Is_Win;
		sClient.tp[i].Is_Lose				= this->RankingChar[i].Is_Lose;
		sClient.tp[i].Is_Kboss				= this->RankingChar[i].Is_Kboss;

		strncpy(sClient.tp[i].Is_JoinDate,	 this->RankingChar[i].Is_JoinDate, 22);
		strncpy(sClient.tp[i].Is_LastIP,	 this->RankingChar[i].Is_LastIP, 16);
		#endif
		strncpy(sClient.tp[i].Is_SvSub,		 this->RankingChar[i].Is_SvSub, 9);
	}
	DataSend(lpObj->Index, (BYTE*)&sClient, sizeof(sClient));
}
//
void cRanking::SendDSTP()
{
	BYTE send[4096];
	GDTopTP pMsg;
	pMsg.h.set(0x7A, 0x02, 0);
	int size = sizeof(pMsg);
	pMsg.Result = 1;
	pMsg.h.size[0] = SET_NUMBERHB(size);
	pMsg.h.size[1] = SET_NUMBERLB(size);
	memcpy(send, &pMsg, sizeof(pMsg));
	gDataServerConnection.DataSend(send, size);
}
void cRanking::RecvDSTP(DGCharTopTP* Recv)
{
	for (int i = 0; i < MAXTOP; i++)
	{
		strncpy(this->RankingCharTP[i].Name, Recv->tp[i].Name, 11);
		this->RankingCharTP[i].Is_WcoinC = Recv->tp[i].Is_WcoinC;
		this->RankingCharTP[i].Is_WcoinP = Recv->tp[i].Is_WcoinP;
		this->RankingCharTP[i].Is_WcoinG = Recv->tp[i].Is_WcoinG;
		this->RankingCharTP[i].Is_WcoinR = Recv->tp[i].Is_WcoinR;
		this->RankingCharTP[i].Is_CoinAtm = Recv->tp[i].Is_CoinAtm;
	}
}
void cRanking::SendUserTP(LPOBJ lpObj)
{
	DGCharTop2TP	sClient;
	sClient.h.set(0x05, sizeof(sClient));
	for (int i = 0; i < MAXTOP; i++)
	{
		strncpy(sClient.tp[i].Name, this->RankingCharTP[i].Name, 11);
		sClient.tp[i].Is_WcoinC = this->RankingCharTP[i].Is_WcoinC;
		sClient.tp[i].Is_WcoinP = this->RankingCharTP[i].Is_WcoinP;
		sClient.tp[i].Is_WcoinG = this->RankingCharTP[i].Is_WcoinG;
		sClient.tp[i].Is_WcoinR = this->RankingCharTP[i].Is_WcoinR;
		sClient.tp[i].Is_CoinAtm = this->RankingCharTP[i].Is_CoinAtm;
	}
	DataSend(lpObj->Index, (BYTE*)&sClient, sizeof(sClient));
}

void cRanking::SendDSET()
{
	BYTE send[4096];
	GDTopET pMsg;
	pMsg.h.set(0x7A, 0x03, 0);
	int size = sizeof(pMsg);
	pMsg.Result = 1;
	pMsg.h.size[0] = SET_NUMBERHB(size);
	pMsg.h.size[1] = SET_NUMBERLB(size);
	memcpy(send, &pMsg, sizeof(pMsg));
	gDataServerConnection.DataSend(send, size);
}
void cRanking::RecvDSET(DGCharTopET* Recv)
{
	for (int i = 0; i < MAXTOP; i++)
	{
		strncpy(this->RankingCharET[i].Name, Recv->tp[i].Name, 11);
		this->RankingCharET[i].Is_BloodCS = Recv->tp[i].Is_BloodCS;
		this->RankingCharET[i].Is_ChaosCS = Recv->tp[i].Is_ChaosCS;
		this->RankingCharET[i].Is_DevilCS = Recv->tp[i].Is_DevilCS;
	}
}
void cRanking::SendUserET(LPOBJ lpObj)
{
	DGCharTop2ET	sClient;
	sClient.h.set(0x06, sizeof(sClient));
	for (int i = 0; i < MAXTOP; i++)
	{
		strncpy(sClient.tp[i].Name, this->RankingCharET[i].Name, 11);
		sClient.tp[i].Is_BloodCS = this->RankingCharET[i].Is_BloodCS;
		sClient.tp[i].Is_ChaosCS = this->RankingCharET[i].Is_ChaosCS;
		sClient.tp[i].Is_DevilCS = this->RankingCharET[i].Is_DevilCS;
	}
	DataSend(lpObj->Index, (BYTE*)&sClient, sizeof(sClient));
}

void cRanking::SendDSTG()
{
	BYTE send[4096];
	GDTopTG pMsg;
	pMsg.h.set(0x7A, 0x04, 0);
	int size = sizeof(pMsg);
	pMsg.Result = 1;
	pMsg.h.size[0] = SET_NUMBERHB(size);
	pMsg.h.size[1] = SET_NUMBERLB(size);
	memcpy(send, &pMsg, sizeof(pMsg));
	gDataServerConnection.DataSend(send, size);
}
void cRanking::RecvDSTG(DGCharTopTG* Recv)
{
	for (int i = 0; i < MAXTOP; i++)
	{
		strncpy(this->RankingCharTG[i].G_Name, Recv->tp[i].G_Name, 8);
		strncpy(this->RankingCharTG[i].G_CName, Recv->tp[i].G_CName, 11);
		this->RankingCharTG[i].Is_GLevel = Recv->tp[i].Is_GLevel;
		this->RankingCharTG[i].Is_GScore = Recv->tp[i].Is_GScore;
		this->RankingCharTG[i].Is_GMem = Recv->tp[i].Is_GMem;
		strncpy(this->RankingCharTG[i].Is_JoinDate, Recv->tp[i].Is_JoinDate, 22);
	}
}
void cRanking::SendUserTG(LPOBJ lpObj)
{
	DGCharTop2TG	sClient;
	sClient.h.set(0x07, sizeof(sClient));
	for (int i = 0; i < MAXTOP; i++)
	{
		strncpy(sClient.tp[i].G_Name, this->RankingCharTG[i].G_Name, 9);
		strncpy(sClient.tp[i].G_CName, this->RankingCharTG[i].G_CName, 11);
		sClient.tp[i].Is_GLevel = this->RankingCharTG[i].Is_GLevel;
		sClient.tp[i].Is_GScore = this->RankingCharTG[i].Is_GScore;
		sClient.tp[i].Is_GMem = this->RankingCharTG[i].Is_GMem;
		strncpy(sClient.tp[i].Is_JoinDate, this->RankingCharTG[i].Is_JoinDate, 22);
	}
	DataSend(lpObj->Index, (BYTE*)&sClient, sizeof(sClient));
}

void cRanking::SendDSPK()
{
	BYTE send[4096];
	GDTopPK pMsg;
	pMsg.h.set(0x7A, 0x05, 0);
	int size = sizeof(pMsg);
	pMsg.Result = 1;
	pMsg.h.size[0] = SET_NUMBERHB(size);
	pMsg.h.size[1] = SET_NUMBERLB(size);
	memcpy(send, &pMsg, sizeof(pMsg));
	gDataServerConnection.DataSend(send, size);
}
void cRanking::RecvDSPK(DGCharTopPK* Recv)
{
	for (int i = 0; i < MAXTOP; i++)
	{
		strncpy(this->RankingCharPK[i].Name, Recv->tp[i].Name, 11);
		this->RankingCharPK[i].Is_Kills = Recv->tp[i].Is_Kills;
		this->RankingCharPK[i].Is_Deads = Recv->tp[i].Is_Deads;
		this->RankingCharPK[i].Is_OnlineHours = Recv->tp[i].Is_OnlineHours;
		this->RankingCharPK[i].Is_Win = Recv->tp[i].Is_Win;
		this->RankingCharPK[i].Is_Lose = Recv->tp[i].Is_Lose;
		this->RankingCharPK[i].Is_Kboss = Recv->tp[i].Is_Kboss;
		strncpy(this->RankingCharPK[i].Is_JoinDate, Recv->tp[i].Is_JoinDate, 22);
	}
}
void cRanking::SendUserPK(LPOBJ lpObj)
{
	DGCharTop2PK	sClient;
	sClient.h.set(0x08, sizeof(sClient));
	for (int i = 0; i < MAXTOP; i++)
	{
		strncpy(sClient.tp[i].Name, this->RankingCharPK[i].Name, 11);
		sClient.tp[i].Is_Kills = this->RankingCharPK[i].Is_Kills;
		sClient.tp[i].Is_Deads = this->RankingCharPK[i].Is_Deads;
		sClient.tp[i].Is_OnlineHours = this->RankingCharPK[i].Is_OnlineHours;
		sClient.tp[i].Is_Win = this->RankingCharPK[i].Is_Win;
		sClient.tp[i].Is_Lose = this->RankingCharPK[i].Is_Lose;
		sClient.tp[i].Is_Kboss = this->RankingCharPK[i].Is_Kboss;
		strncpy(sClient.tp[i].Is_JoinDate, this->RankingCharPK[i].Is_JoinDate, 22);
	}
	DataSend(lpObj->Index, (BYTE*)&sClient, sizeof(sClient));
}

void cRanking::SendDSMT()
{
	BYTE send[4096];
	GDTopMT pMsg;
	pMsg.h.set(0x7A, 0x06, 0);
	int size = sizeof(pMsg);
	pMsg.Result = 1;
	pMsg.h.size[0] = SET_NUMBERHB(size);
	pMsg.h.size[1] = SET_NUMBERLB(size);
	memcpy(send, &pMsg, sizeof(pMsg));
	gDataServerConnection.DataSend(send, size);
}
void cRanking::RecvDSMT(DGCharTopMT* Recv)
{
	for (int i = 0; i < MAXTOP; i++)
	{
		strncpy(this->RankingCharMT[i].Name, Recv->tp[i].Name, 11);
		this->RankingCharMT[i].cLevel = Recv->tp[i].cLevel;
		this->RankingCharMT[i].cPoint = Recv->tp[i].cPoint;
		this->RankingCharMT[i].cExp = Recv->tp[i].cExp;
	}
}
void cRanking::SendUserMT(LPOBJ lpObj)
{
	DGCharTop2MT	sClient;
	sClient.h.set(0x09, sizeof(sClient));
	for (int i = 0; i < MAXTOP; i++)
	{
		strncpy(sClient.tp[i].Name, this->RankingCharMT[i].Name, 11);
		sClient.tp[i].cLevel = this->RankingCharMT[i].cLevel;
		sClient.tp[i].cPoint = this->RankingCharMT[i].cPoint;
		sClient.tp[i].cExp = this->RankingCharMT[i].cExp;
	}
	DataSend(lpObj->Index, (BYTE*)&sClient, sizeof(sClient));
}

void cRanking::CGetInfoCharTop(REQUESTINFO_CHARTOP* lpMsg, int aIndex)
{
	if (lpMsg == NULL)
	{
		return;
	}
	if (gObjIsConnected(aIndex) == false)
	{
		return;
	}
	if (GetTickCount() < gObj[aIndex].ClickClientSend + 1000)
	{
		return;
	}
	gObj[aIndex].ClickClientSend = GetTickCount();
	REQUESTINFO_CHARTOP pMsg;
	pMsg.header.set(0xD3, 0x40, sizeof(pMsg));
	pMsg.aIndex = aIndex;
	memcpy(pMsg.NameChar, lpMsg->NameChar, sizeof(pMsg.NameChar) - 1);
	gDataServerConnection.DataSend((BYTE*)&pMsg, pMsg.header.size);
}
void cRanking::RecvInfoCharTop(DATA_VIEWTOPRANKING* lpMsg)
{
	if (lpMsg == NULL)
	{
		return;
	}
	int aIndex = lpMsg->aIndex;

	if (gObjIsConnected(aIndex) == false)
	{
		return;
	}
	BYTE send[4096];

	DATA_VIEWTOPRANKING pMsg;

	pMsg.header.set(0xD3, 0x40, 0);

	int size = sizeof(pMsg);

	memcpy(pMsg.NameChar, lpMsg->NameChar, sizeof(pMsg.NameChar));
	pMsg.aIndex = lpMsg->aIndex;
	CItem lpItemTest[12];

	for (int n = 0; n < 12; n++)
	{
		lpItemTest[n].Clear();
		gItemManager.ConvertItemByte(&lpItemTest[n], lpMsg->Item[n]);
		if (lpItemTest[n].IsItem())
		{
			gItemManager.ItemByteConvert(pMsg.Item[n], lpItemTest[n]);
		}
		else
		{
			memset(&pMsg.Item[n], 0xFF, sizeof(pMsg.Item[n]));
		}
	}
	memcpy(pMsg.GuildName, lpMsg->GuildName, sizeof(pMsg.GuildName));
	memcpy(pMsg.GuildMark, lpMsg->GuildMark, sizeof(pMsg.GuildMark));
	pMsg.header.size[0] = SET_NUMBERHB(size);
	pMsg.header.size[1] = SET_NUMBERLB(size);
	memcpy(send, &pMsg, sizeof(pMsg));
	DataSend(aIndex, send, size);
}