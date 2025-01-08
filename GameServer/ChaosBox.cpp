// ChaosBox.cpp: implementation of the CChaosBox class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ChaosBox.h"
#include "CastleSiegeSync.h"
#include "CustomWingMix.h"
#include "DSProtocol.h"
#include "ItemBagManager.h"
#include "ItemOptionRate.h"
#include "JewelOfHarmonyOption.h"
#include "JewelOfHarmonyType.h"
#include "Log.h"
#include "LuckyItem.h"
#include "Notice.h"
#include "PentagramSystem.h"
#include "RandomManager.h"
#include "ServerInfo.h"
#include "SetItemType.h"
#include "Shop.h"
#include "SocketItemOption.h"
#include "SocketItemType.h"
#include "Util.h"
#include "CustomMix.h"
#include "Message.h"
#include "ItemManager.h"
#include "SPK_NewVip.h"
CChaosBox gChaosBox;
CChaosBox::CChaosBox() // OK
{
	this->m_SeniorMixLimitDay = 0;
	this->m_SeniorMixLimitMonth = 0;
	this->m_SeniorMixLimitYear = 0;
}

CChaosBox::~CChaosBox() // OK
{

}

void CChaosBox::ChaosBoxInit(LPOBJ lpObj) // OK
{
	for (int n = 0; n < CHAOS_BOX_SIZE; n++)
	{
		lpObj->ChaosBox[n].Clear();
		lpObj->ChaosBoxMap[n] = 0xFF;
	}
}

void CChaosBox::ChaosBoxItemDown(LPOBJ lpObj, int slot) // OK
{
	for (int n = 0; n < CHAOS_BOX_SIZE; n++)
	{
		if (n != slot)
		{
			lpObj->ChaosBox[n].Clear();
			lpObj->ChaosBoxMap[n] = 0xFF;
			continue;
		}

		CItem* lpItem = &lpObj->ChaosBox[n];

		lpItem->m_Level = 0;

		float dur = (float)gItemManager.GetItemDurability(lpItem->m_Index, lpItem->m_Level, lpItem->IsExcItem(), lpItem->IsSetItem());

		lpItem->m_Durability = dur*(lpItem->m_Durability / lpItem->m_BaseDurability);

		lpItem->Convert(lpItem->m_Index, lpItem->m_Option1, lpItem->m_Option2, lpItem->m_Option3, lpItem->m_NewOption, lpItem->m_SetOption, lpItem->m_JewelOfHarmonyOption, lpItem->m_ItemOptionEx, lpItem->m_SocketOption, lpItem->m_SocketOptionBonus);
	}
}

void CChaosBox::ChaosBoxItemKeep(LPOBJ lpObj, int slot) // OK
{
	for (int n = 0; n < CHAOS_BOX_SIZE; n++)
	{
		if (n != slot)
		{
			lpObj->ChaosBox[n].Clear();
			lpObj->ChaosBoxMap[n] = 0xFF;
			continue;
		}
	}
}

void CChaosBox::ChaosBoxItemSave(LPOBJ lpObj) // OK
{
	if (gObjInventoryCommit(lpObj->Index) != 0)
	{
		for (int n = 0; n < CHAOS_BOX_SIZE; n++)
		{
			if (lpObj->ChaosBox[n].IsItem() != 0)
			{
				gItemManager.InventoryInsertItem(lpObj->Index, lpObj->ChaosBox[n]);
			}
		}
	}
}

bool CChaosBox::GetTalismanOfLuckRate(LPOBJ lpObj, int* rate) // OK
{
	int count = 0;

	for (int n = 0; n < CHAOS_BOX_SIZE; n++)
	{
		if (lpObj->ChaosBox[n].IsItem() == 0)
		{
			continue;
		}

		if (lpObj->ChaosBox[n].m_Index == GET_ITEM(14, 53)) // Talisman of Luck
		{
			count += (int)lpObj->ChaosBox[n].m_Durability;
		}
	}

	(*rate) += count;

	return ((count>MAX_TALISMAN_OF_LUCK) ? 0 : 1);
}

bool CChaosBox::GetElementalTalismanOfLuckRate(LPOBJ lpObj, int* rate) // OK
{
	int count = 0;

	for (int n = 0; n < CHAOS_BOX_SIZE; n++)
	{
		if (lpObj->ChaosBox[n].IsItem() == 0)
		{
			continue;
		}

		if (lpObj->ChaosBox[n].m_Index == GET_ITEM(14, 189)) // Elemental Talisman of Luck
		{
			count += (int)lpObj->ChaosBox[n].m_Durability;
		}
	}

	(*rate) += count;

	return ((count>MAX_TALISMAN_OF_LUCK) ? 0 : 1);
}

void CChaosBox::ChaosItemMix(LPOBJ lpObj) // OK
{
	int ChaosCount = 0;
	int ItemCount = 0;
	int ItemMoney = 0;
	int ItemSlot = 0;
	for (int n = 0; n < CHAOS_BOX_SIZE; n++)
	{
		if (lpObj->ChaosBox[n].IsItem() == 0)
		{
			continue;
		}

		#if(BUNDLE_ITEM)
		if (lpObj->ChaosBox[n].m_Index == GET_ITEM(12, 15) && lpObj->ChaosBox[n].m_Durability == 1)
		{
			ChaosCount++;
			lpObj->ChaosBox[n].OldValue();
			ItemMoney += lpObj->ChaosBox[n].m_OldBuyMoney;
		}
		else if (lpObj->ChaosBox[n].m_Index == GET_ITEM(14, 13) && lpObj->ChaosBox[n].m_Durability == 1 || lpObj->ChaosBox[n].m_Index == GET_ITEM(14, 14) && lpObj->ChaosBox[n].m_Durability == 1)
		{
			ItemCount++;
			lpObj->ChaosBox[n].OldValue();
			ItemMoney += lpObj->ChaosBox[n].m_OldBuyMoney;
		}
		#else
		if (lpObj->ChaosBox[n].m_Index == GET_ITEM(12, 15))
		{
			ChaosCount++;
			lpObj->ChaosBox[n].OldValue();
			ItemMoney += lpObj->ChaosBox[n].m_OldBuyMoney;
		}
		else if (lpObj->ChaosBox[n].m_Index == GET_ITEM(14, 13) || lpObj->ChaosBox[n].m_Index == GET_ITEM(14, 14))
		{
			ItemCount++;
			lpObj->ChaosBox[n].OldValue();
			ItemMoney += lpObj->ChaosBox[n].m_OldBuyMoney;
		}
		#endif
		else if (lpObj->ChaosBox[n].m_Level >= 4 && lpObj->ChaosBox[n].m_Option3 >= 1)
		{
			ItemSlot = n;
			ItemCount++;
			ItemMoney += lpObj->ChaosBox[n].m_BuyMoney;
		}
	}

	if (ChaosCount == 0 || ItemCount == 0)
	{
		this->GCChaosMixSend(lpObj->Index, 7, 0);
		return;
	}

	if (gServerInfo.m_ChaosItemMixRate[lpObj->AccountLevel] == -1)
	{
		lpObj->ChaosSuccessRate = (ItemMoney / 20000);
	}
	else
	{
		lpObj->ChaosSuccessRate = gServerInfo.m_ChaosItemMixRate[lpObj->AccountLevel];
	}

	if (this->GetTalismanOfLuckRate(lpObj, &lpObj->ChaosSuccessRate) == 0)
	{
		this->GCChaosMixSend(lpObj->Index, 240, 0);
		return;
	}
	
	lpObj->ChaosSuccessRate = ((lpObj->ChaosSuccessRate>100) ? 100 : lpObj->ChaosSuccessRate);

	lpObj->ChaosMoney = lpObj->ChaosSuccessRate * 10000;

	int TaxMoney = (lpObj->ChaosMoney*gCastleSiegeSync.GetTaxRateChaos(lpObj->Index)) / 100;

	lpObj->ChaosMoney += TaxMoney;

	if (lpObj->Money < ((DWORD)lpObj->ChaosMoney))
	{
		this->GCChaosMixSend(lpObj->Index, 2, 0);
		return;
	}

	lpObj->Money -= lpObj->ChaosMoney;

	GCMoneySend(lpObj->Index, lpObj->Money);

	gCastleSiegeSync.AddTributeMoney(TaxMoney);
	
	CItem item = lpObj->ChaosBox[ItemSlot];
	if ((GetLargeRand() % 100) < lpObj->ChaosSuccessRate)
	{
		WORD ItemIndex = 0;
		BYTE ItemLevel = 0;
		BYTE ItemOption1 = 0;
		BYTE ItemOption2 = 0;
		BYTE ItemOption3 = 0;

		CRandomManager RandomManager;
		RandomManager.AddElement(GET_ITEM(2, 6), 1);
		RandomManager.AddElement(GET_ITEM(4, 6), 1);
		RandomManager.AddElement(GET_ITEM(5, 7), 1);
		RandomManager.GetRandomElement(&ItemIndex);
		gItemOptionRate.GetItemOption0(3, &ItemLevel);
		gItemOptionRate.GetItemOption1(3, &ItemOption1);
		gItemOptionRate.GetItemOption2(3, &ItemOption2);
		gItemOptionRate.GetItemOption3(3, &ItemOption3);

		GDCreateItemSend(lpObj->Index, 0xFF, 0, 0, ItemIndex, ItemLevel, 0, ItemOption1, ItemOption2, ItemOption3, -1, 0, 0, 0, 0, 0, 0xFF, 0);

		gLog.Output(LOG_CHAOS_MIX, "[Vũ khí chaos][Thành Công][%s][%s] Item: %s+ %d, %d  %d  %d,(Tỉ lệ: %d, Zen: %d) %s+%d,  %d %d %d %d %d, HC: %03d, 380: %03d, Socket: %03d, %03d, %03d, %03d, %03d, Serial: %08X)"
			, lpObj->Account, lpObj->Name,gItemManager.GetItemName(ItemIndex),ItemLevel,ItemOption1,ItemOption2,ItemOption3*4, lpObj->ChaosSuccessRate, lpObj->ChaosMoney
				, gItemManager.GetItemName(item.m_Index)
				, item.m_Level
				, item.m_Option1
				, item.m_Option2
				, item.m_Option3 * 4
				, item.m_NewOption
				, item.m_SetOption
				, item.m_JewelOfHarmonyOption
				, item.m_ItemOptionEx
				, item.m_SocketOption[0]
				, item.m_SocketOption[1]
				, item.m_SocketOption[2]
				, item.m_SocketOption[3]
				, item.m_SocketOption[4]
				, item.m_Serial);
				
				
		
	}
	else
	{
		this->ChaosBoxInit(lpObj);
		this->GCChaosBoxSend(lpObj, 0);
		this->GCChaosMixSend(lpObj->Index, 0, 0);

		//gLog.Output(LOG_CHAOS_MIX, "[ChaosItemMix][Xịt][%s][%s] - (ChaosSuccessRate: %d, ChaosMoney: %d)", lpObj->Account, lpObj->Name, lpObj->ChaosSuccessRate, lpObj->ChaosMoney);
				gLog.Output(LOG_CHAOS_MIX, "[Vũ khí chaos][Xịt][%s/%s]  %s+%d,  %d %d %d %d %d, HC: %03d, 380: %03d, Socket: %03d, %03d, %03d, %03d, %03d, Rate: %d%%, Money: %d, Serial: %08X)"
				, lpObj->Account
				, lpObj->Name
				, gItemManager.GetItemName(item.m_Index)
				, item.m_Level
				, item.m_Option1
				, item.m_Option2
				, item.m_Option3 * 4
				, item.m_NewOption
				, item.m_SetOption
				, item.m_JewelOfHarmonyOption
				, item.m_ItemOptionEx
				, item.m_SocketOption[0]
				, item.m_SocketOption[1]
				, item.m_SocketOption[2]
				, item.m_SocketOption[3]
				, item.m_SocketOption[4]
				, lpObj->ChaosSuccessRate
				, lpObj->ChaosMoney				
				, item.m_Serial);
		
	}
}

void CChaosBox::DevilSquareMix(LPOBJ lpObj) // OK
{
	int ChaosCount = 0;
	int EyeCount = 0;
	int EyeLevel = 0;
	int KeyCount = 0;
	int KeyLevel = 0;
	for (int n = 0; n < CHAOS_BOX_SIZE; n++)
	{
		if (lpObj->ChaosBox[n].IsItem() == 0)
		{
			continue;
		}
		#if(BUNDLE_ITEM)
		if (lpObj->ChaosBox[n].m_Index == GET_ITEM(12, 15) && lpObj->ChaosBox[n].m_Durability == 1)
		#else
		if (lpObj->ChaosBox[n].m_Index == GET_ITEM(12, 15))
		#endif
		{
			ChaosCount++;
		}
		else if (lpObj->ChaosBox[n].m_Index == GET_ITEM(14, 17))
		{
			EyeCount++;
			EyeLevel = lpObj->ChaosBox[n].m_Level;
		}
		else if (lpObj->ChaosBox[n].m_Index == GET_ITEM(14, 18))
		{
			KeyCount++;
			KeyLevel = lpObj->ChaosBox[n].m_Level;
		}
	}
	if (ChaosCount != 1 || EyeCount != 1 || KeyCount != 1 || EyeLevel != KeyLevel)
	{
		this->GCChaosMixSend(lpObj->Index, 7, 0);
		return;
	}
	if (EyeLevel > 7 || KeyLevel > 7)
	{
		this->GCChaosMixSend(lpObj->Index, 8, 0);
		return;
	}
	switch (EyeLevel)
	{
	case 1:
		lpObj->ChaosSuccessRate = gServerInfo.m_DevilSquareMixRate1[lpObj->AccountLevel];
		lpObj->ChaosMoney = 100000;
		break;
	case 2:
		lpObj->ChaosSuccessRate = gServerInfo.m_DevilSquareMixRate2[lpObj->AccountLevel];
		lpObj->ChaosMoney = 200000;
		break;
	case 3:
		lpObj->ChaosSuccessRate = gServerInfo.m_DevilSquareMixRate3[lpObj->AccountLevel];
		lpObj->ChaosMoney = 400000;
		break;
	case 4:
		lpObj->ChaosSuccessRate = gServerInfo.m_DevilSquareMixRate4[lpObj->AccountLevel];
		lpObj->ChaosMoney = 700000;
		break;
	case 5:
		lpObj->ChaosSuccessRate = gServerInfo.m_DevilSquareMixRate5[lpObj->AccountLevel];
		lpObj->ChaosMoney = 1100000;
		break;
	case 6:
		lpObj->ChaosSuccessRate = gServerInfo.m_DevilSquareMixRate6[lpObj->AccountLevel];
		lpObj->ChaosMoney = 1600000;
		break;
	case 7:
		lpObj->ChaosSuccessRate = gServerInfo.m_DevilSquareMixRate7[lpObj->AccountLevel];
		lpObj->ChaosMoney = 2000000;
		break;
	}

	if (this->GetTalismanOfLuckRate(lpObj, &lpObj->ChaosSuccessRate) == 0)
	{
		this->GCChaosMixSend(lpObj->Index, 240, 0);
		return;
	}
	
	lpObj->ChaosSuccessRate = ((lpObj->ChaosSuccessRate>100) ? 100 : lpObj->ChaosSuccessRate);
	int TaxMoney = (lpObj->ChaosMoney*gCastleSiegeSync.GetTaxRateChaos(lpObj->Index)) / 100;
	lpObj->ChaosMoney += TaxMoney;
	if (lpObj->Money < ((DWORD)lpObj->ChaosMoney))
	{
		this->GCChaosMixSend(lpObj->Index, 2, 0);
		return;
	}

	lpObj->Money -= lpObj->ChaosMoney;
	GCMoneySend(lpObj->Index, lpObj->Money);
	gCastleSiegeSync.AddTributeMoney(TaxMoney);
	int ItemSlot = 0;
	CItem item = lpObj->ChaosBox[ItemSlot];
	if ((GetLargeRand() % 100) < lpObj->ChaosSuccessRate)
	{
		GDCreateItemSend(lpObj->Index, 0xFF, 0, 0, GET_ITEM(14, 19), EyeLevel, 1, 0, 0, 0, -1, 0, 0, 0, 0, 0, 0xFF, 0);

		gLog.Output(LOG_CHAOS_MIX, "[DevilSquareMix][Thành Công][%s][%s] - (ChaosSuccessRate: %d, ChaosMoney: %d)", lpObj->Account, lpObj->Name, lpObj->ChaosSuccessRate, lpObj->ChaosMoney);
		
	}
	else
	{
		this->ChaosBoxInit(lpObj);
		this->GCChaosBoxSend(lpObj, 0);
		this->GCChaosMixSend(lpObj->Index, 0, 0);

		gLog.Output(LOG_CHAOS_MIX, "[DevilSquareMix][Xịt][%s][%s] - (ChaosSuccessRate: %d, ChaosMoney: %d)", lpObj->Account, lpObj->Name, lpObj->ChaosSuccessRate, lpObj->ChaosMoney);
		
	}
}

void CChaosBox::PlusItemLevelMix(LPOBJ lpObj, int type) // OK
{
	int ChaosCount = 0;
	int BlessCount = 0;
	int SoulCount = 0;
	int ItemCount = 0;
	int ItemSlot = 0;
	int ChaosAmulet = 0;
	int ElementalChaosAmulet = 0;
	for (int n = 0; n < CHAOS_BOX_SIZE; n++)
	{
		if (lpObj->ChaosBox[n].IsItem() == 0)
		{
			continue;
		}

		if (lpObj->ChaosBox[n].m_Index == GET_ITEM(12, 15) && lpObj->ChaosBox[n].m_Durability == 1 )
		{
			ChaosCount++;
		}
		else if (lpObj->ChaosBox[n].m_Index == GET_ITEM(14, 13) && lpObj->ChaosBox[n].m_Durability == 1)
		{
			BlessCount++;
		}
		else if (lpObj->ChaosBox[n].m_Index == GET_ITEM(14, 14) && lpObj->ChaosBox[n].m_Durability == 1)
		{
			SoulCount++;
		}
		else if (lpObj->ChaosBox[n].m_Index == GET_ITEM(14, 96))
		{
			ChaosAmulet++;
		}
		else if (lpObj->ChaosBox[n].m_Index == GET_ITEM(14, 190))
		{
			ElementalChaosAmulet++;
		}
		else if (lpObj->ChaosBox[n].m_Level == (9 + type))
		{
			ItemCount++;
			ItemSlot = n;
		}
	}

	if (ChaosCount != 1 || SoulCount < (type + 1) || BlessCount < (type + 1) || ItemCount != 1)
	{
		this->GCChaosMixSend(lpObj->Index, 7, 0);
		return;
	}

	switch (type)
	{
		case 0:	lpObj->ChaosSuccessRate = gServerInfo.m_PlusItemLevelMixRate1[lpObj->AccountLevel];		break;
		case 1:	lpObj->ChaosSuccessRate = gServerInfo.m_PlusItemLevelMixRate2[lpObj->AccountLevel];		break;
		case 2:	lpObj->ChaosSuccessRate = gServerInfo.m_PlusItemLevelMixRate3[lpObj->AccountLevel];		break;
		case 3:	lpObj->ChaosSuccessRate = gServerInfo.m_PlusItemLevelMixRate4[lpObj->AccountLevel];		break;
		case 4:	lpObj->ChaosSuccessRate = gServerInfo.m_PlusItemLevelMixRate5[lpObj->AccountLevel];		break;
		case 5:	lpObj->ChaosSuccessRate = gServerInfo.m_PlusItemLevelMixRate6[lpObj->AccountLevel];		break;
	}
	if (lpObj->ChaosBox[ItemSlot].m_Option2 != 0)
	{
		lpObj->ChaosSuccessRate += gServerInfo.m_AddLuckSuccessRate2[lpObj->AccountLevel];
	}
	if (lpObj->ChaosBox[ItemSlot].IsPentagramItem() == 0 && this->GetTalismanOfLuckRate(lpObj, &lpObj->ChaosSuccessRate) == 0)
	{
		this->GCChaosMixSend(lpObj->Index, 240, 0);
		return;
	}
	if (lpObj->ChaosBox[ItemSlot].IsPentagramItem() != 0 && this->GetElementalTalismanOfLuckRate(lpObj, &lpObj->ChaosSuccessRate) == 0)
	{
		this->GCChaosMixSend(lpObj->Index, 240, 0);
		return;
	}
	
	lpObj->ChaosSuccessRate = ((lpObj->ChaosSuccessRate>100) ? 100 : lpObj->ChaosSuccessRate);
	lpObj->ChaosMoney = 2000000 * (type + 1);
	int TaxMoney = (lpObj->ChaosMoney*gCastleSiegeSync.GetTaxRateChaos(lpObj->Index)) / 100;
	lpObj->ChaosMoney += TaxMoney;
	if (lpObj->Money < ((DWORD)lpObj->ChaosMoney))
	{
		this->GCChaosMixSend(lpObj->Index, 2, 0);
		return;
	}

	lpObj->Money -= lpObj->ChaosMoney;
	GCMoneySend(lpObj->Index, lpObj->Money);
	gCastleSiegeSync.AddTributeMoney(TaxMoney);
	if ((GetLargeRand() % 100) < lpObj->ChaosSuccessRate)
	{
		CItem item = lpObj->ChaosBox[ItemSlot];
		item.m_Level++;
		item.m_Durability = (float)gItemManager.GetItemDurability(item.m_Index, item.m_Level, item.IsExcItem(), item.IsSetItem());
		item.m_Durability = (item.m_Durability*lpObj->ChaosBox[ItemSlot].m_Durability) / item.m_BaseDurability;
		this->ChaosBoxInit(lpObj);
		gItemManager.ChaosBoxAddItem(lpObj->Index, item, 0);
		this->GCChaosMixSend(lpObj->Index, 1, &item);
		gObjCustomLogPlusChaosMix(lpObj, type, item.m_Index);
		gNotice.GCNoticeSendToAll(0, 0, 0, 0, 0, 0, gMessage.GetMessage(900), lpObj->Name, gItemManager.GetItemName(item.m_Index), item.m_Level);

		gLog.Output(LOG_CHAOS_MIX, "[LevelMix][Success][%s/%s] Item: %s, Level: %d, %d %d %d %d, Thần: %d, GC: %03d, 380: %03d, Sk: %03d, %03d, %03d, %03d, %03d, Type: %d, Rate: %d%%, Money: %d, Serial: %08X)"
				, lpObj->Account
				, lpObj->Name
				, gItemManager.GetItemName(item.m_Index)
				, item.m_Level
				, item.m_Option1
				, item.m_Option2
				, item.m_Option3 * 4
				, item.m_NewOption
				, item.m_SetOption
				, item.m_JewelOfHarmonyOption
				, item.m_ItemOptionEx
				, item.m_SocketOption[0]
				, item.m_SocketOption[1]
				, item.m_SocketOption[2]
				, item.m_SocketOption[3]
				, item.m_SocketOption[4]
				, type
				, lpObj->ChaosSuccessRate
				, lpObj->ChaosMoney				
				, item.m_Serial);
		
	}
	else
	{
		if (ChaosAmulet == 0 && ElementalChaosAmulet == 0)
		{
			#if(GAMESERVER_UPDATE>=701)
			gPentagramSystem.DelAllPentagramJewelInfo(lpObj, &lpObj->ChaosBox[ItemSlot], 0);
			#endif
			CItem item = lpObj->ChaosBox[ItemSlot];
			item.m_Level++;
			this->ChaosBoxInit(lpObj);
			this->GCChaosBoxSend(lpObj, 0);
			this->GCChaosMixSend(lpObj->Index, 0, 0);
			gObjCustomLogPlusChaosMix(lpObj, type, item.m_Index);
			gNotice.GCNoticeSendToAll(0, 0, 0, 0, 0, 0, gMessage.GetMessage(901), lpObj->Name, gItemManager.GetItemName(item.m_Index), item.m_Level);

				//gLog.Output(LOG_CHAOS_MIX, "[LevelMix][Thành Công][%s][%s] - (ChaosSuccessRate: %d, ChaosMoney: %d)", lpObj->Account, lpObj->Name, lpObj->ChaosSuccessRate, lpObj->ChaosMoney);
			gLog.Output(LOG_CHAOS_MIX, "[LevelMix][Fail][%s/%s] Item: %s, Level: %d, Skill: %d, Luck: %d, Tính năng: %d, Hoàn hảo: %d, Thần: %d, Hamorny: %03d, Item 380: %03d, Socket: %03d, %03d, %03d, %03d, %03d, Type: %d, Rate: %d%%, Money: %d, Amulet: %d , Serial: %08X)"
				, lpObj->Account
				, lpObj->Name
				, gItemManager.GetItemName(item.m_Index)
				, item.m_Level
				, item.m_Option1
				, item.m_Option2
				, item.m_Option3 * 4
				, item.m_NewOption
				, item.m_SetOption
				, item.m_JewelOfHarmonyOption
				, item.m_ItemOptionEx
				, item.m_SocketOption[0]
				, item.m_SocketOption[1]
				, item.m_SocketOption[2]
				, item.m_SocketOption[3]
				, item.m_SocketOption[4]
				, type
				, lpObj->ChaosSuccessRate
				, lpObj->ChaosMoney
				, (ChaosAmulet + ElementalChaosAmulet)
				, item.m_Serial);
				
		}
		else
		{

			CItem item = lpObj->ChaosBox[ItemSlot];
			item.m_Level++;
			this->ChaosBoxItemDown(lpObj, ItemSlot);
			this->GCChaosBoxSend(lpObj, 0);
			this->GCChaosMixSend(lpObj->Index, 0, 0);
			gObjCustomLogPlusChaosMix(lpObj, type, item.m_Index);
			gNotice.GCNoticeSendToAll(0, 0, 0, 0, 0, 0, gMessage.GetMessage(901), lpObj->Name, gItemManager.GetItemName(item.m_Index), item.m_Level);

				//gLog.Output(LOG_CHAOS_MIX, "[LevelMix][Fail][%s][%s] - (ChaosSuccessRate: %d, ChaosMoney: %d)", lpObj->Account, lpObj->Name, lpObj->ChaosSuccessRate, lpObj->ChaosMoney);
			gLog.Output(LOG_CHAOS_MIX, "[LevelMix][Fail][%s/%s] Item: %s, Level: %d, Skill: %d, Luck: %d, Tính năng: %d, Hoàn hảo: %d, Thần: %d, Hamorny: %03d, Item 380: %03d, Socket: %03d, %03d, %03d, %03d, %03d, Type: %d, Rate: %d, Money: %d%%, Amulet: %d , Serial: %08X)"
				, lpObj->Account
				, lpObj->Name
				, gItemManager.GetItemName(item.m_Index)
				, item.m_Level
				, item.m_Option1
				, item.m_Option2
				, item.m_Option3 * 4
				, item.m_NewOption
				, item.m_SetOption
				, item.m_JewelOfHarmonyOption
				, item.m_ItemOptionEx
				, item.m_SocketOption[0]
				, item.m_SocketOption[1]
				, item.m_SocketOption[2]
				, item.m_SocketOption[3]
				, item.m_SocketOption[4]
				, type
				, lpObj->ChaosSuccessRate
				, lpObj->ChaosMoney
				, (ChaosAmulet + ElementalChaosAmulet)
				, item.m_Serial);
		}
	}
}
 
void CChaosBox::DinorantMix(LPOBJ lpObj) // OK
{
	int ChaosCount = 0;
	int UniriaCount = 0;
	for (int n = 0; n < CHAOS_BOX_SIZE; n++)
	{
		if (lpObj->ChaosBox[n].IsItem() == 0)
		{
			continue;
		}
		#if(BUNDLE_ITEM)
		if (lpObj->ChaosBox[n].m_Index == GET_ITEM(12, 15) && lpObj->ChaosBox[n].m_Durability == 1)
		#else
		if (lpObj->ChaosBox[n].m_Index == GET_ITEM(12, 15))
		#endif
		{
			ChaosCount++;
		}
		else if (lpObj->ChaosBox[n].m_Index == GET_ITEM(13, 2) && lpObj->ChaosBox[n].m_Durability == 255)
		{
			UniriaCount++;
		}
	}

	if (ChaosCount != 1 || UniriaCount != 10)
	{
		this->GCChaosMixSend(lpObj->Index, 7, 0);
		return;
	}

	lpObj->ChaosSuccessRate = gServerInfo.m_DinorantMixRate[lpObj->AccountLevel];

	if (this->GetTalismanOfLuckRate(lpObj, &lpObj->ChaosSuccessRate) == 0)
	{
		this->GCChaosMixSend(lpObj->Index, 240, 0);
		return;
	}
	
	lpObj->ChaosSuccessRate = ((lpObj->ChaosSuccessRate>100) ? 100 : lpObj->ChaosSuccessRate);
	lpObj->ChaosMoney = 500000;
	int TaxMoney = (lpObj->ChaosMoney*gCastleSiegeSync.GetTaxRateChaos(lpObj->Index)) / 100;
	lpObj->ChaosMoney += TaxMoney;
	if (lpObj->Money < ((DWORD)lpObj->ChaosMoney))
	{
		this->GCChaosMixSend(lpObj->Index, 2, 0);
		return;
	}

	lpObj->Money -= lpObj->ChaosMoney;
	GCMoneySend(lpObj->Index, lpObj->Money);
	gCastleSiegeSync.AddTributeMoney(TaxMoney);
	int ItemSlot = 0;
	CItem item = lpObj->ChaosBox[ItemSlot];
	if ((GetLargeRand() % 100) < lpObj->ChaosSuccessRate)
	{
		WORD ItemIndex = GET_ITEM(13, 3);
		BYTE ItemNewOption = 0;
		gItemOptionRate.GetItemOption4(3, &ItemNewOption);
		gItemOptionRate.MakeNewOption(ItemIndex, ItemNewOption, &ItemNewOption);
		GDCreateItemSend(lpObj->Index, 0xFF, 0, 0, ItemIndex, 0, 255, 0, 0, 0, -1, ItemNewOption, 0, 0, 0, 0, 0xFF, 0);
		gNotice.GCNoticeSendToAll(0, 0, 0, 0, 0, 0, "%s đã nâng cấp thành công Kỳ Lân", lpObj->Name);

			gLog.Output(LOG_CHAOS_MIX, "[Kỳ lân][Thành Công][%s][%s] - (ChaosSuccessRate: %d, ChaosMoney: %d)", lpObj->Account, lpObj->Name, lpObj->ChaosSuccessRate, lpObj->ChaosMoney);
			
	}
	else
	{
		this->ChaosBoxInit(lpObj);
		this->GCChaosBoxSend(lpObj, 0);
		this->GCChaosMixSend(lpObj->Index, 0, 0);
		gNotice.GCNoticeSendToAll(0, 0, 0, 0, 0, 0, gMessage.GetMessage(893), lpObj->Name);
		
		gLog.Output(LOG_CHAOS_MIX, "[Kỳ lân][Xịt][%s][%s] - (ChaosSuccessRate: %d, ChaosMoney: %d)", lpObj->Account, lpObj->Name, lpObj->ChaosSuccessRate, lpObj->ChaosMoney);
	
	}
}

