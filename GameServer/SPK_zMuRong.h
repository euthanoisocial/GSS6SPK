#pragma once
#include "StdAfx.h"
#if ACTIVE_VKR
#include "user.h"
#include "Protocol.h"

struct IS_STRUCT_ITEM07
{
	int	Enabled;
	BYTE IsITEM07[3];
};

struct PMSG_ITEM07_REQ
{
	PSBMSG_HEAD	Head;
	BYTE ItemRong;
};

class ITEM07
{
	public:
	void	Load(char *path);
	void	Option(LPOBJ lpObj, bool flag);
	int		CountItem(LPOBJ lpObj, int level);
	IS_STRUCT_ITEM07 m_ITEM07;
};

extern ITEM07 GITEM07;
#endif