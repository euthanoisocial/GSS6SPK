#pragma once
#include "StdAfx.h"
#if ACTIVE_VKR
#include "user.h"
#include "Protocol.h"

struct IS_STRUCT_ITEM11
{
	int	Enabled;
	BYTE IsITEM11[3];
};

struct PMSG_ITEM11_REQ
{
	PSBMSG_HEAD	Head;
	BYTE ItemRong;
};

class ITEM11
{
	public:
	void	Load(char *path);
	void	Option(LPOBJ lpObj, bool flag);
	int		CountItem(LPOBJ lpObj, int level);
	IS_STRUCT_ITEM11 m_ITEM11;
};

extern ITEM11 GITEM11;
#endif