void CChaosBox::FruitMix(LPOBJ lpObj) // OK
{
	int ChaosCount = 0;
	int CreationCount = 0;

	for (int n = 0; n < CHAOS_BOX_SIZE; n++)
	{
		if (lpObj->ChaosBox[n].IsItem() == 0)
		{
			continue;
		}
		if (lpObj->ChaosBox[n].m_Index == GET_ITEM(12, 15) && lpObj->ChaosBox[n].m_Durability == 1 )
		{
			ChaosCount++;
		}
		else if (lpObj->ChaosBox[n].m_Index == GET_ITEM(14, 22) && lpObj->ChaosBox[n].m_Durability == 1)
		{
			CreationCount++;
		}
	}
	if (ChaosCount != 1 || CreationCount != 1)
	{
		this->GCChaosMixSend(lpObj->Index, 7, 0);
		return;
	}
	lpObj->ChaosSuccessRate = gServerInfo.m_FruitMixRate[lpObj->AccountLevel];
	if (this->GetTalismanOfLuckRate(lpObj, &lpObj->ChaosSuccessRate) == 0)
	{
		this->GCChaosMixSend(lpObj->Index, 240, 0);
		return;
	}
	
	lpObj->ChaosSuccessRate = ((lpObj->ChaosSuccessRate>100) ? 100 : lpObj->ChaosSuccessRate);
	lpObj->ChaosMoney = 3000000;
	int TaxMoney = (lpObj->ChaosMoney*gCastleSiegeSync.GetTaxRateChaos(lpObj->Index)) / 100;
	lpObj->ChaosMoney += TaxMoney;
	if (lpObj->Money < ((DWORD)lpObj->ChaosMoney))
	{
		this->GCChaosMixSend(lpObj->Index, 2, 0);
		return;
	}

	lpObj->Money -= lpObj->ChaosMoney;
	GCMoneySend(lpObj->Index, lpObj->Money);
	gCastleSiegeSync.AddTributeMoney(TaxMoney);
	int ItemSlot = 0;
	CItem item = lpObj->ChaosBox[ItemSlot];
	if ((GetLargeRand() % 100) < lpObj->ChaosSuccessRate)
	{
		BYTE ItemLevel = GetLargeRand() % 5;

		GDCreateItemSend(lpObj->Index, 0xFF, 0, 0, GET_ITEM(13, 15), ItemLevel, 0, 0, 0, 0, -1, 0, 0, 0, 0, 0, 0xFF, 0);
		
		gLog.Output(LOG_CHAOS_MIX, "[Quả][Thành Công][%s][%s] - Item: %s-%d(ChaosSuccessRate: %d, ChaosMoney: %d)", lpObj->Account, lpObj->Name,gItemManager.GetItemName(GET_ITEM(13, 15)),ItemLevel, lpObj->ChaosSuccessRate, lpObj->ChaosMoney);
		
	}
	else
	{
		this->ChaosBoxInit(lpObj);
		this->GCChaosBoxSend(lpObj, 0);
		this->GCChaosMixSend(lpObj->Index, 0, 0);
		
		gLog.Output(LOG_CHAOS_MIX, "[Quả][Xịt][%s][%s] - (ChaosSuccessRate: %d, ChaosMoney: %d)", lpObj->Account, lpObj->Name, lpObj->ChaosSuccessRate, lpObj->ChaosMoney);
		
	}
}

void CChaosBox::Wing2Mix(LPOBJ lpObj, int type) // OK
{
	int ChaosCount = 0;
	int FeatherCount = 0;
	int SleeveCount = 0;
	int WingItemCount = 0;
	int WingItemMoney = 0;
	int ItemCount = 0;
	int ItemMoney = 0;
	int ItemSlot = 0;
	int TalismanOfWingType = 0;
	for (int n = 0; n < CHAOS_BOX_SIZE; n++)
	{
		if (lpObj->ChaosBox[n].IsItem() == 0)
		{
			continue;
		}
		if (lpObj->ChaosBox[n].m_Index == GET_ITEM(12, 15)  && lpObj->ChaosBox[n].m_Durability == 1)
		{
			ChaosCount++;
		}
		else if (lpObj->ChaosBox[n].m_Index == GET_ITEM(13, 14) && lpObj->ChaosBox[n].m_Level == 0)
		{
			FeatherCount++;
		}
		else if (lpObj->ChaosBox[n].m_Index == GET_ITEM(13, 14) && lpObj->ChaosBox[n].m_Level == 1)
		{
			SleeveCount++;
		}
		else if ((lpObj->ChaosBox[n].m_Index >= GET_ITEM(12, 0) && lpObj->ChaosBox[n].m_Index <= GET_ITEM(12, 2)) || lpObj->ChaosBox[n].m_Index == GET_ITEM(12, 41))
		{
			WingItemCount++;
			WingItemMoney += lpObj->ChaosBox[n].m_BuyMoney;	
		}
		else if (lpObj->ChaosBox[n].IsExcItem() != 0 && lpObj->ChaosBox[n].m_Level >= 4)
		{
			ItemCount++;
			ItemSlot = n;
			ItemMoney += lpObj->ChaosBox[n].m_BuyMoney;
		}
		else if (lpObj->ChaosBox[n].m_Index >= GET_ITEM(13, 88) && lpObj->ChaosBox[n].m_Index >= GET_ITEM(13, 92))
		{
			TalismanOfWingType = lpObj->ChaosBox[n].m_Index - GET_ITEM(13, 87);
		}
	}

	if (ChaosCount != 1 || (type == 0 && FeatherCount != 1) || (type == 1 && SleeveCount != 1) || WingItemCount != 1)
	{
		this->GCChaosMixSend(lpObj->Index, 7, 0);
		return;
	}

	if (gServerInfo.m_Wing2MixRate[lpObj->AccountLevel] == -1)
	{
		lpObj->ChaosSuccessRate = (WingItemMoney / 4000000) + (ItemMoney / 40000);
	}
	else
	{
		lpObj->ChaosSuccessRate = gServerInfo.m_Wing2MixRate[lpObj->AccountLevel];
	}

	if (this->GetTalismanOfLuckRate(lpObj, &lpObj->ChaosSuccessRate) == 0)
	{
		this->GCChaosMixSend(lpObj->Index, 240, 0);
		return;
	}

	if (gServerInfo.m_Wing2MixRate[lpObj->AccountLevel] == -1)
	{
		lpObj->ChaosSuccessRate = ((lpObj->ChaosSuccessRate>90) ? 90 : lpObj->ChaosSuccessRate);
	}
	else
	{
		
		lpObj->ChaosSuccessRate = ((lpObj->ChaosSuccessRate>100) ? 100 : lpObj->ChaosSuccessRate);
	}
	
	if(lpObj->ChaosBox[ItemSlot].m_Level >=4 && lpObj->ChaosBox[ItemSlot].m_NewOption >= 1 )
	{
		lpObj->ChaosSuccessRate = lpObj->ChaosSuccessRate + 10;
	}

	lpObj->ChaosMoney = 5000000;
	int TaxMoney = (lpObj->ChaosMoney*gCastleSiegeSync.GetTaxRateChaos(lpObj->Index)) / 100;
	lpObj->ChaosMoney += TaxMoney;
	if (lpObj->Money < ((DWORD)lpObj->ChaosMoney))
	{
		this->GCChaosMixSend(lpObj->Index, 2, 0);
		return;
	}

	lpObj->Money -= lpObj->ChaosMoney;
	GCMoneySend(lpObj->Index, lpObj->Money);
	gCastleSiegeSync.AddTributeMoney(TaxMoney);
	CItem item = lpObj->ChaosBox[ItemSlot];
	if ((GetLargeRand() % 100) < lpObj->ChaosSuccessRate)
	{
		if (type == 0)
		{
			WORD ItemIndex = 0;
			BYTE ItemOption2 = 0;
			BYTE ItemOption3 = 0;
			BYTE ItemNewOption = 0;
			CRandomManager RandomManager;
			RandomManager.AddElement(GET_ITEM(12, 3), 1);
			RandomManager.AddElement(GET_ITEM(12, 4), 1);
			RandomManager.AddElement(GET_ITEM(12, 5), 1);
			RandomManager.AddElement(GET_ITEM(12, 6), 1);
			if (gServerInfo.m_DisableWingMixSU != 1)
			{
				RandomManager.AddElement(GET_ITEM(12, 42), 1);
			}
			RandomManager.GetRandomElement(&ItemIndex);
			switch (TalismanOfWingType)
			{
			case 1:				ItemIndex = GET_ITEM(12, 5);		break;
			case 2:				ItemIndex = GET_ITEM(12, 4);		break;
			case 3:				ItemIndex = GET_ITEM(12, 3);		break;
			case 4:
				if (gServerInfo.m_DisableWingMixSU != 1)
				{
					ItemIndex = GET_ITEM(12, 42);
				}
				else
				{
					ItemIndex = GET_ITEM(12, 5);
				}
				break;
			case 5:		ItemIndex = GET_ITEM(12, 6);	break;
			}

			gItemOptionRate.GetItemOption2(4, &ItemOption2);
			gItemOptionRate.GetItemOption3(4, &ItemOption3);
			gItemOptionRate.GetItemOption4(4, &ItemNewOption);
			gItemOptionRate.MakeNewOption(ItemIndex, ItemNewOption, &ItemNewOption);
			GDCreateItemSend(lpObj->Index, 0xFF, 0, 0, ItemIndex, 0, 0, 0, ItemOption2, ItemOption3, -1, (ItemNewOption + (32 * (GetLargeRand() % 2))), 0, 0, 0, 0, 0xFF, 0);
			gNotice.GCNoticeSendToAll(0, 0, 0, 0, 0, 0, gMessage.GetMessage(902), lpObj->Name, gItemManager.GetItemName(ItemIndex));
			
			gLog.Output(LOG_CHAOS_MIX, "[Wing2Mix - OK][%s][%s] Item: %s, %d %d %d | Tỉ lệ: %d, Zen: %d  )",
			lpObj->Account, 
			lpObj->Name,
			gItemManager.GetItemName(ItemIndex),
			ItemOption2,
			ItemOption3, 
			ItemNewOption,
			lpObj->ChaosSuccessRate, 
			lpObj->ChaosMoney);
			
		}
		else
		{
			WORD ItemIndex = 0;
			BYTE ItemOption2 = 0;
			BYTE ItemOption3 = 0;
			BYTE ItemNewOption = 0;
			CRandomManager RandomManager;
			#if(GAMESERVER_UPDATE>=601)
			if (gServerInfo.m_DisableWingMixRF != 1)
			{
				RandomManager.AddElement(GET_ITEM(12, 49), 1);
			}
			#endif
			if (gServerInfo.m_DisableWingMixDL != 1)
			{
				RandomManager.AddElement(GET_ITEM(13, 30), 1);
			}
			else
			{
				RandomManager.AddElement(GET_ITEM(12, 3), 1);
			}

			RandomManager.GetRandomElement(&ItemIndex);
			gItemOptionRate.GetItemOption2(4, &ItemOption2);
			gItemOptionRate.GetItemOption3(4, &ItemOption3);
			gItemOptionRate.GetItemOption4(5, &ItemNewOption);
			gItemOptionRate.MakeNewOption(ItemIndex, ItemNewOption, &ItemNewOption);
			GDCreateItemSend(lpObj->Index, 0xFF, 0, 0, ItemIndex, 0, 0, 0, ItemOption2, ItemOption3, -1, (ItemNewOption + 32), 0, 0, 0, 0, 0xFF, 0);
			gNotice.GCNoticeSendToAll(0, 0, 0, 0, 0, 0, gMessage.GetMessage(902), lpObj->Name, gItemManager.GetItemName(ItemIndex));

			gLog.Output(LOG_CHAOS_MIX, "[Wing2Mix - OK][%s][%s] Item: %s, %d %d %d | Tỉ lệ: %d, Zen: %d)",
			lpObj->Account, 
			lpObj->Name,
			gItemManager.GetItemName(ItemIndex),
			ItemOption2,
			ItemOption3, 
			ItemNewOption,
			lpObj->ChaosSuccessRate, 
			lpObj->ChaosMoney);
		}
	}
	else
	{
		this->ChaosBoxInit(lpObj);
		this->GCChaosBoxSend(lpObj, 0);
		this->GCChaosMixSend(lpObj->Index, 0, 0);
		gNotice.GCNoticeSendToAll(0, 0, 0, 0, 0, 0, gMessage.GetMessage(890), lpObj->Name);
		
		gLog.Output(LOG_CHAOS_MIX, "[Wing2Mix - OK][%s][%s] Item: %s | Tỉ lệ: %d, Zen: %d)",
		lpObj->Account, 
		lpObj->Name,
		gItemManager.GetItemName(ItemIndex),
		lpObj->ChaosSuccessRate, 
		lpObj->ChaosMoney);
	}
}

void CChaosBox::BloodCastleMix(LPOBJ lpObj) // OK
{
	int ChaosCount = 0;
	int ScrollCount = 0;
	int ScrollLevel = 0;
	int BoneCount = 0;
	int BoneLevel = 0;
	for (int n = 0; n < CHAOS_BOX_SIZE; n++)
	{
		if (lpObj->ChaosBox[n].IsItem() == 0)
		{
			continue;
		}
		#if(BUNDLE_ITEM)
		if (lpObj->ChaosBox[n].m_Index == GET_ITEM(12, 15) && lpObj->ChaosBox[n].m_Durability == 1)
		#else
		if (lpObj->ChaosBox[n].m_Index == GET_ITEM(12, 15))
		#endif
		{
			ChaosCount++;
		}
		else if (lpObj->ChaosBox[n].m_Index == GET_ITEM(13, 16))
		{
			ScrollCount++;
			ScrollLevel = lpObj->ChaosBox[n].m_Level;
		}
		else if (lpObj->ChaosBox[n].m_Index == GET_ITEM(13, 17))
		{
			BoneCount++;
			BoneLevel = lpObj->ChaosBox[n].m_Level;
		}
	}

	if (ChaosCount != 1 || ScrollCount != 1 || BoneCount != 1 || ScrollLevel != BoneLevel)
	{
		this->GCChaosMixSend(lpObj->Index, 7, 0);
		return;
	}

	if (ScrollLevel > 8 || BoneLevel > 8)
	{
		this->GCChaosMixSend(lpObj->Index, 8, 0);
		return;
	}
	
	//+BGetDataNewVip->RateMix;
	switch (ScrollLevel)
	{
		case 1:
			lpObj->ChaosSuccessRate = gServerInfo.m_BloodCastleMixRate1[lpObj->AccountLevel];
			lpObj->ChaosMoney = 50000;		break;
		case 2:
			lpObj->ChaosSuccessRate = gServerInfo.m_BloodCastleMixRate2[lpObj->AccountLevel];
			lpObj->ChaosMoney = 80000;		break;
		case 3:
			lpObj->ChaosSuccessRate = gServerInfo.m_BloodCastleMixRate3[lpObj->AccountLevel];
			lpObj->ChaosMoney = 150000;		break;
		case 4:
			lpObj->ChaosSuccessRate = gServerInfo.m_BloodCastleMixRate4[lpObj->AccountLevel];
			lpObj->ChaosMoney = 250000;		break;
		case 5:
			lpObj->ChaosSuccessRate = gServerInfo.m_BloodCastleMixRate5[lpObj->AccountLevel];
			lpObj->ChaosMoney = 400000;		break;
		case 6:
			lpObj->ChaosSuccessRate = gServerInfo.m_BloodCastleMixRate6[lpObj->AccountLevel];
			lpObj->ChaosMoney = 600000;		break;
		case 7:
			lpObj->ChaosSuccessRate = gServerInfo.m_BloodCastleMixRate7[lpObj->AccountLevel];
			lpObj->ChaosMoney = 850000;		break;
		case 8:
			lpObj->ChaosSuccessRate = gServerInfo.m_BloodCastleMixRate8[lpObj->AccountLevel];
			lpObj->ChaosMoney = 1050000;		break;
	}

	if (this->GetTalismanOfLuckRate(lpObj, &lpObj->ChaosSuccessRate) == 0)
	{
		this->GCChaosMixSend(lpObj->Index, 240, 0);
		return;
	}

	lpObj->ChaosSuccessRate = ((lpObj->ChaosSuccessRate>100) ? 100 : lpObj->ChaosSuccessRate);
	int TaxMoney = (lpObj->ChaosMoney*gCastleSiegeSync.GetTaxRateChaos(lpObj->Index)) / 100;
	lpObj->ChaosMoney += TaxMoney;
	if (lpObj->Money < ((DWORD)lpObj->ChaosMoney))
	{
		this->GCChaosMixSend(lpObj->Index, 2, 0);
		return;
	}
	lpObj->Money -= lpObj->ChaosMoney;
	GCMoneySend(lpObj->Index, lpObj->Money);
	gCastleSiegeSync.AddTributeMoney(TaxMoney);
	int ItemSlot = 0;
	CItem item = lpObj->ChaosBox[ItemSlot];
	if ((GetLargeRand() % 100) <= lpObj->ChaosSuccessRate)
	{
		GDCreateItemSend(lpObj->Index, 0xFF, 0, 0, GET_ITEM(13, 18), ScrollLevel, 1, 0, 0, 0, -1, 0, 0, 0, 0, 0, 0xFF, 0);
		
		gLog.Output(LOG_CHAOS_MIX, "[BloodCastleMix][Thành Công][%s][%s] Item %s+%d(ChaosSuccessRate: %d, ChaosMoney: %d)", lpObj->Account, lpObj->Name, gItemManager.GetItemName(GET_ITEM(13, 18)), ScrollLevel,lpObj->ChaosSuccessRate, lpObj->ChaosMoney);
			
	}
	else
	{
		this->ChaosBoxInit(lpObj);
		this->GCChaosBoxSend(lpObj, 0);
		this->GCChaosMixSend(lpObj->Index, 0, 0);
		
		gLog.Output(LOG_CHAOS_MIX, "[BloodCastleMix][Xịt][%s][%s] - (ChaosSuccessRate: %d, ChaosMoney: %d)", lpObj->Account, lpObj->Name, lpObj->ChaosSuccessRate, lpObj->ChaosMoney);
	
	}
}

void CChaosBox::Wing1Mix(LPOBJ lpObj) // OK
{
	int ChaosCount = 0;
	int ChaosItem = 0;
	int ItemCount = 0;
	int ItemMoney = 0;
	int ItemSlot = 0;
	int TalismanOfWingType = 0;

	for (int n = 0; n < CHAOS_BOX_SIZE; n++)
	{
		if (lpObj->ChaosBox[n].IsItem() == 0)
		{
			continue;
		}
	
		if (lpObj->ChaosBox[n].m_Index == GET_ITEM(12, 15) && lpObj->ChaosBox[n].m_Durability == 1)		
		{
			ChaosCount++;
			lpObj->ChaosBox[n].OldValue();
			ItemMoney += lpObj->ChaosBox[n].m_OldBuyMoney;
		}
		else if (lpObj->ChaosBox[n].m_Index == GET_ITEM(14, 13)  && lpObj->ChaosBox[n].m_Durability == 1 || lpObj->ChaosBox[n].m_Index == GET_ITEM(14, 14) && lpObj->ChaosBox[n].m_Durability == 1)
		{
			ItemCount++;
			lpObj->ChaosBox[n].OldValue();
			ItemMoney += lpObj->ChaosBox[n].m_OldBuyMoney;
		}

		else if ((lpObj->ChaosBox[n].m_Index == GET_ITEM(2, 6) || lpObj->ChaosBox[n].m_Index == GET_ITEM(4, 6) || lpObj->ChaosBox[n].m_Index == GET_ITEM(5, 7)) && lpObj->ChaosBox[n].m_Level >= 4 && lpObj->ChaosBox[n].m_Option3 >= 1)
		{
			ItemSlot = n;
			ChaosItem++;
			ItemCount++;
			ItemMoney += lpObj->ChaosBox[n].m_BuyMoney;
		}
		else if (lpObj->ChaosBox[n].m_Level >= 4 && lpObj->ChaosBox[n].m_Option3 >= 1)
		{
			ItemCount++;
			ItemMoney += lpObj->ChaosBox[n].m_BuyMoney;
		}
		else if (lpObj->ChaosBox[n].m_Index >= GET_ITEM(13, 83) && lpObj->ChaosBox[n].m_Index >= GET_ITEM(13, 86))
		{
			TalismanOfWingType = lpObj->ChaosBox[n].m_Index - GET_ITEM(13, 82);
		}
	}

	if (ChaosCount == 0 || ChaosItem == 0)
	{
		this->GCChaosMixSend(lpObj->Index, 7, 0);
		return;
	}
	if (gServerInfo.m_Wing1MixRate[lpObj->AccountLevel] == -1)
	{
		lpObj->ChaosSuccessRate = (ItemMoney / 20000);
	}
	else
	{
		lpObj->ChaosSuccessRate = gServerInfo.m_Wing1MixRate[lpObj->AccountLevel];
	}

	if (this->GetTalismanOfLuckRate(lpObj, &lpObj->ChaosSuccessRate) == 0)
	{
		this->GCChaosMixSend(lpObj->Index, 240, 0);
		return;
	}
	
	lpObj->ChaosSuccessRate = ((lpObj->ChaosSuccessRate>100) ? 100 : lpObj->ChaosSuccessRate);
	lpObj->ChaosMoney = lpObj->ChaosSuccessRate * 10000;
	int TaxMoney = (lpObj->ChaosMoney*gCastleSiegeSync.GetTaxRateChaos(lpObj->Index)) / 100;
	lpObj->ChaosMoney += TaxMoney;
	if (lpObj->Money < ((DWORD)lpObj->ChaosMoney))
	{
		this->GCChaosMixSend(lpObj->Index, 2, 0);
		return;
	}
	lpObj->Money -= lpObj->ChaosMoney;
	GCMoneySend(lpObj->Index, lpObj->Money);
	gCastleSiegeSync.AddTributeMoney(TaxMoney);
	
	CItem item = lpObj->ChaosBox[ItemSlot];
	if ((GetLargeRand() % 100) < lpObj->ChaosSuccessRate)
	{
		WORD ItemIndex = 0;
		BYTE ItemOption2 = 0;
		BYTE ItemOption3 = 0;
		CRandomManager RandomManager;
		RandomManager.AddElement(GET_ITEM(12, 0), 1);
		RandomManager.AddElement(GET_ITEM(12, 1), 1);
		RandomManager.AddElement(GET_ITEM(12, 2), 1);
		if (gServerInfo.m_DisableWingMixSU != 1)
		{
			RandomManager.AddElement(GET_ITEM(12, 41), 1);
		}
		RandomManager.GetRandomElement(&ItemIndex);
		switch (TalismanOfWingType)
		{
		case 1:			ItemIndex = GET_ITEM(12, 2);			break;
		case 2:			ItemIndex = GET_ITEM(12, 1);			break;
		case 3:			ItemIndex = GET_ITEM(12, 0);			break;
		case 4:
			if (gServerInfo.m_DisableWingMixSU != 1)
			{
				ItemIndex = GET_ITEM(12, 41); //asa de SU
			}
			else
			{
				ItemIndex = GET_ITEM(12, 0); // Asa de SM
			}
			break;
		}
		gItemOptionRate.GetItemOption2(5, &ItemOption2);
		gItemOptionRate.GetItemOption3(5, &ItemOption3);
		GDCreateItemSend(lpObj->Index, 0xFF, 0, 0, ItemIndex, 0, 0, 0, ItemOption2, ItemOption3, -1, 0, 0, 0, 0, 0, 0xFF, 0);
		gNotice.GCNoticeSendToAll(0, 0, 0, 0, 0, 0, gMessage.GetMessage(902), lpObj->Name, gItemManager.GetItemName(ItemIndex));		
		#if(LOG_NEW_ITEM)
		gLog.Output(LOG_CHAOS_MIX, "[Wing1Mix - OK][%s][%s] Item: %s, %d %d | Tỉ lệ: %d, Zen: %d)",
			lpObj->Account, 
			lpObj->Name,
			gItemManager.GetItemName(ItemIndex),
			ItemOption2,
			ItemOption3, 
			lpObj->ChaosSuccessRate, 
			lpObj->ChaosMoney);
		#else
		gLog.Output(LOG_CHAOS_MIX, "[Wing1Mix][Success][%s][%s] - (ChaosSuccessRate: %d, ChaosMoney: %d)", lpObj->Account, lpObj->Name, lpObj->ChaosSuccessRate, lpObj->ChaosMoney);
		#endif
	}
	else
	{
		this->ChaosBoxInit(lpObj);
		this->GCChaosBoxSend(lpObj, 0);
		this->GCChaosMixSend(lpObj->Index, 0, 0);
		gNotice.GCNoticeSendToAll(0, 0, 0, 0, 0, 0, gMessage.GetMessage(889), lpObj->Name);
		#if(LOG_NEW_ITEM)
		gLog.Output(LOG_CHAOS_MIX, "[Wing2Mix - OK][%s][%s] Item: %s | Tỉ lệ: %d, Zen: %d)",
			lpObj->Account, 
			lpObj->Name,
			gItemManager.GetItemName(ItemIndex),
			lpObj->ChaosSuccessRate, 
			lpObj->ChaosMoney);
		#else
		gLog.Output(LOG_CHAOS_MIX, "[Wing1Mix][Failure][%s][%s] - (ChaosSuccessRate: %d, ChaosMoney: %d)", lpObj->Account, lpObj->Name, lpObj->ChaosSuccessRate, lpObj->ChaosMoney);
		#endif
	}
}

void CChaosBox::PetMix(LPOBJ lpObj, int type) // OK
{
	int ChaosCount = 0;
	int SoulOfDarkHorseCount = 0;
	int SoulOfDarkSpiritCount = 0;
	int BlessCount = 0;
	int SoulCount = 0;
	int CreationCount = 0;
	for (int n = 0; n < CHAOS_BOX_SIZE; n++)
	{
		if (lpObj->ChaosBox[n].IsItem() == 0)
		{
			continue;
		}

		#if(BUNDLE_ITEM)
		if (lpObj->ChaosBox[n].m_Index == GET_ITEM(12, 15) && lpObj->ChaosBox[n].m_Durability == 1)
		#else
		if (lpObj->ChaosBox[n].m_Index == GET_ITEM(12, 15))
		#endif
		{
			ChaosCount++;
		}
		else if (lpObj->ChaosBox[n].m_Index == GET_ITEM(13, 31) && lpObj->ChaosBox[n].m_Level == 0)
		{
			SoulOfDarkHorseCount++;
		}
		else if (lpObj->ChaosBox[n].m_Index == GET_ITEM(13, 31) && lpObj->ChaosBox[n].m_Level == 1)
		{
			SoulOfDarkSpiritCount++;
		}
		#if(BUNDLE_ITEM)
		else if (lpObj->ChaosBox[n].m_Index == GET_ITEM(14, 13) && lpObj->ChaosBox[n].m_Durability == 1)
		{
			BlessCount++;
		}
		else if (lpObj->ChaosBox[n].m_Index == GET_ITEM(14, 14) && lpObj->ChaosBox[n].m_Durability == 1)
		{
			SoulCount++;
		}
		else if (lpObj->ChaosBox[n].m_Index == GET_ITEM(14, 22) && lpObj->ChaosBox[n].m_Durability == 1)
		{
			CreationCount++;
		}
		#else
		else if (lpObj->ChaosBox[n].m_Index == GET_ITEM(14, 13))
		{
			BlessCount++;
		}
		else if (lpObj->ChaosBox[n].m_Index == GET_ITEM(14, 14))
		{
			SoulCount++;
		}
		else if (lpObj->ChaosBox[n].m_Index == GET_ITEM(14, 22))
		{
			CreationCount++;
		}
		#endif
	}

	if (ChaosCount != 1 || (type == 0 && SoulOfDarkHorseCount != 1) || (type == 1 && SoulOfDarkSpiritCount != 1) || BlessCount != (5 - (type * 3)) || SoulCount != (5 - (type * 3)) || CreationCount != 1)
	{
		this->GCChaosMixSend(lpObj->Index, 7, 0);
		return;
	}

	lpObj->ChaosSuccessRate = gServerInfo.m_PetMixRate[lpObj->AccountLevel];
	if (this->GetTalismanOfLuckRate(lpObj, &lpObj->ChaosSuccessRate) == 0)
	{
		this->GCChaosMixSend(lpObj->Index, 240, 0);
		return;
	}
	
	lpObj->ChaosSuccessRate = ((lpObj->ChaosSuccessRate>100) ? 100 : lpObj->ChaosSuccessRate);
	lpObj->ChaosMoney = 5000000 - (4000000 * type);
	int TaxMoney = (lpObj->ChaosMoney*gCastleSiegeSync.GetTaxRateChaos(lpObj->Index)) / 100;
	lpObj->ChaosMoney += TaxMoney;
	if (lpObj->Money < ((DWORD)lpObj->ChaosMoney))
	{
		this->GCChaosMixSend(lpObj->Index, 2, 0);
		return;
	}
	lpObj->Money -= lpObj->ChaosMoney;
	GCMoneySend(lpObj->Index, lpObj->Money);
	gCastleSiegeSync.AddTributeMoney(TaxMoney);
	int ItemSlot = 0;
	CItem item = lpObj->ChaosBox[ItemSlot];
	if ((GetLargeRand() % 100) < lpObj->ChaosSuccessRate)
	{
		GDCreateItemSend(lpObj->Index, 0xFE, 0, 0, (GET_ITEM(13, 4) + type), 0, 0, 0, 0, 0, -1, 0, 0, 0, 0, 0, 0xFF, 0);
		
		gLog.Output(LOG_CHAOS_MIX, "[PetMix][Thành Công][%s][%s] - (Item: %d, ChaosSuccessRate: %d, ChaosMoney: %d)", lpObj->Account, lpObj->Name,gItemManager.GetItemName((GET_ITEM(13, 4) + type)), lpObj->ChaosSuccessRate, lpObj->ChaosMoney);
		
	}
	else
	{
		this->ChaosBoxInit(lpObj);
		this->GCChaosBoxSend(lpObj, 1);
		this->GCChaosMixSend(lpObj->Index, 0, 0);
		gNotice.GCNoticeSendToAll(0, 0, 0, 0, 0, 0, gMessage.GetMessage(894), lpObj->Name);
		
		gLog.Output(LOG_CHAOS_MIX, "[PetMix][Xịt][%s][%s] - (Type: %d, ChaosSuccessRate: %d, ChaosMoney: %d)", lpObj->Account, lpObj->Name, type, lpObj->ChaosSuccessRate, lpObj->ChaosMoney);
	
	}
}

