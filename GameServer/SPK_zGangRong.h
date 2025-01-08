#pragma once
#include "StdAfx.h"
#if ACTIVE_VKR
#include "user.h"
#include "Protocol.h"

struct IS_STRUCT_ITEM10
{
	int	Enabled;
	BYTE IsITEM10[3];
};

struct PMSG_ITEM10_REQ
{
	PSBMSG_HEAD	Head;
	BYTE ItemRong;
};

class ITEM10
{
	public:
	void	Load(char *path);
	void	Option(LPOBJ lpObj, bool flag);
	int		CountItem(LPOBJ lpObj, int level);
	IS_STRUCT_ITEM10 m_ITEM10;
};

extern ITEM10 GITEM10;
#endif