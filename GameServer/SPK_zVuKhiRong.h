#pragma once
#include "StdAfx.h"
#if ACTIVE_VKR
#include "user.h"
#include "Protocol.h"

struct IS_STRUCT_ITEM00
{
	int	Enabled;
	BYTE IsITEM00[3];
};

struct PMSG_ITEM00_REQ
{
	PSBMSG_HEAD	Head;
	BYTE ItemRong;
};

class ITEM00
{
	public:
	void	Load(char *path);
	void	Option(LPOBJ lpObj, bool flag);
	int		CountItem(LPOBJ lpObj, int level);
	IS_STRUCT_ITEM00 m_ITEM00;
};

extern ITEM00 GITEM00;
#endif