void CChaosBox::SiegePotionMix(LPOBJ lpObj, int type) // OK
{
	int BlessCount = 0;
	int SoulCount = 0;
	for (int n = 0; n < CHAOS_BOX_SIZE; n++)
	{
		if (lpObj->ChaosBox[n].IsItem() == 0)
		{
			continue;
		}
		#if(BUNDLE_ITEM)
		if (lpObj->ChaosBox[n].m_Index == GET_ITEM(14, 13) && lpObj->ChaosBox[n].m_Durability == 1)
		{
			BlessCount++;
		}
		else if (lpObj->ChaosBox[n].m_Index == GET_ITEM(14, 14) && lpObj->ChaosBox[n].m_Durability == 1)
		{
			SoulCount++;
		}
		#else
		if (lpObj->ChaosBox[n].m_Index == GET_ITEM(14, 13))
		{
			BlessCount++;
		}
		else if (lpObj->ChaosBox[n].m_Index == GET_ITEM(14, 14))
		{
			SoulCount++;
		}
		#endif
	}

	if ((type == 0 && (BlessCount == 0 || BlessCount > 25)) || (type == 1 && (SoulCount == 0 || SoulCount > 25)))
	{
		this->GCChaosMixSend(lpObj->Index, 7, 0);
		return;
	}

	lpObj->ChaosMoney = 100000 - (50000 * type);

	int TaxMoney = (lpObj->ChaosMoney*gCastleSiegeSync.GetTaxRateChaos(lpObj->Index)) / 100;

	lpObj->ChaosMoney += TaxMoney;

	if (lpObj->Money < ((DWORD)lpObj->ChaosMoney))
	{
		this->GCChaosMixSend(lpObj->Index, 2, 0);
		return;
	}

	lpObj->Money -= lpObj->ChaosMoney;
	GCMoneySend(lpObj->Index, lpObj->Money);
	gCastleSiegeSync.AddTributeMoney(TaxMoney);
	GDCreateItemSend(lpObj->Index, 0xFF, 0, 0, GET_ITEM(14, 7), type, ((BlessCount + SoulCount) * 10), 0, 0, 0, -1, 0, 0, 0, 0, 0, 0xFF, 0);

	int ItemSlot = 0;
	CItem item = lpObj->ChaosBox[ItemSlot];

	gLog.Output(LOG_CHAOS_MIX, "[SiegePotionMix][Thành Công][%s][%s] - (Type: %d, ChaosSuccessRate: %d, ChaosMoney: %d)", lpObj->Account, lpObj->Name, type, lpObj->ChaosSuccessRate, lpObj->ChaosMoney);
	
}

void CChaosBox::LifeStoneMix(LPOBJ lpObj) // OK
{
	int ChaosCount = 0;
	int BlessCount = 0;
	int SoulCount = 0;
	int GuardianCount = 0;

	for (int n = 0; n < CHAOS_BOX_SIZE; n++)
	{
		if (lpObj->ChaosBox[n].IsItem() == 0)
		{
			continue;
		}
		 
		if (lpObj->ChaosBox[n].m_Index == GET_ITEM(12, 15) && lpObj->ChaosBox[n].m_Durability == 1)	
		{
			ChaosCount++;
		}
		else if (lpObj->ChaosBox[n].m_Index == GET_ITEM(14, 13) && lpObj->ChaosBox[n].m_Durability == 1)
		{
			BlessCount++;
		}
		else if (lpObj->ChaosBox[n].m_Index == GET_ITEM(14, 14) && lpObj->ChaosBox[n].m_Durability == 1)
		{
			SoulCount++;
		}
		else if (lpObj->ChaosBox[n].m_Index == GET_ITEM(14, 31) && lpObj->ChaosBox[n].m_Durability == 1)
		{
			GuardianCount++;
		}
 
	}

	if (ChaosCount != 1 || BlessCount != 5 || SoulCount != 5 || GuardianCount != 1)
	{
		this->GCChaosMixSend(lpObj->Index, 7, 0);
		return;
	}
	lpObj->ChaosMoney = 5000000;
	int TaxMoney = (lpObj->ChaosMoney*gCastleSiegeSync.GetTaxRateChaos(lpObj->Index)) / 100;
	lpObj->ChaosMoney += TaxMoney;
	if (lpObj->Money < ((DWORD)lpObj->ChaosMoney))
	{
		this->GCChaosMixSend(lpObj->Index, 2, 0);
		return;
	}
	lpObj->Money -= lpObj->ChaosMoney;
	GCMoneySend(lpObj->Index, lpObj->Money);
	gCastleSiegeSync.AddTributeMoney(TaxMoney);
	GDCreateItemSend(lpObj->Index, 0xFF, 0, 0, GET_ITEM(13, 11), 1, 0, 0, 0, 0, -1, 0, 0, 0, 0, 0, 0xFF, 0);
	int ItemSlot = 0;
	CItem item = lpObj->ChaosBox[ItemSlot];
	gLog.Output(LOG_CHAOS_MIX, "[LifeStoneMix][Thành Công][%s][%s] - Item %s: (ChaosSuccessRate: %d, ChaosMoney: %d)", lpObj->Account, lpObj->Name, gItemManager.GetItemName(GET_ITEM(13, 11)),lpObj->ChaosSuccessRate, lpObj->ChaosMoney);	

}

void CChaosBox::SeniorMix(LPOBJ lpObj) // OK
{
	if (gCastleSiegeSync.CheckCastleOwnerMember(lpObj->Index) == 0 || lpObj->GuildStatus != 0x80)
	{
		this->GCChaosMixSend(lpObj->Index, 0, 0);
		return;
	}
	SYSTEMTIME SystemTime;
	GetSystemTime(&SystemTime);
	if (this->m_SeniorMixLimitDay == SystemTime.wDay && this->m_SeniorMixLimitMonth == SystemTime.wMonth && this->m_SeniorMixLimitYear == SystemTime.wYear)
	{
		this->GCChaosMixSend(lpObj->Index, 0, 0);
		return;
	}
	int SoulPack10 = 0;
	int BlessPack10 = 0;
	int GuardianCount = 0;
	for (int n = 0; n < CHAOS_BOX_SIZE; n++)
	{
		if (lpObj->ChaosBox[n].IsItem() == 0)
		{
			continue;
		}

		if (lpObj->ChaosBox[n].m_Index == GET_ITEM(12, 30))
		{
			BlessPack10 += lpObj->ChaosBox[n].m_Level + 1;
		}
		else if (lpObj->ChaosBox[n].m_Index == GET_ITEM(12, 31))
		{
			SoulPack10 += lpObj->ChaosBox[n].m_Level + 1;
		}
		else if (lpObj->ChaosBox[n].m_Index == GET_ITEM(14, 31))
		{
			GuardianCount++;
		}
	}
	if (SoulPack10 != 3 || BlessPack10 != 3 || GuardianCount != 30)
	{
		this->GCChaosMixSend(lpObj->Index, 7, 0);
		return;
	}
	lpObj->ChaosMoney = 1000000000;
	if (lpObj->Money < ((DWORD)lpObj->ChaosMoney))
	{
		this->GCChaosMixSend(lpObj->Index, 2, 0);
		return;
	}
	CItem item;
	if (gItemBagManager.GetItemBySpecialValue(ITEM_BAG_SENIOR_MIX, lpObj, &item) == 0)
	{
		this->GCChaosMixSend(lpObj->Index, 0, 0);
		return;
	}

	lpObj->Money -= lpObj->ChaosMoney;
	GCMoneySend(lpObj->Index, lpObj->Money);
	this->m_SeniorMixLimitDay = SystemTime.wDay;
	this->m_SeniorMixLimitMonth = SystemTime.wMonth;
	this->m_SeniorMixLimitYear = SystemTime.wYear;
	GDCreateItemSend(lpObj->Index, 0xFF, 0, 0, item.m_Index, (BYTE)item.m_Level, 0, (BYTE)item.m_Option1, (BYTE)item.m_Option2, (BYTE)item.m_Option3, -1, item.m_NewOption, item.m_SetOption, item.m_JewelOfHarmonyOption, item.m_ItemOptionEx, item.m_SocketOption, item.m_SocketOptionBonus, 0);

	gLog.Output(LOG_CHAOS_MIX, "[SeniorMix][Thành Công][%s/%s] Item:%s+%d %d %d %d  %d Thần: %d, GC: %03d 380: %03d, SK %03d, %03d, %03d, %03d, %03d, Rate: %d%%, Money: %d, Serial: %08X)"
				, lpObj->Account
				, lpObj->Name
				, gItemManager.GetItemName(item.m_Index)
				, item.m_Level
				, item.m_Option1
				, item.m_Option2
				, item.m_Option3 * 4
				, item.m_NewOption
				, item.m_SetOption
				, item.m_JewelOfHarmonyOption
				, item.m_ItemOptionEx
				, item.m_SocketOption[0]
				, item.m_SocketOption[1]
				, item.m_SocketOption[2]
				, item.m_SocketOption[3]
				, item.m_SocketOption[4]
				, lpObj->ChaosSuccessRate
				, lpObj->ChaosMoney				
				, item.m_Serial);

}

void CChaosBox::PieceOfHornMix(LPOBJ lpObj) // OK
{
	int ChaosCount = 0;
	int SplinterOfArmorCount = 0;
	int BlessOfGuardianCount = 0;
	for (int n = 0; n < CHAOS_BOX_SIZE; n++)
	{
		if (lpObj->ChaosBox[n].IsItem() == 0)
		{
			continue;
		}
		#if(BUNDLE_ITEM)
		if (lpObj->ChaosBox[n].m_Index == GET_ITEM(12, 15) && lpObj->ChaosBox[n].m_Durability == 1)
		#else
		if (lpObj->ChaosBox[n].m_Index == GET_ITEM(12, 15))
		#endif
		{
			ChaosCount++;
		}
		else if (lpObj->ChaosBox[n].m_Index == GET_ITEM(13, 32))
		{
			SplinterOfArmorCount += (int)lpObj->ChaosBox[n].m_Durability;
		}
		else if (lpObj->ChaosBox[n].m_Index == GET_ITEM(13, 33))
		{
			BlessOfGuardianCount += (int)lpObj->ChaosBox[n].m_Durability;
		}
	}

	if (ChaosCount != 1 || SplinterOfArmorCount != 20 || BlessOfGuardianCount != 20)
	{
		this->GCChaosMixSend(lpObj->Index, 7, 0);
		return;
	}

	lpObj->ChaosSuccessRate = gServerInfo.m_PieceOfHornMixRate[lpObj->AccountLevel];

	if (this->GetTalismanOfLuckRate(lpObj, &lpObj->ChaosSuccessRate) == 0)
	{
		this->GCChaosMixSend(lpObj->Index, 240, 0);
		return;
	}
	
	lpObj->ChaosSuccessRate = ((lpObj->ChaosSuccessRate>100) ? 100 : lpObj->ChaosSuccessRate);
	int ItemSlot = 0;
	CItem item = lpObj->ChaosBox[ItemSlot];
	if ((GetLargeRand() % 100) < lpObj->ChaosSuccessRate)
	{
		GDCreateItemSend(lpObj->Index, 0xFF, 0, 0, GET_ITEM(13, 35), 0, 1, 0, 0, 0, -1, 0, 0, 0, 0, 0, 0xFF, 0);
		gLog.Output(LOG_CHAOS_MIX, "[PieceOfHornMix][Thành Công][%s][%s] - %s(ChaosSuccessRate: %d, ChaosMoney: %d)", lpObj->Account, lpObj->Name,gItemManager.GetItemName(GET_ITEM(13, 35)), lpObj->ChaosSuccessRate, lpObj->ChaosMoney);

		
	}
	else
	{
		this->ChaosBoxInit(lpObj);
		this->GCChaosBoxSend(lpObj, 0);
		this->GCChaosMixSend(lpObj->Index, 0, 0);
		
		gLog.Output(LOG_CHAOS_MIX, "[PieceOfHornMix][Xịt][%s][%s] - (ChaosSuccessRate: %d, ChaosMoney: %d)", lpObj->Account, lpObj->Name, lpObj->ChaosSuccessRate, lpObj->ChaosMoney);
		
	}
}

void CChaosBox::BrokenHornMix(LPOBJ lpObj) // OK
{
	int ChaosCount = 0;
	int ClawOfBeastCount = 0;
	int PieceOfHornCount = 0;
	for (int n = 0; n < CHAOS_BOX_SIZE; n++)
	{
		if (lpObj->ChaosBox[n].IsItem() == 0)
		{
			continue;
		}
		#if(BUNDLE_ITEM)
		if (lpObj->ChaosBox[n].m_Index == GET_ITEM(12, 15) && lpObj->ChaosBox[n].m_Durability == 1)
		#else
		if (lpObj->ChaosBox[n].m_Index == GET_ITEM(12, 15))
		#endif
		{
			ChaosCount++;
		}
		else if (lpObj->ChaosBox[n].m_Index == GET_ITEM(13, 34))
		{
			ClawOfBeastCount += (int)lpObj->ChaosBox[n].m_Durability;
		}
		else if (lpObj->ChaosBox[n].m_Index == GET_ITEM(13, 35))
		{
			PieceOfHornCount++;
		}
	}
	if (ChaosCount != 1 || ClawOfBeastCount != 10 || PieceOfHornCount != 5)
	{
		this->GCChaosMixSend(lpObj->Index, 7, 0);
		return;
	}

	lpObj->ChaosSuccessRate = gServerInfo.m_BrokenHornMixRate[lpObj->AccountLevel];

	if (this->GetTalismanOfLuckRate(lpObj, &lpObj->ChaosSuccessRate) == 0)
	{
		this->GCChaosMixSend(lpObj->Index, 240, 0);
		return;
	}
	
	lpObj->ChaosSuccessRate = ((lpObj->ChaosSuccessRate>100) ? 100 : lpObj->ChaosSuccessRate);

	int ItemSlot = 0;
	CItem item = lpObj->ChaosBox[ItemSlot];
	if ((GetLargeRand() % 100) < lpObj->ChaosSuccessRate)
	{
		GDCreateItemSend(lpObj->Index, 0xFF, 0, 0, GET_ITEM(13, 36), 0, 1, 0, 0, 0, -1, 0, 0, 0, 0, 0, 0xFF, 0);
		gNotice.GCNoticeSendToAll(0, 0, 0, 0, 0, 0, "%s đã nâng cấp thành công Sừng Gãy", lpObj->Name);
		gLog.Output(LOG_CHAOS_MIX, "[Sừng Gãy][Thành Công][%s][%s] -Item: %s(ChaosSuccessRate: %d, ChaosMoney: %d)", lpObj->Account, lpObj->Name,gItemManager.GetItemName(GET_ITEM(13,37)), lpObj->ChaosSuccessRate, lpObj->ChaosMoney);
	}
	else
	{
		this->ChaosBoxInit(lpObj);
		this->GCChaosBoxSend(lpObj, 0);
		this->GCChaosMixSend(lpObj->Index, 0, 0);
		gNotice.GCNoticeSendToAll(0, 0, 0, 0, 0, 0, gMessage.GetMessage(895), lpObj->Name);
		gLog.Output(LOG_CHAOS_MIX, "[Sừng Gãy][Xịt][%s][%s] - (ChaosSuccessRate: %d, ChaosMoney: %d)", lpObj->Account, lpObj->Name, lpObj->ChaosSuccessRate, lpObj->ChaosMoney);
		
	}
}

void CChaosBox::HornOfFenrirMix(LPOBJ lpObj) // OK
{
	int ChaosCount = 0;
	int BrokenHornCount = 0;
	int LifeCount = 0;

	for (int n = 0; n < CHAOS_BOX_SIZE; n++)
	{
		if (lpObj->ChaosBox[n].IsItem() == 0)
		{
			continue;
		}
		#if(BUNDLE_ITEM)
		if (lpObj->ChaosBox[n].m_Index == GET_ITEM(12, 15) && lpObj->ChaosBox[n].m_Durability == 1)
		#else
		if (lpObj->ChaosBox[n].m_Index == GET_ITEM(12, 15))
		#endif
		{
			ChaosCount++;
		}
		else if (lpObj->ChaosBox[n].m_Index == GET_ITEM(13, 36))
		{
			BrokenHornCount++;
		}
		else if (lpObj->ChaosBox[n].m_Index == GET_ITEM(14, 16))
		{
			LifeCount++;
		}
	}

	if (ChaosCount != 1 || BrokenHornCount != 1 || LifeCount != 3)
	{
		this->GCChaosMixSend(lpObj->Index, 7, 0);
		return;
	}
	
	
	lpObj->ChaosSuccessRate = gServerInfo.m_HornOfFenrirMixRate[lpObj->AccountLevel];

	if (this->GetTalismanOfLuckRate(lpObj, &lpObj->ChaosSuccessRate) == 0)
	{
		this->GCChaosMixSend(lpObj->Index, 240, 0);
		return;
	}
	lpObj->ChaosSuccessRate = ((lpObj->ChaosSuccessRate>100) ? 100 : lpObj->ChaosSuccessRate);

	lpObj->ChaosMoney = 10000000;
	int TaxMoney = (lpObj->ChaosMoney*gCastleSiegeSync.GetTaxRateChaos(lpObj->Index)) / 100;
	lpObj->ChaosMoney += TaxMoney;
	if (lpObj->Money < ((DWORD)lpObj->ChaosMoney))
	{
		this->GCChaosMixSend(lpObj->Index, 2, 0);
		return;
	}
	lpObj->Money -= lpObj->ChaosMoney;
	GCMoneySend(lpObj->Index, lpObj->Money);
	gCastleSiegeSync.AddTributeMoney(TaxMoney);
	int ItemSlot = 0;
	CItem item = lpObj->ChaosBox[ItemSlot];
	if ((GetLargeRand() % 100) < lpObj->ChaosSuccessRate)
	{
		GDCreateItemSend(lpObj->Index, 0xFF, 0, 0, GET_ITEM(13, 37), 0, 255, 1, 0, 0, -1, 0, 0, 0, 0, 0, 0xFF, 0);
		gNotice.GCNoticeSendToAll(0, 0, 0, 0, 0, 0, "%s nâng cấp thành công Sói Tinh", lpObj->Name);
		gLog.Output(LOG_CHAOS_MIX, "[Sói Tinh][Thành Công][%s][%s] -Item: %s(ChaosSuccessRate: %d, ChaosMoney: %d)", lpObj->Account, lpObj->Name,gItemManager.GetItemName(GET_ITEM(13, 37)), lpObj->ChaosSuccessRate, lpObj->ChaosMoney);

	}
	else
	{
		this->ChaosBoxInit(lpObj);
		this->GCChaosBoxSend(lpObj, 0);
		this->GCChaosMixSend(lpObj->Index, 0, 0);
		gNotice.GCNoticeSendToAll(0, 0, 0, 0, 0, 0, gMessage.GetMessage(896), lpObj->Name);	
		gLog.Output(LOG_CHAOS_MIX, "[HornOfFenrirMix][Xịt][%s][%s] - (ChaosSuccessRate: %d, ChaosMoney: %d)", lpObj->Account, lpObj->Name, lpObj->ChaosSuccessRate, lpObj->ChaosMoney);
	}
}

void CChaosBox::HornOfFenrirUpgradeMix(LPOBJ lpObj) // OK
{
	int ChaosCount = 0;
	int HornOfFenrirCount = 0;
	int LifeCount = 0;
	int WeaponCount = 0;
	int WeaponMoney = 0;
	int ArmorCount = 0;
	int ArmorMoney = 0;
	int ItemSlot = 0;

	for (int n = 0; n < CHAOS_BOX_SIZE; n++)
	{
		if (lpObj->ChaosBox[n].IsItem() == 0)
		{
			continue;
		}

		#if(BUNDLE_ITEM)
		if (lpObj->ChaosBox[n].m_Index == GET_ITEM(12, 15) && lpObj->ChaosBox[n].m_Durability == 1)
		#else
		if (lpObj->ChaosBox[n].m_Index == GET_ITEM(12, 15))
		#endif
		{
			ChaosCount++;
		}
		else if (lpObj->ChaosBox[n].m_Index == GET_ITEM(13, 37))
		{
			HornOfFenrirCount++;
		}
		else if (lpObj->ChaosBox[n].m_Index == GET_ITEM(14, 16))
		{
			LifeCount++;
		}
		else if (lpObj->ChaosBox[n].m_Index >= GET_ITEM(0, 0) && lpObj->ChaosBox[n].m_Index < GET_ITEM(6, 0) && lpObj->ChaosBox[n].m_Level >= 4 && lpObj->ChaosBox[n].m_Option3 >= 1)
		{
			WeaponCount++;
			WeaponMoney = lpObj->ChaosBox[n].m_BuyMoney;ItemSlot=n;
		}
		else if (lpObj->ChaosBox[n].m_Index >= GET_ITEM(6, 0) && lpObj->ChaosBox[n].m_Index < GET_ITEM(12, 0) && lpObj->ChaosBox[n].m_Level >= 4 && lpObj->ChaosBox[n].m_Option3 >= 1)
		{
			ArmorCount++;
			ArmorMoney = lpObj->ChaosBox[n].m_BuyMoney;ItemSlot=n;
		}
	}

	if (ChaosCount != 1 || HornOfFenrirCount != 1 || LifeCount != 5 || (WeaponCount == 0 && ArmorCount == 0) || (WeaponCount != 0 && ArmorCount != 0))
	{
		this->GCChaosMixSend(lpObj->Index, 7, 0);
		return;
	}

	if (gServerInfo.m_HornOfFenrirUpgradeMixRate[lpObj->AccountLevel] == -1)
	{
		lpObj->ChaosSuccessRate = (WeaponMoney + ArmorMoney) / 10000;
	}
	else
	{
		lpObj->ChaosSuccessRate = gServerInfo.m_HornOfFenrirUpgradeMixRate[lpObj->AccountLevel];
	}

	if (this->GetTalismanOfLuckRate(lpObj, &lpObj->ChaosSuccessRate) == 0)
	{
		this->GCChaosMixSend(lpObj->Index, 240, 0);
		return;
	}

	if (gServerInfo.m_HornOfFenrirUpgradeMixRate[lpObj->AccountLevel] == -1)
	{
		lpObj->ChaosSuccessRate = ((lpObj->ChaosSuccessRate>80) ? 80 : lpObj->ChaosSuccessRate);
	}
	else
	{
		
		lpObj->ChaosSuccessRate = ((lpObj->ChaosSuccessRate>100) ? 100 : lpObj->ChaosSuccessRate);
	}
	lpObj->ChaosMoney = 10000000;
	int TaxMoney = (lpObj->ChaosMoney*gCastleSiegeSync.GetTaxRateChaos(lpObj->Index)) / 100;

	lpObj->ChaosMoney += TaxMoney;

	if (lpObj->Money < ((DWORD)lpObj->ChaosMoney))
	{
		this->GCChaosMixSend(lpObj->Index, 2, 0);
		return;
	}

	lpObj->Money -= lpObj->ChaosMoney;
	GCMoneySend(lpObj->Index, lpObj->Money);
	gCastleSiegeSync.AddTributeMoney(TaxMoney);
	
	CItem item = lpObj->ChaosBox[ItemSlot];
	if ((GetLargeRand() % 100) < lpObj->ChaosSuccessRate)
	{
		BYTE ItemNewOption = ((WeaponCount == 0) ? ((ArmorCount == 0) ? 0 : 2) : 1);
		GDCreateItemSend(lpObj->Index, 0xFF, 0, 0, GET_ITEM(13, 37), 0, 255, 1, 0, 0, -1, ItemNewOption, 0, 0, 0, 0, 0xFF, 0);
		gNotice.GCNoticeSendToAll(0, 0, 0, 0, 0, 0, "%s đã nâng cấp thành công Sói Tấn Công", lpObj->Name);
		gLog.Output(LOG_CHAOS_MIX, "[Nâng Sói][Thành Công][%s/%s] Item: %s  Từ: %s+%d,  %d %d %d %d, Thần: %d, HC %03d 380: %03d, SK %03d, %03d, %03d, %03d, %03d, Rate: %d%%, Money: %d, Serial: %08X)"
				, lpObj->Account
				, lpObj->Name
				,gItemManager.GetItemName(GET_ITEM(13, 37)) 
				
				, gItemManager.GetItemName(item.m_Index)
				, item.m_Level
				, item.m_Option1
				, item.m_Option2
				, item.m_Option3 * 4
				, item.m_NewOption
				, item.m_SetOption
				, item.m_JewelOfHarmonyOption
				, item.m_ItemOptionEx
				, item.m_SocketOption[0]
				, item.m_SocketOption[1]
				, item.m_SocketOption[2]
				, item.m_SocketOption[3]
				, item.m_SocketOption[4]
				, lpObj->ChaosSuccessRate
				, lpObj->ChaosMoney				
				, item.m_Serial);	

	}
	else
	{
		this->ChaosBoxInit(lpObj);

		this->GCChaosBoxSend(lpObj, 0);

		this->GCChaosMixSend(lpObj->Index, 0, 0);

		gNotice.GCNoticeSendToAll(0, 0, 0, 0, 0, 0, gMessage.GetMessage(896), lpObj->Name);
		gLog.Output(LOG_CHAOS_MIX, "[Nâng Sói][Xịt][%s/%s] Item: %s+%d  %d %d %d %d, Thần: %d, GC: %03d, 380: %03d, SK: %03d, %03d, %03d, %03d, %03d, Rate: %d%%, Money: %d, Serial: %08X)"
				, lpObj->Account
				, lpObj->Name
				, gItemManager.GetItemName(item.m_Index)
				, item.m_Level
				, item.m_Option1
				, item.m_Option2
				, item.m_Option3 * 4
				, item.m_NewOption
				, item.m_SetOption
				, item.m_JewelOfHarmonyOption
				, item.m_ItemOptionEx
				, item.m_SocketOption[0]
				, item.m_SocketOption[1]
				, item.m_SocketOption[2]
				, item.m_SocketOption[3]
				, item.m_SocketOption[4]
				, lpObj->ChaosSuccessRate
				, lpObj->ChaosMoney				
				, item.m_Serial);		

	}
}

