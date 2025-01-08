#pragma once
#if TU_LUYEN_NEW
#include "DefaultClassInfo.h"
#include "User.h"
#include "Protocol.h"

struct CONFIDATA_TULUYEN
{
	WORD LvTuLuyen;
	WORD YCItemSL;
	WORD YCItemID;
	WORD YCItemLv;
	DWORD YCWC;
	DWORD YCWP;
	DWORD YCGP;
	DWORD TangMau;
	DWORD TangSD;
	DWORD TangST;
	DWORD TangPT;

};
struct XULY_TULUYEN
{
	PSBMSG_HEAD header;
	DWORD ThaoTac;
};
struct MESSAGE_INFO_TULUYEN
{
	int Index;
	char Message[256];
};
class SPKTuLuyen
{
public:
	SPKTuLuyen();
	virtual ~SPKTuLuyen();
	void LoadConfig(char* FilePath);
	bool Enable;
	bool ThongBao;
	int ThoiGian,GioiHan;
	std::map<int, CONFIDATA_TULUYEN> mDataConfigTuLuyen;
	CONFIDATA_TULUYEN* GetConfigLvTuLuyen(int LvTuLuyen);
	void SendInfoClient(int aIndex);
	bool NangCapTuLuyen(int aIndex);
	void GetOptionTuLuyen(LPOBJ lpObj, bool flag);

	private:
	std::map<int, MESSAGE_INFO_TULUYEN> m_MessageInfoBP;
	char* GetMessage(int index);
};
extern SPKTuLuyen gSPKTuLuyen;
#endif