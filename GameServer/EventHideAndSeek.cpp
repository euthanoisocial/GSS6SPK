// CEventHideAndSeek.cpp: interface for the CEventHideAndSeek class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CommandManager.h"
#include "DSProtocol.h"
#include "EventHideAndSeek.h"
#include "GameMaster.h"
#include "Message.h"
#include "Monster.h"
#include "Notice.h"
#include "Protocol.h"
#include "ServerInfo.h"
#include "Util.h"
#include "ItemBagManager.h"

CEventHideAndSeek gEventHideAndSeek;

char *MapHNS[] ={
	"Lorencia",					//0
	"Dungeon",					//1
	"Davias",					//2
	"Noria",					//3
	"LostTower",				//4
	"Exile",					//5
	"Arena",					//6
	"Atlans",					//7
	"Tarkan",					//8
	"Devil Square",				//9
	"Icarus",					//10
	"Blood Castle 1",			//11
	"Blood Castle 2",			//12
	"Blood Castle 3",			//13
	"Blood Castle 4",			//14
	"Blood Castle 5",			//15
	"Blood Castle 6",			//16
	"Blood Castle 7",			//17
	"Chaos Castle 1",			//18
	"Chaos Castle 2",			//19
	"Chaos Castle 3",			//20
	"Chaos Castle 4",			//21
	"Chaos Castle 5",			//22
	"Chaos Castle 6",			//23
	"Kalima 1",					//24
	"Kalima 2",					//25
	"Kalima 3",					//26
	"Kalima 4",					//27
	"Kalima 5",					//28
	"Kalima 6",					//29
	"Valley of Loren",			//30
	"Land of Trials",			//31
	"Devil Square",				//32
	"Aida",						//33
	"Crywolf Fortress",			//34
	"NULL35",					//35
	"Kalima 7",					//36
	"Kantru",					//37
	"Kantru K4",				//38
	"Kantru 3",					//39
	"Silent Map",				//40
	"Doanh Trại Barack",		//41
	"Balgass Refuge",			//42
	"NULL43",					//43
	"NULL44",					//44
	"Illusion Temple 1",		//45
	"Illusion Temple 2",		//46
	"Illusion Temple 3",		//47
	"Illusion Temple 4",		//48
	"Illusion Temple 5",		//49
	"Illusion Temple 6",		//50
	"Elbeland",					//51
	"Blood Castle 8",			//52
	"Chaos Castle 7",			//53
	"NULL54",					//54
	"NULL55",					//55
	"Swamp of Calmness",		//56
	"Raklion",					//57
	"Raklion Boss",				//58
	"NULL59",					//59
	"NULL60",					//60
	"NULL61",					//61
	"Village's Santa",			//62
	"Vulcanus",					//63
	"Duel Arena",				//64
	"Doppelganger 1",			//65
	"Doppelganger 2",			//66
	"Doppelganger 3",			//67
	"Doppelganger 4",			//68
	"Empire Fortress 1",		//69
	"Empire Fortress 2",		//70
	"Empire Fortress 3",		//71
	"Empire Fortress 4",		//72
	"NULL73",					//73
	"NULL74",					//74
	"NULL75",					//75
	"NULL76",					//76
	"NULL77",					//77
	"NULL78",					//78
	"NULL79",					//79
	"Karuntan 1",				//80
	"Karuntan 2",				//81
	"NULL82",					//82
	"NULL83",					//83
	"NULL84",					//84
	"NULL85",					//85
	"Nars",					//86
	"NULL87",					//87
	"NULL88",					//88
	"Acheron",					//89
	"Debanter",					//90
	"NULL91",					//91
	"NULL92",					//92
	"NULL93",					//93
	"NULL94",					//94
	"NULL95",					//95
	"Thần Ma Chiến",			//96
	"Chiến Trường Cổ",			//97
	"NULL98",					//98
	"NULL99",					//99
	"NULL100"};					//100

CEventHideAndSeek::CEventHideAndSeek() // OK
{
	this->m_RemainTime = 0;
	this->m_GmIndex = 0;
	this->m_TickCount = GetTickCount();
	this->m_EventHideAndSeek = 0;
}

CEventHideAndSeek::~CEventHideAndSeek() // OK
{

}