void CChaosBox::HornOfFenrirGoldMix(LPOBJ lpObj) // OK
{
	int FenrirThuong = 0;
	int FenrirTC = 0;
	int FenrirPT = 0;
	for (int n = 0; n < CHAOS_BOX_SIZE; n++)
	{
		if (lpObj->ChaosBox[n].IsItem() == 0)
		{
			continue;
		}

		if (lpObj->ChaosBox[n].m_Index == GET_ITEM(13, 37) && lpObj->ChaosBox[n].m_NewOption == 0)
		{
			FenrirThuong++;
		}
		else if (lpObj->ChaosBox[n].m_Index == GET_ITEM(13, 37) && lpObj->ChaosBox[n].m_NewOption == 1)
		{
			FenrirTC++;
		}
		else if (lpObj->ChaosBox[n].m_Index == GET_ITEM(13, 37) && lpObj->ChaosBox[n].m_NewOption == 2)
		{
			FenrirPT++;
		}
	}
	if (FenrirThuong != 1 || FenrirTC != 1 || FenrirPT != 1)
	{
		this->GCChaosMixSend(lpObj->Index, 7, 0);
		return;
	}

	lpObj->ChaosSuccessRate = gServerInfo.m_HornOfFenrirGoldMixRate[lpObj->AccountLevel];

	if (this->GetTalismanOfLuckRate(lpObj, &lpObj->ChaosSuccessRate) == 0)
	{
		this->GCChaosMixSend(lpObj->Index, 240, 0);
		return;
	}
	
	lpObj->ChaosSuccessRate = ((lpObj->ChaosSuccessRate>100) ? 100 : lpObj->ChaosSuccessRate);
	lpObj->ChaosMoney = 10000000;
	int TaxMoney = (lpObj->ChaosMoney*gCastleSiegeSync.GetTaxRateChaos(lpObj->Index)) / 100;
	lpObj->ChaosMoney += TaxMoney;
	if (lpObj->Money < ((DWORD)lpObj->ChaosMoney))
	{
		this->GCChaosMixSend(lpObj->Index, 2, 0);
		return;
	}
	lpObj->Money -= lpObj->ChaosMoney;
	GCMoneySend(lpObj->Index, lpObj->Money);
	gCastleSiegeSync.AddTributeMoney(TaxMoney);
	int ItemSlot = 0;
	CItem item = lpObj->ChaosBox[ItemSlot];

	if ((GetLargeRand() % 100) < lpObj->ChaosSuccessRate)
	{
		GDCreateItemSend(lpObj->Index, 0xFF, 0, 0, GET_ITEM(13, 37), 0, 255, 1, 0, 0, -1, 4, 0, 0, 0, 0, 0xFF, 0);
		gNotice.GCNoticeSendToAll(0, 0, 0, 0, 0, 0, "%s nâng cấp thành công Sói Tinh Hoàng Kim", lpObj->Name);
		gLog.Output(LOG_CHAOS_MIX, "[Sói Hoàng Kim][Thành Công][%s][%s] -Item: %s  (ChaosSuccessRate: %d, ChaosMoney: %d)", lpObj->Account, lpObj->Name,gItemManager.GetItemName(GET_ITEM(13, 37)),lpObj->ChaosSuccessRate, lpObj->ChaosMoney);
		
	}
	else
	{
		this->ChaosBoxInit(lpObj);
		this->GCChaosBoxSend(lpObj, 0);
		this->GCChaosMixSend(lpObj->Index, 0, 0);
		gNotice.GCNoticeSendToAll(0, 0, 0, 0, 0, 0, "%s nâng cấp Xịt Sói Tinh Hoàng Kim", lpObj->Name);
		gLog.Output(LOG_CHAOS_MIX, "[Sói Hoàng Kim][Xịt][%s][%s] - (ChaosSuccessRate: %d, ChaosMoney: %d)", lpObj->Account, lpObj->Name, lpObj->ChaosSuccessRate, lpObj->ChaosMoney);
		
	}
}
void CChaosBox::ShieldPotionMix(LPOBJ lpObj, int type) // OK
{
	int LargeHealingPotionCount = 0;
	int SmallCompoundPotionCount = 0;
	int MediumCompoundPotionCount = 0;
	for (int n = 0; n < CHAOS_BOX_SIZE; n++)
	{
		if (lpObj->ChaosBox[n].IsItem() == 0)
		{
			continue;
		}

		if (lpObj->ChaosBox[n].m_Index == GET_ITEM(14, 3))
		{
			LargeHealingPotionCount += (int)lpObj->ChaosBox[n].m_Durability;
		}
		else if (lpObj->ChaosBox[n].m_Index == GET_ITEM(14, 38))
		{
			SmallCompoundPotionCount += (int)lpObj->ChaosBox[n].m_Durability;
		}
		else if (lpObj->ChaosBox[n].m_Index == GET_ITEM(14, 39))
		{
			MediumCompoundPotionCount += (int)lpObj->ChaosBox[n].m_Durability;
		}
	}

	if ((type == 0 && LargeHealingPotionCount != 3) || (type == 1 && SmallCompoundPotionCount != 3) || (type == 2 && MediumCompoundPotionCount != 3))
	{
		this->GCChaosMixSend(lpObj->Index, 7, 0);
		return;
	}

	switch (type)
	{
		case 0:		lpObj->ChaosSuccessRate = gServerInfo.m_ShieldPotionMixRate1[lpObj->AccountLevel];		break;
		case 1:		lpObj->ChaosSuccessRate = gServerInfo.m_ShieldPotionMixRate2[lpObj->AccountLevel];		break;
		case 2:		lpObj->ChaosSuccessRate = gServerInfo.m_ShieldPotionMixRate3[lpObj->AccountLevel];		break;
	}
	if (this->GetTalismanOfLuckRate(lpObj, &lpObj->ChaosSuccessRate) == 0)
	{
		this->GCChaosMixSend(lpObj->Index, 240, 0);
		return;
	}
	
	lpObj->ChaosSuccessRate = ((lpObj->ChaosSuccessRate>100) ? 100 : lpObj->ChaosSuccessRate);
	lpObj->ChaosMoney = ((type == 0) ? 100000 : (500000 * type));
	int TaxMoney = (lpObj->ChaosMoney*gCastleSiegeSync.GetTaxRateChaos(lpObj->Index)) / 100;
	lpObj->ChaosMoney += TaxMoney;
	if (lpObj->Money < ((DWORD)lpObj->ChaosMoney))
	{
		this->GCChaosMixSend(lpObj->Index, 2, 0);
		return;
	}

	lpObj->Money -= lpObj->ChaosMoney;
	GCMoneySend(lpObj->Index, lpObj->Money);
	gCastleSiegeSync.AddTributeMoney(TaxMoney);
	int ItemSlot = 0;
	CItem item = lpObj->ChaosBox[ItemSlot];
	if ((GetLargeRand() % 100) < lpObj->ChaosSuccessRate)
	{
		GDCreateItemSend(lpObj->Index, 0xFF, 0, 0, (GET_ITEM(14, 35) + type), 0, 1, 0, 0, 0, -1, 0, 0, 0, 0, 0, 0xFF, 0);
		
		gLog.Output(LOG_CHAOS_MIX, "[ShieldPotionMix][Thành Công][%s][%s] - (Type: %d, ChaosSuccessRate: %d, ChaosMoney: %d)", lpObj->Account, lpObj->Name, type, lpObj->ChaosSuccessRate, lpObj->ChaosMoney);
		
	}
	else
	{
		this->ChaosBoxInit(lpObj);
		this->GCChaosBoxSend(lpObj, 0);
		this->GCChaosMixSend(lpObj->Index, 0, 0);

		gLog.Output(LOG_CHAOS_MIX, "[ShieldPotionMix][Xịt][%s][%s] - (Type: %d, ChaosSuccessRate: %d, ChaosMoney: %d)", lpObj->Account, lpObj->Name, type, lpObj->ChaosSuccessRate, lpObj->ChaosMoney);
		
	}
}

void CChaosBox::JewelOfHarmonyItemPurityMix(LPOBJ lpObj) // OK
{
	int GemStoneCount = 0;
	int ItemSlot = 0;
	for (int n = 0; n < CHAOS_BOX_SIZE; n++)
	{
		if (lpObj->ChaosBox[n].IsItem() == 0)
		{
			continue;
		}

		if (lpObj->ChaosBox[n].m_Index == GET_ITEM(14, 41))
		{
			GemStoneCount++;
		}
	}

	if (GemStoneCount != 1)
	{
		this->GCChaosMixSend(lpObj->Index, 7, 0);
		return;
	}
	
	lpObj->ChaosSuccessRate = gServerInfo.m_JewelOfHarmonyItemPurityMixRate[lpObj->AccountLevel];
	lpObj->ChaosSuccessRate = ((lpObj->ChaosSuccessRate>100) ? 100 : lpObj->ChaosSuccessRate);
	CItem item = lpObj->ChaosBox[ItemSlot];
	if ((GetLargeRand() % 100) < lpObj->ChaosSuccessRate)
	{
		GDCreateItemSend(lpObj->Index, 0xFF, 0, 0, GET_ITEM(14, 42), 0, 1, 0, 0, 0, -1, 0, 0, 0, 0, 0, 0xFF, 0);
		gLog.Output(LOG_CHAOS_MIX, "[Đá tạo hóa][Thành Công][%s][%s] - (ChaosSuccessRate: %d, ChaosMoney: %d)", lpObj->Account, lpObj->Name, lpObj->ChaosSuccessRate, lpObj->ChaosMoney);

	}
	else
	{
		this->ChaosBoxInit(lpObj);
		this->GCChaosBoxSend(lpObj, 0);
		this->GCChaosMixSend(lpObj->Index, 0, 0);
		gLog.Output(LOG_CHAOS_MIX, "[Đá tạo hóa][Xịt][%s][%s] - (ChaosSuccessRate: %d, ChaosMoney: %d)", lpObj->Account, lpObj->Name, lpObj->ChaosSuccessRate, lpObj->ChaosMoney);
		
	}
}

void CChaosBox::JewelOfHarmonyItemSmeltMix(LPOBJ lpObj) // OK
{
	int ItemCount = 0;
	int ItemSlot = 0;

	for (int n = 0; n < CHAOS_BOX_SIZE; n++)
	{
		if (lpObj->ChaosBox[n].IsItem() == 0)
		{
			continue;
		}

		if (gJewelOfHarmonyType.CheckJewelOfHarmonyItemType(&lpObj->ChaosBox[n]) != 0 && lpObj->ChaosBox[n].IsSetItem() == 0 && lpObj->ChaosBox[n].IsJewelOfHarmonyItem() == 0)
		{
			ItemCount++;
			ItemSlot = n;
		}
	}

	if (ItemCount != 1)
	{
		this->GCChaosMixSend(lpObj->Index, 7, 0);
		return;
	}

	if (lpObj->ChaosBox[ItemSlot].IsExcItem() == 0)
	{
		lpObj->ChaosSuccessRate = gServerInfo.m_JewelOfHarmonyItemSmeltMixRate1[lpObj->AccountLevel];
	}
	else
	{
		lpObj->ChaosSuccessRate = gServerInfo.m_JewelOfHarmonyItemSmeltMixRate2[lpObj->AccountLevel];
	}

	if ((GetLargeRand() % 100) < lpObj->ChaosSuccessRate)
	{
		WORD ItemIndex = ((lpObj->ChaosBox[ItemSlot].IsExcItem() == 0) ? GET_ITEM(14, 43) : GET_ITEM(14, 44));

		GDCreateItemSend(lpObj->Index, 0xFF, 0, 0, ItemIndex, 0, 1, 0, 0, 0, -1, 0, 0, 0, 0, 0, 0xFF, 0);
		CItem item = lpObj->ChaosBox[ItemSlot];
	gLog.Output(LOG_CHAOS_MIX, "[Đá Tinh Luyện][Thành Công][%s/%s] Từ: %s+%d,  %d %d %d %d %d, HC: %03d, 380: %03d, Socket: %03d, %03d, %03d, %03d, %03d, Rate: %d%%, Money: %d, Serial: %08X)"
				, lpObj->Account
				, lpObj->Name
				, gItemManager.GetItemName(item.m_Index)
				, item.m_Level
				, item.m_Option1
				, item.m_Option2
				, item.m_Option3 * 4
				, item.m_NewOption
				, item.m_SetOption
				, item.m_JewelOfHarmonyOption
				, item.m_ItemOptionEx
				, item.m_SocketOption[0]
				, item.m_SocketOption[1]
				, item.m_SocketOption[2]
				, item.m_SocketOption[3]
				, item.m_SocketOption[4]
				, lpObj->ChaosSuccessRate
				, lpObj->ChaosMoney				
				, item.m_Serial);		

	}
	else
	{
		this->ChaosBoxInit(lpObj);

		this->GCChaosBoxSend(lpObj, 0);

		this->GCChaosMixSend(lpObj->Index, 0, 0);
		CItem item = lpObj->ChaosBox[ItemSlot];
				gLog.Output(LOG_CHAOS_MIX, "[Đá Tinh Luyện][Thất bại][%s/%s] Từ: %s+%d,  %d %d %d %d %d, HC: %03d, 380: %03d, Socket: %03d, %03d, %03d, %03d, %03d, Rate: %d%%, Money: %d, Serial: %08X)"
				, lpObj->Account
				, lpObj->Name
				, gItemManager.GetItemName(item.m_Index)
				, item.m_Level
				, item.m_Option1
				, item.m_Option2
				, item.m_Option3 * 4
				, item.m_NewOption
				, item.m_SetOption
				, item.m_JewelOfHarmonyOption
				, item.m_ItemOptionEx
				, item.m_SocketOption[0]
				, item.m_SocketOption[1]
				, item.m_SocketOption[2]
				, item.m_SocketOption[3]
				, item.m_SocketOption[4]
				, lpObj->ChaosSuccessRate
				, lpObj->ChaosMoney				
				, item.m_Serial);			

	}
}

void CChaosBox::JewelOfHarmonyItemRestoreMix(LPOBJ lpObj) // OK
{
	int ItemCount = 0;
	int ItemSlot = 0;

	for (int n = 0; n < CHAOS_BOX_SIZE; n++)
	{
		if (lpObj->ChaosBox[n].IsItem() == 0)
		{
			continue;
		}

		if (lpObj->ChaosBox[n].IsJewelOfHarmonyItem() != 0)
		{
			ItemCount++;
			ItemSlot = n;
		}
	}

	if (ItemCount != 1)
	{
		this->GCChaosMixSend(lpObj->Index, 7, 0);
		return;
	}

	lpObj->ChaosMoney = gJewelOfHarmonyOption.GetJewelOfHarmonyItemRestoreMoney(&lpObj->ChaosBox[ItemSlot]);

	if (lpObj->ChaosMoney == -1)
	{
		this->GCChaosMixSend(lpObj->Index, 0, 0);
		return;
	}

	int TaxMoney = (lpObj->ChaosMoney*gCastleSiegeSync.GetTaxRateChaos(lpObj->Index)) / 100;

	lpObj->ChaosMoney += TaxMoney;

	if (lpObj->Money < ((DWORD)lpObj->ChaosMoney))
	{
		this->GCChaosMixSend(lpObj->Index, 2, 0);
		return;
	}

	lpObj->Money -= lpObj->ChaosMoney;
	GCMoneySend(lpObj->Index, lpObj->Money);
	gCastleSiegeSync.AddTributeMoney(TaxMoney);
	CItem item = lpObj->ChaosBox[ItemSlot];
	item.m_JewelOfHarmonyOption = JEWEL_OF_HARMONY_OPTION_NONE;
	this->ChaosBoxInit(lpObj);
	gItemManager.ChaosBoxAddItem(lpObj->Index, item, 0);
	this->GCChaosMixSend(lpObj->Index, 1, &item);
	gLog.Output(LOG_CHAOS_MIX, "[Xóa gia cường][Thành Công][%s/%s] Item: %s+%d, %d %d %d %d %d, HC: %03d, 380: %03d, Socket: %03d, %03d, %03d, %03d, %03d, Rate: %d%%, Money: %d, Serial: %08X)"
				, lpObj->Account
				, lpObj->Name
				, gItemManager.GetItemName(item.m_Index)
				, item.m_Level
				, item.m_Option1
				, item.m_Option2
				, item.m_Option3 * 4
				, item.m_NewOption
				, item.m_SetOption
				, item.m_JewelOfHarmonyOption
				, item.m_ItemOptionEx
				, item.m_SocketOption[0]
				, item.m_SocketOption[1]
				, item.m_SocketOption[2]
				, item.m_SocketOption[3]
				, item.m_SocketOption[4]
				, lpObj->ChaosSuccessRate
				, lpObj->ChaosMoney				
				, item.m_Serial);	

}

void CChaosBox::Item380Mix(LPOBJ lpObj) // OK
{
	int GuardianCount = 0;
	int HarmonyCount = 0;
	int ItemCount = 0;
	int ItemSlot = 0;

	for (int n = 0; n < CHAOS_BOX_SIZE; n++)
	{
		if (lpObj->ChaosBox[n].IsItem() == 0)
		{
			continue;
		}

		if (lpObj->ChaosBox[n].m_Index == GET_ITEM(14, 31))
		{
			GuardianCount++;
		}
		else if (lpObj->ChaosBox[n].m_Index == GET_ITEM(14, 42))
		{
			HarmonyCount++;
		}
		else if (lpObj->ChaosBox[n].Is380Item() == 0 && lpObj->ChaosBox[n].m_Level >= 4 && lpObj->ChaosBox[n].m_Option3 >= 1)
		{
			ItemCount++;
			ItemSlot = n;
		}
	}

	if (GuardianCount != 1 || HarmonyCount != 1 || ItemCount != 1)
	{
		this->GCChaosMixSend(lpObj->Index, 7, 0);
		return;
	}

	if (lpObj->ChaosBox[ItemSlot].m_Level >= 7)
	{
		lpObj->ChaosSuccessRate = gServerInfo.m_Item380MixRate2[lpObj->AccountLevel];
	}
	else
	{
		lpObj->ChaosSuccessRate = gServerInfo.m_Item380MixRate1[lpObj->AccountLevel];
	}

	if (this->GetTalismanOfLuckRate(lpObj, &lpObj->ChaosSuccessRate) == 0)
	{
		this->GCChaosMixSend(lpObj->Index, 240, 0);
		return;
	}
	
	lpObj->ChaosSuccessRate = ((lpObj->ChaosSuccessRate>100) ? 100 : lpObj->ChaosSuccessRate);

	lpObj->ChaosMoney = 10000000;

	int TaxMoney = (lpObj->ChaosMoney*gCastleSiegeSync.GetTaxRateChaos(lpObj->Index)) / 100;

	lpObj->ChaosMoney += TaxMoney;

	if (lpObj->Money < ((DWORD)lpObj->ChaosMoney))
	{
		this->GCChaosMixSend(lpObj->Index, 2, 0);
		return;
	}

	lpObj->Money -= lpObj->ChaosMoney;

	GCMoneySend(lpObj->Index, lpObj->Money);

	gCastleSiegeSync.AddTributeMoney(TaxMoney);


	CItem item = lpObj->ChaosBox[ItemSlot];

	if ((GetLargeRand() % 100) < lpObj->ChaosSuccessRate)
	{
		CItem item = lpObj->ChaosBox[ItemSlot];
		item.m_ItemOptionEx |= 0x80;
		this->ChaosBoxInit(lpObj);
		gItemManager.ChaosBoxAddItem(lpObj->Index, item, 0);
		this->GCChaosMixSend(lpObj->Index, 1, &item);
		gLog.Output(LOG_CHAOS_MIX, "[Item380Mix][Thành Công][%s/%s]  %s+%d,  %d %d %d %d %d, HC: %03d, 380: %03d, Socket: %03d, %03d, %03d, %03d, %03d, Rate: %d%%, Money: %d, Serial: %08X)"
				, lpObj->Account
				, lpObj->Name
				, gItemManager.GetItemName(item.m_Index)
				, item.m_Level
				, item.m_Option1
				, item.m_Option2
				, item.m_Option3 * 4
				, item.m_NewOption
				, item.m_SetOption
				, item.m_JewelOfHarmonyOption
				, item.m_ItemOptionEx
				, item.m_SocketOption[0]
				, item.m_SocketOption[1]
				, item.m_SocketOption[2]
				, item.m_SocketOption[3]
				, item.m_SocketOption[4]
				, lpObj->ChaosSuccessRate
				, lpObj->ChaosMoney				
				, item.m_Serial);		

	}
	else
	{
		this->ChaosBoxItemKeep(lpObj, ItemSlot);
		this->GCChaosBoxSend(lpObj, 0);
		this->GCChaosMixSend(lpObj->Index, 0, 0);
	gLog.Output(LOG_CHAOS_MIX, "[Item380Mix][Xịt][%s/%s] ITEM: %s+%d,  %d %d %d %d %d, HC: %03d, 380: %03d, Socket: %03d, %03d, %03d, %03d, %03d, Rate: %d%%, Money: %d, Serial: %08X)"
				, lpObj->Account
				, lpObj->Name
				, gItemManager.GetItemName(item.m_Index)
				, item.m_Level
				, item.m_Option1
				, item.m_Option2
				, item.m_Option3 * 4
				, item.m_NewOption
				, item.m_SetOption
				, item.m_JewelOfHarmonyOption
				, item.m_ItemOptionEx
				, item.m_SocketOption[0]
				, item.m_SocketOption[1]
				, item.m_SocketOption[2]
				, item.m_SocketOption[3]
				, item.m_SocketOption[4]
				, lpObj->ChaosSuccessRate
				, lpObj->ChaosMoney				
				, item.m_Serial);		

	}
}

void CChaosBox::IllusionTempleMix(LPOBJ lpObj) // OK
{
	int ChaosCount = 0;
	int ScrollCount = 0;
	int ScrollLevel = 0;
	int PotionCount = 0;
	int PotionLevel = 0;

	for (int n = 0; n < CHAOS_BOX_SIZE; n++)
	{
		if (lpObj->ChaosBox[n].IsItem() == 0)
		{
			continue;
		}

		#if(BUNDLE_ITEM)
		if (lpObj->ChaosBox[n].m_Index == GET_ITEM(12, 15) && lpObj->ChaosBox[n].m_Durability == 1)
		#else
		if (lpObj->ChaosBox[n].m_Index == GET_ITEM(12, 15))
		#endif
		{
			ChaosCount++;
		}
		else if (lpObj->ChaosBox[n].m_Index == GET_ITEM(13, 49))
		{
			ScrollCount++;
			ScrollLevel = lpObj->ChaosBox[n].m_Level;
		}
		else if (lpObj->ChaosBox[n].m_Index == GET_ITEM(13, 50))
		{
			PotionCount++;
			PotionLevel = lpObj->ChaosBox[n].m_Level;
		}
	}

	if (ChaosCount != 1 || ScrollCount != 1 || PotionCount != 1 || ScrollLevel != PotionLevel)
	{
		this->GCChaosMixSend(lpObj->Index, 7, 0);
		return;
	}

	if (ScrollLevel > 6 || PotionLevel > 6)
	{
		this->GCChaosMixSend(lpObj->Index, 8, 0);
		return;
	}

	switch (ScrollLevel)
	{
	case 1:
		lpObj->ChaosSuccessRate = gServerInfo.m_IllusionTempleMixRate1[lpObj->AccountLevel];
		lpObj->ChaosMoney = 3000000;
		break;
	case 2:
		lpObj->ChaosSuccessRate = gServerInfo.m_IllusionTempleMixRate2[lpObj->AccountLevel];
		lpObj->ChaosMoney = 5000000;
		break;
	case 3:
		lpObj->ChaosSuccessRate = gServerInfo.m_IllusionTempleMixRate3[lpObj->AccountLevel];
		lpObj->ChaosMoney = 7000000;
		break;
	case 4:
		lpObj->ChaosSuccessRate = gServerInfo.m_IllusionTempleMixRate4[lpObj->AccountLevel];
		lpObj->ChaosMoney = 9000000;
		break;
	case 5:
		lpObj->ChaosSuccessRate = gServerInfo.m_IllusionTempleMixRate5[lpObj->AccountLevel];
		lpObj->ChaosMoney = 11000000;
		break;
	case 6:
		lpObj->ChaosSuccessRate = gServerInfo.m_IllusionTempleMixRate6[lpObj->AccountLevel];
		lpObj->ChaosMoney = 13000000;
		break;
	}

	if (this->GetTalismanOfLuckRate(lpObj, &lpObj->ChaosSuccessRate) == 0)
	{
		this->GCChaosMixSend(lpObj->Index, 240, 0);
		return;
	}
	
	lpObj->ChaosSuccessRate = ((lpObj->ChaosSuccessRate>100) ? 100 : lpObj->ChaosSuccessRate);
	int TaxMoney = (lpObj->ChaosMoney*gCastleSiegeSync.GetTaxRateChaos(lpObj->Index)) / 100;
	lpObj->ChaosMoney += TaxMoney;
	if (lpObj->Money < ((DWORD)lpObj->ChaosMoney))
	{
		this->GCChaosMixSend(lpObj->Index, 2, 0);
		return;
	}
	lpObj->Money -= lpObj->ChaosMoney;
	GCMoneySend(lpObj->Index, lpObj->Money);
	gCastleSiegeSync.AddTributeMoney(TaxMoney);
	int ItemSlot = 0;
	CItem item = lpObj->ChaosBox[ItemSlot];
	if ((GetLargeRand() % 100) < lpObj->ChaosSuccessRate)
	{
		GDCreateItemSend(lpObj->Index, 0xFF, 0, 0, GET_ITEM(13, 51), ScrollLevel, 1, 0, 0, 0, -1, 0, 0, 0, 0, 0, 0xFF, 0);

		gLog.Output(LOG_CHAOS_MIX, "[IllusionTempleMix][Thành Công][%s][%s] - (ChaosSuccessRate: %d, ChaosMoney: %d)", lpObj->Account, lpObj->Name, lpObj->ChaosSuccessRate, lpObj->ChaosMoney);
		
	}
	else
	{
		this->ChaosBoxInit(lpObj);
		this->GCChaosBoxSend(lpObj, 0);
		this->GCChaosMixSend(lpObj->Index, 0, 0);

		gLog.Output(LOG_CHAOS_MIX, "[IllusionTempleMix][Xịt][%s][%s] - (ChaosSuccessRate: %d, ChaosMoney: %d)", lpObj->Account, lpObj->Name, lpObj->ChaosSuccessRate, lpObj->ChaosMoney);
		
	}
}

void CChaosBox::FeatherOfCondorMix(LPOBJ lpObj) // OK
{
	int ChaosCount = 0;
	int CreationCount = 0;
	int SoulPack10 = 0;
	int WingCount = 0;
	int ItemCount = 0;
	int ItemMoney = 0;
	int ItemSlot = 0;
	for (int n = 0; n < CHAOS_BOX_SIZE; n++)
	{
		if (lpObj->ChaosBox[n].IsItem() == 0)
		{
			continue;
		}

		#if(BUNDLE_ITEM)
		if (lpObj->ChaosBox[n].m_Index == GET_ITEM(12, 15) && lpObj->ChaosBox[n].m_Durability == 1)
		#else
		if (lpObj->ChaosBox[n].m_Index == GET_ITEM(12, 15))
		#endif
		{
			ChaosCount++;
		}
		else if (lpObj->ChaosBox[n].m_Index == GET_ITEM(14, 22))
		{
			CreationCount++;
		}
		else if (lpObj->ChaosBox[n].m_Index == GET_ITEM(12, 31) && lpObj->ChaosBox[n].m_Level == 0)
		{
			SoulPack10++;
		}
		else if (((lpObj->ChaosBox[n].m_Index >= GET_ITEM(12, 3) && lpObj->ChaosBox[n].m_Index <= GET_ITEM(12, 6)) || lpObj->ChaosBox[n].m_Index == GET_ITEM(12, 42) || lpObj->ChaosBox[n].m_Index == GET_ITEM(12, 49) || lpObj->ChaosBox[n].m_Index == GET_ITEM(13, 30)) && (lpObj->ChaosBox[n].m_Level >= 9 && lpObj->ChaosBox[n].m_Option3 >= 1))
		{
			WingCount++;ItemSlot = n;
		}
		else if (lpObj->ChaosBox[n].IsSetItem() != 0 && lpObj->ChaosBox[n].m_Level >= 7 && lpObj->ChaosBox[n].m_Option3 >= 1)
		{
			ItemCount++;
			ItemMoney += lpObj->ChaosBox[n].m_BuyMoney;
		}
	}

	if (ChaosCount != 1 || CreationCount != 1 || SoulPack10 != 1 || WingCount != 1 || ItemCount == 0)
	{
		this->GCChaosMixSend(lpObj->Index, 7, 0);
		return;
	}

	if (gServerInfo.m_FeatherOfCondorMixRate[lpObj->AccountLevel] == -1)
	{
		lpObj->ChaosSuccessRate = (ItemMoney / 300000) + 1;
	}
	else
	{
		lpObj->ChaosSuccessRate = gServerInfo.m_FeatherOfCondorMixRate[lpObj->AccountLevel];
	}

	if (this->GetTalismanOfLuckRate(lpObj, &lpObj->ChaosSuccessRate) == 0)
	{
		this->GCChaosMixSend(lpObj->Index, 240, 0);
		return;
	}

	if (gServerInfo.m_FeatherOfCondorMixRate[lpObj->AccountLevel] == -1)
	{
		lpObj->ChaosSuccessRate = ((lpObj->ChaosSuccessRate>60) ? 60 : lpObj->ChaosSuccessRate);
	}
	else
	{
		
		lpObj->ChaosSuccessRate = ((lpObj->ChaosSuccessRate>100) ? 100 : lpObj->ChaosSuccessRate);
	}

	lpObj->ChaosMoney = lpObj->ChaosSuccessRate * 200000;

	int TaxMoney = (lpObj->ChaosMoney*gCastleSiegeSync.GetTaxRateChaos(lpObj->Index)) / 100;

	lpObj->ChaosMoney += TaxMoney;

	if (lpObj->Money < ((DWORD)lpObj->ChaosMoney))
	{
		this->GCChaosMixSend(lpObj->Index, 2, 0);
		return;
	}

	lpObj->Money -= lpObj->ChaosMoney;
	GCMoneySend(lpObj->Index, lpObj->Money);
	gCastleSiegeSync.AddTributeMoney(TaxMoney);
	
	CItem item = lpObj->ChaosBox[ItemSlot];
	if ((GetLargeRand() % 100) < lpObj->ChaosSuccessRate)
	{
		GDCreateItemSend(lpObj->Index, 0xFF, 0, 0, GET_ITEM(13, 53), 0, 0, 0, 0, 0, -1, 0, 0, 0, 0, 0, 0xFF, 0);
		
		gLog.Output(LOG_CHAOS_MIX, "[Lông Vũ 3][Thành Công][%s/%s]Từ: %s+%d,  %d %d %d %d %d, HC: %03d, 380: %03d, Socket: %03d, %03d, %03d, %03d, %03d, Rate: %d%%, Money: %d, Serial: %08X)"
				, lpObj->Account
				, lpObj->Name
				, gItemManager.GetItemName(item.m_Index)
				, item.m_Level
				, item.m_Option1
				, item.m_Option2
				, item.m_Option3 * 4
				, item.m_NewOption
				, item.m_SetOption
				, item.m_JewelOfHarmonyOption
				, item.m_ItemOptionEx
				, item.m_SocketOption[0]
				, item.m_SocketOption[1]
				, item.m_SocketOption[2]
				, item.m_SocketOption[3]
				, item.m_SocketOption[4]
				, lpObj->ChaosSuccessRate
				, lpObj->ChaosMoney				
				, item.m_Serial);		

	}
	else
	{
		this->ChaosBoxInit(lpObj);
		this->GCChaosBoxSend(lpObj, 0);
		this->GCChaosMixSend(lpObj->Index, 0, 0);
				gLog.Output(LOG_CHAOS_MIX, "[FeatherOfCondorMix][Thất bại][%s/%s] Từ: %s+%d,  %d %d %d %d %d, HC: %03d, 380: %03d, Socket: %03d, %03d, %03d, %03d, %03d, Rate: %d%%, Money: %d, Serial: %08X)"
				, lpObj->Account
				, lpObj->Name
				, gItemManager.GetItemName(item.m_Index)
				, item.m_Level
				, item.m_Option1
				, item.m_Option2
				, item.m_Option3 * 4
				, item.m_NewOption
				, item.m_SetOption
				, item.m_JewelOfHarmonyOption
				, item.m_ItemOptionEx
				, item.m_SocketOption[0]
				, item.m_SocketOption[1]
				, item.m_SocketOption[2]
				, item.m_SocketOption[3]
				, item.m_SocketOption[4]
				, lpObj->ChaosSuccessRate
				, lpObj->ChaosMoney				
				, item.m_Serial);

	}
}

