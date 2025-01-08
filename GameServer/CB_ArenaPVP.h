#pragma once
#include "Protocol.h"
#include "DSProtocol.h"
#include "User.h"
#if(CB_EventArenaPvP)
class CBArenaPVP
{
	enum eTimeState
	{
		EVENT_STATE_BLANK = 0,
		EVENT_STATE_EMPTY = 1,
		EVENT_STATE_STAND = 2,
		EVENT_STATE_START = 3,
		EVENT_STATE_CLEAN = 4,
	};
	struct TIME
	{
		int Year;
		int Month;
		int Day;
		int DayOfWeek;
		int Hour;
		int Minute;
		int Second;
	};

	struct MESSAGE_INFO
	{
		int Index;
		char Message[256];
	};

	struct ItemDropInfo
	{
		int Index;
		int IndexMin;
		int IndexMax;
		int LevelMax;
		int Skill;
		int Luck;
		int Option;
		int Exc;
	};
		
	struct EventReward_Data
	{
		int TopMin;
		int TopMax;
		int MinScore;
		int WC;
		int WP;
		int GP;
		int EventItemBag;
	};

	struct EVENT_USER
	{
		int Index;
		int Score;
		int Kills;
		int Deaths;
		int Rank;
		bool Active;
		OBJECTSTRUCT BackupData;		
	};

	enum TypeDataSend
	{
		eSendInfoNPCJoin = 0x01,
		eSendTime = 0x02,
		eSendBXHKill = 0x03,
		eSendInfoUser = 0x04,
		eSendClearInfo = 0x06,
	};
	struct XULY_CGPACKET
	{
		PSBMSG_HEAD header; // C3:F3:03
		DWORD ThaoTac;

	};
	struct PMSG_COUNTLIST
	{
		PSWMSG_HEAD header;
		int Count;
		BYTE TypeSend;
	};
	struct DATA_TIME_SEND
	{
		short Map;
		DWORD TimeCount;
		DWORD State;
	};
	struct DATA_BXH_KILL
	{
		char Name[11];
		int Kill;
		int Score;
	};
	struct DATA_INFOUSER_SEND
	{
		int Score;
		int Kills;
		int Deaths;
		int Rank;
	};
public:
	CBArenaPVP();
	void Init();
	void Load(char* FilePath);

	int m_RemainTime;
	int m_State;
	int m_EventTime;
	void CBArenaPVP::SendBXHVeClient();
	 
	std::vector<ItemDropInfo> m_ItemDropInfo;
	std::vector<EventReward_Data> m_EventReward;

	DATA_BXH_KILL CacheTopBXH[11];
	void MakeDropItem();

	void CBArenaPVP::SetNPC();
	void CheckSync();
	void MainProc();
	void ProcState_EMPTY();
	void ProcState_STAND();
	void ProcState_START();
	void ProcState_CLEAN();

	bool Dialog(LPOBJ lpObj, LPOBJ lpNpc);

	void StartEvent();
	void ClientRecvAction(BYTE* aRecv, int aIndex);
	bool CheckEnterEnabled(LPOBJ lpObj);
	bool CBArenaPVP::GetUserRespawnLocation(LPOBJ lpObj, int* gate, int* map, int* x, int* y, int* dir, int* level);

	void SetState(int state);

	int m_Enable;

	int m_StandTime;
	int m_CloseTime;
	int m_TickCount;
	int m_WarningTime;
	int EnterEnabled;
	int AlarmMinSave;
	int AlarmMinLeft;
	int TargetTime;

	std::vector<TIME> m_StartTime;
	//===Mess
	std::map<int, MESSAGE_INFO> m_MessageInfo;
	char* GetMessage(int index);

	int m_NPC;
	int m_NPCMap;
	int m_NPCX;
	int m_NPCY;
	int m_NPCDir;

	int m_MaxUser;
	int m_PointSet;
	int m_ScoreKill;
	int m_ScoreDie;
	int m_MapEvent;
	int m_StartX;
	int m_StartY;
	int m_EndX;
	int m_EndY;

	int ReqZen;
	int ReqWC;
	int ReqWP;
	int ReqGP;
	int ReqItemIndex;
	int ReqItemLevel;
	int ReqItemCount;
	int MinLevel;
	int MaxLevel;
	int MinReset;
	int MaxReset;
	int MinMasterReset;
	int MaxMasterReset;

	int DelayDrop;
	int DropCount;

	void NoticeSendToAll(int type, char* message, ...);
	EVENT_USER*  GetUser(int aIndex);
	bool CheckReqItems(LPOBJ lpObj);
	std::map<int, EVENT_USER> mDataUsePvP;
	void SendTimeEventAll(int time, int State, int aIndex = -1);

	bool GetPosRandom(int* x, int* y);
	void EventTeleport(int aIndex, int map, int x, int y);
	void gObjSendMoveViewport(LPOBJ lpObj, BYTE x, BYTE y);
	void CheckUser();
	int GetState();
	bool DelUser(int aIndex);
	bool CBArenaPVP::AddUser(int aIndex);
	void ClearDataUser(int aIndex);
	void CBArenaPVP::XoaDataEvent();
	int CBArenaPVP::GetItemRand(int Min, int Max);
	int CBArenaPVP::GetUserCount();
	bool CBArenaPVP::CheckEnteredUser(int aIndex);
	void CBArenaPVP::UserDieProc(LPOBJ lpObj, LPOBJ lpTarget);
	bool CBArenaPVP::CheckPlayerJoined(LPOBJ lpObj, LPOBJ lpTarget);
	void CBArenaPVP::CalcUserRank();
	void CBArenaPVP::SendInfoKillUser(int aIndex);
	void CBArenaPVP::ClearCacheBXH();
	void CBArenaPVP::SendClearInfo();
	void CBArenaPVP::ActionTraoThuong();
	int CBArenaPVP::CheckRankIsReward(int Rank, int Score);
	bool StausTraoThuong;
};

extern CBArenaPVP gCBArenaPVP;
#endif