void CEventHideAndSeek::Clear()
{
	this->m_RemainTime = 0;
	this->m_TickCount = GetTickCount();
	this->m_GmIndex = 0;
	this->MinutesLeft = -1;
	this->m_EventHideAndSeek = 0;
}

void CEventHideAndSeek::MainProc() // OK
{
	DWORD elapsed = GetTickCount()-this->m_TickCount;

	if(elapsed < 1000)
	{
		return;
	}

	this->m_TickCount = GetTickCount();

	if(this->m_RemainTime > 0)
	{ 
		int minutes = this->m_RemainTime/60;

		if((this->m_RemainTime%60) == 0)
		{
			minutes--;
		}

		if(this->MinutesLeft != minutes)
		{
			this->MinutesLeft = minutes;

				gNotice.GCNoticeSendToAll(0,0,0,0,0,0,gMessage.GetMessage(682),(MinutesLeft+1));
		}

		if(this->m_RemainTime <= 10)
		{
			gNotice.GCNoticeSendToAll(0,0,0,0,0,0,gMessage.GetMessage(676),m_RemainTime);
		}

		this->m_RemainTime--;

		if(this->m_RemainTime <= 0)
		{
			this->m_EventHideAndSeek = 0;
			gNotice.GCNoticeSendToAll(0,0,0,0,0,0,gMessage.GetMessage(677));
			LogAdd(LOG_EVENT,"[EVENT HIDE AND SEEK] Close");
		}
	}
}

void CEventHideAndSeek::CommandEventHideAndSeek(LPOBJ lpObj,char* arg) // OK 
{

	if(gServerInfo.m_EventHideAndSeekSwitch == 0)
	{
		return;
	}

	if(gGameMaster.CheckGameMasterLevel(lpObj,1) == 0)
	{
		return;
	}

	if (this->m_RemainTime > 0) 
	{
		this->Clear();
		LogAdd(LOG_EVENT,"[EVENT HIDE AND SEEK] Canceled");

		gNotice.GCNoticeSendToAll(0,0,0,0,0,0,gMessage.GetMessage(678));
		return;
	}

	this->m_EventHideAndSeek = 1;
	this->m_GmIndex = lpObj->Index;

	this->m_RemainTime = gServerInfo.m_EventHideAndSeekMaxTime*60;

	gNotice.GCNoticeSendToAll(0,0,0,0,0,0,gMessage.GetMessage(679),lpObj->Name,MapHNS[lpObj->Map],lpObj->X,lpObj->Y);
	gNotice.GCNoticeSendToAll(0,0,0,0,0,0,gMessage.GetMessage(680));
	LogAdd(LOG_EVENT,"[EVENT HIDE AND SEEK] Start");
}

int CEventHideAndSeek::EventHideAndSeekTrade(int aIndex,int bIndex) // OK 
{

	if(gServerInfo.m_EventHideAndSeekSwitch == 0)
	{
		return 0;
	}

	if (this->m_EventHideAndSeek == 0) 
	{
		return 0;
	}

	if (this->m_GmIndex != bIndex) 
	{
		return 0;
	}

	LPOBJ lpObj = &gObj[aIndex];


	this->Clear();

	gNotice.GCNoticeSendToAll(0,0,0,0,0,0,gMessage.GetMessage(681),lpObj->Name);

	GCFireworksSend(lpObj,lpObj->X,lpObj->Y);

	if (gServerInfo.m_EventHideAndSeekAutoReward1 > 0 || gServerInfo.m_EventHideAndSeekAutoReward2 > 0 || gServerInfo.m_EventHideAndSeekAutoReward3 > 0)
	{	
		GDSetCoinSend(lpObj->Index, gServerInfo.m_EventHideAndSeekAutoReward1, gServerInfo.m_EventHideAndSeekAutoReward2, gServerInfo.m_EventHideAndSeekAutoReward3, 0, 0, "HideAndSeek");
		
		gItemBagManager.DropItemBySpecialValue(ITEM_BAG_FIND_ADMIN,lpObj,lpObj->Map,lpObj->X,lpObj->Y);
	}

	LogAdd(LOG_EVENT,"[EVENT HIDE AND SEEK] Finish (Winner: %s)",lpObj->Name);

	return 1;
}
