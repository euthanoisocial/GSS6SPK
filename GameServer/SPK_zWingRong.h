#pragma once
#include "StdAfx.h"
#if ACTIVE_VKR
#include "user.h"
#include "Protocol.h"

struct IS_STRUCT_ITEM12
{
	int	Enabled;
	BYTE IsITEM12[3];
};

struct PMSG_ITEM12_REQ
{
	PSBMSG_HEAD	Head;
	BYTE ItemRong;
};

class ITEM12
{
	public:
	void	Load(char *path);
	void	Option(LPOBJ lpObj, bool flag);
	int		CountItem(LPOBJ lpObj, int level);
	IS_STRUCT_ITEM12 m_ITEM12;
};

extern ITEM12 GITEM12;
#endif