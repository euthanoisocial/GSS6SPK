#pragma once
#include "DefaultClassInfo.h"
#include "User.h"
#include "Protocol.h"
#if(B_HON_HOAN)
struct CONFIDATA_HONHOAN
{
	WORD LvHonHoan;
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
	DWORD ItemReward;

};
struct XULY_HONHOAN
{
	PSBMSG_HEAD header; // C3:F3:03
	DWORD ThaoTac;

};
struct MESSAGE_INFO_HONHOAN
{
	int Index;
	char Message[256];
};
class CBHonHoan
{
public:
	CBHonHoan();
	virtual ~CBHonHoan();
	void LoadConfig(char* FilePath);
	bool Enable;
	bool ThongBao;
	int ThoiGian,GioiHan;
	std::map<int, CONFIDATA_HONHOAN> mDataConfigHonHoan;
	CONFIDATA_HONHOAN* GetConfigLvHonHoan(int LvHonHoan);
	void SendInfoClient(int aIndex);
	bool NangCapHonHoan(int aIndex);
	void GetOptionHonHoan(LPOBJ lpObj, bool flag);
private:

	//===Mess
	std::map<int, MESSAGE_INFO_HONHOAN> m_MessageInfoBP;
	char* GetMessage(int index);
};

extern CBHonHoan gBHonHoan;
#endif