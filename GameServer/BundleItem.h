#pragma once
#include "Protocol.h"

struct BI_Data
{
	short	Index;
	char	Level;
	BYTE	Max;
};

struct PMSG_BUNDLE_ITEM_INFO_HEADER_SEND
{
	PSWMSG_HEAD header; // C3:F3:03
	DWORD Count;
};

class cBundleItem
{
public:
	cBundleItem();
	~cBundleItem();
	void Read(char* path);
	bool IsBundleItem(short Index, BYTE Level);
	bool IsBundleItemSlot(int aIndex, int SourceSlot);
	BYTE GetBundleItemMaxStack(short Index, BYTE Level);
	void GCSendConfig(int aIndex);
	void OnCharacterLogin(int aIndex);
	bool CGBundleItemUnmix(int aIndex, int slot);
	bool CGStackItem(int aIndex, int SourceSlot, int TargetSlot);
	bool UseStack(int aIndex, int SourceSlot, int ValueUse);
	bool InventoryInsertItemStack(LPOBJ lpObj, CItem* lpItem);
private:
	std::vector<BI_Data> m_List;
};

extern cBundleItem BundleItem;