void CChaosBox::Wing3Mix(LPOBJ lpObj) // OK
{
	int ChaosCount = 0;
	int CreationCount = 0;
	int SoulPack10 = 0;
	int BlessPack10 = 0;
	int FlameCount = 0;
	int FeatherCount = 0;
	int ItemCount = 0;
	int ItemMoney = 0;
	int ItemSlot = 0;

	for (int n = 0; n < CHAOS_BOX_SIZE; n++)
	{
		if (lpObj->ChaosBox[n].IsItem() == 0)
		{
			continue;
		}

		#if(BUNDLE_ITEM)
		if (lpObj->ChaosBox[n].m_Index == GET_ITEM(12, 15) && lpObj->ChaosBox[n].m_Durability == 1)
		#else
		if (lpObj->ChaosBox[n].m_Index == GET_ITEM(12, 15))
		#endif
		{
			ChaosCount++;
		}
		else if (lpObj->ChaosBox[n].m_Index == GET_ITEM(14, 22))
		{
			CreationCount++;
		}
		else if (lpObj->ChaosBox[n].m_Index == GET_ITEM(12, 31) && lpObj->ChaosBox[n].m_Level == 0)
		{
			SoulPack10++;
		}
		else if (lpObj->ChaosBox[n].m_Index == GET_ITEM(12, 30) && lpObj->ChaosBox[n].m_Level == 0)
		{
			BlessPack10++;
		}
		else if (lpObj->ChaosBox[n].m_Index == GET_ITEM(13, 52))
		{
			FlameCount++;
		}
		else if (lpObj->ChaosBox[n].m_Index == GET_ITEM(13, 53))
		{
			FeatherCount++;
		}
		else if (lpObj->ChaosBox[n].IsExcItem() != 0 && lpObj->ChaosBox[n].m_Level >= 9 && lpObj->ChaosBox[n].m_Option3 >= 1)
		{
			ItemCount++;
			ItemMoney += lpObj->ChaosBox[n].m_BuyMoney;
			ItemSlot = n;
		}
	}

	if (ChaosCount != 1 || CreationCount != 1 || SoulPack10 != 1 || BlessPack10 != 1 || FlameCount != 1 || FeatherCount != 1 || ItemCount == 0)
	{
		this->GCChaosMixSend(lpObj->Index, 7, 0);
		return;
	}

	if (gServerInfo.m_Wing3MixRate[lpObj->AccountLevel] == -1)
	{
		lpObj->ChaosSuccessRate = (ItemMoney / 3000000) + 1;
	}
	else
	{
		lpObj->ChaosSuccessRate = gServerInfo.m_Wing3MixRate[lpObj->AccountLevel];
	}

	if (this->GetTalismanOfLuckRate(lpObj, &lpObj->ChaosSuccessRate) == 0)
	{
		this->GCChaosMixSend(lpObj->Index, 240, 0);
		return;
	}

	if (gServerInfo.m_Wing3MixRate[lpObj->AccountLevel] == -1)
	{
		lpObj->ChaosSuccessRate = ((lpObj->ChaosSuccessRate>40) ? 40 : lpObj->ChaosSuccessRate);
	}
	else
	{
		
		lpObj->ChaosSuccessRate = ((lpObj->ChaosSuccessRate>100) ? 100 : lpObj->ChaosSuccessRate);
	}

	lpObj->ChaosMoney = lpObj->ChaosSuccessRate * 200000;

	int TaxMoney = (lpObj->ChaosMoney*gCastleSiegeSync.GetTaxRateChaos(lpObj->Index)) / 100;

	lpObj->ChaosMoney += TaxMoney;

	if (lpObj->Money < ((DWORD)lpObj->ChaosMoney))
	{
		this->GCChaosMixSend(lpObj->Index, 2, 0);
		return;
	}

	lpObj->Money -= lpObj->ChaosMoney;

	GCMoneySend(lpObj->Index, lpObj->Money);

	gCastleSiegeSync.AddTributeMoney(TaxMoney);

	
	CItem item = lpObj->ChaosBox[ItemSlot];

	if ((GetLargeRand() % 100) < lpObj->ChaosSuccessRate)
	{
		WORD ItemIndex = 0;
		BYTE ItemOption2 = 0;
		BYTE ItemOption3 = 0;
		BYTE ItemNewOption = 0;
		CRandomManager RandomManager;
		RandomManager.AddElement(GET_ITEM(12, 36), 1);
		RandomManager.AddElement(GET_ITEM(12, 37), 1);
		RandomManager.AddElement(GET_ITEM(12, 38), 1);
		RandomManager.AddElement(GET_ITEM(12, 39), 1);
		for (int i = 0; i < gCustomWingMix.m_count; i++)
		{
			RandomManager.AddElement(GET_ITEM(gCustomWingMix.m_Data[i].m_Category, gCustomWingMix.m_Data[i].m_Index), 1);
		}

		if (gServerInfo.m_DisableWingMixDL != 1)
		{
			RandomManager.AddElement(GET_ITEM(12, 40), 1);
		}

		if (gServerInfo.m_DisableWingMixSU != 1)
		{
			RandomManager.AddElement(GET_ITEM(12, 43), 1);
		}

		#if(GAMESERVER_UPDATE>=601)
		if (gServerInfo.m_DisableWingMixRF != 1)
		{
			RandomManager.AddElement(GET_ITEM(12, 50), 1);
		}
		#endif
		RandomManager.GetRandomElement(&ItemIndex);
		gItemOptionRate.GetItemOption2(6, &ItemOption2);
		gItemOptionRate.GetItemOption3(6, &ItemOption3);
		gItemOptionRate.GetItemOption4(6, &ItemNewOption);
		gItemOptionRate.MakeNewOption(ItemIndex, ItemNewOption, &ItemNewOption);
		GDCreateItemSend(lpObj->Index, 0xFF, 0, 0, ItemIndex, 0, 0, 0, ItemOption2, ItemOption3, -1, (ItemNewOption + (16 * (GetLargeRand() % 3))), 0, 0, 0, 0, 0xFF, 0);
		gNotice.GCNoticeSendToAll(0, 0, 0, 0, 0, 0, gMessage.GetMessage(902), lpObj->Name, gItemManager.GetItemName(ItemIndex));

		gLog.Output(LOG_CHAOS_MIX, "[Wing3Mix][Thành Công][%s/%s] Tạo Thành: %s, Luck: %d, Op: %d, Hoàn hảo: %d,  Từ: %s+%d,  %d %d %d %d %d, HC: %03d, 380: %03d, Socket: %03d, %03d, %03d, %03d, %03d, Rate: %d%%, Money: %d, Serial: %08X)"
				, lpObj->Account
				, lpObj->Name
				,gItemManager.GetItemName(ItemIndex),ItemOption2,ItemOption3,ItemNewOption

				, gItemManager.GetItemName(item.m_Index)
				, item.m_Level
				, item.m_Option1
				, item.m_Option2
				, item.m_Option3 * 4
				, item.m_NewOption
				, item.m_SetOption
				, item.m_JewelOfHarmonyOption
				, item.m_ItemOptionEx
				, item.m_SocketOption[0]
				, item.m_SocketOption[1]
				, item.m_SocketOption[2]
				, item.m_SocketOption[3]
				, item.m_SocketOption[4]
				, lpObj->ChaosSuccessRate
				, lpObj->ChaosMoney				
				, item.m_Serial);		

	}
	else
	{
		this->ChaosBoxInit(lpObj);
		this->GCChaosBoxSend(lpObj, 0);
		this->GCChaosMixSend(lpObj->Index, 0, 0);
		gNotice.GCNoticeSendToAll(0, 0, 0, 0, 0, 0, gMessage.GetMessage(891), lpObj->Name);

		gLog.Output(LOG_CHAOS_MIX, "[Wing3Mix][Xịt][%s/%s] Từ: %s+%d,  %d %d %d %d %d, HC: %03d, 380: %03d, Socket: %03d, %03d, %03d, %03d, %03d, Rate: %d%%, Money: %d, Serial: %08X)"
				, lpObj->Account
				, lpObj->Name
				, gItemManager.GetItemName(item.m_Index)
				, item.m_Level
				, item.m_Option1
				, item.m_Option2
				, item.m_Option3 * 4
				, item.m_NewOption
				, item.m_SetOption
				, item.m_JewelOfHarmonyOption
				, item.m_ItemOptionEx
				, item.m_SocketOption[0]
				, item.m_SocketOption[1]
				, item.m_SocketOption[2]
				, item.m_SocketOption[3]
				, item.m_SocketOption[4]
				, lpObj->ChaosSuccessRate
				, lpObj->ChaosMoney				
				, item.m_Serial);		

	}
}

void CChaosBox::CustomItemMix(LPOBJ lpObj, int HeadCode) // OK
{
	CUSTOM_MIX* CustomMix = gCustomMix.GetCustomMix(HeadCode);
	int ItemSlot = 0;
		CItem item = lpObj->ChaosBox[ItemSlot];
	if (CustomMix != 0)
	{
		int OptionExcelente1[6] = { 1, 2, 4, 8, 16, 32 };
		int OptionExcelente2[15] = { 3, 5, 6, 9, 10, 12, 17, 18, 20, 24, 33, 34, 36, 40, 48 };
		int OptionExcelente3[20] = { 7, 11, 13, 14, 19, 21, 22, 25, 26, 28, 35, 37, 38, 41, 42, 44, 49, 50, 52, 56 };
		int OptionExcelente4[15] = { 15, 23, 27, 29, 30, 39, 43, 45, 46, 51, 53, 54, 57, 58, 60 };
		int OptionExcelente5[6] = { 31, 47, 55, 59, 61, 62 };
		int itemMixCount = 0;
		int RenderGroup = -1;
		int ItemBagMixCount = 0;
		int ItemMixGroupCount = CustomMix->m_CountItem - gCustomMix.GetCountItemBagMix(HeadCode);
		for (int slot = 0; slot < CHAOS_BOX_SIZE; slot++)
		{
			if (lpObj->ChaosBox[slot].IsItem() == 0)
			{
				continue;
			}

			if (gCustomMix.istItemBagMix(HeadCode, lpObj->ChaosBox[slot]))
			{
				ItemBagMixCount++;
			}
		}
		for (int Group = 0; Group < CustomMix->m_CountGroup; Group++)
		{
			itemMixCount = 0;

			for (int slot = 0; slot < CHAOS_BOX_SIZE; slot++)
			{
				if (lpObj->ChaosBox[slot].IsItem() == 0)
				{
					continue;
				}

				if (gCustomMix.istItemMix(CustomMix->m_Index, Group, lpObj->ChaosBox[slot]))	// validación si existe el item
				{
					itemMixCount++;
				}
			}

			if (itemMixCount == ItemMixGroupCount)
			{
				RenderGroup = Group;
				Group = CustomMix->m_CountGroup;
			}
		}

		if ((itemMixCount + ItemBagMixCount) != CustomMix->m_CountItem) // falló la combinación
		{
			this->GCChaosMixSend(lpObj->Index, 7, 0);
			return;
		}

		if (this->GetTalismanOfLuckRate(lpObj, &lpObj->ChaosSuccessRate) == 0)
		{
			this->GCChaosMixSend(lpObj->Index, 240, 0);
			return;
		}

		switch (lpObj->AccountLevel)
		{
			case 1:		lpObj->ChaosSuccessRate = ((CustomMix->m_MixRate_AL1 < 0 || CustomMix->m_MixRate_AL1 > 100) ? 90 : CustomMix->m_MixRate_AL1);			break;
			case 2:		lpObj->ChaosSuccessRate = ((CustomMix->m_MixRate_AL2 < 0 || CustomMix->m_MixRate_AL2 > 100) ? 90 : CustomMix->m_MixRate_AL2);			break;
			case 3:		lpObj->ChaosSuccessRate = ((CustomMix->m_MixRate_AL3 < 0 || CustomMix->m_MixRate_AL3 > 100) ? 90 : CustomMix->m_MixRate_AL3);			break;
			default:	lpObj->ChaosSuccessRate = ((CustomMix->m_MixRate_AL0 < 0 || CustomMix->m_MixRate_AL0 > 100) ? 90 : CustomMix->m_MixRate_AL0);			break;
		}

		lpObj->ChaosMoney = lpObj->ChaosSuccessRate * ((CustomMix->m_MixMoney < 0) ? 1000 : CustomMix->m_MixMoney);
		int TaxMoney = (lpObj->ChaosMoney*gCastleSiegeSync.GetTaxRateChaos(lpObj->Index)) / 100;
		lpObj->ChaosMoney += TaxMoney;
		if (lpObj->Money < ((DWORD)lpObj->ChaosMoney))
		{
			this->GCChaosMixSend(lpObj->Index, 2, 0);
			return;
		}
		lpObj->Money -= lpObj->ChaosMoney;
		GCMoneySend(lpObj->Index, lpObj->Money);
		gCastleSiegeSync.AddTributeMoney(TaxMoney);
		if ((GetLargeRand() % 100) < lpObj->ChaosSuccessRate)
		{
			WORD ItemIndex = 0;
			BYTE ItemLevel = 0;
			BYTE ItemSkill = 0;
			BYTE ItemLuck = 0;
			BYTE ItemOption = 0;
			BYTE ItemExcellent = 0;
			BYTE ItemSetAncestral = 0;
			BYTE ItemSocketOption[MAX_SOCKET_OPTION] = { SOCKET_ITEM_OPTION_NONE, SOCKET_ITEM_OPTION_NONE, SOCKET_ITEM_OPTION_NONE, SOCKET_ITEM_OPTION_NONE, SOCKET_ITEM_OPTION_NONE };
			CRandomManager RandomManager = gCustomMix.RandomManager(HeadCode, RenderGroup);
			RandomManager.GetRandomElement(&ItemIndex);
			CUSTOM_ITEM_MIX_RESULT * ItemCreate = gCustomMix.GetItemResult(HeadCode, RenderGroup, ItemIndex);
			if (CustomMix->m_OpcionExeRand == 0)
			{
				ItemExcellent = (ItemCreate->m_Excelent < 0) ? 0 : ItemCreate->m_Excelent;
			}
			else
			{
				switch (ItemCreate->m_Excelent)
				{
					case 1:	 ItemExcellent = OptionExcelente1[rand() % 6];					break;
					case 2:	 ItemExcellent = OptionExcelente2[rand() % 15];					break;
					case 3:	 ItemExcellent = OptionExcelente3[rand() % 20];					break;
					case 4:	 ItemExcellent = OptionExcelente4[rand() % 15];					break;
					case 5:	 ItemExcellent = OptionExcelente5[rand() % 6];					break;
					case 6:	 ItemExcellent = 63;					break;
					default: ItemExcellent = 0;					break;
				}
			}

			if (ItemCreate->m_Opcion > 7)
			{
				ItemOption = 7;
			}
			else if (ItemCreate->m_Opcion < 0)
			{
				ItemOption = 0;
			}
			else
			{
				ItemOption = ItemCreate->m_Opcion;
			}


			int qtd = gSocketItemType.GetSocketItemMaxSocket(ItemIndex);
			ItemSocketOption[0] = (BYTE)((qtd > 0) ? ((ItemCreate->Socket1 != 255) ? ItemCreate->Socket1 : 255) : SOCKET_ITEM_OPTION_NONE);
			ItemSocketOption[1] = (BYTE)((qtd > 1) ? ((ItemCreate->Socket2 != 255) ? ItemCreate->Socket2 : 255) : SOCKET_ITEM_OPTION_NONE);
			ItemSocketOption[2] = (BYTE)((qtd > 2) ? ((ItemCreate->Socket3 != 255) ? ItemCreate->Socket3 : 255) : SOCKET_ITEM_OPTION_NONE);
			ItemSocketOption[3] = (BYTE)((qtd > 3) ? ((ItemCreate->Socket4 != 255) ? ItemCreate->Socket4 : 255) : SOCKET_ITEM_OPTION_NONE);
			ItemSocketOption[4] = (BYTE)((qtd > 4) ? ((ItemCreate->Socket5 != 255) ? ItemCreate->Socket5 : 255) : SOCKET_ITEM_OPTION_NONE);

			GDCreateItemSend(lpObj->Index,
				0xFF, 0, 0, ItemIndex,
				(ItemCreate->m_ItemLevel != -1) ? ItemCreate->m_ItemLevel : rand() % 6, 0,
				((ItemCreate->m_Skill != 0) ? gItemManager.CheckItemSkill(ItemIndex) : 0),
				(ItemCreate->m_Luck != -1) ? 1 : rand() % 2,
				ItemOption,
				-1, ItemExcellent,
				(ItemCreate->m_SetACC != -1) ? ItemCreate->m_SetACC : rand() % 2,
				0, 0, ItemSocketOption, 0xFF, 0);

			gNotice.GCNoticeSendToAll(0, 0, 0, 0, 0, 0, gMessage.GetMessage(902), lpObj->Name, gItemManager.GetItemName(ItemIndex));
			
			gLog.Output(LOG_CHAOS_MIX, "CustomMix[%s/%s]ID:[%d]:Chế tạo %s+%d,  %d  %d  %d %d, sk: %03d, %03d, %03d, %03d, %03d,Từ: %s+%d,  %d %d %d %d %d, HC: %03d, 380: %03d, Socket: %03d, %03d, %03d, %03d, %03d, Rate: %d%%, Money: %d, Serial: %08X)"
				, lpObj->Account
				, lpObj->Name,ItemIndex,
				gItemManager.GetItemName(ItemIndex),(ItemCreate->m_ItemLevel != -1) ? ItemCreate->m_ItemLevel : rand() % 6,
				(ItemCreate->m_Luck != -1) ? 1 : rand() % 2,
				
				ItemOption,ItemExcellent,ItemSetAncestral,ItemSocketOption[0],ItemSocketOption[1],ItemSocketOption[2],ItemSocketOption[3],ItemSocketOption[4]

				, gItemManager.GetItemName(item.m_Index)
				, item.m_Level
				, item.m_Option1
				, item.m_Option2
				, item.m_Option3 * 4
				, item.m_NewOption
				, item.m_SetOption
				, item.m_JewelOfHarmonyOption
				, item.m_ItemOptionEx
				, item.m_SocketOption[0]
				, item.m_SocketOption[1]
				, item.m_SocketOption[2]
				, item.m_SocketOption[3]
				, item.m_SocketOption[4]
				, lpObj->ChaosSuccessRate
				, lpObj->ChaosMoney				
				, item.m_Serial);			

		}
		else
		{
			this->ChaosBoxInit(lpObj);
			this->GCChaosBoxSend(lpObj, 0);
			this->GCChaosMixSend(lpObj->Index, 0, 0);
			gNotice.GCNoticeSendToAll(0, 0, 0, 0, 0, 0, gMessage.GetMessage(903), lpObj->Name);
			
			gLog.Output(LOG_CHAOS_MIX, "[CustomMix][Xịt][%s/%s] Từ: %s+%d,  %d %d %d %d %d, HC: %03d, 380: %03d, Socket: %03d, %03d, %03d, %03d, %03d, Rate: %d%%, Money: %d, Serial: %08X)"
				, lpObj->Account
				, lpObj->Name
				, gItemManager.GetItemName(item.m_Index)
				, item.m_Level
				, item.m_Option1
				, item.m_Option2
				, item.m_Option3 * 4
				, item.m_NewOption
				, item.m_SetOption
				, item.m_JewelOfHarmonyOption
				, item.m_ItemOptionEx
				, item.m_SocketOption[0]
				, item.m_SocketOption[1]
				, item.m_SocketOption[2]
				, item.m_SocketOption[3]
				, item.m_SocketOption[4]
				, lpObj->ChaosSuccessRate
				, lpObj->ChaosMoney				
				, item.m_Serial);			

		}
	}
	else
	{
		gNotice.GCNoticeSendToAll(0, 0, 0, 0, 0, 0, gMessage.GetMessage(903), lpObj->Name);
		 
		gLog.Output(LOG_CHAOS_MIX, "[CustomMix][Lỗi][%s/%s]   %s+%d,  %d %d %d %d %d, HC: %03d, 380: %03d, Socket: %03d, %03d, %03d, %03d, %03d, Rate: %d%%, Money: %d, Serial: %08X)"
				, lpObj->Account
				, lpObj->Name
				, gItemManager.GetItemName(item.m_Index)
				, item.m_Level
				, item.m_Option1
				, item.m_Option2
				, item.m_Option3 * 4
				, item.m_NewOption
				, item.m_SetOption
				, item.m_JewelOfHarmonyOption
				, item.m_ItemOptionEx
				, item.m_SocketOption[0]
				, item.m_SocketOption[1]
				, item.m_SocketOption[2]
				, item.m_SocketOption[3]
				, item.m_SocketOption[4]
				, lpObj->ChaosSuccessRate
				, lpObj->ChaosMoney				
				, item.m_Serial);	
		
	}
}
//=================================================================================================
void CChaosBox::ChaosCardMix(LPOBJ lpObj, int type) // OK
{
	int ChaosCardCount = 0;
	int ChaosCardGoldCount = 0;
	int ChaosCardRareCount = 0;
	int ChaosCardMiniCount = 0;
	for (int n = 0; n < CHAOS_BOX_SIZE; n++)
	{
		if (lpObj->ChaosBox[n].IsItem() == 0)
		{
			continue;
		}

		if (lpObj->ChaosBox[n].m_Index == GET_ITEM(14, 54))
		{
			type = 1;
			ChaosCardCount++;
		}
		else if (lpObj->ChaosBox[n].m_Index == GET_ITEM(14, 92))
		{
			type = 2;
			ChaosCardGoldCount++;
		}
		else if (lpObj->ChaosBox[n].m_Index == GET_ITEM(14, 93))
		{
			type = 3;
			ChaosCardRareCount++;
		}
		else if (lpObj->ChaosBox[n].m_Index == GET_ITEM(14, 95))
		{
			type = 4;
			ChaosCardMiniCount++;
		}
	}

	if (type == 0 || (type == 1 && ChaosCardCount != 1) || (type == 2 && ChaosCardGoldCount != 1) || (type == 3 && ChaosCardRareCount != 1) || (type == 4 && ChaosCardMiniCount != 1))
	{
		this->GCChaosMixSend(lpObj->Index, 7, 0);
		return;
	}

	CItem item;

	if (gItemBagManager.GetItemBySpecialValue((ITEM_BAG_CHAOS_CARD_MIX1 + (type - 1)), lpObj, &item) == 0)
	{
		this->GCChaosMixSend(lpObj->Index, 0, 0);
		return;
	}

	GDCreateItemSend(lpObj->Index, 0xFF, 0, 0, item.m_Index, (BYTE)item.m_Level, 0, (BYTE)item.m_Option1, (BYTE)item.m_Option2, (BYTE)item.m_Option3, -1, item.m_NewOption, item.m_SetOption, item.m_JewelOfHarmonyOption, item.m_ItemOptionEx, item.m_SocketOption, item.m_SocketOptionBonus, 0);
	gLog.Output(LOG_CHAOS_MIX, "[ChaosCardMix][Thành Công][%s/%s] IETM: %s+%d,  %d %d %d %d %d, HC: %03d, 380: %03d, Socket: %03d, %03d, %03d, %03d, %03d, Rate: %d%%, Money: %d, Serial: %08X)"
				, lpObj->Account
				, lpObj->Name
				, gItemManager.GetItemName(item.m_Index)
				, item.m_Level
				, item.m_Option1
				, item.m_Option2
				, item.m_Option3 * 4
				, item.m_NewOption
				, item.m_SetOption
				, item.m_JewelOfHarmonyOption
				, item.m_ItemOptionEx
				, item.m_SocketOption[0]
				, item.m_SocketOption[1]
				, item.m_SocketOption[2]
				, item.m_SocketOption[3]
				, item.m_SocketOption[4]
				, lpObj->ChaosSuccessRate
				, lpObj->ChaosMoney				
				, item.m_Serial);	

}

void CChaosBox::CherryBlossomMix(LPOBJ lpObj, int type) // OK
{
	int WhiteCherryBlossomCount = 0;
	int RedCherryBlossomCount = 0;
	int GoldenCherryBlossomCount = 0;

	for (int n = 0; n < CHAOS_BOX_SIZE; n++)
	{
		if (lpObj->ChaosBox[n].IsItem() == 0)
		{
			continue;
		}

		if (lpObj->ChaosBox[n].m_Index == GET_ITEM(14, 88))
		{
			type = 1;
			WhiteCherryBlossomCount += (int)lpObj->ChaosBox[n].m_Durability;
		}
		else if (lpObj->ChaosBox[n].m_Index == GET_ITEM(14, 89))
		{
			type = 2;
			RedCherryBlossomCount += (int)lpObj->ChaosBox[n].m_Durability;
		}
		else if (lpObj->ChaosBox[n].m_Index == GET_ITEM(14, 90))
		{
			type = 3;
			GoldenCherryBlossomCount += (int)lpObj->ChaosBox[n].m_Durability;
		}
	}

	if (type == 0 || (type == 1 && WhiteCherryBlossomCount != 10) || (type == 2 && RedCherryBlossomCount != 30) || (type == 3 && GoldenCherryBlossomCount != 255))
	{
		this->GCChaosMixSend(lpObj->Index, 7, 0);
		return;
	}

	CItem item;

	if (gItemBagManager.GetItemBySpecialValue((ITEM_BAG_CHERRY_BLOSSOM_MIX1 + (type - 1)), lpObj, &item) == 0)
	{
		this->GCChaosMixSend(lpObj->Index, 0, 0);
		return;
	}

	GDCreateItemSend(lpObj->Index, 0xFF, 0, 0, item.m_Index, (BYTE)item.m_Level, 0, (BYTE)item.m_Option1, (BYTE)item.m_Option2, (BYTE)item.m_Option3, -1, item.m_NewOption, item.m_SetOption, item.m_JewelOfHarmonyOption, item.m_ItemOptionEx, item.m_SocketOption, item.m_SocketOptionBonus, 0);

	gLog.Output(LOG_CHAOS_MIX, "[Hoa Tiên][Thành Công][%s/%s] ITEM: %s+%d,  %d %d %d %d %d, HC: %03d, 380: %03d, Socket: %03d, %03d, %03d, %03d, %03d, Rate: %d%%, Money: %d, Serial: %08X)"
				, lpObj->Account
				, lpObj->Name
				, gItemManager.GetItemName(item.m_Index)
				, item.m_Level
				, item.m_Option1
				, item.m_Option2
				, item.m_Option3 * 4
				, item.m_NewOption
				, item.m_SetOption
				, item.m_JewelOfHarmonyOption
				, item.m_ItemOptionEx
				, item.m_SocketOption[0]
				, item.m_SocketOption[1]
				, item.m_SocketOption[2]
				, item.m_SocketOption[3]
				, item.m_SocketOption[4]
				, lpObj->ChaosSuccessRate
				, lpObj->ChaosMoney				
				, item.m_Serial);	

}

