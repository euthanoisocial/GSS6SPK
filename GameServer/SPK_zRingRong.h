#pragma once
#include "StdAfx.h"
#if ACTIVE_VKR
#include "user.h"
#include "Protocol.h"

struct IS_STRUCT_ITEM13
{
	int	Enabled;
	BYTE IsITEM13[3];
};

struct PMSG_ITEM13_REQ
{
	PSBMSG_HEAD	Head;
	BYTE ItemRong;
};

class ITEM13
{
public:
	void	Load(char* path);
	void	Option(LPOBJ lpObj, bool flag);
	int		CountItem(LPOBJ lpObj, int level);
	IS_STRUCT_ITEM13 m_ITEM13;
};

extern ITEM13 GITEM13;
#endif