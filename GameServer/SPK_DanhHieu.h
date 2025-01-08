#pragma once
#if DANH_HIEU_NEW
#include "DefaultClassInfo.h"
#include "User.h"
#include "Protocol.h"

struct CONFIDATA_DANHHIEU
{
	WORD LvDanhHieu;
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
struct TimeDanhHieu
{
	int IsTimer;
};
struct XULY_DANHHIEU
{
	PSBMSG_HEAD header;
	DWORD ThaoTac;
};
struct MESSAGE_INFO_DANHHIEU
{
	int Index;
	char Message[256];
};
class SPKDanhHieu
{
public:
	SPKDanhHieu();
	virtual ~SPKDanhHieu();
	void LoadConfig(char* FilePath);
	bool Enable;
	bool ThongBao;
	int ThoiGian,GioiHan;
	std::map<int, CONFIDATA_DANHHIEU> mDataConfigDanhHieu;
	CONFIDATA_DANHHIEU* GetConfigLvDanhHieu(int LvDanhHieu);
	void SendInfoClient(int aIndex);
	bool NangCapDanhHieu(int aIndex);
	void GetOptionDanhHieu(LPOBJ lpObj, bool flag);

	private:
	std::map<int, MESSAGE_INFO_DANHHIEU> m_MessageInfoBP;
	char* GetMessage(int index);
};
extern SPKDanhHieu gSPKDanhHieu;
#endif