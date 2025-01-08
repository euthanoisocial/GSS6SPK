#pragma once
#include "StdAfx.h"
#if ACTIVE_VKR
#include "user.h"
#include "Protocol.h"

struct IS_STRUCT_ITEM09
{
	int	Enabled;
	BYTE IsITEM09[3];
};

struct PMSG_ITEM09_REQ
{
	PSBMSG_HEAD	Head;
	BYTE ItemRong;
};

class ITEM09
{
	public:
	void	Load(char *path);
	void	Option(LPOBJ lpObj, bool flag);
	int		CountItem(LPOBJ lpObj, int level);
	IS_STRUCT_ITEM09 m_ITEM09;
};

extern ITEM09 GITEM09;
#endif