#pragma once
#if(HT_THUE_FLAG)
#include "Protocol.h"

struct THUE_FLAG_DATA
{
	int		Id;
	int		Skin;
	int		Time;
	int		Price;
	int		Value1;
	int		Value2;
	int		Value3;
	int		Value4;
	int		Value5;
	int		Value6;
	int		Value7;
	int		Value8;
	int		Value9;
	int		Value10;
};

struct PMSG_THUE_FLAG_RECV
{
	PBMSG_HEAD2_Origin h;
	BYTE Type;
	int  Value;
};

struct PMSG_THUE_FLAG_SEND
{
	PBMSG_HEAD2_Origin h;
	int				Skin;
	THUE_FLAG_DATA	Data;
};
struct GSSENDDS_GETLISTISBUYFLAG
{
	PSBMSG_HEAD header;
	char NameChar[11];
	int aIndex;
};
struct GSSENDDS_SAVEBUYFLAG
{
	PSBMSG_HEAD header;
	char NameChar[11];
	int FlagID;
	int Time;
};
struct DATAFLAG_LOAD_COUNT
{
	PSWMSG_HEAD header;
	int aIndex;
	int count;
};
struct BCUSTOM_FLAGBUY_DATA
{
	int FlagID;
	int Time;
};
class cThueFlag
{
public:
	cThueFlag();
	~cThueFlag();
	void Read(char* path);
	void CGRequestRecv(BYTE* a, int aIndex);
	void GCInfoSend(int a, int aIndex, bool changeskin = true);
	void BuyProc(int a, int aIndex);
	void SkinState(bool state, int aIndex);
	void OnLogin(int aIndex);
	void CharacterCalc(int aIndex, bool flag);
	THUE_FLAG_DATA* FindSkin(int id);
	std::vector<THUE_FLAG_DATA> m_Data;
	void cThueFlag::GDGetListBuy(int aIndex);
	void cThueFlag::GDSaveFlagBuy(int aIndex, int FlagID, int Time);
	bool cThueFlag::UpdateListFlagBuy(int aIndex, int FlagID, int Time);
	void cThueFlag::RecvGDGetListBuy(DATAFLAG_LOAD_COUNT* lpMsg);
	int cThueFlag::CheckFlagIsBuy(int aIndex, int FlagID);
	void cThueFlag::MainProc(int aIndex);
	int TypeBonus;
private:
};

extern cThueFlag ThueFlag;
#endif