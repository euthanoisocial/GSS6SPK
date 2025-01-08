#include "stdafx.h"
#include "BundleItem.h"
#include "MemScript.h"
#include "ScheduleManager.h"
#include "Util.h"
#include "ItemManager.h"
#include "DSProtocol.h"
#if(BUNDLE_ITEM)
cBundleItem BundleItem;

cBundleItem::cBundleItem()
{
}

cBundleItem::~cBundleItem()
{
}

void cBundleItem::Read(char* path)
{
	CMemScript* lpMemScript = new CMemScript;

	if (lpMemScript == 0)
	{
		ErrorMessageBox(MEM_SCRIPT_ALLOC_ERROR, path);
		return;
	}

	if (lpMemScript->SetBuffer(path) == 0)
	{
		ErrorMessageBox(lpMemScript->GetLastError());
		delete lpMemScript;
		return;
	}

	this->m_List.clear();

	try
	{
		while (true)
		{
			if (lpMemScript->GetToken() == TOKEN_END)
			{
				break;
			}

			int section = lpMemScript->GetNumber();

			while (true)
			{
				if (section == 0)
				{
					if (strcmp("end", lpMemScript->GetAsString()) == 0)
					{
						break;
					}

					BI_Data info = {0};

					info.Index = lpMemScript->GetNumber() * 512 + lpMemScript->GetAsNumber();

					info.Level = lpMemScript->GetAsNumber();

					info.Max = lpMemScript->GetAsNumber();

					this->m_List.push_back(info);
				}
				else
				{
					break;
				}
			}
		}
	}
	catch (...)
	{
		ErrorMessageBox(lpMemScript->GetLastError());
	}

	delete lpMemScript;

	if (this->m_List.size())
	{
		for (int n = OBJECT_START_USER; n < MAX_OBJECT; n++)
		{
			if (gObjIsConnectedGP(n) != 0)
			{
				this->GCSendConfig(n);
			}
		}
	}
}

bool cBundleItem::IsBundleItemSlot(int aIndex, int SourceSlot)
{
	LPOBJ lpObj = &gObj[aIndex];

	if (gObjIsConnectedGP(aIndex) == 0)
	{
		return false;
	}

	CItem* lpItem = &lpObj->Inventory[SourceSlot];

	if (lpItem->IsItem() == 0)
	{
		return false;
	}

	return this->IsBundleItem(lpItem->m_Index, lpItem->m_Level);
}

bool cBundleItem::IsBundleItem(short Index, BYTE Level)
{
	for (auto info = m_List.begin(); info != m_List.end(); ++info)
	{
		if (info->Index == Index)
		{
			if (info->Level == -1 || info->Level == Level)
			{
				return true;
			}
		}
	}
	return false;
}

BYTE cBundleItem::GetBundleItemMaxStack(short Index, BYTE Level)
{
	for (auto info = m_List.begin(); info != m_List.end(); ++info)
	{
		if (info->Index == Index)
		{
			if (info->Level == -1 || info->Level == Level)
			{
				return info->Max;
			}
		}
	}
	return 1;
}

void cBundleItem::GCSendConfig(int aIndex)
{
	BYTE send[2048];

	PMSG_BUNDLE_ITEM_INFO_HEADER_SEND pMsg = { 0 };

	pMsg.header.set(0xF3, 0xF6, 0);

	int size = sizeof(pMsg);
	pMsg.Count = this->m_List.size();
	if (pMsg.Count <= 0)
	{
		return;
	}

	BI_Data info;

	for (int n = 0; n < this->m_List.size(); n++)
	{
		info.Index = this->m_List[n].Index;
		info.Level = this->m_List[n].Level;
		info.Max = this->m_List[n].Max;

		memcpy(&send[size], &info, sizeof(info));
		size += sizeof(info);
		pMsg.Count++;
	}

	pMsg.header.size[0] = SET_NUMBERHB(size);
	pMsg.header.size[1] = SET_NUMBERLB(size);

	memcpy(send, &pMsg, sizeof(pMsg));

	DataSend(aIndex, send, size);
}

void cBundleItem::OnCharacterLogin(int aIndex)
{
	this->GCSendConfig(aIndex);
}

