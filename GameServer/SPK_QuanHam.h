#pragma once
#if QUAN_HAM_NEW
#include "DefaultClassInfo.h"
#include "User.h"
#include "Protocol.h"

struct CONFIDATA_QUANHAM
{
	WORD LvQuanHam;
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
struct XULY_QUANHAM
{
	PSBMSG_HEAD header;
	DWORD ThaoTac;
};
struct MESSAGE_INFO_QUANHAM
{
	int Index;
	char Message[256];
};
class SPKQuanHam
{
public:
	SPKQuanHam();
	virtual ~SPKQuanHam();
	void LoadConfig(char* FilePath);
	bool Enable;
	bool ThongBao;
	int ThoiGian,GioiHan;
	std::map<int, CONFIDATA_QUANHAM> mDataConfigQuanHam;
	CONFIDATA_QUANHAM* GetConfigLvQuanHam(int LvQuanHam);
	void SendInfoClient(int aIndex);
	bool NangCapQuanHam(int aIndex);
	void GetOptionQuanHam(LPOBJ lpObj, bool flag);

	private:
	std::map<int, MESSAGE_INFO_QUANHAM> m_MessageInfoBP;
	char* GetMessage(int index);
};
extern SPKQuanHam gSPKQuanHam;
#endif