void CChaosBox::SocketItemCreateSeedMix(LPOBJ lpObj) // OK
{
	int ChaosCount = 0;
	int CreationCount = 0;
	int HarmonyCount = 0;
	int ExcItemCount = 0;
	int SetItemCount = 0;
	int ItemMoney = 0;
	int ItemSlot = 0;

	for (int n = 0; n < CHAOS_BOX_SIZE; n++)
	{
		if (lpObj->ChaosBox[n].IsItem() == 0)
		{
			continue;
		}

		#if(BUNDLE_ITEM)
		if (lpObj->ChaosBox[n].m_Index == GET_ITEM(12, 15) && lpObj->ChaosBox[n].m_Durability == 1)
		#else
		if (lpObj->ChaosBox[n].m_Index == GET_ITEM(12, 15))
		#endif
		{
			ChaosCount++;
		}
		else if (lpObj->ChaosBox[n].m_Index == GET_ITEM(14, 22))
		{
			CreationCount++;
		}
		else if (lpObj->ChaosBox[n].m_Index == GET_ITEM(14, 42))
		{
			HarmonyCount++;
		}
		else if (lpObj->ChaosBox[n].IsExcItem() != 0 && lpObj->ChaosBox[n].m_Level >= 4 && ExcItemCount == 0)
		{
			ExcItemCount++;
			ItemMoney += lpObj->ChaosBox[n].m_BuyMoney;
		}
		else if (lpObj->ChaosBox[n].IsSetItem() != 0 && lpObj->ChaosBox[n].m_Level >= 4 && SetItemCount == 0)
		{
			SetItemCount++;
			ItemMoney += lpObj->ChaosBox[n].m_BuyMoney;ItemSlot = n;
		}
	}

	if (ChaosCount != 1 || CreationCount != 1 || HarmonyCount != 1 || ExcItemCount != 1 || SetItemCount != 1)
	{
		this->GCChaosMixSend(lpObj->Index, 7, 0);
		return;
	}

	if (gServerInfo.m_SocketItemCreateSeedMixRate[lpObj->AccountLevel] == -1)
	{
		lpObj->ChaosSuccessRate = (ItemMoney / 2000000) + 80;
	}
	else
	{
		lpObj->ChaosSuccessRate = gServerInfo.m_SocketItemCreateSeedMixRate[lpObj->AccountLevel];
	}

	if (gServerInfo.m_SocketItemCreateSeedMixRate[lpObj->AccountLevel] == -1)
	{
		lpObj->ChaosSuccessRate = ((lpObj->ChaosSuccessRate>90) ? 90 : lpObj->ChaosSuccessRate);
	}
	else
	{
		
		lpObj->ChaosSuccessRate = ((lpObj->ChaosSuccessRate>100) ? 100 : lpObj->ChaosSuccessRate);
	}

	lpObj->ChaosMoney = 1000000;

	int TaxMoney = (lpObj->ChaosMoney*gCastleSiegeSync.GetTaxRateChaos(lpObj->Index)) / 100;

	lpObj->ChaosMoney += TaxMoney;

	if (lpObj->Money < ((DWORD)lpObj->ChaosMoney))
	{
		this->GCChaosMixSend(lpObj->Index, 2, 0);
		return;
	}

	lpObj->Money -= lpObj->ChaosMoney;

	GCMoneySend(lpObj->Index, lpObj->Money);

	gCastleSiegeSync.AddTributeMoney(TaxMoney);

	
	CItem item = lpObj->ChaosBox[ItemSlot];
	if ((GetLargeRand() % 100) < lpObj->ChaosSuccessRate)
	{
		int SocketOptionType = 1 + (GetLargeRand() % MAX_SOCKET_ITEM_OPTION_TYPE);

		int SocketOptionIndex = gSocketItemOption.GetSocketItemRandomOption(SocketOptionType);

		int SocketOptionValue = gSocketItemOption.GetSocketItemSeedOption(SocketOptionIndex, SocketOptionType);

		if (SocketOptionIndex == -1 || SocketOptionValue == -1)
		{
			this->GCChaosMixSend(lpObj->Index, 0, 0);
			return;
		}

		GDCreateItemSend(lpObj->Index, 0xFF, 0, 0, (GET_ITEM(12, 60) + (SocketOptionType - 1)), SocketOptionValue, 0, 0, 0, 0, -1, 0, 0, 0, 0, 0, 0xFF, 0);

		gLog.Output(LOG_CHAOS_MIX, "[Tạo Hạt Socket][Thành Công][%s][%s] -Item :%s : Op: %d (ChaosSuccessRate: %d, ChaosMoney: %d)", lpObj->Account, lpObj->Name,gItemManager.GetItemName((GET_ITEM(12, 60) + (SocketOptionType - 1))),SocketOptionValue, lpObj->ChaosSuccessRate, lpObj->ChaosMoney);
	
	}
	else
	{
		this->ChaosBoxInit(lpObj);
		this->GCChaosBoxSend(lpObj, 0);
		this->GCChaosMixSend(lpObj->Index, 0, 0);

		gLog.Output(LOG_CHAOS_MIX, "[Tạo Hạt Socket][Xịt][%s/%s] Từ %s+%d,  %d %d %d %d %d, HC: %03d, 380: %03d, Socket: %03d, %03d, %03d, %03d, %03d, Rate: %d%%, Money: %d, Serial: %08X)"
				, lpObj->Account
				, lpObj->Name
				, gItemManager.GetItemName(item.m_Index)
				, item.m_Level
				, item.m_Option1
				, item.m_Option2
				, item.m_Option3 * 4
				, item.m_NewOption
				, item.m_SetOption
				, item.m_JewelOfHarmonyOption
				, item.m_ItemOptionEx
				, item.m_SocketOption[0]
				, item.m_SocketOption[1]
				, item.m_SocketOption[2]
				, item.m_SocketOption[3]
				, item.m_SocketOption[4]
				, lpObj->ChaosSuccessRate
				, lpObj->ChaosMoney				
				, item.m_Serial);		

	}
}

void CChaosBox::SocketItemCreateSeedSphereMix(LPOBJ lpObj) // OK
{
	int ChaosCount = 0;
	int CreationCount = 0;
	int SeedCount = 0;
	int SeedType = 0;
	int SeedLevel = 0;
	int SphereCount = 0;
	int SphereType = 0;
	int ItemMoney = 0;

	for (int n = 0; n < CHAOS_BOX_SIZE; n++)
	{
		if (lpObj->ChaosBox[n].IsItem() == 0)
		{
			continue;
		}

		#if(BUNDLE_ITEM)
		if (lpObj->ChaosBox[n].m_Index == GET_ITEM(12, 15) && lpObj->ChaosBox[n].m_Durability == 1)
		#else
		if (lpObj->ChaosBox[n].m_Index == GET_ITEM(12, 15))
		#endif
		{
			ChaosCount++;
		}
		else if (lpObj->ChaosBox[n].m_Index == GET_ITEM(14, 22))
		{
			CreationCount++;
		}
		else if (lpObj->ChaosBox[n].m_Index >= GET_ITEM(12, 60) && lpObj->ChaosBox[n].m_Index <= GET_ITEM(12, 66))
		{
			SeedCount++;
			SeedType = lpObj->ChaosBox[n].m_Index - GET_ITEM(12, 60);
			SeedLevel = lpObj->ChaosBox[n].m_Level;
			ItemMoney += lpObj->ChaosBox[n].m_BuyMoney;
		}
		else if (lpObj->ChaosBox[n].m_Index >= GET_ITEM(12, 70) && lpObj->ChaosBox[n].m_Index <= GET_ITEM(12, 74))
		{
			SphereCount++;
			SphereType = lpObj->ChaosBox[n].m_Index - GET_ITEM(12, 70);
			ItemMoney += lpObj->ChaosBox[n].m_BuyMoney;
		}
	}

	if (ChaosCount != 1 || CreationCount != 1 || SeedCount != 1 || SphereCount != 1)
	{
		this->GCChaosMixSend(lpObj->Index, 7, 0);
		return;
	}

	if (gServerInfo.m_SocketItemCreateSeedSphereMixRate[lpObj->AccountLevel] == -1)
	{
		lpObj->ChaosSuccessRate = (ItemMoney / 200000) + 80;
	}
	else
	{
		lpObj->ChaosSuccessRate = gServerInfo.m_SocketItemCreateSeedSphereMixRate[lpObj->AccountLevel];
	}

	if (gServerInfo.m_SocketItemCreateSeedSphereMixRate[lpObj->AccountLevel] == -1)
	{
		lpObj->ChaosSuccessRate = ((lpObj->ChaosSuccessRate>90) ? 90 : lpObj->ChaosSuccessRate);
	}
	else
	{
		
		lpObj->ChaosSuccessRate = ((lpObj->ChaosSuccessRate>100) ? 100 : lpObj->ChaosSuccessRate);
	}

	lpObj->ChaosMoney = 1000000;

	int TaxMoney = (lpObj->ChaosMoney*gCastleSiegeSync.GetTaxRateChaos(lpObj->Index)) / 100;

	lpObj->ChaosMoney += TaxMoney;

	if (lpObj->Money < ((DWORD)lpObj->ChaosMoney))
	{
		this->GCChaosMixSend(lpObj->Index, 2, 0);
		return;
	}

	lpObj->Money -= lpObj->ChaosMoney;

	GCMoneySend(lpObj->Index, lpObj->Money);

	gCastleSiegeSync.AddTributeMoney(TaxMoney);
	int ItemSlot = 0;
	CItem item = lpObj->ChaosBox[ItemSlot];
	if ((GetLargeRand() % 100) < lpObj->ChaosSuccessRate)
	{
		GDCreateItemSend(lpObj->Index, 0xFF, 0, 0, (GET_ITEM(12, 100) + (SphereType * 6) + SeedType), SeedLevel, 0, 0, 0, 0, -1, 0, 0, 0, 0, 0, 0xFF, 0);

		gLog.Output(LOG_CHAOS_MIX, "[Ép Khuôn][Thành Công][%s][%s] - Item: %s+%d (ChaosSuccessRate: %d, ChaosMoney: %d)", lpObj->Account, lpObj->Name, gItemManager.GetItemName(GET_ITEM(12, 100) + (SphereType * 6) + SeedType), SeedLevel,lpObj->ChaosSuccessRate, lpObj->ChaosMoney);
		
	}
	else
	{
		this->ChaosBoxInit(lpObj);

		this->GCChaosBoxSend(lpObj, 0);

		this->GCChaosMixSend(lpObj->Index, 0, 0);

		gLog.Output(LOG_CHAOS_MIX, "[Ép khuôn][Thất bại][%s/%s] Từ %s+%d,  %d %d %d %d %d, HC: %03d, 380: %03d, Socket: %03d, %03d, %03d, %03d, %03d, Rate: %d%%, Money: %d, Serial: %08X)"
				, lpObj->Account
				, lpObj->Name
				, gItemManager.GetItemName(item.m_Index)
				, item.m_Level
				, item.m_Option1
				, item.m_Option2
				, item.m_Option3 * 4
				, item.m_NewOption
				, item.m_SetOption
				, item.m_JewelOfHarmonyOption
				, item.m_ItemOptionEx
				, item.m_SocketOption[0]
				, item.m_SocketOption[1]
				, item.m_SocketOption[2]
				, item.m_SocketOption[3]
				, item.m_SocketOption[4]
				, lpObj->ChaosSuccessRate
				, lpObj->ChaosMoney				
				, item.m_Serial);		

	}
}

void CChaosBox::SocketItemMountSeedSphereMix(LPOBJ lpObj, BYTE info) // OK
{
	if (info >= MAX_SOCKET_ITEM_OPTION_TABLE)
	{
		this->GCChaosMixSend(lpObj->Index, 0, 0);
		return;
	}

	int ChaosCount = 0;
	int CreationCount = 0;
	int SeedSphereCount = 0;
	int SeedSphereType = 0;
	int SeedSphereLevel = 0;
	int SeedSphereOption = 0;
	int ItemCount = 0;
	int ItemSlot = 0;

	for (int n = 0; n < CHAOS_BOX_SIZE; n++)
	{
		if (lpObj->ChaosBox[n].IsItem() == 0)
		{
			continue;
		}

		#if(BUNDLE_ITEM)
		if (lpObj->ChaosBox[n].m_Index == GET_ITEM(12, 15) && lpObj->ChaosBox[n].m_Durability == 1)
		#else
		if (lpObj->ChaosBox[n].m_Index == GET_ITEM(12, 15))
		#endif
		{
			ChaosCount++;
		}
		else if (lpObj->ChaosBox[n].m_Index == GET_ITEM(14, 22))
		{
			CreationCount++;
		}
		else if (lpObj->ChaosBox[n].m_Index >= GET_ITEM(12, 100) && lpObj->ChaosBox[n].m_Index <= GET_ITEM(12, 129))
		{
			SeedSphereCount++;
			SeedSphereType = (lpObj->ChaosBox[n].m_Index - GET_ITEM(12, 100)) % MAX_SOCKET_ITEM_OPTION_TYPE;
			SeedSphereLevel = (lpObj->ChaosBox[n].m_Index - GET_ITEM(12, 100)) / MAX_SOCKET_ITEM_OPTION_TYPE;
			SeedSphereOption = lpObj->ChaosBox[n].m_Level;
		}
		else if (gSocketItemType.CheckSocketItemType(lpObj->ChaosBox[n].m_Index) != 0 && lpObj->ChaosBox[n].m_SocketOption[info] == SOCKET_ITEM_OPTION_EMPTY)
		{
			ItemCount++;
			ItemSlot = n;
		}
	}

	if (ChaosCount != 1 || CreationCount != 1 || SeedSphereCount != 1 || ItemCount != 1)
	{
		this->GCChaosMixSend(lpObj->Index, 7, 0);
		return;
	}
	if (info >= gSocketItemType.GetSocketItemMaxSocket(lpObj->ChaosBox[ItemSlot].m_Index))
	{
		this->GCChaosMixSend(lpObj->Index, 0, 0);
		return;
	}

	lpObj->ChaosMoney = 1000000;
	int TaxMoney = (lpObj->ChaosMoney*gCastleSiegeSync.GetTaxRateChaos(lpObj->Index)) / 100;
	lpObj->ChaosMoney += TaxMoney;
	if (lpObj->Money < ((DWORD)lpObj->ChaosMoney))
	{
		this->GCChaosMixSend(lpObj->Index, 2, 0);
		return;
	}
	lpObj->Money -= lpObj->ChaosMoney;
	GCMoneySend(lpObj->Index, lpObj->Money);
	gCastleSiegeSync.AddTributeMoney(TaxMoney);
	BYTE SocketOption = gSocketItemOption.GetSocketItemOption(&lpObj->ChaosBox[ItemSlot], (SeedSphereType + 1), SeedSphereOption, SeedSphereLevel);
	if (SocketOption == SOCKET_ITEM_OPTION_NONE)
	{
		this->GCChaosMixSend(lpObj->Index, 0, 0);
		return;
	}

	CItem item = lpObj->ChaosBox[ItemSlot];
	item.m_SocketOption[info] = SocketOption;
	item.m_SocketOptionBonus = gSocketItemOption.GetSocketItemBonusOption(&item);
	this->ChaosBoxInit(lpObj);
	gItemManager.ChaosBoxAddItem(lpObj->Index, item, 0);
	this->GCChaosMixSend(lpObj->Index, 1, &item);

	gLog.Output(LOG_CHAOS_MIX, "[Khảm Ngọc][Thành Công][%s/%s] Item: %s+%d,  %d %d %d %d %d, HC: %03d, 380: %03d, Socket: %03d, %03d, %03d, %03d, %03d, Rate: %d%%, Money: %d, Serial: %08X)"
				, lpObj->Account
				, lpObj->Name
				, gItemManager.GetItemName(item.m_Index)
				, item.m_Level
				, item.m_Option1
				, item.m_Option2
				, item.m_Option3 * 4
				, item.m_NewOption
				, item.m_SetOption
				, item.m_JewelOfHarmonyOption
				, item.m_ItemOptionEx
				, item.m_SocketOption[0]
				, item.m_SocketOption[1]
				, item.m_SocketOption[2]
				, item.m_SocketOption[3]
				, item.m_SocketOption[4]
				, lpObj->ChaosSuccessRate
				, lpObj->ChaosMoney				
				, item.m_Serial);
	
}

void CChaosBox::SocketItemUnMountSeedSphereMix(LPOBJ lpObj, BYTE info) // OK
{
	if (info >= MAX_SOCKET_ITEM_OPTION_TABLE)
	{
		this->GCChaosMixSend(lpObj->Index, 0, 0);
		return;
	}

	int ItemCount = 0;
	int ItemSlot = 0;

	for (int n = 0; n < CHAOS_BOX_SIZE; n++)
	{
		if (lpObj->ChaosBox[n].IsItem() == 0)
		{
			continue;
		}

		if (lpObj->ChaosBox[n].m_SocketOption[info] != SOCKET_ITEM_OPTION_NONE && lpObj->ChaosBox[n].m_SocketOption[info] != SOCKET_ITEM_OPTION_EMPTY)
		{
			ItemCount++;
			ItemSlot = n;
		}
	}

	if (ItemCount != 1)
	{
		this->GCChaosMixSend(lpObj->Index, 7, 0);
		return;
	}

	lpObj->ChaosMoney = 1000000;

	int TaxMoney = (lpObj->ChaosMoney*gCastleSiegeSync.GetTaxRateChaos(lpObj->Index)) / 100;

	lpObj->ChaosMoney += TaxMoney;

	if (lpObj->Money < ((DWORD)lpObj->ChaosMoney))
	{
		this->GCChaosMixSend(lpObj->Index, 2, 0);
		return;
	}

	lpObj->Money -= lpObj->ChaosMoney;
	GCMoneySend(lpObj->Index, lpObj->Money);
	gCastleSiegeSync.AddTributeMoney(TaxMoney);
	CItem item = lpObj->ChaosBox[ItemSlot];
	item.m_SocketOption[info] = SOCKET_ITEM_OPTION_EMPTY;
	item.m_SocketOptionBonus = gSocketItemOption.GetSocketItemBonusOption(&item);
	this->ChaosBoxInit(lpObj);
	gItemManager.ChaosBoxAddItem(lpObj->Index, item, 0);
	this->GCChaosMixSend(lpObj->Index, 1, &item);
			gLog.Output(LOG_CHAOS_MIX, "[Tháo Ngọc][Thành Công][%s/%s] Item: %s+%d,  %d %d %d %d %d, HC: %03d, 380: %03d, Socket: %03d, %03d, %03d, %03d, %03d, Rate: %d%%, Money: %d, Serial: %08X)"
				, lpObj->Account
				, lpObj->Name
				, gItemManager.GetItemName(item.m_Index)
				, item.m_Level
				, item.m_Option1
				, item.m_Option2
				, item.m_Option3 * 4
				, item.m_NewOption
				, item.m_SetOption
				, item.m_JewelOfHarmonyOption
				, item.m_ItemOptionEx
				, item.m_SocketOption[0]
				, item.m_SocketOption[1]
				, item.m_SocketOption[2]
				, item.m_SocketOption[3]
				, item.m_SocketOption[4]
				, lpObj->ChaosSuccessRate
				, lpObj->ChaosMoney				
				, item.m_Serial);
}

void CChaosBox::ImperialGuardianMix(LPOBJ lpObj) // OK
{
#if(GAMESERVER_UPDATE>=501)

	int DimensionalPart1Count = 0;
	int DimensionalPart2Count = 0;
	int DimensionalPart3Count = 0;
	int DimensionalPart4Count = 0;
	int DimensionalPart5Count = 0;
	int DimensionalPart6Count = 0;

	for (int n = 0; n < CHAOS_BOX_SIZE; n++)
	{
		if (lpObj->ChaosBox[n].IsItem() == 0)
		{
			continue;
		}

		if (lpObj->ChaosBox[n].m_Index == GET_ITEM(14, 103))
		{
			DimensionalPart1Count++;
		}
		else if (lpObj->ChaosBox[n].m_Index == GET_ITEM(14, 104))
		{
			DimensionalPart2Count++;
		}
		else if (lpObj->ChaosBox[n].m_Index == GET_ITEM(14, 105))
		{
			DimensionalPart3Count++;
		}
		else if (lpObj->ChaosBox[n].m_Index == GET_ITEM(14, 106))
		{
			DimensionalPart4Count++;
		}
		else if (lpObj->ChaosBox[n].m_Index == GET_ITEM(14, 107))
		{
			DimensionalPart5Count++;
		}
		else if (lpObj->ChaosBox[n].m_Index == GET_ITEM(14, 108))
		{
			DimensionalPart6Count++;
		}
	}

	if (DimensionalPart1Count != 1 && DimensionalPart2Count != 1 && DimensionalPart3Count != 1 && DimensionalPart4Count != 1 && DimensionalPart5Count != 1 && DimensionalPart6Count != 1)
	{
		this->GCChaosMixSend(lpObj->Index, 7, 0);
		return;
	}

	lpObj->ChaosMoney = 1000000;

	int TaxMoney = (lpObj->ChaosMoney*gCastleSiegeSync.GetTaxRateChaos(lpObj->Index)) / 100;

	lpObj->ChaosMoney += TaxMoney;

	if (lpObj->Money < ((DWORD)lpObj->ChaosMoney))
	{
		this->GCChaosMixSend(lpObj->Index, 2, 0);
		return;
	}

	lpObj->Money -= lpObj->ChaosMoney;
	GCMoneySend(lpObj->Index, lpObj->Money);
	gCastleSiegeSync.AddTributeMoney(TaxMoney);
	GDCreateItemSend(lpObj->Index, 0xFF, 0, 0, GET_ITEM(14, 109), 0, 1, 0, 0, 0, -1, 0, 0, 0, 0, 0, 0xFF, 0);
	int ItemSlot = 0;
	CItem item = lpObj->ChaosBox[ItemSlot];

	gLog.Output(LOG_CHAOS_MIX, "[Vé IG][Thành Công][%s][%s] - (ChaosSuccessRate: %d, ChaosMoney: %d)", lpObj->Account, lpObj->Name, lpObj->ChaosSuccessRate, lpObj->ChaosMoney);
	
#endif
}

void CChaosBox::ChestMix(LPOBJ lpObj) // OK
{
#if(GAMESERVER_UPDATE>=501)

	int GoldKeyCount = 0;
	int GoldChestCount = 0;
	int SilverKeyCount = 0;
	int SilverChestCount = 0;

	for (int n = 0; n < CHAOS_BOX_SIZE; n++)
	{
		if (lpObj->ChaosBox[n].IsItem() == 0)
		{
			continue;
		}

		if (lpObj->ChaosBox[n].m_Index == GET_ITEM(14, 112))
		{
			SilverKeyCount++;
		}
		else if (lpObj->ChaosBox[n].m_Index == GET_ITEM(14, 113))
		{
			GoldKeyCount++;
		}
		else if (lpObj->ChaosBox[n].m_Index == GET_ITEM(14, 121))
		{
			GoldChestCount++;
		}
		else if (lpObj->ChaosBox[n].m_Index == GET_ITEM(14, 122))
		{
			SilverChestCount++;
		}
	}

	if ((GoldKeyCount != 1 && SilverKeyCount != 1) || (GoldKeyCount == 1 && GoldChestCount != 1) || (SilverKeyCount == 1 && SilverChestCount != 1))
	{
		this->GCChaosMixSend(lpObj->Index, 7, 0);
		return;
	}

	GDCreateItemSend(lpObj->Index, 0xFF, 0, 0, (GET_ITEM(14, 123) + SilverKeyCount), 0, 0, 0, 0, 0, -1, 0, 0, 0, 0, 0, 0xFF, 0);

	int ItemSlot = 0;
	CItem item = lpObj->ChaosBox[ItemSlot];

	gLog.Output(LOG_CHAOS_MIX, "[ChestMix][Thành Công][%s][%s] - (ChaosSuccessRate: %d, ChaosMoney: %d)", lpObj->Account, lpObj->Name, lpObj->ChaosSuccessRate, lpObj->ChaosMoney);
	
#endif
}

void CChaosBox::LuckyItemCreateMix(LPOBJ lpObj) // OK
{
#if(GAMESERVER_UPDATE>=602)

	int LuckyTicketCount = 0;
	int LuckyTicketGroup = 0;
	int LuckyTicketSection = 0;

	for (int n = 0; n < CHAOS_BOX_SIZE; n++)
	{
		if (lpObj->ChaosBox[n].IsItem() == 0)
		{
			continue;
		}

		if (lpObj->ChaosBox[n].m_Index == GET_ITEM(13, 135))
		{
			LuckyTicketCount++;
			LuckyTicketGroup = 0;
			LuckyTicketSection = 8;
		}
		else if (lpObj->ChaosBox[n].m_Index == GET_ITEM(13, 136))
		{
			LuckyTicketCount++;
			LuckyTicketGroup = 0;
			LuckyTicketSection = 9;
		}
		else if (lpObj->ChaosBox[n].m_Index == GET_ITEM(13, 137))
		{
			LuckyTicketCount++;
			LuckyTicketGroup = 0;
			LuckyTicketSection = 7;
		}
		else if (lpObj->ChaosBox[n].m_Index == GET_ITEM(13, 138))
		{
			LuckyTicketCount++;
			LuckyTicketGroup = 0;
			LuckyTicketSection = 10;
		}
		else if (lpObj->ChaosBox[n].m_Index == GET_ITEM(13, 139))
		{
			LuckyTicketCount++;
			LuckyTicketGroup = 0;
			LuckyTicketSection = 11;
		}
		else if (lpObj->ChaosBox[n].m_Index == GET_ITEM(13, 140))
		{
			LuckyTicketCount++;
			LuckyTicketGroup = 1;
			LuckyTicketSection = 8;
		}
		else if (lpObj->ChaosBox[n].m_Index == GET_ITEM(13, 141))
		{
			LuckyTicketCount++;
			LuckyTicketGroup = 1;
			LuckyTicketSection = 9;
		}
		else if (lpObj->ChaosBox[n].m_Index == GET_ITEM(13, 142))
		{
			LuckyTicketCount++;
			LuckyTicketGroup = 1;
			LuckyTicketSection = 7;
		}
		else if (lpObj->ChaosBox[n].m_Index == GET_ITEM(13, 143))
		{
			LuckyTicketCount++;
			LuckyTicketGroup = 1;
			LuckyTicketSection = 10;
		}
		else if (lpObj->ChaosBox[n].m_Index == GET_ITEM(13, 144))
		{
			LuckyTicketCount++;
			LuckyTicketGroup = 1;
			LuckyTicketSection = 11;
		}
	}

	if (LuckyTicketCount != 1)
	{
		this->GCChaosMixSend(lpObj->Index, 7, 0);
		return;
	}

	WORD ItemIndex = 0;
	BYTE ItemLevel = 0;
	BYTE ItemOption1 = 0;
	BYTE ItemOption2 = 0;
	BYTE ItemOption3 = 0;
	BYTE ItemNewOption = 0;
	BYTE ItemSetOption = 0;
	BYTE ItemSocketOption[MAX_SOCKET_OPTION] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };

	if (gLuckyItem.GetLuckyItemIndex(lpObj, LuckyTicketSection, LuckyTicketGroup, &ItemIndex) == 0)
	{
		this->GCChaosMixSend(lpObj->Index, 0, 0);
		return;
	}

	gLuckyItem.GetLuckyItemOption0(ItemIndex, &ItemLevel);
	gLuckyItem.GetLuckyItemOption1(ItemIndex, &ItemOption1);
	gLuckyItem.GetLuckyItemOption2(ItemIndex, &ItemOption2);
	gLuckyItem.GetLuckyItemOption3(ItemIndex, &ItemOption3);
	gLuckyItem.GetLuckyItemOption4(ItemIndex, &ItemNewOption);
	gLuckyItem.GetLuckyItemOption5(ItemIndex, &ItemSetOption);
	gLuckyItem.GetLuckyItemOption6(ItemIndex, &ItemSocketOption[0]);
	gItemOptionRate.MakeNewOption(ItemIndex, ItemNewOption, &ItemNewOption);
	gItemOptionRate.MakeSetOption(ItemIndex, ItemSetOption, &ItemSetOption);
	gItemOptionRate.MakeSocketOption(ItemIndex, ItemSocketOption[0], &ItemSocketOption[0]);
	GDCreateItemSend(lpObj->Index, 0xFF, 0, 0, ItemIndex, ItemLevel, 255, ItemOption1, ItemOption2, ItemOption3, -1, ItemNewOption, ItemSetOption, 0, 0, ItemSocketOption, 0xFF, 0);
	int ItemSlot = 0;
	CItem item = lpObj->ChaosBox[ItemSlot];

	gLog.Output(LOG_CHAOS_MIX, "[LuckyItemCreateMix][Thành Công][%s][%s] - (ChaosSuccessRate: %d, ChaosMoney: %d)", lpObj->Account, lpObj->Name, lpObj->ChaosSuccessRate, lpObj->ChaosMoney);
	
#endif
}

void CChaosBox::LuckyItemRefineMix(LPOBJ lpObj) // OK
{
#if(GAMESERVER_UPDATE>=602)

	int LuckyItemCount = 0;
	int LuckyItemSlot = 0;

	for (int n = 0; n < CHAOS_BOX_SIZE; n++)
	{
		if (lpObj->ChaosBox[n].IsItem() == 0)
		{
			continue;
		}

		if (lpObj->ChaosBox[n].IsLuckyItem() != 0)
		{
			LuckyItemCount++;
			LuckyItemSlot = n;
		}
	}

	if (LuckyItemCount != 1)
	{
		this->GCChaosMixSend(lpObj->Index, 7, 0);
		return;
	}

	if (lpObj->ChaosBox[LuckyItemSlot].m_Durability == 255)
	{
		lpObj->ChaosSuccessRate = gServerInfo.m_LuckyItemRefineMixRate1[lpObj->AccountLevel];
	}
	else
	{
		lpObj->ChaosSuccessRate = gServerInfo.m_LuckyItemRefineMixRate2[lpObj->AccountLevel];
	}
	
	lpObj->ChaosSuccessRate = ((lpObj->ChaosSuccessRate>100) ? 100 : lpObj->ChaosSuccessRate);
	int ItemSlot = 0;
	CItem item = lpObj->ChaosBox[ItemSlot];
	if ((GetLargeRand() % 100) < lpObj->ChaosSuccessRate)
	{
		GDCreateItemSend(lpObj->Index, 0xFF, 0, 0, GET_ITEM(14, 160), 0, 1, 0, 0, 0, -1, 0, 0, 0, 0, 0, 0xFF, 0);


		gLog.Output(LOG_CHAOS_MIX, "[LuckyItemRefineMix][Thành Công][%s][%s] - (ChaosSuccessRate: %d, ChaosMoney: %d)", lpObj->Account, lpObj->Name, lpObj->ChaosSuccessRate, lpObj->ChaosMoney);
		
	}
	else
	{
		this->ChaosBoxInit(lpObj);
		this->GCChaosBoxSend(lpObj, 0);
		this->GCChaosMixSend(lpObj->Index, 0, 0);

		gLog.Output(LOG_CHAOS_MIX, "[LuckyItemRefineMix][Xịt][%s][%s] - (ChaosSuccessRate: %d, ChaosMoney: %d)", lpObj->Account, lpObj->Name, lpObj->ChaosSuccessRate, lpObj->ChaosMoney);
		
	}
#endif
}

