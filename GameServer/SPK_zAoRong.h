#pragma once
#include "StdAfx.h"
#if ACTIVE_VKR
#include "user.h"
#include "Protocol.h"

struct IS_STRUCT_ITEM08
{
	int	Enabled;
	BYTE IsITEM08[3];
};

struct PMSG_ITEM08_REQ
{
	PSBMSG_HEAD	Head;
	BYTE ItemRong;
};

class ITEM08
{
	public:
	void	Load(char *path);
	void	Option(LPOBJ lpObj, bool flag);
	int		CountItem(LPOBJ lpObj, int level);
	IS_STRUCT_ITEM08 m_ITEM08;
};

extern ITEM08 GITEM08;
#endif