#pragma once
#include "StdAfx.h"
#include "Protocol.h"
#include "DSProtocol.h"
#include "User.h"
#include "OfflineMode.h"
#include "ItemManager.h"
#if(SPK_ITEMTRADER)
#define eMessageBox				255
struct DOIITEM_MSG
{
	int Index;
	char Message[256];
};
struct XULY_CGPITEMTRADE
{
	PSBMSG_HEAD header; // C3:F3:03
	DWORD ThaoTac;

};
struct NPC_DATA_DOIITEM
{
	int NPCClass;
	int NPCMap;
	int NPCX;
	int NPCY;
	int NPCDir;
	void Clear()
	{
		NPCClass = -1;
		NPCMap = 0;
		NPCX = 0;
		NPCY = 0;
		NPCDir = 0;
	}
};
struct ITEM_DOI_DATA
{
	int IndexItem;
	int Level;
	int Skill;
	int Luck;
	int Option;
	int Exc;
	int Harmony;
};


struct ITEM_KQ_DATA
{
	int IndexItem;
	int IndexItem1;
	int IndexItem2;
	int IndexItem3;
	int IndexItem4;
	int IndexItem5;
};

struct GROUPDOIITEM_DATA
{
	int Index;
	int Notice;
	char Name[90];
	int WCoinC;
	int WCoinP;
	int WCoinG;
	int CoinAtm;

	ITEM_DOI_DATA ItemChinh;
	std::vector<ITEM_DOI_DATA> ItemPhu;
	ITEM_KQ_DATA ItemKetQua;

};
struct INFOITEM_DOIITEM_CLIENT
{
	PSWMSG_HEAD header;
	BYTE ActiveMix;
	BYTE ItemChinh[16];
	BYTE ItemPhu[3][16];
	BYTE ItemKetQua[6][16];
	int WCoinC;
	int WCoinP;
	int WCoinG;
	int CoinAtm;
};

class GC_ItemTrader
{
public:
	GC_ItemTrader();
	virtual ~GC_ItemTrader();
	int Enable;
	NPC_DATA_DOIITEM* mNPCData;
	void LoadConfig(char* FilePath);
	bool GC_ItemTrader::Dialog(LPOBJ lpObj, LPOBJ lpNpc);
	std::map<int, GROUPDOIITEM_DATA> m_GroupDoiItemData;

	void GC_ItemTrader::SetStateInterface(int aIndex, int Type = 0);
	void GC_ItemTrader::ProcItemSend(int aIndex, int SlotItem);
	void GC_ItemTrader::SendInfoItemCache(int aIndex);
	void GC_ItemTrader::BackItem(int aIndex, int BackSlot);
	void RollBackItem(int aIndex, int TypeClear, bool RollBackItem = 1);
	void StepClick(LPOBJ lpObj, int aIndex, BYTE SlotRecv, int BackSlot, BYTE ID);
	void SetNPC();
private:
	//===Mess
	std::map<int, DOIITEM_MSG> m_MessageInfoBP;
	char* GetMessage(int index);
	
};

extern GC_ItemTrader gGC_ItemTrader;



#endif