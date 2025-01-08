#include "stdafx.h"
#include "SPK_ItemTrader.h"
#include "Monster.h"
#include "ItemManager.h"
#include "ObjectManager.h"
#include "SkillManager.h"
#include "EffectManager.h"
#include "MasterSkillTree.h"
#include "ServerInfo.h"
#include "MemScript.h"
#include "Util.h"
#include "CashShop.h"
#include "Message.h"
#include "Notice.h"
#include "Trade.h"
#include "DSProtocol.h"
#include "Log.h"
#include "ItemLevel.h"
#include "GameMain.h"
#include "CSProtocol.h"
#include "MonsterSetBase.h"
#include "MapServerManager.h"
#if(SPK_ITEMTRADER)

GC_ItemTrader gGC_ItemTrader;

GC_ItemTrader::GC_ItemTrader()
{
	this->Enable = 0;
	this->mNPCData = new NPC_DATA_DOIITEM;
}

GC_ItemTrader::~GC_ItemTrader()
{

}

void GC_ItemTrader::SetNPC()
{
	if (gGC_ItemTrader.Enable)
	{
		MONSTER_SET_BASE_INFO info;
		memset(&info, 0, sizeof(info));

		info.Type			= 0;
		info.MonsterClass	= gGC_ItemTrader.mNPCData->NPCClass;
		info.Map			= gGC_ItemTrader.mNPCData->NPCMap;
		info.Dis			= 0;
		info.Dir			= gGC_ItemTrader.mNPCData->NPCDir;
		info.X				= gGC_ItemTrader.mNPCData->NPCX;
		info.Y				= gGC_ItemTrader.mNPCData->NPCY;

		gMonsterSetBase.SetInfo(info);
	}
}
void GC_ItemTrader::LoadConfig(char* FilePath)
{
	this->m_GroupDoiItemData.clear();
	this->mNPCData->Clear();

	pugi::xml_document file;
	pugi::xml_parse_result res = file.load_file(FilePath);
	if (res.status != pugi::status_ok) {
		ErrorMessageBox("File %s load fail. Error: %s", FilePath, res.description());
		return;
	}
	pugi::xml_node gItemTrade = file.child("SPK_ItemTrade");
	this->Enable = gItemTrade.attribute("Enable").as_int();

	pugi::xml_node Message = gItemTrade.child("Message");
	for (pugi::xml_node msg = Message.child("Msg"); msg; msg = msg.next_sibling())
	{
		DOIITEM_MSG info;

		info.Index = msg.attribute("Index").as_int();
		strncpy_s(info.Message, msg.attribute("Text").as_string(), sizeof(info.Message));
		this->m_MessageInfoBP.insert(std::pair<int, DOIITEM_MSG>(info.Index, info));
	}

	pugi::xml_node EventConfigNPC	= gItemTrade.child("NPC");
	this->mNPCData->NPCClass		= EventConfigNPC.attribute("NPCClass").as_int();
	this->mNPCData->NPCMap			= EventConfigNPC.attribute("NPCMap").as_int();
	this->mNPCData->NPCX			= EventConfigNPC.attribute("NPCX").as_int();
	this->mNPCData->NPCY			= EventConfigNPC.attribute("NPCY").as_int();
	this->mNPCData->NPCDir			= EventConfigNPC.attribute("NPCDir").as_int();
	//=== Load Data
	int mIndex = 0;
	for (pugi::xml_node GroupDungLuyen = gItemTrade.child("GroupDoiItem"); GroupDungLuyen; GroupDungLuyen = GroupDungLuyen.next_sibling())
	{
		GROUPDOIITEM_DATA infoGroup;
		infoGroup.Index		= mIndex++;
		infoGroup.Notice	= GroupDungLuyen.attribute("Notice").as_int();
		strncpy_s(infoGroup.Name, GroupDungLuyen.attribute("Name").as_string(), sizeof(infoGroup.Name));
		infoGroup.WCoinC		= GroupDungLuyen.attribute("WCoinC").as_int();
		infoGroup.WCoinP		= GroupDungLuyen.attribute("WCoinP").as_int();
		infoGroup.WCoinG		= GroupDungLuyen.attribute("WCoinG").as_int();
		infoGroup.CoinAtm		= GroupDungLuyen.attribute("CoinAtm").as_int();

		//Config ItemChinh
		pugi::xml_node ItemChinh		= GroupDungLuyen.child("ItemChinh");
		infoGroup.ItemChinh.IndexItem	= SafeGetItem(GET_ITEM(ItemChinh.attribute("Type").as_int(), ItemChinh.attribute("Index").as_int()));
		infoGroup.ItemChinh.Level		= ItemChinh.attribute("Level").as_int();
		infoGroup.ItemChinh.Skill		= ItemChinh.attribute("Skill").as_int();
		infoGroup.ItemChinh.Luck		= ItemChinh.attribute("Luck").as_int();
		infoGroup.ItemChinh.Option		= ItemChinh.attribute("Option").as_int();
		infoGroup.ItemChinh.Exc			= ItemChinh.attribute("Exc").as_int();
		infoGroup.ItemChinh.Harmony		= ItemChinh.attribute("Harmony").as_int();

		//Config KetQua
		pugi::xml_node ItemKetQua = GroupDungLuyen.child("ItemKetQua");
		infoGroup.ItemKetQua.IndexItem  = SafeGetItem(GET_ITEM(ItemKetQua.attribute("Type").as_int(), ItemKetQua.attribute("Index").as_int()));
		infoGroup.ItemKetQua.IndexItem1 = SafeGetItem(GET_ITEM(ItemKetQua.attribute("Type1").as_int(), ItemKetQua.attribute("Index1").as_int()));
		infoGroup.ItemKetQua.IndexItem2 = SafeGetItem(GET_ITEM(ItemKetQua.attribute("Type2").as_int(), ItemKetQua.attribute("Index2").as_int()));
		infoGroup.ItemKetQua.IndexItem3 = SafeGetItem(GET_ITEM(ItemKetQua.attribute("Type3").as_int(), ItemKetQua.attribute("Index3").as_int()));
		infoGroup.ItemKetQua.IndexItem4 = SafeGetItem(GET_ITEM(ItemKetQua.attribute("Type4").as_int(), ItemKetQua.attribute("Index4").as_int()));
		infoGroup.ItemKetQua.IndexItem5 = SafeGetItem(GET_ITEM(ItemKetQua.attribute("Type5").as_int(), ItemKetQua.attribute("Index5").as_int()));

		this->m_GroupDoiItemData.insert(std::pair<int, GROUPDOIITEM_DATA>(infoGroup.Index, infoGroup));
	}
	//LogAdd(LOG_BLUE, "[ItemTrade] Load OK Enable = %d, Size %d", this->Enable, this->m_GroupDoiItemData.size());
}