void CChaosBox::CGChaosMixRecv(PMSG_CHAOS_MIX_RECV* lpMsg, int aIndex) // OK
{
	LPOBJ lpObj = &gObj[aIndex];

	if (gObjIsConnectedGP(aIndex) == 0)
	{
		return;
	}

	if (lpObj->ChaosLock != 0)
	{
		return;
	}

	if (lpObj->PShopOpen != 0)
	{
		return;
	}

	lpObj->ChaosLock = 1;
	lpObj->ChaosMoney = 0;
	lpObj->ChaosSuccessRate = 0;
	lpObj->IsChaosMixCompleted = 1;
	switch (lpMsg->type)
	{
	case CHAOS_MIX_CHAOS_ITEM:
		this->ChaosItemMix(lpObj);
		break;
	case CHAOS_MIX_DEVIL_SQUARE:
		this->DevilSquareMix(lpObj);
		break;
	case CHAOS_MIX_PLUS_ITEM_LEVEL1:
		this->PlusItemLevelMix(lpObj, 0);
		break;
	case CHAOS_MIX_PLUS_ITEM_LEVEL2:
		this->PlusItemLevelMix(lpObj, 1);
		break;
	case CHAOS_MIX_DINORANT:
		this->DinorantMix(lpObj);
		break;
	case CHAOS_MIX_FRUIT:
		this->FruitMix(lpObj);
		break;
	case CHAOS_MIX_WING1:
		this->Wing2Mix(lpObj, 0);
		break;
	case CHAOS_MIX_BLOOD_CASTLE:
		this->BloodCastleMix(lpObj);
		break;
	case CHAOS_MIX_WING2:
		this->Wing1Mix(lpObj);
		break;
	case CHAOS_MIX_PET1:
		this->PetMix(lpObj, 0);
		break;
	case CHAOS_MIX_PET2:
		this->PetMix(lpObj, 1);
		break;
	case CHAOS_MIX_SIEGE_POTION1:
		this->SiegePotionMix(lpObj, 0);
		break;
	case CHAOS_MIX_SIEGE_POTION2:
		this->SiegePotionMix(lpObj, 1);
		break;
	case CHAOS_MIX_LIFE_STONE:
		this->LifeStoneMix(lpObj);
		break;
	case CHAOS_MIX_SENIOR:
		this->SeniorMix(lpObj);
		break;
	case CHAOS_MIX_PLUS_ITEM_LEVEL3:
		this->PlusItemLevelMix(lpObj, 2);
		break;
	case CHAOS_MIX_PLUS_ITEM_LEVEL4:
		this->PlusItemLevelMix(lpObj, 3);
		break;
	case CHAOS_MIX_WING3:
		this->Wing2Mix(lpObj, 1);
		break;
	case CHAOS_MIX_PIECE_OF_HORN:
		this->PieceOfHornMix(lpObj);
		break;
	case CHAOS_MIX_BROKEN_HORN:
		this->BrokenHornMix(lpObj);
		break;
	case CHAOS_MIX_HORN_OF_FENRIR:
		this->HornOfFenrirMix(lpObj);
		break;
	case CHAOS_MIX_HORN_OF_FENRIR_UPGRADE:
		this->HornOfFenrirUpgradeMix(lpObj);
		break;
	case CHAOS_MIX_SHIELD_POTION1:
		this->ShieldPotionMix(lpObj, 0);
		break;
	case CHAOS_MIX_SHIELD_POTION2:
		this->ShieldPotionMix(lpObj, 1);
		break;
	case CHAOS_MIX_SHIELD_POTION3:
		this->ShieldPotionMix(lpObj, 2);
		break;
	case CHAOS_MIX_JEWEL_OF_HARMONY_ITEM_PURITY:
		this->JewelOfHarmonyItemPurityMix(lpObj);
		break;
	case CHAOS_MIX_JEWEL_OF_HARMONY_ITEM_SMELT:
		this->JewelOfHarmonyItemSmeltMix(lpObj);
		break;
	case CHAOS_MIX_JEWEL_OF_HARMONY_ITEM_RESTORE:
		this->JewelOfHarmonyItemRestoreMix(lpObj);
		break;
	case CHAOS_MIX_ITEM_380:
		this->Item380Mix(lpObj);
		break;
	case CHAOS_MIX_ILLUSION_TEMPLE:
		this->IllusionTempleMix(lpObj);
		break;
	case CHAOS_MIX_FEATHER_OF_CONDOR:
		this->FeatherOfCondorMix(lpObj);
		break;
	case CHAOS_MIX_WING4:
		this->Wing3Mix(lpObj);
		break;
	case CHAOS_MIX_CHAOS_CARD:
		this->ChaosCardMix(lpObj, 0);
		break;
	case CHAOS_MIX_CHERRY_BLOSSOM:
		this->CherryBlossomMix(lpObj, 0);
		break;
	case CHAOS_MIX_SOCKET_ITEM_CREATE_SEED:
		this->SocketItemCreateSeedMix(lpObj);
		break;
	case CHAOS_MIX_SOCKET_ITEM_CREATE_SEED_SPHERE:
		this->SocketItemCreateSeedSphereMix(lpObj);
		break;
	case CHAOS_MIX_SOCKET_ITEM_MOUNT_SEED_SPHERE:
		this->SocketItemMountSeedSphereMix(lpObj, lpMsg->info);
		break;
	case CHAOS_MIX_SOCKET_ITEM_UN_MOUNT_SEED_SPHERE:
		this->SocketItemUnMountSeedSphereMix(lpObj, lpMsg->info);
		break;
	case CHAOS_MIX_IMPERIAL_GUARDIAN:
		this->ImperialGuardianMix(lpObj);
		break;
	case CHAOS_MIX_CHEST:
		this->ChestMix(lpObj);
		break;
	case CHAOS_MIX_PLUS_ITEM_LEVEL5:
		this->PlusItemLevelMix(lpObj, 4);
		break;
	case CHAOS_MIX_PLUS_ITEM_LEVEL6:
		this->PlusItemLevelMix(lpObj, 5);
		break;
	case CHAOS_MIX_LUCKY_ITEM_CREATE:
		this->LuckyItemCreateMix(lpObj);
		break;
	case CHAOS_MIX_LUCKY_ITEM_REFINE:
		this->LuckyItemRefineMix(lpObj);
		break;
	case CHAOS_MIX_FENRIR_GOLD:
		this->HornOfFenrirGoldMix(lpObj);
		break;
	default:
		this->CustomItemMix(lpObj, lpMsg->type);
		break;
	}
}

void CChaosBox::CGChaosMixCloseRecv(int aIndex) // OK
{
	LPOBJ lpObj = &gObj[aIndex];

	if (gObjIsConnectedGP(aIndex) == 0)
	{
		return;
	}
	if (lpObj->Interface.use == 0 || lpObj->Interface.type != INTERFACE_CHAOS_BOX)
	{
		return;
	}
	lpObj->Interface.use = 0;
	lpObj->Interface.type = INTERFACE_NONE;
	lpObj->Interface.state = 0;
	lpObj->InterfaceTime = 0;
	lpObj->TargetNumber = -1;
	this->ChaosBoxInit(lpObj);
	gObjInventoryCommit(aIndex);
	lpObj->ChaosLock = 0;
	lpObj->IsChaosMixCompleted = 0;
	lpObj->IsCastleNPCUpgradeCompleted = 0;
	PBMSG_HEAD pMsg;
	pMsg.set(0x87, sizeof(pMsg));
	DataSend(aIndex, (BYTE*)&pMsg, pMsg.size);
}

void CChaosBox::GCChaosBoxSend(LPOBJ lpObj, BYTE type) // OK
{
	if ((type == 0 && (lpObj->Interface.type != INTERFACE_CHAOS_BOX || lpObj->Interface.state == 1)) || (type != 0 && (lpObj->Interface.type != INTERFACE_TRAINER || lpObj->Interface.state == 1)))
	{
		return;
	}
	BYTE send[2048];
	PMSG_SHOP_ITEM_LIST_SEND pMsg;
	pMsg.header.set(0x31, 0);
	int size = sizeof(pMsg);
	pMsg.type = ((type == 0) ? 3 : 5);
	pMsg.count = 0;
	PMSG_SHOP_ITEM_LIST info;
	for (int n = 0; n < CHAOS_BOX_SIZE; n++)
	{
		if (lpObj->ChaosBox[n].IsItem() == 0)
		{
			continue;
		}
		info.slot = n;
		gItemManager.ItemByteConvert(info.ItemInfo, lpObj->ChaosBox[n]);
		memcpy(&send[size], &info, sizeof(info));
		size += sizeof(info);
		pMsg.count++;
	}
	pMsg.header.size[0] = SET_NUMBERHB(size);
	pMsg.header.size[1] = SET_NUMBERLB(size);
	memcpy(send, &pMsg, sizeof(pMsg));
	DataSend(lpObj->Index, send, size);
}

void CChaosBox::GCChaosMixSend(int aIndex, BYTE result, CItem* lpItem) // OK
{
	PMSG_CHAOS_MIX_SEND pMsg;
	LPOBJ lpObj = &gObj[aIndex];
	pMsg.header.set(0x86, sizeof(pMsg));
	pMsg.result = result;
	if (lpItem == 0)
	{
		memset(pMsg.ItemInfo, 0xFF, sizeof(pMsg.ItemInfo));
	}
	else
	{
		gItemManager.ItemByteConvert(pMsg.ItemInfo, (*lpItem));
	}
	DataSend(aIndex, (BYTE*)&pMsg, pMsg.header.size);
	gObj[aIndex].ChaosLock = 0;
}

