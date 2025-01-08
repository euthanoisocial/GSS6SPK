#pragma once
#include "stdafx.h"
#include "protocol.h"

#define MAXTOP 100
struct GDTop
{
	PSWMSG_HEAD h;
	BYTE Result;
};

struct CharTop
{
	char Name[11];
	BYTE Is_Class;
	int Is_cLevel;
	int Is_ResetCount;
	int Is_MasterResetCount;
	int Is_ResetTime;
	int Is_TotalPoint;

	#if MAXTOP <= 40
	int Is_Strength;
	int Is_Dexterity;
	int Is_Vitality;
	int Is_Energy;
	int Is_Leadership;
	int Is_Life;
	int Is_MaxLife;
	int Is_Mana;
	int Is_MaxMana;
	int Is_BP;
	int Is_MaxBP;
	int Is_Shield;
	int Is_MaxShield;
	int Is_LucChien;
	int Is_PkCount;
	int Is_Kills;
	int Is_Deads;
	int Is_WcoinC;
	int Is_CoinAtm;
	int Is_ConnectStat;
	int Is_OnlineHours;
	int Is_BC;
	int Is_CC;
	int Is_DS;
	int Is_Win;
	int Is_Lose;
	int Is_Kboss;
	char Is_JoinDate[22];
	char Is_LastIP[16];
	#endif
	char Is_SvSub[11];
};

struct DGCharTop
{
	PSWMSG_HEAD h;
	CharTop tp[MAXTOP];
};

struct DGCharTop2
{
	PWMSG_HEAD h;
	CharTop	tp[MAXTOP];
};
//
struct GDTopTP
{
	PSWMSG_HEAD h;
	BYTE Result;
};

struct CharTopTP
{
	char Name[11];
	int Is_WcoinC;
	int Is_WcoinP;
	int Is_WcoinG;
	int Is_WcoinR;
	int Is_CoinAtm;
};

struct DGCharTopTP
{
	PSWMSG_HEAD h;
	CharTopTP tp[MAXTOP];
};

struct DGCharTop2TP
{
	PWMSG_HEAD h;
	CharTopTP	tp[MAXTOP];
};
//
struct GDTopET
{
	PSWMSG_HEAD h;
	BYTE Result;
};

struct CharTopET
{
	char Name[11];
	int Is_BloodCS;
	int Is_ChaosCS;
	int Is_DevilCS;
};

struct DGCharTopET
{
	PSWMSG_HEAD h;
	CharTopET tp[MAXTOP];
};

struct DGCharTop2ET
{
	PWMSG_HEAD h;
	CharTopET	tp[MAXTOP];
};
//
struct GDTopTG
{
	PSWMSG_HEAD h;
	BYTE Result;
};

struct CharTopTG
{
	char G_Name[8];
	char G_CName[11];
	int Is_GLevel;
	int Is_GScore;
	int Is_GMem;
	char Is_JoinDate[22];
};

struct DGCharTopTG
{
	PSWMSG_HEAD h;
	CharTopTG tp[MAXTOP];
};

struct DGCharTop2TG
{
	PWMSG_HEAD h;
	CharTopTG	tp[MAXTOP];
};

//
struct GDTopPK
{
	PSWMSG_HEAD h;
	BYTE Result;
};

struct CharTopPK
{
	char Name[11];
	int Is_Kills;
	int Is_Deads;
	int Is_OnlineHours;
	int Is_Win;
	int Is_Lose;
	int Is_Kboss;
	char Is_JoinDate[22];
};

struct DGCharTopPK
{
	PSWMSG_HEAD h;
	CharTopPK tp[MAXTOP];
};

struct DGCharTop2PK
{
	PWMSG_HEAD h;
	CharTopPK	tp[MAXTOP];
};

struct GDTopMT
{
	PSWMSG_HEAD h;
	BYTE Result;
};

struct CharTopMT
{
	char Name[11];
	int cLevel;
	int cPoint;
	int cExp;
};

struct DGCharTopMT
{
	PSWMSG_HEAD h;
	CharTopMT tp[MAXTOP];
};

struct DGCharTop2MT
{
	PWMSG_HEAD h;
	CharTopMT	tp[MAXTOP];
};
struct DATA_VIEWTOPRANKING
{
	PSWMSG_HEAD header;
	char NameChar[11];
	char GuildName[11];
	BYTE GuildMark[32];
	BYTE Item[12][16];
	int aIndex;
};
struct REQUESTINFO_CHARTOP
{
	PSBMSG_HEAD header; // C3:F3:03
	char NameChar[11];
	int aIndex;
};

class cRanking
{
	public:
	void SendDS();
	void RecvDS(DGCharTop *Recv);
	void SendUser(LPOBJ lpObj);
	CharTop RankingChar[MAXTOP];

	void SendDSTP();
	void RecvDSTP(DGCharTopTP* Recv);
	void SendUserTP(LPOBJ lpObj);
	CharTopTP RankingCharTP[MAXTOP];

	void SendDSET();
	void RecvDSET(DGCharTopET* Recv);
	void SendUserET(LPOBJ lpObj);
	CharTopET RankingCharET[MAXTOP];

	void SendDSTG();
	void RecvDSTG(DGCharTopTG* Recv);
	void SendUserTG(LPOBJ lpObj);
	CharTopTG RankingCharTG[MAXTOP];

	void SendDSPK();
	void RecvDSPK(DGCharTopPK* Recv);
	void SendUserPK(LPOBJ lpObj);
	CharTopPK RankingCharPK[MAXTOP];

	void SendDSMT();
	void RecvDSMT(DGCharTopMT* Recv);
	void SendUserMT(LPOBJ lpObj);
	CharTopMT RankingCharMT[MAXTOP];

	void CGetInfoCharTop(REQUESTINFO_CHARTOP* lpMsg, int aIndex);
	void RecvInfoCharTop(DATA_VIEWTOPRANKING* lpMsg);

	int FirstTime;
};
extern cRanking gRanking;