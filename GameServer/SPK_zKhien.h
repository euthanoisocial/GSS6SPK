#pragma once
#include "StdAfx.h"
#if ACTIVE_VKR
#include "user.h"
#include "Protocol.h"

struct IS_STRUCT_ITEM06
{
	int	Enabled;
	BYTE IsITEM06[3];
};

struct PMSG_ITEM06_REQ
{
	PSBMSG_HEAD	Head;
	BYTE ItemRong;
};

class ITEM06
{
	public:
	void	Load(char *path);
	void	Option(LPOBJ lpObj, bool flag);
	int		CountItem(LPOBJ lpObj, int level);
	IS_STRUCT_ITEM06 m_ITEM06;
};

extern ITEM06 GITEM06;
#endif