char* GC_ItemTrader::GetMessage(int index) // OK
{
	std::map<int, DOIITEM_MSG>::iterator it = this->m_MessageInfoBP.find(index);

	if (it == this->m_MessageInfoBP.end())
	{
		char Error[256];
		wsprintf(Error, "Could not find message %d!", index);
		return Error;
	}
	else
	{
		return it->second.Message;
	}
}

void GC_ItemTrader::RollBackItem(int aIndex, int TypeClear, bool RollBackItem)
{
	if (OBJECT_RANGE(aIndex) == 0)
	{
		return;
	}
	bool clear = false;

	if ((TypeClear == 2 || TypeClear == 0) && gObj[aIndex].IndexMix != -1)
	{
		if (RollBackItem)
		{
			BYTE SlotRecv = gItemManager.InventoryInsertItem(aIndex, gObj[aIndex].ItemChinh);
			if (SlotRecv != 0xFF)
			{
				gItemManager.GCItemModifySend(aIndex, SlotRecv);

			}
		}
		gObj[aIndex].IndexMix = -1;
		gObj[aIndex].ItemChinh.Clear();
		clear = true;
	}

	if ((TypeClear == 1 || TypeClear == 0) && gObj[aIndex].IndexMix != -1)
	{
		if (gObj[aIndex].IndexMix != -1)
		{
			gNotice.GCNoticeSend(aIndex, 0, 0, 0, 0, 0, 0, this->GetMessage(3));
			return;
		}
		if (RollBackItem)
		{
			BYTE SlotRecv = gItemManager.InventoryInsertItem(aIndex, gObj[aIndex].ItemChinh); 
			if (SlotRecv != 0xFF)
			{
				gItemManager.GCItemModifySend(aIndex, SlotRecv);
			}
		}
		//=== Item Chinh
		gObj[aIndex].IndexMix = -1;
		gObj[aIndex].ItemChinh.Clear();
		clear = true;
	}

	if (clear)
	{
		gObj[aIndex].IndexMix = -1;
		gObj[aIndex].ItemChinh.Clear();
	}
	if (RollBackItem)
	{
		GDCharacterInfoSaveSend(aIndex);
	}
}

