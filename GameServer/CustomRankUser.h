#ifndef _CUSTOM_RANK_USER_H
#define _CUSTOM_RANK_USER_H
// ---
#include "Protocol.h"
// ---
#define MAX_RANK_USER  50
#define MAX_NUM_RSCHANGE 5
struct CUSTOM_RANK_USER_DATA
{
	void Clear() // OK
	{
		memset(this->m_Name, 0, sizeof(this->m_Name));
		this->m_iResetMin = 0;
		this->m_iResetMax = 0;
		this->m_Coin1 = 0;
		this->m_Coin2 = 0;
		this->m_Coin3 = 0;
	}
	char m_Name[32];
	int  m_iResetMin;
	int  m_iResetMax;
	int  m_Coin1;
	int  m_Coin2;
	int  m_Coin3;
};
// ---
struct PMSG_CUSTOM_RANKUSER
{
	PSBMSG_HEAD h;
	char szName[20];
	int iIndex;
	int iLevel;
	int m_RankTitle1;
	int m_RankTitle2;
	int m_RankTitle3;
	int m_RankTitle4;
	#if(B_HON_HOAN)
	WORD m_RankTitle5;
	#endif
	#if(HT_THUE_FLAG)
	int m_RankTitle6;
	#endif
	#if BAN_RS
	int ReqResetChange[MAX_NUM_RSCHANGE];
	int ReqResetCoin[MAX_NUM_RSCHANGE];
	int ReqResetUpPoint[MAX_NUM_RSCHANGE];
	#endif
	int iVipIcon;
	char iVip[20];
	//tue add
	int iReset;
	int iMReset;
	int iKills;
	int iDeads;
	int iFor;
	int iEne;
	int iAgi;
	int iVit;
	int iCom;
	int iClasse;
	int iRank;
	
	//end add
};
// ---
class CCustomRankUser
{
	public:
	void Load(char* path);
	void GCReqRankLevelUser(int aIndex, int aTargetIndex);
	int GetRankIndex(int aIndex);
	void CheckUpdate(LPOBJ lpObj);
	//tue add
	int GetUserCRankIndex(int aIndex);
	//--

	private:
	int m_count;
	CUSTOM_RANK_USER_DATA m_Data[MAX_RANK_USER];
};
extern CCustomRankUser gCustomRankUser;
#endif