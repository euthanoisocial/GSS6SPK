// Log.h: interface for the CLog class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

#define MAX_LOG 20

enum eLogType
{
	LOG_GENERAL		= 0,
	LOG_CHAT		= 1,
	LOG_COMMAND		= 2,
	LOG_TRADE		= 3,
	LOG_CONNECT		= 4,
	LOG_HACK		= 5,
	LOG_CASH_SHOP	= 6,
	LOG_CHAOS_MIX	= 7,
	LOG_ANTIFLOOD	= 8,
	#if BAG_EVENT_DROP
	LOG_ITEMBAGDROP	= 9,
	#endif
	LOG_BAUCUA		= 10,
	LOG_SHOP		= 11,
};

struct LOG_INFO
{
	BOOL Active;
	char Directory[256];
	int Day;
	int Month;
	int Year;
	char Filename[256];
	HANDLE File;
};

class CLog
{
public:
	CLog();
	virtual ~CLog();
	void AddLog(BOOL active,char* directory);
	void Output(eLogType type,char* text,...);
private:
	LOG_INFO m_LogInfo[MAX_LOG];
	int m_count;
};

extern CLog gLog;
