#pragma once

#include "Protocol.h"
#include "User.h"

void gObjMonsterDieGiveItem(LPOBJ lpObj,LPOBJ lpTarget);
bool gObjSetPosMonster(int aIndex,int number);
bool gObjSetMonster(int aIndex,int MonsterClass);
bool gObjSetMonsterBot(int aIndex, int MonsterClass);
bool gObjMonsterRegen(LPOBJ lpObj);
bool gObjMonsterMoveCheck(LPOBJ lpObj,int tx,int ty);
void gObjMonsterInitHitDamage(LPOBJ lpObj);
void gObjMonsterSetHitDamage(LPOBJ lpObj,int aIndex,float damage);
int gObjMonsterDelHitDamageUser(LPOBJ lpObj);
int gObjMonsterGetTopHitDamageUser(LPOBJ lpObj);
int gObjMonsterGetTopHitDamageParty(LPOBJ lpObj,int PartyNumber,int* TopHitDamageUser);
int gObjAddMonster(int map);
int gObjAddSummon();
int gObjSummonSetEnemy(LPOBJ lpObj,int aIndex);
void gObjSummonKill(int aIndex);
void gObjMonsterMagicAttack(LPOBJ lpObj,int skill);
void gObjMonsterBeattackRecv(BYTE* aRecv,int aIndex);
void gObjMonsterAttack(LPOBJ lpObj,LPOBJ lpTarget);
void gObjMonsterDie(LPOBJ lpObj,LPOBJ lpTarget);
void gObjMonsterStateProc(LPOBJ lpObj,int code,int aIndex,int SubCode);

//**************************************************************************//
// RAW FUNCTIONS ***********************************************************//
//**************************************************************************//

struct PMSG_MAGICATTACK_RESULT
{
	PBMSG_HEAD h;	// C3:19
	BYTE MagicNumberH;	// 3
	BYTE MagicNumberL;	// 4
	BYTE SourceNumberH;	// 5
	BYTE SourceNumberL;	// 6
	BYTE TargetNumberH;	// 7
	BYTE TargetNumberL;	// 8
};

struct PMSG_MAGICATTACK
{
	PBMSG_HEAD h;
	BYTE MagicNumberH;	// 3
	BYTE MagicNumberL;	// 4
	BYTE NumberH;	// 5
	BYTE NumberL;	// 6
	BYTE Dis;	// 7
};

struct PMSG_BEATTACK_COUNT
{
	PBMSG_HEAD h;	// C1:D7
	BYTE MagicNumberH;	// 3
	BYTE MagicNumberL;	// 4
	BYTE X;	// 5
	BYTE Y;	// 6
	BYTE Serial;	// 7
	BYTE Count;	// 8
};

struct PMSG_BEATTACK
{
	BYTE NumberH;	// 0
	BYTE NumberL;	// 1
	BYTE MagicKey;	// 2
};

struct PMSG_DURATION_MAGIC_RECV
{
	PBMSG_HEAD h;
	BYTE MagicNumberH;	// 3
	BYTE MagicNumberL;	// 4
	BYTE X;	// 5
	BYTE Y;	// 6
	BYTE Dir;	// 7
	BYTE Dis;	// 8
	BYTE TargetPos;	// 9
	BYTE NumberH;	// A
	BYTE NumberL;	// B
	BYTE MagicKey;	// C
};

struct PMSG_ATTACK
{
	PBMSG_HEAD h;
	BYTE NumberH;	// 3
	BYTE NumberL;	// 4
	BYTE AttackAction;	// 5
	BYTE DirDis;	// 6
};

struct PMSG_MOVE
{
	PBMSG_HEAD h;	// C1:1D
	BYTE X;	// 3
	BYTE Y;	// 4
	BYTE Path[8];	// 5
};
/* tue fix
struct XULY_CGPACKET
{
	PSBMSG_HEAD header; // C3:F3:03
	DWORD ThaoTac;

};
*/
#if BXHDAME
#define		TOPDAME	10
struct TOPDAME_Player
{
	void Reset() // OK
	{
		Index = -1;
		Rank = 0;
		Regen = 0;
		Live = 0;
		IndexQuai = -1;
	}
	bool  Live;
	short Index;
	float	  Dmg;
	int	  Regen;
	int	  Rank;
	int  IndexQuai;
};

struct DMG_BXH
{
	WORD monsterid;
	int Level;
	float MinLife;
	float MaxLife;
	char szName[11];
	QWORD Kill;
	BYTE Rate;
};

class CMonter
{
public:
	CMonter();
	~CMonter();
	int mIndexRank[10];
	int m_TotalPlayer;
	bool AddMember(int aIndex);
	int  CountMember();
	bool DelMember(int aIndex);
public:
	TOPDAME_Player		Player[TOPDAME];
	void CalcUserRank();
	void SendBXH(LPOBJ lpObj, int aIndex);
	void AddPlayerPoint(int aIndex, float damage, int Quai);
	void DrawClient(int aIndex);
	bool DelAllMembers();
};

#endif
int gObjMonsterViewportIsCharacter(LPOBJ lpObj);
BOOL gObjMonsterGetTargetPos(LPOBJ lpObj);
BOOL gObjGetTargetPos(LPOBJ lpObj, int sx, int sy, int & tx , int & ty);
int gObjMonsterSearchEnemy(LPOBJ lpObj, BYTE objtype);
int gObjGuardSearchEnemy(LPOBJ lpObj);
void gObjMonsterProcess(LPOBJ lpObj);
BOOL PathFindMoveMsgSend(LPOBJ lpObj);
bool gObjSetBots(int aIndex,int MonsterClass); //MC bot
void gObjMonsterMoveAction(LPOBJ lpObj);
void gObjMonsterBaseAct(LPOBJ lpObj);
void gObjTrapAttackEnemySearchX(LPOBJ lpObj, int count);
void gObjTrapAttackEnemySearchY(LPOBJ lpObj, int count);
void gObjTrapAttackEnemySearch(LPOBJ lpObj);
void gObjTrapAttackEnemySearchRange(LPOBJ lpObj,int iRange);
void gObjMonsterTrapAct(LPOBJ lpObj);
void gObjMonsterStateProcCase0(LPOBJ lpObj,int aIndex);