void GC_ItemTrader::SetStateInterface(int aIndex,int Type)
{
	if (Type == 1)
	{
		gObj[aIndex].Interface.state = 1;
		gObj[aIndex].Interface.use = 1;
		gObj[aIndex].Interface.type = INTERFACE_DOIITEM;
		gObj[aIndex].Transaction = 0;
		gObjInventoryTransaction(aIndex);
	}
	else
	{
		if (gObj[aIndex].StatusMix > 0) //SAVE Neu Da Mix
		{
			if (gObj[aIndex].StatusMix == 1)
			{
				gItemManager.InventoryInsertItem(aIndex, gObj[aIndex].ItemChinh);
			}
			else
			{
				for(int n=0;n < 6;n++)
				{
					gItemManager.InventoryInsertItem(aIndex, gObj[aIndex].ItemKetQua[n]);
				}
			}	
		}
		else
		{
			gObjInventoryRollback(aIndex);
		}

		//==Clear Cache
		gObj[aIndex].IndexMix = -1;
		gObj[aIndex].ItemChinh.Clear();
		for(int n=0;n < 6;n++)
		{
			gObj[aIndex].ItemKetQua[n].Clear();
		}
		
		gObj[aIndex].StatusMix = -1;
		SendInfoItemCache(aIndex);
		gItemManager.GCItemListSend(aIndex);
		gObjectManager.CharacterMakePreviewCharSet(aIndex);
		gItemManager.GCItemEquipmentSend(aIndex);

		gObj[aIndex].Interface.use = 0;
		gObj[aIndex].Interface.type = INTERFACE_NONE;
		gObj[aIndex].Interface.state = 0;
		gObj[aIndex].TargetNumber = -1;
	}
}

void GC_ItemTrader::SendInfoItemCache(int aIndex)
{
	if (!OBJMAX_RANGE(aIndex))
	{
		return;
	}


	LPOBJ lpObj = &gObj[aIndex];

	if (lpObj->Type != OBJECT_USER || lpObj->m_OfflineMode != 0 || !this->Enable)
	{
		return;

	}

	if (lpObj->Interface.type != INTERFACE_DOIITEM)
	{

		return;
	}

	std::map<int, GROUPDOIITEM_DATA>::iterator it = this->m_GroupDoiItemData.find(lpObj->IndexMix);
	BYTE send[8192];
	INFOITEM_DOIITEM_CLIENT pMsg;
	pMsg.header.set(0xD3, 0x6B, 0);
	int size = sizeof(pMsg);
	pMsg.ActiveMix = 1;

	if (!lpObj->ItemChinh.IsItem()	|| !lpObj->ItemChinh.IsItem())
	{
		pMsg.ActiveMix = 0;
	}

	gItemManager.ItemByteConvert(pMsg.ItemChinh, lpObj->ItemChinh);
	for (int n = 0; n < 6; n++)
	{
		gItemManager.ItemByteConvert(pMsg.ItemKetQua[n], lpObj->ItemKetQua[n]); 
	}

	pMsg.WCoinC  =it->second.WCoinC ;
	pMsg.WCoinP  =it->second.WCoinP ;
	pMsg.WCoinG  =it->second.WCoinG ;
	pMsg.CoinAtm =it->second.CoinAtm;

	pMsg.header.size[0] = SET_NUMBERHB(size);
	pMsg.header.size[1] = SET_NUMBERLB(size);
	memcpy(send, &pMsg, sizeof(pMsg));

	DataSend(aIndex, send, size);
}