bool cBundleItem::CGStackItem(int aIndex, int SourceSlot, int TargetSlot)
{
	LPOBJ lpObj = &gObj[aIndex];

	if (gObjIsConnectedGP(aIndex) == 0)
	{
		return false;
	}

	CItem* lpItem = &lpObj->Inventory[SourceSlot];

	if (lpItem->IsItem() == 0)
	{
		return false;
	}

	CItem* TargetItem = &lpObj->Inventory[TargetSlot];

	if (TargetItem->IsItem() == 0)
	{
		return false;
	}

	if (lpItem->m_Index != TargetItem->m_Index ||
		lpItem->m_Level != TargetItem->m_Level)
	{
		return false;
	}


	if (this->IsBundleItem(TargetItem->m_Index, TargetItem->m_Level))
	{
		BYTE Max = this->GetBundleItemMaxStack(TargetItem->m_Index, TargetItem->m_Level);
		WORD TempDur = (BYTE)lpItem->m_Durability + (BYTE)TargetItem->m_Durability;
		if (TargetItem->m_Durability >= Max)
		{
			return false;
		}

		if (TempDur > Max)
		{
			lpItem->m_Durability -= Max - TargetItem->m_Durability;
			TargetItem->m_Durability = Max;

			gItemManager.GCItemModifySend(aIndex, SourceSlot);
			gItemManager.GCItemModifySend(aIndex, TargetSlot);

			return true;
		}

		TargetItem->m_Durability = TempDur;

		gItemManager.InventoryDelItem(aIndex, SourceSlot);
		gItemManager.GCItemDeleteSend(aIndex, SourceSlot, 1);
		gItemManager.GCItemModifySend(aIndex, TargetSlot);

		return true;
	}
	return false;
}

bool cBundleItem::CGBundleItemUnmix(int aIndex, int SourceSlot)
{
	LPOBJ lpObj = &gObj[aIndex];

	if (gObjIsConnectedGP(aIndex) == 0)
	{
		return false;
	}

	CItem* lpItem = &lpObj->Inventory[SourceSlot];

	if (lpItem->IsItem() == 0)
	{
		return false;
	}

	if (!this->IsBundleItem(lpItem->m_Index, lpItem->m_Level) || lpItem->m_Durability <= 1)
	{
		return false;
	}

	if (!gItemManager.CheckItemInventorySpace(lpObj, 1, 1))
	{
		return false;
	}

	lpItem->m_Durability -= 1.0;

	BYTE ItemSocketOption[MAX_SOCKET_OPTION] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };

	GDCreateItemSend(lpObj->Index, 0xEB, 0, 0, lpItem->m_Index, lpItem->m_Level, 1, 0, 0, 0, -1, 0, 0, 0, 0, ItemSocketOption, 0xFF, 0);

	gItemManager.GCItemModifySend(aIndex, SourceSlot);

	return true;
}
bool cBundleItem::UseStack(int aIndex, int SourceSlot, int ValueUse)
{
	LPOBJ lpObj = &gObj[aIndex];

	if (gObjIsConnectedGP(aIndex) == 0)
	{
		return false;
	}

	CItem* lpItem = &lpObj->Inventory[SourceSlot];

	if (lpItem->IsItem() == 0)
	{
		return false;
	}

	if (lpItem->m_Durability < ValueUse)
	{
		return false;
	}

	lpItem->m_Durability -= ValueUse;

	if (lpItem->m_Durability < 1.0)
	{
		gItemManager.InventoryDelItem(aIndex, SourceSlot);
		gItemManager.GCItemDeleteSend(aIndex, SourceSlot, 1);
	}
	else
	{
		gItemManager.GCItemModifySend2(aIndex, SourceSlot, 2);
	}
	return true;
}
bool cBundleItem::InventoryInsertItemStack(LPOBJ lpObj, CItem* lpItem) // OK
{
	int MaxStack;

	if (lpItem->IsItem() == 0)
	{
		return 0;
	}

	if (!this->IsBundleItem(lpItem->m_Index, lpItem->m_Level))
	{
		return 0;
	}

	if ((MaxStack = this->GetBundleItemMaxStack(lpItem->m_Index, lpItem->m_Level)) <= 0)
	{
		return 0;
	}

	int MaxValue = gItemManager.GetInventoryMaxValue(lpObj);

	for (int n = INVENTORY_WEAR_SIZE; n < MaxValue; n++)
	{
		if (lpObj->Inventory[n].IsItem() != 0)
		{
			if (lpObj->Inventory[n].m_Index == lpItem->m_Index && lpObj->Inventory[n].m_Level == lpItem->m_Level)
			{
				if (lpObj->Inventory[n].m_Durability < MaxStack)
				{
					if (lpObj->Inventory[n].m_Durability + lpItem->m_Durability > MaxStack)
					{
						if (!gItemManager.CheckItemInventorySpace(lpObj, 1, 1))
						{
							return 0;
						}
						lpItem->m_Durability -= MaxStack - lpObj->Inventory[n].m_Durability;
						lpObj->Inventory[n].m_Durability = MaxStack;

						gItemManager.GCItemModifySend(lpObj->Index, n);

						GDCreateItemSend(lpObj->Index, 0xEB, 0, 0, lpItem->m_Index, (BYTE)lpItem->m_Level, lpItem->m_Durability, 0, 0, 0, lpObj->Index, 0, 0, 0, 0, 0, lpItem->m_SocketOptionBonus, 0);
						return 1;
					}
					else
					{
						lpObj->Inventory[n].m_Durability += lpItem->m_Durability;
						gItemManager.GCItemModifySend(lpObj->Index, n);
						return 1;
					}
				}
			}
		}
	}

	return 0;
}
#endif