void CChaosBox::CGChaosMixRateRecv(PMSG_CHAOS_MIX_RATE_RECV* lpMsg, int aIndex) // OK
{
	LPOBJ lpObj = &gObj[aIndex];

	if (gObjIsConnectedGP(aIndex) == 0)
	{
		return;
	}
	if (lpObj->Interface.type == INTERFACE_TRAINER) 
	{
		goto Jump;
	}

	if (lpObj->Interface.use == 0 || lpObj->Interface.type != INTERFACE_CHAOS_BOX)
	{
		return;
	}

	Jump:
	
	PMSG_CHAOS_MIX_RATE_SEND pMsg;

	pMsg.header.set(0x8F, sizeof(pMsg));

	pMsg.rate = 0;

	pMsg.money = 0;

	pMsg.tax = 0;

	CUSTOM_MIX* CustomMix = gCustomMix.GetCustomMix(lpMsg->type);
	
	if (CustomMix != 0)
	{
		switch (lpObj->AccountLevel) {
		case 1:
			pMsg.rate = ((CustomMix->m_MixRate_AL1 < 0 || CustomMix->m_MixRate_AL1 > 100) ? 90 : CustomMix->m_MixRate_AL1);
			break;
		case 2:
			pMsg.rate = ((CustomMix->m_MixRate_AL2 < 0 || CustomMix->m_MixRate_AL2 > 100) ? 90 : CustomMix->m_MixRate_AL2);
			break;
		case 3:
			pMsg.rate = ((CustomMix->m_MixRate_AL3 < 0 || CustomMix->m_MixRate_AL3 > 100) ? 90 : CustomMix->m_MixRate_AL3);
			break;
		default:
			pMsg.rate = ((CustomMix->m_MixRate_AL0 < 0 || CustomMix->m_MixRate_AL0 > 100) ? 90 : CustomMix->m_MixRate_AL0);
			break;
		}

		pMsg.money = pMsg.rate * ((CustomMix->m_MixMoney < 0) ? 1000 : CustomMix->m_MixMoney);

		pMsg.tax = (pMsg.money * gCastleSiegeSync.GetTaxRateChaos(lpObj->Index)) / 100;
	}

	switch (lpMsg->type)
	{
	case CHAOS_MIX_CHAOS_ITEM:
	{
		int ChaosCount = 0;
		int ItemCount = 0;
		int ItemMoney = 0;

		for (int n = 0; n < CHAOS_BOX_SIZE; n++)
		{
			if (lpObj->ChaosBox[n].IsItem() == 0)
			{
				continue;
			}

			if (lpObj->ChaosBox[n].m_Index == GET_ITEM(12, 15))
			{
				ChaosCount++;
				lpObj->ChaosBox[n].OldValue();
				ItemMoney += lpObj->ChaosBox[n].m_OldBuyMoney;
			}
			else if (lpObj->ChaosBox[n].m_Index == GET_ITEM(14, 13) || lpObj->ChaosBox[n].m_Index == GET_ITEM(14, 14))
			{
				lpObj->ChaosBox[n].OldValue();
				ItemMoney += lpObj->ChaosBox[n].m_OldBuyMoney;
			}
			else if (lpObj->ChaosBox[n].m_Level >= 4 && lpObj->ChaosBox[n].m_Option3 >= 1)
			{
				ItemCount++;
				ItemMoney += lpObj->ChaosBox[n].m_BuyMoney;
			}
		}

		if (ChaosCount == 0 || ItemCount == 0)
		{
			return;
		}

		if (gServerInfo.m_ChaosItemMixRate[lpObj->AccountLevel] == -1)
		{
			pMsg.rate = (ItemMoney / 20000);
		}
		else
		{
			pMsg.rate = gServerInfo.m_ChaosItemMixRate[lpObj->AccountLevel];
		}

		if (this->GetTalismanOfLuckRate(lpObj, &pMsg.rate) == 0)
		{
			return;
		}

		pMsg.rate = ((pMsg.rate > 100) ? 100 : pMsg.rate);

		pMsg.money = pMsg.rate * 10000;

		pMsg.tax = (pMsg.money * gCastleSiegeSync.GetTaxRateChaos(lpObj->Index)) / 100;
	}
	break;
	case CHAOS_MIX_DEVIL_SQUARE:
	{
		int ChaosCount = 0;
		int EyeCount = 0;
		int EyeLevel = 0;
		int KeyCount = 0;
		int KeyLevel = 0;

		for (int n = 0; n < CHAOS_BOX_SIZE; n++)
		{
			if (lpObj->ChaosBox[n].IsItem() == 0)
			{
				continue;
			}

			if (lpObj->ChaosBox[n].m_Index == GET_ITEM(12, 15))
			{
				ChaosCount++;
			}
			else if (lpObj->ChaosBox[n].m_Index == GET_ITEM(14, 17))
			{
				EyeCount++;
				EyeLevel = lpObj->ChaosBox[n].m_Level;
			}
			else if (lpObj->ChaosBox[n].m_Index == GET_ITEM(14, 18))
			{
				KeyCount++;
				KeyLevel = lpObj->ChaosBox[n].m_Level;
			}
		}

		if (ChaosCount != 1 || EyeCount != 1 || KeyCount != 1 || EyeLevel != KeyLevel)
		{
			return;
		}

		if (EyeLevel > 7 || KeyLevel > 7)
		{
			return;
		}

		switch (EyeLevel)
		{
		case 1:
			pMsg.rate = gServerInfo.m_DevilSquareMixRate1[lpObj->AccountLevel];
			pMsg.money = 100000;
			break;
		case 2:
			pMsg.rate = gServerInfo.m_DevilSquareMixRate2[lpObj->AccountLevel];
			pMsg.money = 200000;
			break;
		case 3:
			pMsg.rate = gServerInfo.m_DevilSquareMixRate3[lpObj->AccountLevel];
			pMsg.money = 400000;
			break;
		case 4:
			pMsg.rate = gServerInfo.m_DevilSquareMixRate4[lpObj->AccountLevel];
			pMsg.money = 700000;
			break;
		case 5:
			pMsg.rate = gServerInfo.m_DevilSquareMixRate5[lpObj->AccountLevel];
			pMsg.money = 1100000;
			break;
		case 6:
			pMsg.rate = gServerInfo.m_DevilSquareMixRate6[lpObj->AccountLevel];
			pMsg.money = 1600000;
			break;
		case 7:
			pMsg.rate = gServerInfo.m_DevilSquareMixRate7[lpObj->AccountLevel];
			pMsg.money = 2000000;
			break;
		}

		if (this->GetTalismanOfLuckRate(lpObj, &pMsg.rate) == 0)
		{
			return;
		}

		pMsg.tax = (pMsg.money * gCastleSiegeSync.GetTaxRateChaos(lpObj->Index)) / 100;
	}
	break;
	case CHAOS_MIX_PLUS_ITEM_LEVEL1:
	case CHAOS_MIX_PLUS_ITEM_LEVEL2:
	case CHAOS_MIX_PLUS_ITEM_LEVEL3:
	case CHAOS_MIX_PLUS_ITEM_LEVEL4:
	case CHAOS_MIX_PLUS_ITEM_LEVEL5:
	case CHAOS_MIX_PLUS_ITEM_LEVEL6:
	{
		int ItemCount = 0;
		int ItemSlot = 0;
		int type = 0;

		switch (lpMsg->type)
		{
		case CHAOS_MIX_PLUS_ITEM_LEVEL1:
			type = 0;
			break;
		case CHAOS_MIX_PLUS_ITEM_LEVEL2:
			type = 1;
			break;
		case CHAOS_MIX_PLUS_ITEM_LEVEL3:
			type = 2;
			break;
		case CHAOS_MIX_PLUS_ITEM_LEVEL4:
			type = 3;
			break;
		case CHAOS_MIX_PLUS_ITEM_LEVEL5:
			type = 4;
			break;
		case CHAOS_MIX_PLUS_ITEM_LEVEL6:
			type = 5;
			break;
		}

		for (int n = 0; n < CHAOS_BOX_SIZE; n++)
		{
			if (lpObj->ChaosBox[n].IsItem() == 0)
			{
				continue;
			}

			if (lpObj->ChaosBox[n].m_Level == (9 + type))
			{
				ItemCount++;
				ItemSlot = n;
			}
		}

		if (ItemCount != 1)
		{
			return;
		}

		if (lpObj->ChaosBox[ItemSlot].m_Option2 != 0 && lpObj->ChaosBox[ItemSlot].m_Level == 9)
		{
			pMsg.rate += gServerInfo.m_AddLuckSuccessRate2[lpObj->AccountLevel] + gServerInfo.m_PlusItemLevelMixRate1[lpObj->AccountLevel];
		}
		else if (lpObj->ChaosBox[ItemSlot].m_Level == 9)
		{
			pMsg.rate += gServerInfo.m_PlusItemLevelMixRate1[lpObj->AccountLevel];
		}

		if (lpObj->ChaosBox[ItemSlot].m_Option2 != 0 && lpObj->ChaosBox[ItemSlot].m_Level == 10)
		{
			pMsg.rate += gServerInfo.m_AddLuckSuccessRate2[lpObj->AccountLevel] + gServerInfo.m_PlusItemLevelMixRate2[lpObj->AccountLevel];
		}
		else if (lpObj->ChaosBox[ItemSlot].m_Level == 10)
		{
			pMsg.rate += gServerInfo.m_PlusItemLevelMixRate2[lpObj->AccountLevel];
		}

		if (lpObj->ChaosBox[ItemSlot].m_Option2 != 0 && lpObj->ChaosBox[ItemSlot].m_Level == 11)
		{
			pMsg.rate += gServerInfo.m_AddLuckSuccessRate2[lpObj->AccountLevel] + gServerInfo.m_PlusItemLevelMixRate3[lpObj->AccountLevel];
		}
		else if (lpObj->ChaosBox[ItemSlot].m_Level == 11)
		{
			pMsg.rate += gServerInfo.m_PlusItemLevelMixRate3[lpObj->AccountLevel];
		}

		if (lpObj->ChaosBox[ItemSlot].m_Option2 != 0 && lpObj->ChaosBox[ItemSlot].m_Level == 12)
		{
			pMsg.rate += gServerInfo.m_AddLuckSuccessRate2[lpObj->AccountLevel] + gServerInfo.m_PlusItemLevelMixRate4[lpObj->AccountLevel];
		}
		else if (lpObj->ChaosBox[ItemSlot].m_Level == 12)
		{
			pMsg.rate += gServerInfo.m_PlusItemLevelMixRate4[lpObj->AccountLevel];
		}

		if (lpObj->ChaosBox[ItemSlot].m_Option2 != 0 && lpObj->ChaosBox[ItemSlot].m_Level == 13)
		{
			pMsg.rate += gServerInfo.m_AddLuckSuccessRate2[lpObj->AccountLevel] + gServerInfo.m_PlusItemLevelMixRate5[lpObj->AccountLevel];
		}
		else if (lpObj->ChaosBox[ItemSlot].m_Level == 13)
		{
			pMsg.rate += gServerInfo.m_PlusItemLevelMixRate5[lpObj->AccountLevel];
		}

		if (lpObj->ChaosBox[ItemSlot].m_Option2 != 0 && lpObj->ChaosBox[ItemSlot].m_Level == 14)
		{
			pMsg.rate += gServerInfo.m_AddLuckSuccessRate2[lpObj->AccountLevel] + gServerInfo.m_PlusItemLevelMixRate6[lpObj->AccountLevel];
		}
		else if (lpObj->ChaosBox[ItemSlot].m_Level == 14)
		{
			pMsg.rate += gServerInfo.m_PlusItemLevelMixRate6[lpObj->AccountLevel];
		}
		//Bua Hau Sida
		if (this->GetTalismanOfLuckRate(lpObj, &pMsg.rate) == 0)
		{
			return;
		}

		pMsg.money = 2000000 * (type + 1);

		pMsg.tax = (pMsg.money * gCastleSiegeSync.GetTaxRateChaos(lpObj->Index)) / 100;
	}
	break;
	case CHAOS_MIX_DINORANT:
	{
		int ChaosCount = 0;
		int UniriaCount = 0;

		for (int n = 0; n < CHAOS_BOX_SIZE; n++)
		{
			if (lpObj->ChaosBox[n].IsItem() == 0)
			{
				continue;
			}

			if (lpObj->ChaosBox[n].m_Index == GET_ITEM(12, 15))
			{
				ChaosCount++;
			}
			else if (lpObj->ChaosBox[n].m_Index == GET_ITEM(13, 2) && lpObj->ChaosBox[n].m_Durability == 255)
			{
				UniriaCount++;
			}
		}

		if (ChaosCount != 1 || UniriaCount != 10)
		{
			return;
		}

		pMsg.rate = gServerInfo.m_DinorantMixRate[lpObj->AccountLevel];

		if (this->GetTalismanOfLuckRate(lpObj, &pMsg.rate) == 0)
		{
			return;
		}

		pMsg.money = 500000;

		pMsg.tax = (pMsg.money * gCastleSiegeSync.GetTaxRateChaos(lpObj->Index)) / 100;
	}
	break;
	case CHAOS_MIX_FRUIT:
	{
		int ChaosCount = 0;
		int CreationCount = 0;

		for (int n = 0; n < CHAOS_BOX_SIZE; n++)
		{
			if (lpObj->ChaosBox[n].IsItem() == 0)
			{
				continue;
			}

			if (lpObj->ChaosBox[n].m_Index == GET_ITEM(12, 15))
			{
				ChaosCount++;
			}
			else if (lpObj->ChaosBox[n].m_Index == GET_ITEM(14, 22))
			{
				CreationCount++;
			}
		}

		if (ChaosCount != 1 || CreationCount != 1)
		{
			return;
		}

		pMsg.rate = gServerInfo.m_FruitMixRate[lpObj->AccountLevel];

		if (this->GetTalismanOfLuckRate(lpObj, &pMsg.rate) == 0)
		{
			return;
		}

		pMsg.money = 3000000;

		pMsg.tax = (pMsg.money * gCastleSiegeSync.GetTaxRateChaos(lpObj->Index)) / 100;
	}
	break;
	case CHAOS_MIX_WING1:
	case CHAOS_MIX_WING3:
	{
		int WingItemCount = 0;
		int type = 0;

		switch (lpMsg->type)
		{
		case CHAOS_MIX_WING1:
			type = 0;
			break;
		case CHAOS_MIX_WING3:
			type = 1;
			break;
		}

		for (int n = 0; n < CHAOS_BOX_SIZE; n++)
		{
			if (lpObj->ChaosBox[n].IsItem() == 0)
			{
				continue;
			}

			if ((lpObj->ChaosBox[n].m_Index >= GET_ITEM(12, 0) && lpObj->ChaosBox[n].m_Index <= GET_ITEM(12, 2)) || lpObj->ChaosBox[n].m_Index == GET_ITEM(12, 41))
			{
				WingItemCount++;
			}

			if (lpObj->ChaosBox[n].m_Level >= 4 && lpObj->ChaosBox[n].m_NewOption >= 1)
			{
				pMsg.rate = gServerInfo.m_Wing2MixRate[lpObj->AccountLevel] + 10;
			}
			else
			{
				pMsg.rate = gServerInfo.m_Wing2MixRate[lpObj->AccountLevel];
			}
		}
		//xxx

		if (this->GetTalismanOfLuckRate(lpObj, &pMsg.rate) == 0)
		{
			//this->GCChaosMixSend(lpObj->Index, 240, 0);
			return;
		}

		pMsg.money = 5000000;

		pMsg.tax = (pMsg.money * gCastleSiegeSync.GetTaxRateChaos(lpObj->Index)) / 100;
	}
	break;
	case CHAOS_MIX_BLOOD_CASTLE:
	{
		int ChaosCount = 0;
		int ScrollCount = 0;
		int ScrollLevel = 0;
		int BoneCount = 0;
		int BoneLevel = 0;

		for (int n = 0; n < CHAOS_BOX_SIZE; n++)
		{
			if (lpObj->ChaosBox[n].IsItem() == 0)
			{
				continue;
			}

			if (lpObj->ChaosBox[n].m_Index == GET_ITEM(12, 15))
			{
				ChaosCount++;
			}
			else if (lpObj->ChaosBox[n].m_Index == GET_ITEM(13, 16))
			{
				ScrollCount++;
				ScrollLevel = lpObj->ChaosBox[n].m_Level;
			}
			else if (lpObj->ChaosBox[n].m_Index == GET_ITEM(13, 17))
			{
				BoneCount++;
				BoneLevel = lpObj->ChaosBox[n].m_Level;
			}
		}

		if (ChaosCount != 1 || ScrollCount != 1 || BoneCount != 1 || ScrollLevel != BoneLevel)
		{
			return;
		}

		if (ScrollLevel > 8 || BoneLevel > 8)
		{
			return;
		}

		switch (ScrollLevel)
		{
		case 1:
			pMsg.rate = gServerInfo.m_BloodCastleMixRate1[lpObj->AccountLevel];
			pMsg.money = 50000;
			break;
		case 2:
			pMsg.rate = gServerInfo.m_BloodCastleMixRate2[lpObj->AccountLevel];
			pMsg.money = 80000;
			break;
		case 3:
			pMsg.rate = gServerInfo.m_BloodCastleMixRate3[lpObj->AccountLevel];
			pMsg.money = 150000;
			break;
		case 4:
			pMsg.rate = gServerInfo.m_BloodCastleMixRate4[lpObj->AccountLevel];
			pMsg.money = 250000;
			break;
		case 5:
			pMsg.rate = gServerInfo.m_BloodCastleMixRate5[lpObj->AccountLevel];
			pMsg.money = 400000;
			break;
		case 6:
			pMsg.rate = gServerInfo.m_BloodCastleMixRate6[lpObj->AccountLevel];
			pMsg.money = 600000;
			break;
		case 7:
			pMsg.rate = gServerInfo.m_BloodCastleMixRate7[lpObj->AccountLevel];
			pMsg.money = 850000;
			break;
		case 8:
			pMsg.rate = gServerInfo.m_BloodCastleMixRate8[lpObj->AccountLevel];
			pMsg.money = 1050000;
			break;
		}

		if (this->GetTalismanOfLuckRate(lpObj, &pMsg.rate) == 0)
		{
			return;
		}

		pMsg.tax = (pMsg.money * gCastleSiegeSync.GetTaxRateChaos(lpObj->Index)) / 100;
	}
	break;
	case CHAOS_MIX_WING2:
	{
		int ChaosCount = 0;
		int ChaosItem = 0;
		int ItemCount = 0;
		int ItemMoney = 0;
		int TalismanOfWingType = 0;

		for (int n = 0; n < CHAOS_BOX_SIZE; n++)
		{
			if (lpObj->ChaosBox[n].IsItem() == 0)
			{
				continue;
			}

			if (lpObj->ChaosBox[n].m_Index == GET_ITEM(12, 15))
			{
				ChaosCount++;
				lpObj->ChaosBox[n].OldValue();
				ItemMoney += lpObj->ChaosBox[n].m_OldBuyMoney;
			}
			else if (lpObj->ChaosBox[n].m_Index == GET_ITEM(14, 13) || lpObj->ChaosBox[n].m_Index == GET_ITEM(14, 14))
			{
				ItemCount++;
				lpObj->ChaosBox[n].OldValue();
				ItemMoney += lpObj->ChaosBox[n].m_OldBuyMoney;
			}
			else if ((lpObj->ChaosBox[n].m_Index == GET_ITEM(2, 6) || lpObj->ChaosBox[n].m_Index == GET_ITEM(4, 6) || lpObj->ChaosBox[n].m_Index == GET_ITEM(5, 7)) && lpObj->ChaosBox[n].m_Level >= 4 && lpObj->ChaosBox[n].m_Option3 >= 1)
			{
				ChaosItem++;
				ItemCount++;
				ItemMoney += lpObj->ChaosBox[n].m_BuyMoney;
			}
			else if (lpObj->ChaosBox[n].m_Level >= 4 && lpObj->ChaosBox[n].m_Option3 >= 1)
			{
				ItemCount++;
				ItemMoney += lpObj->ChaosBox[n].m_BuyMoney;
			}
			else if (lpObj->ChaosBox[n].m_Index >= GET_ITEM(13, 83) && lpObj->ChaosBox[n].m_Index >= GET_ITEM(13, 86))
			{
				TalismanOfWingType = lpObj->ChaosBox[n].m_Index - GET_ITEM(13, 82);
			}
		}

		if (ChaosCount == 0 || ChaosItem == 0)
		{
			return;
		}

		if (gServerInfo.m_Wing1MixRate[lpObj->AccountLevel] == -1)
		{
			pMsg.rate = (ItemMoney / 20000);
		}
		else
		{
			pMsg.rate = gServerInfo.m_Wing1MixRate[lpObj->AccountLevel];
		}

		if (this->GetTalismanOfLuckRate(lpObj, &pMsg.rate) == 0)
		{
			return;
		}

		pMsg.rate = ((pMsg.rate > 100) ? 100 : pMsg.rate);

		pMsg.money = pMsg.rate * 10000;

		pMsg.tax = (pMsg.money * gCastleSiegeSync.GetTaxRateChaos(lpObj->Index)) / 100;
	}
	break;
	case CHAOS_MIX_PET1:
	case CHAOS_MIX_PET2:
	{
		int ChaosCount = 0;
		int SoulOfDarkHorseCount = 0;
		int SoulOfDarkSpiritCount = 0;
		int BlessCount = 0;
		int SoulCount = 0;
		int CreationCount = 0;
		int type = 0;

		switch (lpMsg->type)
		{
		case CHAOS_MIX_PET1:
			type = 0;
			break;
		case CHAOS_MIX_PET2:
			type = 1;
			break;
		}

		for (int n = 0; n < CHAOS_BOX_SIZE; n++)
		{
			if (lpObj->ChaosBox[n].IsItem() == 0)
			{
				continue;
			}

			if (lpObj->ChaosBox[n].m_Index == GET_ITEM(12, 15))
			{
				ChaosCount++;
			}
			else if (lpObj->ChaosBox[n].m_Index == GET_ITEM(13, 31) && lpObj->ChaosBox[n].m_Level == 0)
			{
				SoulOfDarkHorseCount++;
			}
			else if (lpObj->ChaosBox[n].m_Index == GET_ITEM(13, 31) && lpObj->ChaosBox[n].m_Level == 1)
			{
				SoulOfDarkSpiritCount++;
			}
			else if (lpObj->ChaosBox[n].m_Index == GET_ITEM(14, 13))
			{
				BlessCount++;
			}
			else if (lpObj->ChaosBox[n].m_Index == GET_ITEM(14, 14))
			{
				SoulCount++;
			}
			else if (lpObj->ChaosBox[n].m_Index == GET_ITEM(14, 22))
			{
				CreationCount++;
			}
		}

		if (ChaosCount != 1 || (type == 0 && SoulOfDarkHorseCount != 1) || (type == 1 && SoulOfDarkSpiritCount != 1) || BlessCount != (5 - (type * 3)) || SoulCount != (5 - (type * 3)) || CreationCount != 1)
		{
			return;
		}

		pMsg.rate = gServerInfo.m_PetMixRate[lpObj->AccountLevel];

		if (this->GetTalismanOfLuckRate(lpObj, &pMsg.rate) == 0)
		{
			return;
		}

		pMsg.money = 5000000 - (4000000 * type);

		pMsg.tax = (pMsg.money * gCastleSiegeSync.GetTaxRateChaos(lpObj->Index)) / 100;
	}
	break;
#if(GAMESERVER_UPDATE>=201)
	case CHAOS_MIX_PIECE_OF_HORN:
	{
		int ChaosCount = 0;
		int SplinterOfArmorCount = 0;
		int BlessOfGuardianCount = 0;

		for (int n = 0; n < CHAOS_BOX_SIZE; n++)
		{
			if (lpObj->ChaosBox[n].IsItem() == 0)
			{
				continue;
			}

			if (lpObj->ChaosBox[n].m_Index == GET_ITEM(12, 15))
			{
				ChaosCount++;
			}
			else if (lpObj->ChaosBox[n].m_Index == GET_ITEM(13, 32))
			{
				SplinterOfArmorCount += (int)lpObj->ChaosBox[n].m_Durability;
			}
			else if (lpObj->ChaosBox[n].m_Index == GET_ITEM(13, 33))
			{
				BlessOfGuardianCount += (int)lpObj->ChaosBox[n].m_Durability;
			}
		}

		if (ChaosCount != 1 || SplinterOfArmorCount != 20 || BlessOfGuardianCount != 20)
		{
			return;
		}

		pMsg.rate = gServerInfo.m_PieceOfHornMixRate[lpObj->AccountLevel];

		if (this->GetTalismanOfLuckRate(lpObj, &pMsg.rate) == 0)
		{
			return;
		}
	} 
	break;
	case CHAOS_MIX_BROKEN_HORN:
	{
		int ChaosCount = 0;
		int ClawOfBeastCount = 0;
		int PieceOfHornCount = 0;

		for (int n = 0; n < CHAOS_BOX_SIZE; n++)
		{
			if (lpObj->ChaosBox[n].IsItem() == 0)
			{
				continue;
			}

			if (lpObj->ChaosBox[n].m_Index == GET_ITEM(12, 15))
			{
				ChaosCount++;
			}
			else if (lpObj->ChaosBox[n].m_Index == GET_ITEM(13, 34))
			{
				ClawOfBeastCount += (int)lpObj->ChaosBox[n].m_Durability;
			}
			else if (lpObj->ChaosBox[n].m_Index == GET_ITEM(13, 35))
			{
				PieceOfHornCount++;
			}
		}

		if (ChaosCount != 1 || ClawOfBeastCount != 10 || PieceOfHornCount != 5)
		{
			return;
		}

		pMsg.rate = gServerInfo.m_BrokenHornMixRate[lpObj->AccountLevel];

		if (this->GetTalismanOfLuckRate(lpObj, &pMsg.rate) == 0)
		{
			return;
		}

		//pMsg.rate = gBonusManager.GetBonusValue(lpObj, BONUS_INDEX_CHAOSMIX_RATE, pMsg.rate);
	} break;
	case CHAOS_MIX_HORN_OF_FENRIR:
	{
		int ChaosCount = 0;
		int BrokenHornCount = 0;
		int LifeCount = 0;

		for (int n = 0; n < CHAOS_BOX_SIZE; n++)
		{
			if (lpObj->ChaosBox[n].IsItem() == 0)
			{
				continue;
			}

			if (lpObj->ChaosBox[n].m_Index == GET_ITEM(12, 15))
			{
				ChaosCount++;
			}
			else if (lpObj->ChaosBox[n].m_Index == GET_ITEM(13, 36))
			{
				BrokenHornCount++;
			}
			else if (lpObj->ChaosBox[n].m_Index == GET_ITEM(14, 16))
			{
				LifeCount++;
			}
		}

		if (ChaosCount != 1 || BrokenHornCount != 1 || LifeCount != 3)
		{
			return;
		}

		pMsg.rate = gServerInfo.m_HornOfFenrirMixRate[lpObj->AccountLevel];

		if (this->GetTalismanOfLuckRate(lpObj, &pMsg.rate) == 0)
		{
			return;
		}

		pMsg.money = 10000000;

		pMsg.tax = (pMsg.money * gCastleSiegeSync.GetTaxRateChaos(lpObj->Index)) / 100;
	} break;
	case CHAOS_MIX_HORN_OF_FENRIR_UPGRADE:
	{
		int ChaosCount = 0;
		int HornOfFenrirCount = 0;
		int LifeCount = 0;
		int WeaponCount = 0;
		int WeaponMoney = 0;
		int ArmorCount = 0;
		int ArmorMoney = 0;

		for (int n = 0; n < CHAOS_BOX_SIZE; n++)
		{
			if (lpObj->ChaosBox[n].IsItem() == 0)
			{
				continue;
			}

			if (lpObj->ChaosBox[n].m_Index == GET_ITEM(12, 15))
			{
				ChaosCount++;
			}
			else if (lpObj->ChaosBox[n].m_Index == GET_ITEM(13, 37))
			{
				HornOfFenrirCount++;
			}
			else if (lpObj->ChaosBox[n].m_Index == GET_ITEM(14, 16))
			{
				LifeCount++;
			}
			else if (lpObj->ChaosBox[n].m_Index >= GET_ITEM(0, 0) && lpObj->ChaosBox[n].m_Index < GET_ITEM(6, 0) && lpObj->ChaosBox[n].m_Level >= 4 && lpObj->ChaosBox[n].m_Option3 >= 1)
			{
				WeaponCount++;
				WeaponMoney = lpObj->ChaosBox[n].m_BuyMoney;
			}
			else if (lpObj->ChaosBox[n].m_Index >= GET_ITEM(6, 0) && lpObj->ChaosBox[n].m_Index < GET_ITEM(12, 0) && lpObj->ChaosBox[n].m_Level >= 4 && lpObj->ChaosBox[n].m_Option3 >= 1)
			{
				ArmorCount++;
				ArmorMoney = lpObj->ChaosBox[n].m_BuyMoney;
			}
		}

		if (ChaosCount != 1 || HornOfFenrirCount != 1 || LifeCount != 5 || (WeaponCount == 0 && ArmorCount == 0) || (WeaponCount != 0 && ArmorCount != 0))
		{
			return;
		}

		if (gServerInfo.m_HornOfFenrirUpgradeMixRate[lpObj->AccountLevel] == -1)
		{
			pMsg.rate = (WeaponMoney + ArmorMoney) / 10000;
		}
		else
		{
			pMsg.rate = gServerInfo.m_HornOfFenrirUpgradeMixRate[lpObj->AccountLevel];
		}

		if (this->GetTalismanOfLuckRate(lpObj, &pMsg.rate) == 0)
		{
			return;
		}

		if (gServerInfo.m_HornOfFenrirUpgradeMixRate[lpObj->AccountLevel] == -1)
		{
			pMsg.rate = ((pMsg.rate > 80) ? 80 : pMsg.rate);
		}

		pMsg.money = 10000000;

		pMsg.tax = (pMsg.money * gCastleSiegeSync.GetTaxRateChaos(lpObj->Index)) / 100;
	} break;
	case CHAOS_MIX_FENRIR_GOLD:
	{
		int FenrirThuong = 0;
		int FenrirTC = 0;
		int FenrirPT = 0;
		int DaHoMenh = 0;
		int Chaos = 0;

		for (int n = 0; n < CHAOS_BOX_SIZE; n++)
		{
			if (lpObj->ChaosBox[n].IsItem() == 0)
			{
				continue;
			}

			if (lpObj->ChaosBox[n].m_Index == GET_ITEM(13, 37) && lpObj->ChaosBox[n].m_NewOption == 0)
			{
				FenrirThuong++;
			}
			else if (lpObj->ChaosBox[n].m_Index == GET_ITEM(13, 37) && lpObj->ChaosBox[n].m_NewOption == 1)
			{
				FenrirTC++;
			}
			else if (lpObj->ChaosBox[n].m_Index == GET_ITEM(13, 37) && lpObj->ChaosBox[n].m_NewOption == 2)
			{
				FenrirPT++;
			}
			else if (lpObj->ChaosBox[n].m_Index == GET_ITEM(12, 138) && lpObj->ChaosBox[n].m_Level == 2)
			{
				DaHoMenh++;
			}
			else if (lpObj->ChaosBox[n].m_Index == GET_ITEM(12, 15))
			{
				Chaos++;
			}
		}

		if (gServerInfo.m_HornOfFenrirGoldMixRate[lpObj->AccountLevel] == -1)
		{
		}
		else
		{
			pMsg.rate = gServerInfo.m_HornOfFenrirGoldMixRate[lpObj->AccountLevel];
		}

		if (this->GetTalismanOfLuckRate(lpObj, &pMsg.rate) == 0)
		{
			return;
		}

		pMsg.money = 10000000;

		pMsg.tax = (pMsg.money * gCastleSiegeSync.GetTaxRateChaos(lpObj->Index)) / 100;
	}
	break;
	case CHAOS_MIX_SHIELD_POTION1:
	case CHAOS_MIX_SHIELD_POTION2:
	case CHAOS_MIX_SHIELD_POTION3:
	{
		int LargeHealingPotionCount = 0;
		int SmallCompoundPotionCount = 0;
		int MediumCompoundPotionCount = 0;
		int type = 0;

		switch (lpMsg->type)
		{
		case CHAOS_MIX_SHIELD_POTION1:
			type = 0;
			break;
		case CHAOS_MIX_SHIELD_POTION2:
			type = 1;
			break;
		case CHAOS_MIX_SHIELD_POTION3:
			type = 2;
			break;
		}

		for (int n = 0; n < CHAOS_BOX_SIZE; n++)
		{
			if (lpObj->ChaosBox[n].IsItem() == 0)
			{
				continue;
			}

			if (lpObj->ChaosBox[n].m_Index == GET_ITEM(14, 3))
			{
				LargeHealingPotionCount += (int)lpObj->ChaosBox[n].m_Durability;
			}
			else if (lpObj->ChaosBox[n].m_Index == GET_ITEM(14, 38))
			{
				SmallCompoundPotionCount += (int)lpObj->ChaosBox[n].m_Durability;
			}
			else if (lpObj->ChaosBox[n].m_Index == GET_ITEM(14, 39))
			{
				MediumCompoundPotionCount += (int)lpObj->ChaosBox[n].m_Durability;
			}
		}

		if ((type == 0 && LargeHealingPotionCount != 3) || (type == 1 && SmallCompoundPotionCount != 3) || (type == 2 && MediumCompoundPotionCount != 3))
		{
			return;
		}
		if (this->GetTalismanOfLuckRate(lpObj, &pMsg.rate) == 0)
		{
			return;
		}
		pMsg.money = ((type == 0) ? 100000 : (500000 * type));
		pMsg.tax = (pMsg.money * gCastleSiegeSync.GetTaxRateChaos(lpObj->Index)) / 100;
	} 
	break;
	case CHAOS_MIX_JEWEL_OF_HARMONY_ITEM_PURITY:
	case CHAOS_MIX_JEWEL_OF_HARMONY_ITEM_SMELT:
	{
		int GemStoneCount = 0;
		int ItemCount = 0;
		int ItemSlot = 0;

		for (int n = 0; n < CHAOS_BOX_SIZE; n++)
		{
			if (lpObj->ChaosBox[n].IsItem() == 0)
			{
				continue;
			}

			if (lpObj->ChaosBox[n].m_Index == GET_ITEM(14, 41))
			{
				GemStoneCount++;
			}

			if (gJewelOfHarmonyType.CheckJewelOfHarmonyItemType(&lpObj->ChaosBox[n]) != 0 && lpObj->ChaosBox[n].IsSetItem() == 0 && lpObj->ChaosBox[n].IsJewelOfHarmonyItem() == 0)
			{
				ItemCount++;
				ItemSlot = n;
			}
		}

		if (GemStoneCount == 1)
		{
			pMsg.rate = gServerInfo.m_JewelOfHarmonyItemPurityMixRate[lpObj->AccountLevel];
		}
		else if (ItemCount == 1)
		{
			if (lpObj->ChaosBox[ItemSlot].IsExcItem() == 0)
			{
				pMsg.rate = gServerInfo.m_JewelOfHarmonyItemSmeltMixRate1[lpObj->AccountLevel];
			}
			else
			{
				pMsg.rate = gServerInfo.m_JewelOfHarmonyItemSmeltMixRate2[lpObj->AccountLevel];
			}
		}
	} break;
	case CHAOS_MIX_ITEM_380:
	{
		int GuardianCount = 0;
		int HarmonyCount = 0;
		int ItemCount = 0;
		int ItemSlot = 0;

		for (int n = 0; n < CHAOS_BOX_SIZE; n++)
		{
			if (lpObj->ChaosBox[n].IsItem() == 0)
			{
				continue;
			}

			if (lpObj->ChaosBox[n].m_Index == GET_ITEM(14, 31))
			{
				GuardianCount++;
			}
			else if (lpObj->ChaosBox[n].m_Index == GET_ITEM(14, 42))
			{
				HarmonyCount++;
			}
			else if (lpObj->ChaosBox[n].Is380Item() == 0 && lpObj->ChaosBox[n].m_Level >= 4 && lpObj->ChaosBox[n].m_Option3 >= 1)
			{
				ItemCount++;
				ItemSlot = n;
			}
		}

		if (GuardianCount != 1 || HarmonyCount != 1 || ItemCount != 1)
		{
			return;
		}

		if (lpObj->ChaosBox[ItemSlot].m_Level >= 7)
		{
			pMsg.rate = gServerInfo.m_Item380MixRate2[lpObj->AccountLevel];
		}
		else
		{
			pMsg.rate = gServerInfo.m_Item380MixRate1[lpObj->AccountLevel];
		}

		if (this->GetTalismanOfLuckRate(lpObj, &pMsg.rate) == 0)
		{
			return;
		}
		pMsg.money = 10000000;
		pMsg.tax = (pMsg.money * gCastleSiegeSync.GetTaxRateChaos(lpObj->Index)) / 100;
	} break;
#endif
#if(GAMESERVER_UPDATE>=301)
	case CHAOS_MIX_ILLUSION_TEMPLE:
	{
		int ChaosCount = 0;
		int ScrollCount = 0;
		int ScrollLevel = 0;
		int PotionCount = 0;
		int PotionLevel = 0;

		for (int n = 0; n < CHAOS_BOX_SIZE; n++)
		{
			if (lpObj->ChaosBox[n].IsItem() == 0)
			{
				continue;
			}

			if (lpObj->ChaosBox[n].m_Index == GET_ITEM(12, 15))
			{
				ChaosCount++;
			}
			else if (lpObj->ChaosBox[n].m_Index == GET_ITEM(13, 49))
			{
				ScrollCount++;
				ScrollLevel = lpObj->ChaosBox[n].m_Level;
			}
			else if (lpObj->ChaosBox[n].m_Index == GET_ITEM(13, 50))
			{
				PotionCount++;
				PotionLevel = lpObj->ChaosBox[n].m_Level;
			}
		}

		if (ChaosCount != 1 || ScrollCount != 1 || PotionCount != 1 || ScrollLevel != PotionLevel)
		{
			return;
		}

		if (ScrollLevel > 6 || PotionLevel > 6)
		{
			return;
		}

		switch (ScrollLevel)
		{
		case 1:
			pMsg.rate = gServerInfo.m_IllusionTempleMixRate1[lpObj->AccountLevel];
			pMsg.money = 3000000;
			break;
		case 2:
			pMsg.rate = gServerInfo.m_IllusionTempleMixRate2[lpObj->AccountLevel];
			pMsg.money = 5000000;
			break;
		case 3:
			pMsg.rate = gServerInfo.m_IllusionTempleMixRate3[lpObj->AccountLevel];
			pMsg.money = 7000000;
			break;
		case 4:
			pMsg.rate = gServerInfo.m_IllusionTempleMixRate4[lpObj->AccountLevel];
			pMsg.money = 9000000;
			break;
		case 5:
			pMsg.rate = gServerInfo.m_IllusionTempleMixRate5[lpObj->AccountLevel];
			pMsg.money = 11000000;
			break;
		case 6:
			pMsg.rate = gServerInfo.m_IllusionTempleMixRate6[lpObj->AccountLevel];
			pMsg.money = 13000000;
			break;
		}

		if (this->GetTalismanOfLuckRate(lpObj, &pMsg.rate) == 0)
		{
			return;
		}
		pMsg.tax = (pMsg.money * gCastleSiegeSync.GetTaxRateChaos(lpObj->Index)) / 100;
	} 
	break;
	case CHAOS_MIX_FEATHER_OF_CONDOR:
	{
		int ChaosCount = 0;
		int CreationCount = 0;
		int SoulPack10 = 0;
		int WingCount = 0;
		int ItemCount = 0;
		int ItemMoney = 0;

		for (int n = 0; n < CHAOS_BOX_SIZE; n++)
		{
			if (lpObj->ChaosBox[n].IsItem() == 0)
			{
				continue;
			}

			if (lpObj->ChaosBox[n].m_Index == GET_ITEM(12, 15))
			{
				ChaosCount++;
			}
			else if (lpObj->ChaosBox[n].m_Index == GET_ITEM(14, 22))
			{
				CreationCount++;
			}
			else if (lpObj->ChaosBox[n].m_Index == GET_ITEM(12, 31) && lpObj->ChaosBox[n].m_Level == 0)
			{
				SoulPack10++;
			}
			else if (((lpObj->ChaosBox[n].m_Index >= GET_ITEM(12, 3) && lpObj->ChaosBox[n].m_Index <= GET_ITEM(12, 6)) || lpObj->ChaosBox[n].m_Index == GET_ITEM(12, 42) || lpObj->ChaosBox[n].m_Index == GET_ITEM(12, 49) || lpObj->ChaosBox[n].m_Index == GET_ITEM(13, 30)) && (lpObj->ChaosBox[n].m_Level >= 9 && lpObj->ChaosBox[n].m_Option3 >= 1))
			{
				WingCount++;
			}
			else if (lpObj->ChaosBox[n].IsSetItem() != 0 && lpObj->ChaosBox[n].m_Level >= 7 && lpObj->ChaosBox[n].m_Option3 >= 1)
			{
				ItemCount++;
				ItemMoney += lpObj->ChaosBox[n].m_BuyMoney;
			}
		}

		if (ChaosCount != 1 || CreationCount != 1 || SoulPack10 != 1 || WingCount != 1 || ItemCount == 0)
		{
			return;
		}

		if (gServerInfo.m_FeatherOfCondorMixRate[lpObj->AccountLevel] == -1)
		{
			pMsg.rate = (ItemMoney / 300000) + 1;
		}
		else
		{
			pMsg.rate = gServerInfo.m_FeatherOfCondorMixRate[lpObj->AccountLevel];
		}

		if (this->GetTalismanOfLuckRate(lpObj, &pMsg.rate) == 0)
		{
			return;
		}
		if (gServerInfo.m_FeatherOfCondorMixRate[lpObj->AccountLevel] == -1)
		{
			pMsg.rate = ((pMsg.rate > 60) ? 60 : pMsg.rate);
		}
		else
		{
			pMsg.rate = ((pMsg.rate > 100) ? 100 : pMsg.rate);
		}

		pMsg.money = pMsg.rate * 200000;

		pMsg.tax = (pMsg.money * gCastleSiegeSync.GetTaxRateChaos(lpObj->Index)) / 100;
	} break;
	case CHAOS_MIX_WING4:
	{
		int ChaosCount = 0;
		int CreationCount = 0;
		int SoulPack10 = 0;
		int BlessPack10 = 0;
		int FlameCount = 0;
		int FeatherCount = 0;
		int ItemCount = 0;
		int ItemMoney = 0;

		for (int n = 0; n < CHAOS_BOX_SIZE; n++)
		{
			if (lpObj->ChaosBox[n].IsItem() == 0)
			{
				continue;
			}

			if (lpObj->ChaosBox[n].m_Index == GET_ITEM(12, 15))
			{
				ChaosCount++;
			}
			else if (lpObj->ChaosBox[n].m_Index == GET_ITEM(14, 22))
			{
				CreationCount++;
			}
			else if (lpObj->ChaosBox[n].m_Index == GET_ITEM(12, 31) && lpObj->ChaosBox[n].m_Level == 0)
			{
				SoulPack10++;
			}
			else if (lpObj->ChaosBox[n].m_Index == GET_ITEM(12, 30) && lpObj->ChaosBox[n].m_Level == 0)
			{
				BlessPack10++;
			}
			else if (lpObj->ChaosBox[n].m_Index == GET_ITEM(13, 52))
			{
				FlameCount++;
			}
			else if (lpObj->ChaosBox[n].m_Index == GET_ITEM(13, 53))
			{
				FeatherCount++;
			}
			else if (lpObj->ChaosBox[n].IsExcItem() != 0 && lpObj->ChaosBox[n].m_Level >= 9 && lpObj->ChaosBox[n].m_Option3 >= 1)
			{
				ItemCount++;
				ItemMoney += lpObj->ChaosBox[n].m_BuyMoney;
			}
		}

		if (ChaosCount != 1 || CreationCount != 1 || SoulPack10 != 1 || BlessPack10 != 1 || FlameCount != 1 || FeatherCount != 1 || ItemCount == 0)
		{
			return;
		}

		if (gServerInfo.m_Wing3MixRate[lpObj->AccountLevel] == -1)
		{
			pMsg.rate = (ItemMoney / 3000000) + 1;
		}
		else
		{
			pMsg.rate = gServerInfo.m_Wing3MixRate[lpObj->AccountLevel];
		}

		if (this->GetTalismanOfLuckRate(lpObj, &pMsg.rate) == 0)
		{
			return;
		}
		if (gServerInfo.m_Wing3MixRate[lpObj->AccountLevel] == -1)
		{
			pMsg.rate = ((pMsg.rate > 40) ? 40 : pMsg.rate);
		}
		else
		{
			pMsg.rate = ((pMsg.rate > 100) ? 100 : pMsg.rate);
		}

		pMsg.money = pMsg.rate * 200000;

		pMsg.tax = (pMsg.money * gCastleSiegeSync.GetTaxRateChaos(lpObj->Index)) / 100;
	} break;
#endif
#if(GAMESERVER_UPDATE>=401)
	case CHAOS_MIX_SOCKET_ITEM_CREATE_SEED:
	{
		int ChaosCount = 0;
		int CreationCount = 0;
		int HarmonyCount = 0;
		int ExcItemCount = 0;
		int SetItemCount = 0;
		int ItemMoney = 0;

		for (int n = 0; n < CHAOS_BOX_SIZE; n++)
		{
			if (lpObj->ChaosBox[n].IsItem() == 0)
			{
				continue;
			}

			if (lpObj->ChaosBox[n].m_Index == GET_ITEM(12, 15))
			{
				ChaosCount++;
			}
			else if (lpObj->ChaosBox[n].m_Index == GET_ITEM(14, 22))
			{
				CreationCount++;
			}
			else if (lpObj->ChaosBox[n].m_Index == GET_ITEM(14, 42))
			{
				HarmonyCount++;
			}
			else if (lpObj->ChaosBox[n].IsExcItem() != 0 && lpObj->ChaosBox[n].m_Level >= 4 && ExcItemCount == 0)
			{
				ExcItemCount++;
				ItemMoney += lpObj->ChaosBox[n].m_BuyMoney;
			}
			else if (lpObj->ChaosBox[n].IsSetItem() != 0 && lpObj->ChaosBox[n].m_Level >= 4 && SetItemCount == 0)
			{
				SetItemCount++;
				ItemMoney += lpObj->ChaosBox[n].m_BuyMoney;
			}
		}

		if (ChaosCount != 1 || CreationCount != 1 || HarmonyCount != 1 || ExcItemCount != 1 || SetItemCount != 1)
		{
			return;
		}

		if (gServerInfo.m_SocketItemCreateSeedMixRate[lpObj->AccountLevel] == -1)
		{
			pMsg.rate = (ItemMoney / 2000000) + 80;
		}
		else
		{
			pMsg.rate = gServerInfo.m_SocketItemCreateSeedMixRate[lpObj->AccountLevel];
		}

		if (gServerInfo.m_SocketItemCreateSeedMixRate[lpObj->AccountLevel] == -1)
		{
			pMsg.rate = ((pMsg.rate > 90) ? 90 : pMsg.rate);
		}

		pMsg.money = 1000000;

		pMsg.tax = (pMsg.money * gCastleSiegeSync.GetTaxRateChaos(lpObj->Index)) / 100;
	}break;
	case CHAOS_MIX_SOCKET_ITEM_CREATE_SEED_SPHERE:
	{
		int ChaosCount = 0;
		int CreationCount = 0;
		int SeedCount = 0;
		int SeedType = 0;
		int SeedLevel = 0;
		int SphereCount = 0;
		int SphereType = 0;
		int ItemMoney = 0;

		for (int n = 0; n < CHAOS_BOX_SIZE; n++)
		{
			if (lpObj->ChaosBox[n].IsItem() == 0)
			{
				continue;
			}

			if (lpObj->ChaosBox[n].m_Index == GET_ITEM(12, 15))
			{
				ChaosCount++;
			}
			else if (lpObj->ChaosBox[n].m_Index == GET_ITEM(14, 22))
			{
				CreationCount++;
			}
			else if (lpObj->ChaosBox[n].m_Index >= GET_ITEM(12, 60) && lpObj->ChaosBox[n].m_Index <= GET_ITEM(12, 66))
			{
				SeedCount++;
				SeedType = lpObj->ChaosBox[n].m_Index - GET_ITEM(12, 60);
				SeedLevel = lpObj->ChaosBox[n].m_Level;
				ItemMoney += lpObj->ChaosBox[n].m_BuyMoney;
			}
			else if (lpObj->ChaosBox[n].m_Index >= GET_ITEM(12, 70) && lpObj->ChaosBox[n].m_Index <= GET_ITEM(12, 74))
			{
				SphereCount++;
				SphereType = lpObj->ChaosBox[n].m_Index - GET_ITEM(12, 70);
				ItemMoney += lpObj->ChaosBox[n].m_BuyMoney;
			}
		}

		if (ChaosCount != 1 || CreationCount != 1 || SeedCount != 1 || SphereCount != 1)
		{
			return;
		}

		if (gServerInfo.m_SocketItemCreateSeedSphereMixRate[lpObj->AccountLevel] == -1)
		{
			pMsg.rate = (ItemMoney / 200000) + 80;
		}
		else
		{
			pMsg.rate = gServerInfo.m_SocketItemCreateSeedSphereMixRate[lpObj->AccountLevel];
		}

		if (gServerInfo.m_SocketItemCreateSeedSphereMixRate[lpObj->AccountLevel] == -1)
		{
			pMsg.rate = ((pMsg.rate > 90) ? 90 : pMsg.rate);
		}

		pMsg.money = 1000000;

		pMsg.tax = (pMsg.money * gCastleSiegeSync.GetTaxRateChaos(lpObj->Index)) / 100;
	}break;
#endif
		case CHAOS_MIX_LUCKY_ITEM_REFINE:
		{
			int LuckyItemCount = 0;
			int LuckyItemSlot = 0;

			for (int n = 0; n < CHAOS_BOX_SIZE; n++)
			{
				if (lpObj->ChaosBox[n].IsItem() == 0)
				{
					continue;
				}

				if (lpObj->ChaosBox[n].IsLuckyItem() != 0)
				{
					LuckyItemCount++;
					LuckyItemSlot = n;
				}
			}

			if (LuckyItemCount != 1)
			{
				return;
			}

			if (lpObj->ChaosBox[LuckyItemSlot].m_Durability == 255)
			{
				pMsg.rate = gServerInfo.m_LuckyItemRefineMixRate1[lpObj->AccountLevel];
			}
			else
			{
				pMsg.rate = gServerInfo.m_LuckyItemRefineMixRate2[lpObj->AccountLevel];
			}

		}
		break;
		default:
		break;
	}
	pMsg.rate = ((pMsg.rate > 100) ? 100 : ((pMsg.rate < 0) ? 0 : pMsg.rate));
	DataSend(lpObj->Index, (BYTE*)&pMsg, pMsg.header.size);
}