void GC_ItemTrader::ProcItemSend(int aIndex, int SlotItem)
{
	if (!OBJMAX_RANGE(aIndex))
	{
		return;
	}
	LPOBJ lpObj = &gObj[aIndex];
	if (lpObj->Type != OBJECT_USER || lpObj->m_OfflineMode != 0 || !this->Enable)
	{
		return;

	}

	if (lpObj->Interface.type != INTERFACE_DOIITEM)
	{
		return;
	}

	if (gItemManager.CheckItemMoveToTrade(lpObj, &lpObj->Inventory[SlotItem], 0) == 0)
	{
		gNotice.GCNoticeSend(aIndex, 1, 0, 0, 0, 0, 0, this->GetMessage(1));//
		return;
	}

	if (gItemManager.ChaosBoxHasItem(lpObj) || gItemManager.TradeHasItem(lpObj))
	{	
		return;
	}

	if (gItemManager.CheckItemInventorySpace(lpObj, 4, 4) == 0)
	{
		gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, this->GetMessage(5));
		return;
	}

	if (!lpObj->Inventory[SlotItem].IsItem())
	{
		gNotice.GCNoticeSend(aIndex, 1, 0, 0, 0, 0, 0, this->GetMessage(1));
		return;
	}
	if (gObj[aIndex].StatusMix > 0)
	{
		gNotice.GCNoticeSend(aIndex, 1, 0, 0, 0, 0, 0, "Vui lòng đóng và mở lại item");
		return;
	}
	
	if (!lpObj->ItemChinh.IsItem())
	{
		for (std::map<int, GROUPDOIITEM_DATA>::iterator it = this->m_GroupDoiItemData.begin(); it != this->m_GroupDoiItemData.end(); it++)
		{
			if (it == this->m_GroupDoiItemData.end())
			{
				gNotice.GCNoticeSend(aIndex, 1, 0, 0, 0, 0, 0, this->GetMessage(1));//
				return;
			}
			if (lpObj->Inventory[SlotItem].m_Index == it->second.ItemChinh.IndexItem)

			{
				if ((it->second.ItemChinh.Level != -1 && it->second.ItemChinh.Level > lpObj->Inventory[SlotItem].m_Level)
					|| (it->second.ItemChinh.Skill != -1 && it->second.ItemChinh.Skill > lpObj->Inventory[SlotItem].m_Option1)
					|| (it->second.ItemChinh.Luck != -1 && it->second.ItemChinh.Luck > lpObj->Inventory[SlotItem].m_Option2)
					|| (it->second.ItemChinh.Option != -1 && it->second.ItemChinh.Option > lpObj->Inventory[SlotItem].m_Option3)
					|| it->second.ItemChinh.Exc != 0 && !lpObj->Inventory[SlotItem].IsExcItem()
					|| it->second.ItemChinh.Harmony != 0 && !lpObj->Inventory[SlotItem].IsJewelOfHarmonyItem())
				{
					gNotice.GCNoticeSend(aIndex, 1, 0, 0, 0, 0, 0, this->GetMessage(1));//
					return;
				}
				
				lpObj->IndexMix = it->second.Index;
				lpObj->ItemChinh = lpObj->Inventory[SlotItem];

				//=== Set Item Ket Qua
				for (int i = 0; i < 6; ++i) 
				{
					lpObj->ItemKetQua[i] = lpObj->Inventory[SlotItem];
				}
				lpObj->ItemKetQua[0].m_Index = it->second.ItemKetQua.IndexItem;
				lpObj->ItemKetQua[1].m_Index = it->second.ItemKetQua.IndexItem1;
				lpObj->ItemKetQua[2].m_Index = it->second.ItemKetQua.IndexItem2;
				lpObj->ItemKetQua[3].m_Index = it->second.ItemKetQua.IndexItem3;
				lpObj->ItemKetQua[4].m_Index = it->second.ItemKetQua.IndexItem4;
				lpObj->ItemKetQua[5].m_Index = it->second.ItemKetQua.IndexItem5;

				//=== Send Del item
				gItemManager.InventoryDelItem(aIndex, SlotItem);
				gItemManager.GCItemDeleteSend(aIndex, SlotItem, 1);
				this->SendInfoItemCache(aIndex);
				//====
				return;
				
			}
		}
	}
}
void GC_ItemTrader::StepClick(LPOBJ lpObj, int aIndex, BYTE SlotRecv, int BackSlot, BYTE ID)
{
	if (lpObj->ItemKetQua[ID].m_Index < 0)
	{
		gNotice.GCNoticeSend(aIndex, 1, 0, 0, 0, 0, 0, this->GetMessage(3));
		return;
	}

	if (lpObj->ItemChinh.IsItem())
	{
		SlotRecv = gItemManager.InventoryInsertItem(aIndex, lpObj->ItemKetQua[ID]);
		gNotice.GCNoticeSend(lpObj->Index, 0, 0, 0, 0, 0, 0, this->GetMessage(6), lpObj->Name, gItemLevel.GetItemName(lpObj->ItemChinh.m_Index, lpObj->ItemChinh.m_Level), gItemLevel.GetItemName(lpObj->ItemKetQua[BackSlot - 1].m_Index, lpObj->ItemKetQua[BackSlot - 1].m_Level));
		gLog.Output(LOG_TRADE, "[NPC ItemTrader] %s | đổi %s -> %s", lpObj->Name, gItemLevel.GetItemName(lpObj->ItemChinh.m_Index, lpObj->ItemChinh.m_Level), gItemLevel.GetItemName(lpObj->ItemKetQua[BackSlot - 1].m_Index, lpObj->ItemKetQua[BackSlot - 1].m_Level));
		lpObj->ItemChinh.Clear();
		for (int n = 0; n < 6; n++)
		{
			lpObj->ItemKetQua[n].Clear();
		}
	}
	gObjInventoryCommit(aIndex);
}
void GC_ItemTrader::BackItem(int aIndex, int BackSlot)
{
	if (!OBJMAX_RANGE(aIndex))
	{
		return;
	}


	LPOBJ lpObj = &gObj[aIndex];

	if (lpObj->Type != OBJECT_USER || lpObj->m_OfflineMode != 0 || !this->Enable)
	{
		return;

	}

	std::map<int, GROUPDOIITEM_DATA>::iterator it = this->m_GroupDoiItemData.find(lpObj->IndexMix);

	if (lpObj->Interface.type != INTERFACE_DOIITEM)
	{
		return;
	}
	if ( BackSlot >= 1 && lpObj->Coin1 < it->second.WCoinC)
	{
		gNotice.GCNoticeSend(aIndex, 1, 0, 0, 0, 0, 0, this->GetMessage(2));
		return;
	}
	if (BackSlot >= 1 && lpObj->Coin2 < it->second.WCoinP)
	{
		gNotice.GCNoticeSend(aIndex, 1, 0, 0, 0, 0, 0, this->GetMessage(2));
		return;
	}
	if (BackSlot >= 1 && lpObj->Coin3 < it->second.WCoinG)
	{
		gNotice.GCNoticeSend(aIndex, 1, 0, 0, 0, 0, 0, this->GetMessage(2));
		return;
	}
	if (BackSlot >= 1 && lpObj->CoinAtm < it->second.CoinAtm)
	{
		gNotice.GCNoticeSend(aIndex, 1, 0, 0, 0, 0, 0, this->GetMessage(2));
		return;
	}
	if (gItemManager.CheckItemInventorySpace(lpObj, 4, 4) == 0)
	{
		gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, this->GetMessage(5));
		return;
	}

	BYTE SlotRecv = 0xFF;
	switch (BackSlot)
	{
		case 0: 
		{
			if (lpObj->ItemChinh.IsItem())
			{
				SlotRecv = gItemManager.InventoryInsertItem(aIndex, lpObj->ItemChinh); 
				lpObj->ItemChinh.Clear();
				for(int n=0;n < 6;n++)
				{
					lpObj->ItemKetQua[n].Clear();
				}
			}
		}
		break;
		case 1:	StepClick(lpObj, aIndex, SlotRecv, BackSlot, 0); break;
		case 2:	StepClick(lpObj, aIndex, SlotRecv, BackSlot, 1); break;
		case 3:	StepClick(lpObj, aIndex, SlotRecv, BackSlot, 2); break;
		case 4:	StepClick(lpObj, aIndex, SlotRecv, BackSlot, 3); break;
		case 5: StepClick(lpObj, aIndex, SlotRecv, BackSlot, 4); break;
		case 6:	StepClick(lpObj, aIndex, SlotRecv, BackSlot, 5); break;
		default:
		break;
	}
	if ( BackSlot >= 1)
	{
		GDSetCoinSend(lpObj->Index, -it->second.WCoinC, -it->second.WCoinP, -it->second.WCoinG,0, -it->second.CoinAtm,"ItemTrade");
	}

	if (SlotRecv != 0xFF)
	{
		gItemManager.GCItemModifySend(aIndex, SlotRecv);
		this->SendInfoItemCache(aIndex);
	}
}

bool GC_ItemTrader::Dialog(LPOBJ lpObj, LPOBJ lpNpc)
{
	if (!this->Enable || lpObj->Interface.type != INTERFACE_NONE)
	{
		gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, this->GetMessage(0));
		return false;
	}

	if (lpNpc->Class == this->mNPCData->NPCClass &&
		lpNpc->Map == this->mNPCData->NPCMap &&
		lpNpc->X == this->mNPCData->NPCX &&
		lpNpc->Y == this->mNPCData->NPCY)
	{
		XULY_CGPITEMTRADE pMsg;
		pMsg.header.set(0xD3, 0x6A, sizeof(pMsg));
		pMsg.ThaoTac = 111;	
		DataSend(lpObj->Index, (BYTE*)& pMsg, pMsg.header.size);
		SetStateInterface(lpObj->Index, 1);
		return true;
	}
	return false;
}
#endif