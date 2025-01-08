#pragma once
#if NEW_VIP_NEW
#include "DefaultClassInfo.h"
#include "User.h"
#include "Protocol.h"

struct CONFIDATA_NEWVIP
{
	WORD LvNewVip;
	DWORD YCWC;
	DWORD ExpThuong;
	DWORD ExpMaster;
	int Days;
};
struct XULY_NEWVIP
{
	PSBMSG_HEAD header;
	DWORD ThaoTac;
};
struct MESSAGE_INFO_NEWVIP
{
	int Index;
	char Message[256];
};
class SPKNewVip
{
public:
	SPKNewVip();
	virtual ~SPKNewVip();
	void LoadConfig(char* FilePath);
	bool Enable;
	bool ThongBao;
	int ThoiGian,GioiHan;
	std::map<int, CONFIDATA_NEWVIP> mDataConfigNewVip;
	CONFIDATA_NEWVIP* GetConfigLvNewVip(int LvNewVip);
	void SendInfoClient(int aIndex);
	bool NangCapNewVip(int aIndex);
	void GetOptionNewVip(LPOBJ lpObj, bool flag);

	private:
	std::map<int, MESSAGE_INFO_NEWVIP> m_MessageInfoBP;
	char* GetMessage(int index);
};
extern SPKNewVip gSPKNewVip;
#endif