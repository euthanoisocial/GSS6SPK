#pragma once
#include "User.h"
#define MAX_GAME_MASTER 100
struct GAME_MASTER_INFO
{
	char Account[11];
	char Name[11];
	BYTE Level;
	char CheckIP[16];
};

class CGameMaster
{
public:
	CGameMaster();
	void Load(char* path);
	void SetInfo(GAME_MASTER_INFO info);
	void SetGameMasterLevel(LPOBJ lpObj,int level);
	bool CheckGameMasterLevel(LPOBJ lpObj,int level);
private:
	GAME_MASTER_INFO m_GameMasterInfo[MAX_GAME_MASTER];
	int m_count;
};

extern CGameMaster gGameMaster;
