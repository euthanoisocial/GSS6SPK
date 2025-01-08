#include "stdafx.h"
#include "Protocol.h"
#include "Attack.h"
#include "BloodCastle.h"
#include "CashShop.h"
#include "CastleSiege.h"
#include "CastleSiegeSync.h"
#include "CastleSiegeWeapon.h"
#include "ChaosBox.h"
#include "ChaosCastle.h"
#include "CommandManager.h"
#include "CustomCommandDescription.h"
#include "CustomEventTime.h"
#include "CustomMove.h"
#include "CustomStore.h"
#include "CustomRanking.h"
#include "Crywolf.h"
#include "CSProtocol.h"
#include "DarkSpirit.h"
#include "DefaultClassInfo.h"
#include "DevilSquare.h"
#include "DoubleGoer.h"
#include "DSProtocol.h"
#include "Duel.h"
#include "EffectManager.h"
#include "ESProtocol.h"
#include "EventInventory.h"
#include "Filter.h"
#include "Fruit.h"
#include "GensSystem.h"
#include "Guild.h"
#include "GuildClass.h"
#include "HackPacketCheck.h"
#include "Helper.h"
#include "IllusionTemple.h"
#include "ImperialGuardian.h"
#include "InventoryEquipment.h"
#include "ItemManager.h"
#include "JewelMix.h"
#include "JSProtocol.h"
#include "KanturuEntranceNPC.h"
#include "Log.h"
#include "LuckyCoin.h"
#include "Map.h"
#include "MapServerManager.h"
#include "MasterSkillTree.h"
#include "Message.h"
#include "MiniMap.h"
#include "MiningSystem.h"
#include "Move.h"
#include "MuunSystem.h"
#include "Notice.h"
#include "NpcTalk.h"
#include "ObjectManager.h"
#include "Party.h"
#include "PartyMatching.h"
#include "PcPoint.h"
#include "PentagramSystem.h"
#include "PersonalShop.h"
#include "Protect.h"
#include "Quest.h"
#include "QuestWorld.h"
#include "ServerInfo.h"
#include "SkillManager.h"
#include "Trade.h"
#include "Util.h"
#include "Viewport.h"
#include "Warehouse.h"
#include "OfflineMode.h"
#include "RuudTokenSellTemplate.h"
#include "CustomMoveEvent.h"
#include "PartySearch.h"
#include "HwidManager.h"
#include "FakeOnline.h"
#include "PartySetPass.h"
#include "CustomJewelBank.h"
#include "CTCMini.h"
#if(SAU_ANTIHACK)
#include "BAntihack.h"
#endif
#include "ResetChange.h"
#include "GuildUpgrade.h"
#include "CustomLuckySpin.h"
#include "BEventThanMa.h"
#include "SPK_HonHoan.h"
#include "SPK_DanhHieu.h"
#include "SPK_QuanHam.h"
#include "SPK_TuChan.h"
#include "SPK_NewVip.h"
#include "CBCongHuong.h"
#include "CB_ArenaPVP.h"
#include "DSProtocol.h"
#include "GameMain.h"
#include "ActiveInvasions.h"
#include "ThueFlag.h"
#if SLOTPET2
#include "NewUIMyInventory.h"
#endif
#include "SPK_ItemTrader.h"
#include "SPK_Relife.h"
#include "SPK_NewXShop.h"
#include "B_MocNap.h"
#include "BDungLuyenItem.h"
#include "CustomBauCua.h"
#include "SPK_Harmony.h"
#include "SPK_AutoHp.h"
#include "SPK_AddBuff.h"
#include "ChienTruongCo.h"

void GCEventStateSendToAll(int map ,BYTE InvasionState, BYTE InvasionIndex)
{
	for ( int n=0; n<MAX_OBJECT ; n++ )
	{
		if ( gObj[n].Type == OBJECT_USER && map == gObj[n].Map )
		{
			GCEventStateSend(n, InvasionState, InvasionIndex);
		}
	}
}
#if CHANGE_PASS
void CSEND_CHANGEPASS(CSENDGS_DOIMK* lpMsg, int aIndex)
{
	if (!OBJECT_RANGE(aIndex))
	{
		return;
	}
	LPOBJ lpObj = &gObj[aIndex];

	if (lpMsg->TYPE != 0x01)
	{
		gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, "Sai thông tin");
		return;
	}
	if (strlen(lpMsg->PASS_OLD) < 1 || strlen(lpMsg->PASS_NEW) < 1)
	{
		gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, "Hãy nhập đầy đủ mật khẩu.");
		return;
	}
	if (strcmp(lpMsg->PASS_OLD, lpMsg->PASS_NEW) == 0)
	{
		gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, "Mật khẩu cũ và mới giống nhau!");
		return;
	}
	if (strcmp(lpMsg->PASS_NEW, lpMsg->PASS_NEW1) != 0)
	{
		gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, "Xác minh mật khẩu không giống nhau!");
		return;
	}
	if (lpObj->Coin1 < gServerInfo.ChangePassCoin)
	{
		gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, "Không đủ Wcoin chi phí là: %d WC", gServerInfo.ChangePassCoin);
		return;
	}
	CSENDGS_DOIMK_INFOSAVE	pMsg = { 0 };
	CSENDGS_DOIMK_INFOSAVE pRequest;
	pRequest.header.set(0x2C, 0x02, sizeof(pRequest));
	pRequest.gIndex = aIndex;
	memcpy(pRequest.ACCOUNT, lpObj->Account, MAX_ACCOUNT_LEN);
	memcpy(pRequest.PASS_OLD, lpMsg->PASS_OLD, MAX_ACCOUNT_LEN);
	memcpy(pRequest.PASS_NEW, lpMsg->PASS_NEW, MAX_ACCOUNT_LEN);
	gDataServerConnection.DataSend((BYTE*)& pRequest, pRequest.header.size);
}

void DSSENDGS_DOIMK_KQ(DSGS_DOIMK_KQ* lpMsg)
{
	if (!OBJECT_RANGE(lpMsg->gIndex))
	{
		return;
	}
	LPOBJ lpObj = &gObj[lpMsg->gIndex];

	if (lpMsg->result == 1)
	{
		gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, "Tài khoản này không đúng");
		return;
	}
	if (lpMsg->result == 2)
	{
		gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, "Mật khẩu cũ không đúng");
		return;
	}	
	GDSetCoinSend(lpObj->Index, -(gServerInfo.ChangePassCoin), 0, 0, 0, 0, "DoiPass");
	if (lpMsg->result == 3)
	{
		gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, "Đổi mật khẩu thành công!");
		return;
	}
}
#endif
void GCPostItemProc(PMSG_POST_ITEM_RECV* lpMsg, int aIndex ) //ok  
{
	LPOBJ lpObj = &gObj[aIndex];

	if (gObjIsConnectedGP(aIndex) == 0)
	{
		return;
	}

	char name[11] = { 0 };

	memcpy(name, lpMsg->chatid, sizeof(lpMsg->chatid));

	if (strcmp(name, lpObj->Name) != 0)
	{
		return;
	}

	lpMsg->chatmsg[(sizeof(lpMsg->chatmsg) - 1)] = 0;

	int slen = strlen(lpMsg->chatmsg);

	if (slen < 1 || slen >(MAX_CHAT_LEN - 1)) { return; }

	if (gServerInfo.m_CommandPostType < 4)
	{
		PMSG_POST_ITEM_SEND pMsg;
		pMsg.header.set(0x78, sizeof(pMsg));
		memcpy(pMsg.chatid, lpMsg->chatid, sizeof(pMsg.chatid));
		memcpy(pMsg.chatmsg, lpMsg->chatmsg, sizeof(pMsg.chatmsg));
		memcpy(pMsg.item_data, lpMsg->item_data, sizeof(pMsg.item_data));
		for (int n = OBJECT_START_USER; n < MAX_OBJECT; n++)
		{
			if (gObjIsConnectedGP(n) != 0)
			{
				DataSend(n, (BYTE*)&pMsg, pMsg.header.size);
			}
		}
	}
	else
	{
		short tmpservercode = gServerInfo.m_ServerCode;
		while (tmpservercode >= 20) { tmpservercode -= 20; }
		tmpservercode += 1;
		char szTemp[256];
		wsprintf(szTemp, "[SV-%d] %s", tmpservercode, lpMsg->chatmsg);
		GDGlobalItemPostSend(gMapServerManager.GetMapServerGroup(), lpMsg->chatid, szTemp, lpMsg->item_data);
	}
}

void ProtocolCore(BYTE head,BYTE* lpMsg,int size,int aIndex,int encrypt,int serial) // OK
{
	PROTECT_START
	LPOBJ lpObj = &gObj[aIndex];
	if (gObj[aIndex].Type == OBJECT_USER &&lpMsg[0] == 0xC1 && head == 0xFB)
	{
		switch (lpMsg[3])
		{
			case 0x3D:
			{
				g_OfflineMode.Start((CG_OFFMODE_RESULT*)lpMsg, aIndex);
			}
			break;
			#if(HT_THUE_FLAG)
			case 0x03:			ThueFlag.CGRequestRecv(lpMsg, aIndex);			break;
			#endif
			case 0x61:
			{
				lpObj->WarehouseSave = 0;
				return;
			}
			break;
			case 102: 			gCommandManager.CommandChange(&gObj[aIndex], "", 0);			break;
			case 103:			gCommandManager.CommandPKClear(&gObj[aIndex], "", 0);			break;
			case 104:			gCommandManager.CommandReAdd(&gObj[aIndex], "", 0);				break;
			case 105: 			gCommandManager.CommandReMaster(&gObj[aIndex], "", 0);			break;
			default:
			LogAdd(LOG_RED, "Packet Unk from Client(%d) (%x): %x %x %x (%x) %x %x %x %x %x %x", aIndex, head, lpMsg[0], lpMsg[1], lpMsg[2], lpMsg[3], lpMsg[4], lpMsg[5], lpMsg[6], lpMsg[7], lpMsg[8], lpMsg[9]);
			break;
		}
		return;
	}

	if(gObj[aIndex].Type == OBJECT_USER && gHackPacketCheck.CheckPacketHack(aIndex,head,((lpMsg[0]==0xC1)?lpMsg[3]:lpMsg[4]),encrypt,serial) == 0)
	{
		return;
	}
	#if(SAU_ANTIHACK)
	if (gBAntihack.BAntihackPacketRecv(head, lpMsg, size, aIndex, encrypt, serial) == false)
	{
		return;
	}
	#endif

	switch(head)
	{
		case 0x00: CGChatRecv((PMSG_CHAT_RECV*)lpMsg,aIndex);															break;
		case 0x02: CGChatWhisperRecv((PMSG_CHAT_WHISPER_RECV*)lpMsg,aIndex);											break;
		case 0x03: 
			CGMainCheckRecv((PMSG_MAIN_CHECK_RECV*)lpMsg,aIndex);
			#if(ADD_POINT)
			ExtraCpanelSendInfo(aIndex);
			#endif
			break;
		case 0x0E: CGLiveClientRecv((PMSG_LIVE_CLIENT_RECV*)lpMsg,aIndex);												break;
		case 0x11: gAttack.CGAttackRecv((PMSG_ATTACK_RECV*)lpMsg,aIndex);												break;
		case 0x15: CGPositionRecv((PMSG_POSITION_RECV*)lpMsg,aIndex);													break;
		case 0x18: CGActionRecv((PMSG_ACTION_RECV*)lpMsg,aIndex);														break;
		case 0x19: gSkillManager.CGSkillAttackRecv((PMSG_SKILL_ATTACK_RECV*)lpMsg,aIndex);								break;
		case 0x1B: gSkillManager.CGSkillCancelRecv((PMSG_SKILL_CANCEL_RECV*)lpMsg,aIndex);								break;
		case 0x1C: gMove.CGTeleportRecv((PMSG_TELEPORT_RECV*)lpMsg,aIndex);												break;
		case 0x1E: gSkillManager.CGDurationSkillAttackRecv((PMSG_DURATION_SKILL_ATTACK_RECV*)lpMsg,aIndex);				break;
		case 0x22: gItemManager.CGItemGetRecv((PMSG_ITEM_GET_RECV*)lpMsg,aIndex);										break;
		case 0x23: gItemManager.CGItemDropRecv((PMSG_ITEM_DROP_RECV*)lpMsg,aIndex);										break;
		case 0x24: gItemManager.CGItemMoveRecv((PMSG_ITEM_MOVE_RECV*)lpMsg,aIndex);										break;
		case 0x26: gItemManager.CGItemUseRecv((PMSG_ITEM_USE_RECV*)lpMsg,aIndex);										break;
		case 0x30: gNpcTalk.CGNpcTalkRecv((PMSG_NPC_TALK_RECV*)lpMsg,aIndex);											break;
		case 0x31: gNpcTalk.CGNpcTalkCloseRecv(aIndex);																	break;
		case 0x32: gItemManager.CGItemBuyRecv((PMSG_ITEM_BUY_RECV*)lpMsg,aIndex,0);										break;
		case 0x33: gItemManager.CGItemSellRecv((PMSG_ITEM_SELL_RECV*)lpMsg,aIndex);										break;
		case 0x34: gItemManager.CGItemRepairRecv((PMSG_ITEM_REPAIR_RECV*)lpMsg,aIndex);									break;
		case 0x36: gTrade.CGTradeRequestRecv((PMSG_TRADE_REQUEST_RECV*)lpMsg,aIndex);									break;
		case 0x37: gTrade.CGTradeResponseRecv((PMSG_TRADE_RESPONSE_RECV*)lpMsg,aIndex);									break;
		case 0x3A: gTrade.CGTradeMoneyRecv((PMSG_TRADE_MONEY_RECV*)lpMsg,aIndex);										break;
		case 0x3C: gTrade.CGTradeOkButtonRecv((PMSG_TRADE_OK_BUTTON_RECV*)lpMsg,aIndex);								break;
		case 0x3D: gTrade.CGTradeCancelButtonRecv(aIndex);																break;
		case 0x3F:
		switch(lpMsg[3])
		{
			case 0x01: gPersonalShop.CGPShopSetItemPriceRecv((PMSG_PSHOP_SET_ITEM_PRICE_RECV*)lpMsg,aIndex);			break;
			case 0x02: gPersonalShop.CGPShopOpenRecv((PMSG_PSHOP_OPEN_RECV*)lpMsg,aIndex);								break;
			case 0x03: gPersonalShop.CGPShopCloseRecv(aIndex);															break;
			case 0x05: gPersonalShop.CGPShopItemListRecv((PMSG_PSHOP_ITEM_LIST_RECV*)lpMsg,aIndex);						break;
			case 0x06: gPersonalShop.CGPShopBuyItemRecv((PMSG_PSHOP_BUY_ITEM_RECV*)lpMsg,aIndex);						break;
			case 0x07: gPersonalShop.CGPShopLeaveRecv((PMSG_PSHOP_LEAVE_RECV*)lpMsg,aIndex);							break;
		}
		break;
		case 0x40: gParty.CGPartyRequestRecv((PMSG_PARTY_REQUEST_RECV*)lpMsg,aIndex);									break;
		case 0x41: gParty.CGPartyRequestResultRecv((PMSG_PARTY_REQUEST_RESULT_RECV*)lpMsg,aIndex);						break;
		case 0x42: gParty.CGPartyListRecv(aIndex);																		break;
		case 0x43: gParty.CGPartyDelMemberRecv((PMSG_PARTY_DEL_MEMBER_RECV*)lpMsg,aIndex);								break;
		case 0x4A: gSkillManager.CGRageFighterSkillAttackRecv((PMSG_RAGE_FIGHTER_SKILL_ATTACK_RECV*)lpMsg,aIndex);		break;
		case 0x4B: gSkillManager.CGSkillDarkSideRecv((PMSG_SKILL_DARK_SIDE_RECV*)lpMsg,aIndex);							break;

		case 0x4C:
		switch(lpMsg[3])
		{
			case 0x00: break;
			case 0x01: break;
			case 0x03: break;
		}
		break;

		case 0x4D:
		switch(lpMsg[3])
		{
			case 0x00: break;
			case 0x01: break;
			case 0x0F: break;
			case 0x10: break;
			case 0x11: break;
			case 0x12: break;
			case 0x13: break;
			case 0x14: break;
			case 0x15: break;
		}
		break;

		case 0x4E:
		switch(lpMsg[3])
		{
			case 0x00:
				#if SLOTPET2 == 0
				#if(GAMESERVER_UPDATE>=603)	// SLOTPET2
				gMuunSystem.CGMuunItemGetRecv((PMSG_MUUN_ITEM_GET_RECV*)lpMsg,aIndex);
				#endif
				#endif
				break;
			case 0x01:
				#if SLOTPET2 == 0
				#if(GAMESERVER_UPDATE>=603) // SLOTPET2
				gMuunSystem.CGMuunDropItemRecv((PMSG_MUUN_ITEM_DROP_SEND*)lpMsg, aIndex);
				#endif
				#endif
				break;
			case 0x08:
				#if SLOTPET2 == 0
				#if(GAMESERVER_UPDATE>=603)	// SLOTPET2
				gMuunSystem.CGMuunItemUseRecv((PMSG_MUUN_ITEM_USE_RECV*)lpMsg,aIndex);
				#endif
				#endif
				break;
			case 0x09:
				#if SLOTPET2 == 0
				#if(GAMESERVER_UPDATE>=603) // SLOTPET2
				gMuunSystem.CGMuunItemSellRecv((PMSG_MUUN_ITEM_SELL_RECV*)lpMsg,aIndex);
				#endif
				#endif
				break;
			#if(SLOTPET2)
			case 0x0A:	g_pNewMyInventory.CGItemSellRecv((PMSG_EQUIP_ITEM_SELL_RECV*)lpMsg, aIndex);	break;
			#endif
		}
		break;

		case 0x50: gGuild.CGGuildRequestRecv((PMSG_GUILD_REQUEST_RECV*)lpMsg,aIndex);							break;
		case 0x51: gGuild.CGGuildResultRecv((PMSG_GUILD_RESULT_RECV*)lpMsg,aIndex);								break;
		case 0x52: gGuild.CGGuildListRecv(aIndex);																break;
		case 0x53: gGuild.CGGuildDeleteRecv((PMSG_GUILD_DELETE_RECV*)lpMsg,aIndex);								break;
		case 0x54: CGGuildMasterAnswerRecv((PMSG_GUILDMASTERANSWER*)lpMsg,aIndex);								break;
		case 0x55: CGGuildMasterInfoSave(aIndex,(PMSG_GUILDINFOSAVE*)lpMsg);									break;
		case 0x57: CGGuildMasterCreateCancel(aIndex);															break;
		case 0x61: GCGuildWarRequestSendRecv((PMSG_GUILDWARSEND_RESULT*)lpMsg,aIndex);							break;
		case 0x66: GCGuildViewportInfo((PMSG_REQ_GUILDVIEWPORT*)lpMsg,aIndex);									break;
		case 0x81: gWarehouse.CGWarehouseMoneyRecv((PMSG_WAREHOUSE_MONEY_RECV*)lpMsg,aIndex);					break;
		case 0x82: gWarehouse.CGWarehouseClose(aIndex);															break;
		case 0x83: gWarehouse.CGWarehousePasswordRecv((PMSG_WAREHOUSE_PASSWORD_RECV*)lpMsg,aIndex);				break;
		case 0x86: gChaosBox.CGChaosMixRecv((PMSG_CHAOS_MIX_RECV*)lpMsg,aIndex);								break;
		case 0x87: gChaosBox.CGChaosMixCloseRecv(aIndex);														break;
		case 0x8F: gChaosBox.CGChaosMixRateRecv((PMSG_CHAOS_MIX_RATE_RECV*)lpMsg, aIndex);						break;
		case 0x8A: break;
		case 0x8E: gMove.CGTeleportMoveRecv((PMSG_TELEPORT_MOVE_RECV*)lpMsg,aIndex);							break;
		case 0x90: gDevilSquare.CGDevilSquareEnterRecv((PMSG_DEVIL_SQUARE_ENTER_RECV*)lpMsg,aIndex);			break;
		case 0x91: CGEventRemainTimeRecv((PMSG_EVENT_REMAIN_TIME_RECV*)lpMsg,aIndex);							break;
		case 0x9A: gBloodCastle.CGBloodCastleEnterRecv((PMSG_BLOOD_CASTLE_ENTER_RECV*)lpMsg,aIndex);			break;
		case 0xA0: gQuest.CGQuestInfoRecv(aIndex);																break;
		case 0xA2: gQuest.CGQuestStateRecv((PMSG_QUEST_STATE_RECV*)lpMsg,aIndex);								break;
		case 0xA7: CGPetItemCommandRecv((PMSG_PET_ITEM_COMMAND_RECV*)lpMsg,aIndex);								break;
		case 0xA9: CGPetItemInfoRecv((PMSG_PET_ITEM_INFO_RECV*)lpMsg,aIndex);									break;

		case 0xAA:
		switch(lpMsg[3])
		{
			case 0x01: gDuel.CGDuelStartRecv((PMSG_DUEL_START_RECV*)lpMsg,aIndex);							break;
			case 0x02: gDuel.CGDuelOkRecv((PMSG_DUEL_OK_RECV*)lpMsg,aIndex);								break;
			case 0x03: break;
			case 0x07: gDuel.CGDuelWatchRecv((PMSG_DUEL_WATCH_RECV*)lpMsg,aIndex);							break;
			case 0x09: gDuel.CGDuelButtonRecv((PMSG_DUEL_BUTTON_RECV*)lpMsg,aIndex);						break;
		}
		break;

		case 0xAB: break;
		case 0xAC: break;
		case 0xAE: gHelper.CGHelperDataRecv((PMSG_HELPER_DATA_RECV*)lpMsg,aIndex);								break;

		case 0xAF:
		switch(lpMsg[3])
		{
			case 0x01: gChaosCastle.CGChaosCastleEnterRecv((PMSG_CHAOS_CASTLE_ENTER_RECV*)lpMsg,aIndex);		break;
			case 0x02: gChaosCastle.CGChaosCastlePositionRecv((PMSG_CHAOS_CASTLE_POSITION_RECV*)lpMsg,aIndex);	break;
		}
		break;

		case 0xB0: gSkillManager.CGSkillTeleportAllyRecv((PMSG_SKILL_TELEPORT_ALLY_RECV*)lpMsg,aIndex);			break;

		case 0xB1:
		switch(lpMsg[3])
		{
			case 0x01: CGMapServerMoveAuthRecv((PMSG_MAP_SERVER_MOVE_AUTH_RECV*)lpMsg,aIndex);					break;
		}
		break;

		case 0xB2:
		switch(lpMsg[3])
		{
			case 0x00: CGReqCastleSiegeState((PMSG_REQ_CASTLESIEGESTATE*)lpMsg,aIndex);			break;
			case 0x01: CGReqRegCastleSiege((PMSG_REQ_REGCASTLESIEGE*)lpMsg,aIndex);				break;
			case 0x02: CGReqGiveUpCastleSiege((PMSG_REQ_GIVEUPCASTLESIEGE*)lpMsg,aIndex);		break;
			case 0x03: CGReqGuildRegInfo((PMSG_REQ_GUILDREGINFO*)lpMsg,aIndex);					break;
			case 0x04: CGReqRegGuildMark((PMSG_REQ_REGGUILDMARK*)lpMsg,aIndex);					break;
			case 0x05: CGReqNpcBuy((PMSG_REQ_NPCBUY*)lpMsg,aIndex);								break;
			case 0x06: CGReqNpcRepair((PMSG_REQ_NPCREPAIR*)lpMsg,aIndex);						break;
			case 0x07: CGReqNpcUpgrade((PMSG_REQ_NPCUPGRADE*)lpMsg,aIndex);						break;
			case 0x08: CGReqTaxMoneyInfo((PMSG_REQ_TAXMONEYINFO*)lpMsg,aIndex);					break;
			case 0x09: CGReqTaxRateChange((PMSG_REQ_TAXRATECHANGE*)lpMsg,aIndex);				break;
			case 0x10: CGReqMoneyDrawOut((PMSG_REQ_MONEYDRAWOUT*)lpMsg,aIndex);					break;
			case 0x12: CGReqCsGateOperate((PMSG_REQ_CSGATEOPERATE*)lpMsg,aIndex);				break;
			case 0x1B: CGReqCsMiniMapData((PMSG_REQ_MINIMAPDATA*)lpMsg,aIndex);					break;
			case 0x1C: CGReqStopCsMiniMapData((PMSG_REQ_STOPMINIMAPDATA*)lpMsg,aIndex);			break;
			case 0x1D: CGReqCsSendCommand((PMSG_REQ_CSCOMMAND*)lpMsg,aIndex);					break;
			case 0x1F: CGReqCsSetEnterHuntZone((PMSG_REQ_CSHUNTZONEENTER*)lpMsg,aIndex);		break;
		}
		break;

		case 0xB3: CGReqNpcDbList((PMSG_REQ_NPCDBLIST*)lpMsg,aIndex);							break;
		case 0xB4: CGReqCsRegGuildList((PMSG_REQ_CSREGGUILDLIST*)lpMsg,aIndex);					break;
		case 0xB5: CGReqCsAttkGuildList((PMSG_REQ_CSATTKGUILDLIST*)lpMsg,aIndex);				break;

		case 0xB7:
		switch(lpMsg[3])
		{
			case 0x01: gCastleSiegeWeapon.CGCastleSiegeWeaponUseRecv((PMSG_CASTLE_SIEGE_WEAPON_USE_RECV*)lpMsg,aIndex);					break;
			case 0x04: gCastleSiegeWeapon.CGCastleSiegeWeaponDamageRecv((PMSG_CASTLE_SIEGE_WEAPON_DAMAGE_RECV*)lpMsg,aIndex);			break;
		}
		break;

		case 0xB9:
		switch(lpMsg[3])
		{
			case 0x02: CGReqGuildMarkOfCastleOwner((PMSG_REQ_GUILDMARK_OF_CASTLEOWNER*)lpMsg,aIndex);					break;
			case 0x05: CGReqCastleHuntZoneEntrance((PMSG_REQ_MOVE_TO_CASTLE_HUNTZONE*)lpMsg,aIndex);					break;
		}
		break;

		case 0xBC:
		switch(lpMsg[3])
		{
			case 0x00: gJewelMix.CGJewelMixRecv((PMSG_JEWEL_MIX_RECV*)lpMsg,aIndex);							break;
			case 0x01: gJewelMix.CGJewelUnMixRecv((PMSG_JEWEL_UNMIX_RECV*)lpMsg,aIndex);						break;
		}
		break;

		case 0xBD:
		switch(lpMsg[3])
		{
			case 0x00: gCrywolf.CGCrywolfInfoRecv(aIndex);														break;
			case 0x03: gCrywolf.CGCrywolfAltarContractRecv((PMSG_CRYWOLF_ALTAR_CONTRACT_RECV*)lpMsg,aIndex);	break;
			case 0x09: gCrywolf.CGCrywolfChaosRateRecv(aIndex);													break;
		}
		break;

		case 0xBF:
		switch(lpMsg[3])
		{
			case 0x00: gIllusionTemple.CGIllusionTempleEnterRecv((PMSG_ILLUSION_TEMPLE_ENTER_RECV*)lpMsg,aIndex);	break;
			case 0x02: gIllusionTemple.CGIllusionTempleSkillRecv((PMSG_ILLUSION_TEMPLE_SKILL_RECV*)lpMsg,aIndex);	break;
			case 0x05: gIllusionTemple.CGIllusionTempleButtonClickRecv(aIndex);										break;
			case 0x0B: gLuckyCoin.CGLuckyCoinCountRecv((PMSG_LUCKY_COIN_COUNT_RECV*)lpMsg,aIndex);					break;
			case 0x0C: gLuckyCoin.CGLuckyCoinRegisterRecv((PMSG_LUCKY_COIN_REGISTER_RECV*)lpMsg,aIndex);			break;
			case 0x0D: gLuckyCoin.CGLuckyCoinExchangeRecv((PMSG_LUCKY_COIN_EXCHANGE_RECV*)lpMsg,aIndex);			break;
			case 0x0E: gDoubleGoer.CGDoubleGoerEnterRecv((PMSG_DOUBLE_GOER_ENTER_RECV*)lpMsg,aIndex);				break;
			case 0x17: gNpcTalk.CGNpcJuliaRecv(aIndex);																break;
			case 0x20: gInventoryEquipment.CGInventoryEquipmentRecv((PMSG_INVENTORY_EQUIPMENT_RECV*)lpMsg,aIndex);	break;
			case 0x51: gHelper.CGHelperStartRecv((PMSG_HELPER_START_RECV*)lpMsg,aIndex);							break;
			case 0x70: break;
			default:
			LogAdd(LOG_RED, "[0xBF] Unknown packet BF:%d",lpMsg[3]);
			break;
		}
		break;

		case 0xC0: FriendListRequest(aIndex);											break;
		case 0xC1: FriendAddRequest((PMSG_FRIEND_ADD_REQ*)lpMsg,aIndex);				break;
		case 0xC2: WaitFriendAddRequest((PMSG_FRIEND_ADD_SIN_RESULT*)lpMsg,aIndex);		break;
		case 0xC3: FriendDelRequest((PMSG_FRIEND_DEL_REQ*)lpMsg,aIndex);				break;
		case 0xC4: FriendStateClientRecv((PMSG_FRIEND_STATE_C*)lpMsg,aIndex);			break;
		case 0xC5: FriendMemoSend((PMSG_FRIEND_MEMO*)lpMsg,aIndex);						break;
		case 0xC7: FriendMemoReadReq((PMSG_FRIEND_READ_MEMO_REQ*)lpMsg,aIndex);			break;
		case 0xC8: FriendMemoDelReq((PMSG_FRIEND_MEMO_DEL_REQ*)lpMsg,aIndex);			break;
		case 0xC9: FriendMemoListReq(aIndex);											break;
		case 0xCA: FriendChatRoomCreateReq((PMSG_FRIEND_ROOMCREATE_REQ*)lpMsg,aIndex);	break;
		case 0xCB: FriendRoomInvitationReq((PMSG_ROOM_INVITATION*)lpMsg,aIndex);		break;

		case 0xD0:
		switch(lpMsg[3])
		{
			case 0x05: break;
			case 0x06: break;
			case 0x07: gQuest.CGQuestNpcWarewolfRecv(aIndex);					break;
			case 0x08: gQuest.CGQuestNpcKeeperRecv(aIndex);						break;
			case 0x09: gNpcTalk.CGNpcLeoTheHelperRecv(aIndex);					break;
			case 0x0A: gNpcTalk.CGNpcSnowmanRecv(aIndex);						break;
			case 0x10: gNpcTalk.CGNpcSantaClausRecv(aIndex);					break;
		}
		break;

		case 0xD1:
		switch(lpMsg[3])
		{
			case 0x00: gKanturuEntranceNPC.CGKanturuEnterInfoRecv(aIndex);		break;
			case 0x01: gKanturuEntranceNPC.CGKanturuEnterRecv(aIndex);			break;
		}
		break;

		case 0xD2:
		switch(lpMsg[3])
		{
			case 0x01: gCashShop.CGCashShopPointRecv(aIndex);															break;
			case 0x02: gCashShop.CGCashShopOpenRecv((PMSG_CASH_SHOP_OPEN_RECV*)lpMsg,aIndex);							break;
			case 0x03: gCashShop.CGCashShopItemBuyRecv((PMSG_CASH_SHOP_ITEM_BUY_RECV*)lpMsg,aIndex);					break;
			case 0x04: gCashShop.CGCashShopItemGifRecv((PMSG_CASH_SHOP_ITEM_GIF_RECV*)lpMsg,aIndex);					break;
			case 0x05: gCashShop.CGCashShopItemNumRecv((PMSG_CASH_SHOP_ITEM_NUM_RECV*)lpMsg,aIndex);					break;
			case 0x0B: gCashShop.CGCashShopItemUseRecv((PMSG_CASH_SHOP_ITEM_USE_RECV*)lpMsg,aIndex);					break;
		}
		break;

		case 0xD5:
		switch (((lpMsg[0] == 0xC1) ? lpMsg[3] : lpMsg[4]))
		{
			#if(B_HON_HOAN)	
			case 0x01:
			{
				XULY_HONHOAN* mlpMsg = (XULY_HONHOAN*)lpMsg;
				if (!mlpMsg->ThaoTac) { return; }
				gBHonHoan.SendInfoClient(aIndex);
			}
			break;
			case 0x02:
			{
				XULY_HONHOAN* mlpMsg = (XULY_HONHOAN*)lpMsg;
				if (!mlpMsg->ThaoTac) { return; }
				gBHonHoan.NangCapHonHoan(aIndex);
			}
			break;
			#endif
			#if TU_LUYEN_NEW
			case 0x03:
			{
				XULY_TULUYEN* mlpMsg = (XULY_TULUYEN*)lpMsg;
				if (!mlpMsg->ThaoTac) { return; }
				gSPKTuLuyen.SendInfoClient(aIndex);
			}
			break;
			case 0x04:
			{
				XULY_TULUYEN* mlpMsg = (XULY_TULUYEN*)lpMsg;
				if (!mlpMsg->ThaoTac) { return; }
				gSPKTuLuyen.NangCapTuLuyen(aIndex);
			}
			break;	
			#endif
			#if(DANH_HIEU_NEW)
			case 0x05:
			{
				XULY_DANHHIEU* mlpMsg = (XULY_DANHHIEU*)lpMsg;
				if (!mlpMsg->ThaoTac) { return; }
				gSPKDanhHieu.SendInfoClient(aIndex);
			}
			break;
			case 0x06:
			{
				XULY_DANHHIEU* mlpMsg = (XULY_DANHHIEU*)lpMsg;
				if (!mlpMsg->ThaoTac) { return; }
				gSPKDanhHieu.NangCapDanhHieu(aIndex);
			}
			break;
			#endif
			#if(QUAN_HAM_NEW)
			case 0x07:
			{
				XULY_QUANHAM* mlpMsg = (XULY_QUANHAM*)lpMsg;
				if (!mlpMsg->ThaoTac) { return; }
				gSPKQuanHam.SendInfoClient(aIndex);
			}
			break;
			case 0x08:
			{
				XULY_QUANHAM* mlpMsg = (XULY_QUANHAM*)lpMsg;
				if (!mlpMsg->ThaoTac) { return; }
				gSPKQuanHam.NangCapQuanHam(aIndex);
			}
			break;
			#endif
			#if(NEW_VIP_NEW)
			case 0x09:
			{
				XULY_NEWVIP* mlpMsg = (XULY_NEWVIP*)lpMsg;
				if (!mlpMsg->ThaoTac) { return; }
				gSPKNewVip.SendInfoClient(aIndex);
			}
			break;
			case 0x0A:
			{
				XULY_NEWVIP* mlpMsg = (XULY_NEWVIP*)lpMsg;
				if (!mlpMsg->ThaoTac) { return; }
				gSPKNewVip.NangCapNewVip(aIndex);
			}
			break;
			#endif
			#if NEWXSHOP
			case 0x11:  gNewCashShop.GsSendPackSize11(lpMsg, aIndex);	break;
			case 0x12:	gNewCashShop.GsSendPackSize12(lpMsg, aIndex);	break;
			case 0x13:	gNewCashShop.GsSendPackSize13(lpMsg, aIndex);	break;
			#endif

			#if CUSTOM_HARMONY
			case 0x2A:
			{
				XULY_CGPACKET_CH* Recv = (XULY_CGPACKET_CH*)lpMsg;
				if (Recv->ThaoTac != 111) return;
				gCustomHarmony.SetStateInterface(lpObj->Index, 0);
			}
			break;
			case 0x2B:
			{
				PMSG_MOVEITEM* Recv = (PMSG_MOVEITEM*)lpMsg;
				gCustomHarmony.ProcItemSend(aIndex, Recv->Source);
			}

			break;
			case 0x2C:
			{
				XULY_CGPACKET_CH* Recv = (XULY_CGPACKET_CH*)lpMsg;
				if (Recv->ThaoTac == -1) return;
				gCustomHarmony.BackItem(lpObj->Index, Recv->ThaoTac);
			}
			break;
			case 0x2D:
			{
				XULY_CGPACKET_CH* Recv = (XULY_CGPACKET_CH*)lpMsg;
				if (Recv->ThaoTac != 113) return;
				gCustomHarmony.ProcMix(lpObj->Index);
			}
			break;
			#endif
		}
		break;

		case PROTOCOL_CODE1:	CGMoveRecv((PMSG_MOVE_RECV*)lpMsg,aIndex);													break;
		case PROTOCOL_CODE4:	gSkillManager.CGMultiSkillAttackRecv((PMSG_MULTI_SKILL_ATTACK_RECV*)lpMsg,aIndex,0);		break;
		case 0xE1:				gGuild.CGGuildAssignStatusRecv((PMSG_GUILD_ASSIGN_STATUS_RECV*)lpMsg,aIndex);				break;
		case 0xE2:				CGGuildAssignType((PMSG_GUILD_ASSIGN_TYPE_REQ*)lpMsg,aIndex);								break;
		case 0xE5:				CGRelationShipReqJoinBreakOff((PMSG_RELATIONSHIP_JOIN_BREAKOFF_REQ*)lpMsg,aIndex);			break;
		case 0xE6:				CGRelationShipAnsJoinBreakOff((PMSG_RELATIONSHIP_JOIN_BREAKOFF_ANS*)lpMsg,aIndex);			break;

		case 0xE7:
		switch(lpMsg[3])
		{
			case 0x01: break;
			case 0x02: break;
			case 0x03: break;
		}
		break;

		case 0xE9: CGUnionList((PMSG_UNIONLIST_REQ*)lpMsg,aIndex);			break;

		case 0xEB:
		switch(lpMsg[3])
		{
			case 0x01: CGRelationShipReqKickOutUnionMember((PMSG_KICKOUT_UNIONMEMBER_REQ*)lpMsg,aIndex);					break;
		}
		break;

		case 0xEC:
		switch(lpMsg[3])
		{
			case 0x00: break;
			case 0x01: break;
			case 0x02: break;
			case 0x03: break;
			case 0x31: break;
			case 0x33: break;
		}
		break;

		case 0xED:
		switch(lpMsg[3])
		{
			case 0x00: break;
			case 0x01: break;
			case 0x02: break;
			case 0x03: break;
			case 0x04: break;
			case 0x05: break;
			case 0x06: break;
			case 0x07: break;
			case 0x08: break;
		}
		break;

		case 0xEE: g_RuudTokenSellTemplate._RecvRuudBUY((PMSG_BUY_ITEMRUUD_RECV*)lpMsg,aIndex);			break;

		case 0xEF:
		switch(lpMsg[3])
		{
			case 0x00: break;
			case 0x01: break;
			case 0x02: break;
			case 0x03: break;
			case 0x04: break;
			case 0x05: break;
			case 0x06: break;
		}
		break;

		case 0xF1:
		switch(lpMsg[3])
		{
			case 0x01:	CGConnectAccountRecv((PMSG_CONNECT_ACCOUNT_RECV*)lpMsg,aIndex);		break;
			case 0x02:	CGCloseClientRecv((PMSG_CLOSE_CLIENT_RECV*)lpMsg,aIndex);			break;
			#if(REGISTER_INGAME)
			case 0x12:	CGRegisterAccountRecv((PMSG_REGISTER_ACCOUNT_RECV*)lpMsg, aIndex);	return;
			#endif
		}
		break;

		#if(SOIITEM)
		case 0x4F:
		switch (lpMsg[3])
		{
			case 0x03: gItemManager.GCItemListViewSend((PMSG_VIEW_REQUEST_RECV*)lpMsg, aIndex);			break;
		}
		break;
		#endif

		case 0xF3:
		switch(lpMsg[3])
		{				
#if BAUCUA
		case 0xF9:
			gCustomBauCua.XuLyBauCua((PSBMSG_BauCua_REVC*)lpMsg, aIndex);
			break;
#endif
			#if(NGAN_HANG_NGOC)
			case 0xF5: gCustomJewelBank.JewelBankRecv((PSBMSG_JEWELBANK_RECV*)lpMsg, aIndex);					break;
			case 0xF6: gCustomJewelBank.JewelBankWithDrawRecv((PSBMSG_JEWELBANKWITHDRAW_RECV*)lpMsg, aIndex);	break;
			#endif
			case 0x00: CGCharacterListRecv(aIndex); 												break;
			case 0x01: CGCharacterCreateRecv((PMSG_CHARACTER_CREATE_RECV*)lpMsg,aIndex);			break;
			case 0x02: CGCharacterDeleteRecv((PMSG_CHARACTER_DELETE_RECV*)lpMsg,aIndex);			break;
			case 0x03: CGCharacterInfoRecv((PMSG_CHARACTER_INFO_RECV*)lpMsg,aIndex);				break;
			case 0x06: CGLevelUpPointRecv((PMSG_LEVEL_UP_POINT_RECV*)lpMsg,aIndex);					break;
			case 0x12: CGCharacterMoveViewportEnableRecv(aIndex);									break;
			case 0x15: CGCharacterNameCheckRecv((PMSG_CHARACTER_NAME_CHECK_RECV*)lpMsg,aIndex);		break;
			case 0x16: CGCharacterNameChangeRecv((PMSG_CHARACTER_NAME_CHANGE_RECV*)lpMsg,aIndex);	break;
			case 0x21: break;
			case 0x30: CGOptionDataRecv((PMSG_OPTION_DATA_RECV*)lpMsg,aIndex);						break;
			case 0x31: break;
			//tue add
#if(CHIEN_TRUONG_CO)
			case 0x38: //Xu Ly Packet CTC
				gChienTruongCo.CGPacketCTC((XULY_CGPACKET*)lpMsg, aIndex);
				break;
#endif
			//------
			#if(CTCMINI)
			case 0x39:	gCTCMini.CGPacketCTCMini((CTCMINI_CGPACKET*)lpMsg, aIndex);					break;
			#endif
			case 0x52: gMasterSkillTree.CGMasterSkillRecv((PMSG_MASTER_SKILL_RECV*)lpMsg,aIndex);	break;
			#if(BAN_RS)
			case 0xD9:					gResetChange.ResetChange(aIndex, (RESETCHANGE_REQ*)lpMsg);	break;
			#endif
			case 0xE8: gCustomEventTime.GCReqEventTime(aIndex,(PMSG_CUSTOM_EVENTTIME_RECV*)lpMsg);	break;
			case 0xE9: gHwidManager.ConnectHwid((CG_HWID_SEND*)lpMsg, &gObj[aIndex]);				break;
			case 0xEB: gCustomStore.CGOffTradeRecv((PMSG_OFFTRADE_RECV*)lpMsg, aIndex);				break;
			case 0xED: gItemManager.CGItemBuyConfirmRecv((PMSG_ITEM_BUY_NEW*)lpMsg,aIndex);			break;

			#if(PARTYSEARCH==1)
			case 0xF1: gPartySearch.PartySearchAddToList((PMSG_RECV_PARTYSEARCH_ADD*)lpMsg, aIndex);	break;
			case 0xF2: gPartySearch.RequestParty((PMSG_PARTY_REQ_REQ*)lpMsg, aIndex);					break;
			case 0x25: gPartySearch.GCPartyListSend(aIndex);											break;
            #endif
			case 0x27: gCustomMoveEvent.MOVE_EVENT(lpObj, (MOVE_EVENT_REQ*)lpMsg, aIndex);				break;
		}
		break;

		case 0xF6:
		switch(lpMsg[3])
		{
			case 0x0A: gQuestWorld.CGQuestWorldDialogRecv((PMSG_QUEST_WORLD_DIALOG_RECV*)lpMsg,aIndex);		break;
			case 0x0B: gQuestWorld.CGQuestWorldAcceptRecv((PMSG_QUEST_WORLD_ACCEPT_RECV*)lpMsg,aIndex);		break;
			case 0x0D: gQuestWorld.CGQuestWorldFinishRecv((PMSG_QUEST_WORLD_FINISH_RECV*)lpMsg,aIndex);		break;
			case 0x0F: gQuestWorld.CGQuestWorldCancelRecv((PMSG_QUEST_WORLD_CANCEL_RECV*)lpMsg,aIndex);		break;
			case 0x10: gQuestWorld.CGQuestWorldButtonRecv((PMSG_QUEST_WORLD_BUTTON_RECV*)lpMsg,aIndex);		break;
			case 0x1B: gQuestWorld.CGQuestWorldDetailRecv((PMSG_QUEST_WORLD_DETAIL_RECV*)lpMsg,aIndex);		break;
			case 0x30: gQuestWorld.CGQuestWorldListRecv(aIndex);											break;
			case 0x31: gQuestWorld.CGQuestWorldNpcElfBufferRecv(aIndex);									break;
		}
		break;

		case 0xF7:
		switch(lpMsg[3])
		{
			case 0x01: gImperialGuardian.CGImperialGuardianEnterRecv((PMSG_IMPERIAL_GUARDIAN_ENTER_RECV*)lpMsg,aIndex);					break;
			#if SO_LUONG_BOSS
			case 0x02:	gActiveInvasions.send_list_to_client(aIndex);	break;
			#endif
		}
		break;

		case 0xF8:
		switch(lpMsg[3])
		{
			case 0x01: gGensSystem.CGGensSystemInsertRecv((PMSG_GENS_SYSTEM_INSERT_RECV*)lpMsg,aIndex);					break;
			case 0x03: gGensSystem.CGGensSystemDeleteRecv((PMSG_GENS_SYSTEM_DELETE_RECV*)lpMsg,aIndex);					break;
			case 0x09: gGensSystem.CGGensSystemRewardRecv((PMSG_GENS_SYSTEM_REWARD_RECV*)lpMsg,aIndex);					break;
			case 0x0B: gGensSystem.CGGensSystemMemberRecv((PMSG_GENS_SYSTEM_MEMBER_RECV*)lpMsg,aIndex);					break;
			case 0x20: break;
			case 0x30: break;
			case 0x32: break;
			case 0x34: break;
			case 0x36: break;
			case 0x3C: break;
			case 0x41: break;
			case 0x43: break;
			case 0x45: break;
			case 0x4B: break;
			#if(CUSTOM_GUILD_UPGRADE == TRUE)
			case 0x4C: GuildUpgrade.RecvRequest((PMSG_GUILD_UPGRADE_REQUEST_RECV*)lpMsg, aIndex);						break;
			#endif
		}
		break;	

		case 0xFC:
		switch (lpMsg[3])
		{	
			#if(VONGQUAY_NEW)
			case 0x16:	gLuckySpin.StartRoll(lpObj);	break;
			case 0x17:	gLuckySpin.GCItemSend(aIndex);	break;
			#endif
			#if(ADD_POINT)
			case 101: gCommandManager.CommandReAdd(&gObj[aIndex],		"",		0);	break;
			#endif
			case 102: gCommandManager.CommandResetAuto(&gObj[aIndex], "auto", 0);	break;
			case 103: gAUTOHP.CommandHp(lpObj); (&gObj[aIndex], "", 0);	break;
			case 104: gAddBuffer.CommandAddBuff(&gObj[aIndex]);	break;
			#if RELIFE
			case 0x08: gTaiSinh.RunRelifeToProc(lpObj, (RelifeRequest*)lpMsg);		break;
			#endif
		}
		break;
		//tue add
		//-- ping
		case 2712:
			GCSendPing(aIndex);
			break;
		//---
		case 0xFF:
		switch (((lpMsg[0] == 0xC1) ? lpMsg[3] : lpMsg[4]))
		{
			#if(ADD_POINT)
			case 0x0C: CGStatAddRecv((PMSG_UPDATE_STATS_RECV*)lpMsg, aIndex);			break;
			#endif
			#if CHANGE_PASS
			case 0x91:	CSEND_CHANGEPASS((CSENDGS_DOIMK*) lpMsg, aIndex);				break;
			#endif
			#if(THANMA)
			case 0x92:	gThanMaChien.CGOpenWindowNPC((XULY_CGPACKET1*)lpMsg, aIndex);	break;
			#endif
		}
		break;

		case 0x78:	GCPostItemProc((PMSG_POST_ITEM_RECV*)lpMsg,aIndex); 	break;
		case 0x79:
		switch (lpMsg[3])
		{
			case 0x01:	gItemManager.CGMoveItemProc((PMSG_MOVEITEM*)lpMsg, aIndex);	break;
		}
		break;

		case 0x7A:
		switch (((lpMsg[0] == 0xC1) ? lpMsg[3] : lpMsg[4]))
		{
			case 0x01:	gRanking.SendUser(lpObj);			break;
			case 0x02:	gRanking.SendUserTP(lpObj);			break;
			case 0x03:	gRanking.SendUserET(lpObj);			break;
			case 0x04:	gRanking.SendUserTG(lpObj);			break;
			case 0x05:	gRanking.SendUserPK(lpObj);			break;
			case 0x06:	gRanking.SendUserMT(lpObj);			break;
		}
		break;
		
		case 0xD3:
		switch (((lpMsg[0] == 0xC1) ? lpMsg[3] : lpMsg[4]))
		{
			#if (SPK_ITEMLOCK)
			case 0x1A:
			{
				CGPACKET_LOCKWINDOW* Recv = (CGPACKET_LOCKWINDOW*)lpMsg;
				if (Recv->InPass != 1234) return;
				SendStatusLockItem(aIndex);

			}
			break;
			case 0x1B:
			{
				SendLockUnLock((CGPACKET_LOCKWINDOW*)lpMsg, aIndex);
			}
			break;
			#endif
			#if (SPK_DUNGLUYEN)
			case 0x2A:
			{
				XULY_CGPACKET_DL* Recv = (XULY_CGPACKET_DL*)lpMsg;
				if (Recv->ThaoTac != 111) return;
				gBDungLuyenItem.SetStateInterface(lpObj->Index, 0);
			}
			break;
			case 0x2B: //Send Item
			{
				PMSG_MOVEITEM* Recv = (PMSG_MOVEITEM*)lpMsg;
				gBDungLuyenItem.ProcItemSend(aIndex, Recv->Source);
			}

			break;
			case 0x2C: //Back Item
			{
				XULY_CGPACKET_DL* Recv = (XULY_CGPACKET_DL*)lpMsg;
				if (Recv->ThaoTac == -1) return;
				gBDungLuyenItem.BackItem(lpObj->Index, Recv->ThaoTac);
			}
			break;
			case 0x2D: //MIX
			{
				XULY_CGPACKET_DL* Recv = (XULY_CGPACKET_DL*)lpMsg;
				if (Recv->ThaoTac != 113) return;
				gBDungLuyenItem.ProcMix(lpObj->Index);
			}
			break;
			#endif
			case 0x40: gRanking.CGetInfoCharTop((REQUESTINFO_CHARTOP*)lpMsg, aIndex);			break;
			#if(B_MOCNAP)	
			case 0x9A: 
			{
				XULY_CGPITEMTRADE* mlpMsg = (XULY_CGPITEMTRADE*)lpMsg;
				if (!mlpMsg->ThaoTac) { return; }
				gCBMocNap.NhanThuongMocNap(aIndex, mlpMsg->ThaoTac);
			}
			break;
			case 0x9B:
			{
				XULY_CGPITEMTRADE* mlpMsg = (XULY_CGPITEMTRADE*)lpMsg;
				if (!mlpMsg->ThaoTac) { return; }
				gCBMocNap.SendListNhanThuong(aIndex, mlpMsg->ThaoTac);
			}
			break;
			case 0x9C: 
			{
				XULY_CGPITEMTRADE* mlpMsg = (XULY_CGPITEMTRADE*)lpMsg;
				if (!mlpMsg->ThaoTac) { return; }
				gCBMocNap.UserSendClientInfo(lpObj->Index);
			}
			break;
			#endif
			#if (SPK_ITEMTRADER)
			case 0x6A:
			{
				XULY_CGPITEMTRADE* Recv = (XULY_CGPITEMTRADE*)lpMsg;
				if (Recv->ThaoTac != 111) return;
				gGC_ItemTrader.SetStateInterface(lpObj->Index, 0);
			}
			break;
			case 0x6B: //Send Item
			{
				PMSG_MOVEITEM* Recv = (PMSG_MOVEITEM*)lpMsg;
				gGC_ItemTrader.ProcItemSend(aIndex, Recv->Source);
			}

			break;
			case 0x6C: //Back Item
			{
				XULY_CGPITEMTRADE* Recv = (XULY_CGPITEMTRADE*)lpMsg;
				if (Recv->ThaoTac == -1) return;
				gGC_ItemTrader.BackItem(lpObj->Index, Recv->ThaoTac);
			}
			break;
			#endif
			#if(CB_EventArenaPvP)
			case 0x1E: gCBArenaPVP.ClientRecvAction(lpMsg, aIndex); break;
			#endif
		}
		break;

		#if (CongHuongV2)
		case 0xFA:
		switch (((lpMsg[0] == 0xC1) ? lpMsg[3] : lpMsg[4]))
		{
			case 0x15: gCBCongHuong.CH_ClientSetItemGS(aIndex, (PMSG_MOVEITEM*)lpMsg);			break;
			case 0x16: 
			{
				XULY_CGPACKET1* mRecv = (XULY_CGPACKET1*)lpMsg;
				gCBCongHuong.CH_ClearDB(aIndex, mRecv->ThaoTac);
				gCBCongHuong.CH_GSSendInfoClient(aIndex);
			}
			break;
			case 0x17: gCBCongHuong.CH_MixItemGS(aIndex, (XULY_CGPACKET1*)lpMsg);			break;
		}
		break;
		#endif
	}
	PROTECT_FINAL
}

void CGChatRecv(PMSG_CHAT_RECV* lpMsg,int aIndex) // OK
{
	LPOBJ lpObj = &gObj[aIndex];

	if(gObjIsConnectedGP(aIndex) == 0)
	{
		return;
	}

	char name[11] = {0};
	memcpy(name,lpMsg->name,sizeof(lpMsg->name));
	if(strcmp(name,lpObj->Name) != 0)
	{
		return;
	}

	lpMsg->message[(sizeof(lpMsg->message)-1)] = 0;
	gLog.Output(LOG_CHAT,"[General][%s][%s] - (Message: %s)",lpObj->Account,lpObj->Name,lpMsg->message);
	if(lpMsg->message[0] == '/')
	{
		if(gCommandManager.ManagementCore(&gObj[aIndex],lpMsg->message,-1) == 1)
		{
			return;
		}
		else if(gCustomCommandDescription.GetInfoByName(lpObj,lpMsg->message) != 0)
		{
			return;
		}
		else
		{
			CUSTOMMOVE_INFO CustomMoveInfo;

			if(gCustomMove.GetInfoByName(lpMsg->message,&CustomMoveInfo) != 0)
			{
				gCustomMove.GetMove(lpObj,CustomMoveInfo.Index);
			}
			return;
		}
	}

	gFilter.CheckSyntax(lpMsg->message);
	if(lpObj->ChatLimitTime > 0)
	{
		gNotice.GCNoticeSend(aIndex,1,0,0,0,0,0,gMessage.GetMessage(269),lpObj->ChatLimitTime);
		return;
	}
	if((lpObj->Penalty & 2) != 0)
	{
		return;
	}

	PMSG_CHAT_SEND pMsg;
	pMsg.header.set(0x00,sizeof(pMsg));
	memcpy(pMsg.name,lpMsg->name,sizeof(pMsg.name));
	memcpy(pMsg.message,lpMsg->message,sizeof(pMsg.message));
	if(lpMsg->message[0] == '~')
	{
		if(OBJECT_RANGE(lpObj->PartyNumber) != 0)
		{
			for(int n=0;n < MAX_PARTY_USER;n++)
			{
				if(OBJECT_RANGE(gParty.m_PartyInfo[lpObj->PartyNumber].Index[n]) != 0)
				{
					DataSend(gParty.m_PartyInfo[lpObj->PartyNumber].Index[n],(BYTE*)&pMsg,pMsg.header.size);
				}
			}
		}
	}
	else if(lpMsg->message[0] == '@')
	{
		if(lpObj->Guild != 0)
		{
			if(lpMsg->message[1] == '>')
			{
				if(strcmp(lpObj->Name,lpObj->Guild->Names[0]) == 0)
				{
					GDGuildNoticeSave(lpObj->Guild->Name,&lpMsg->message[2]);
				}
			}
			else if(lpMsg->message[1] == '@')
			{
				if(lpObj->Guild->GuildUnion != 0)
				{
					GDUnionServerGroupChattingSend(lpObj->Guild->GuildUnion,lpMsg->name,lpMsg->message);
				}
			}
			else
			{
				if(lpObj->Guild->Number != 0)
				{
					GDGuildServerGroupChattingSend(lpObj->Guild->Number,lpMsg->name,lpMsg->message);
				}
			}
		}
	}
	else if(lpMsg->message[0] == '$')
	{
		if(lpObj->GensFamily != 0)
		{
			for(int n=OBJECT_START_USER;n < MAX_OBJECT;n++)
			{
				if(gObj[n].GensFamily == lpObj->GensFamily)
				{
					DataSend(n,(BYTE*)&pMsg,pMsg.header.size);
				}
			}
		}
	}
	else
	{
		DataSend(aIndex,(BYTE*)&pMsg,pMsg.header.size);
		MsgSendV2(lpObj,(BYTE*)&pMsg,pMsg.header.size);
	}
}

void CGChatWhisperRecv(PMSG_CHAT_WHISPER_RECV* lpMsg,int aIndex) // OK
{
	LPOBJ lpObj = &gObj[aIndex];

	if(gObjIsConnectedGP(aIndex) == 0)
	{
		return;
	}
	char name[11] = {0};
	memcpy(name,lpMsg->name,sizeof(lpMsg->name));
	if(lpObj->ChatLimitTime > 0)
	{
		gNotice.GCNoticeSend(aIndex,1,0,0,0,0,0,gMessage.GetMessage(269),lpObj->ChatLimitTime);
		return;
	}
	if((lpObj->Penalty & 2) != 0)
	{
		return;
	}
	LPOBJ lpTarget = gObjFind(name);
	if(lpTarget == 0)
	{
		GDGlobalWhisperSend(aIndex,name,lpMsg->message);
		return;
	}
	if(aIndex == lpTarget->Index)
	{
		gNotice.GCNoticeSend(aIndex,1,0,0,0,0,0,gMessage.GetMessage(270));
		return;
	}
	lpMsg->message[(sizeof(lpMsg->message)-1)] = 0;
	gLog.Output(LOG_CHAT,"[Whisper][%s][%s] - (Name: %s, Message: %s)",lpObj->Account,lpObj->Name,name,lpMsg->message);
	gFilter.CheckSyntax(lpMsg->message);
	#if(PARTYSETPASS)
	PMSG_PARTY_SETPASS_REQ_REQ_SETPASS pMsg;
	memcpy(pMsg.Name, lpTarget->Name, sizeof(pMsg.Name));
	memcpy(pMsg.Password, lpMsg->message, sizeof(pMsg.Password));
	gPartySetPass.RequestParty(pMsg, aIndex);
	#endif
	GCChatWhisperSend(lpTarget->Index,lpObj->Name,lpMsg->message);
}

void CGMainCheckRecv(PMSG_MAIN_CHECK_RECV* lpMsg,int aIndex) // OK
{
	LPOBJ lpObj = &gObj[aIndex];
	if(gServerInfo.m_MainChecksum != 0)
	{
		if(lpObj->CheckSumTableNum < 0 || lpObj->CheckSumTableNum >= MAX_CHECKSUM_KEY)
		{
			gLog.Output(LOG_HACK,"[%s][%s] Main CheckSum error",lpObj->Account,lpObj->Name);
			GCCloseClientSend(aIndex,0);
			return;
		}
		if(gCheckSum[lpObj->CheckSumTableNum] != lpMsg->key)
		{
			gLog.Output(LOG_HACK,"[%s][%s] Invalid Main CheckSum",lpObj->Account,lpObj->Name);
			GCCloseClientSend(aIndex,0);
			return;
		}
	}
	lpObj->CheckSumTime = 0;
}

void CGLiveClientRecv(PMSG_LIVE_CLIENT_RECV* lpMsg,int aIndex) // OK
{
	LPOBJ lpObj = &gObj[aIndex];
	if(lpObj->Connected != OBJECT_LOGGED && lpObj->Connected != OBJECT_ONLINE)
	{
		return;
	}

	if(gServerInfo.m_CheckLatencyHack != 0 && abs((int)((GetTickCount()-lpObj->ServerTickCount)-(lpMsg->TickCount-lpObj->ClientTickCount))) > gServerInfo.m_CheckLatencyHackTolerance)
	{
		gLog.Output(LOG_HACK,"[%s][%s] Client Latency Hack Detected [%d][%d]",lpObj->Account,lpObj->Name,(GetTickCount()-lpObj->ServerTickCount),(lpMsg->TickCount-lpObj->ClientTickCount));
		GCCloseClientSend(aIndex,0);
		return;
	}

	if(lpObj->Connected == OBJECT_ONLINE && gServerInfo.m_CheckSpeedHack != 0 && lpMsg->PhysiSpeed > lpObj->PhysiSpeed && abs((int)(lpObj->PhysiSpeed-lpMsg->PhysiSpeed)) > gServerInfo.m_CheckSpeedHackTolerance)
	{
		gLog.Output(LOG_HACK,"[%s][%s] Client Speed Hack Detected [%d][%d]",lpObj->Account,lpObj->Name,lpObj->PhysiSpeed,lpMsg->PhysiSpeed);
		GCCloseClientSend(aIndex,0);
		return;
	}

	if(lpObj->Connected == OBJECT_ONLINE && gServerInfo.m_CheckSpeedHack != 0 && lpMsg->MagicSpeed > lpObj->MagicSpeed && abs((int)(lpObj->MagicSpeed-lpMsg->MagicSpeed)) > gServerInfo.m_CheckSpeedHackTolerance)
	{
		gLog.Output(LOG_HACK,"[%s][%s] Client Speed Hack Detected [%d][%d]",lpObj->Account,lpObj->Name,lpObj->MagicSpeed,lpMsg->MagicSpeed);
		GCCloseClientSend(aIndex,0);
		return;
	}
	lpObj->ConnectTickCount = GetTickCount();
}

void CGPositionRecv(PMSG_POSITION_RECV* lpMsg,int aIndex) // OK
{
	LPOBJ lpObj = &gObj[aIndex];

	if(gObjIsConnectedGS(aIndex) == 0)
	{
		return;
	}

	if(lpObj->Teleport != 0)
	{
		return;
	}

	if(gObjCheckMapTile(lpObj,2) != 0)
	{
		return;
	}

	if(CC_MAP_RANGE(lpObj->Map) != 0 && (GetTickCount()-lpObj->ChaosCastleBlowTime) < 1000)
	{
		return;
	}

	if(lpMsg->x < (lpObj->X-15) || lpMsg->x > (lpObj->X+15) || lpMsg->y < (lpObj->Y-15) || lpMsg->y > (lpObj->Y+15))
	{
		return;
	}

	gMap[lpObj->Map].DelStandAttr(lpObj->OldX,lpObj->OldY);

	lpObj->X = lpMsg->x;
	lpObj->Y = lpMsg->y;
	lpObj->TX = lpMsg->x;
	lpObj->TY = lpMsg->y;
	lpObj->OldX = lpMsg->x;
	lpObj->OldY = lpMsg->y;
	gMap[lpObj->Map].SetStandAttr(lpObj->TX,lpObj->TY);
	PMSG_POSITION_SEND pMsg;
	pMsg.header.set(PROTOCOL_CODE3,sizeof(pMsg));
	pMsg.index[0] = SET_NUMBERHB(aIndex);
	pMsg.index[1] = SET_NUMBERLB(aIndex);
	pMsg.x = (BYTE)lpObj->TX;
	pMsg.y = (BYTE)lpObj->TY;
	if(lpObj->Type == OBJECT_USER)
	{
		DataSend(aIndex,(BYTE*)&pMsg,pMsg.header.size);
	}

	for(int n=0;n < MAX_VIEWPORT;n++)
	{
		if(lpObj->VpPlayer2[n].type == OBJECT_USER)
		{
			if(lpObj->VpPlayer2[n].state != OBJECT_EMPTY && lpObj->VpPlayer2[n].state != OBJECT_DIECMD && lpObj->VpPlayer2[n].state != OBJECT_DIED)
			{
				DataSend(lpObj->VpPlayer2[n].index,(BYTE*)&pMsg,pMsg.header.size);
			}
		}
	}
}

void CGActionRecv(PMSG_ACTION_RECV* lpMsg,int aIndex) // OK
{
	LPOBJ lpObj = &gObj[aIndex];

	if(gObjIsConnectedGP(aIndex) == 0)
	{
		return;
	}

	lpObj->Dir = lpMsg->dir;

	lpObj->ActionNumber = lpMsg->action;

	if(lpObj->ActionNumber == ACTION_SIT1)
	{
		lpObj->ViewState = 2;
		lpObj->Rest = lpObj->ActionNumber;
	}
	else if(lpObj->ActionNumber == ACTION_POSE1)
	{
		lpObj->ViewState = 3;
		lpObj->Rest = lpObj->ActionNumber;
	}
	else if(lpObj->ActionNumber == ACTION_HEALING1)
	{
		lpObj->ViewState = 4;
		lpObj->Rest = lpObj->ActionNumber;
	}

	PMSG_ACTION_SEND pMsg;
	pMsg.header.set(0x18,sizeof(pMsg));
	pMsg.index[0] = SET_NUMBERHB(aIndex);
	pMsg.index[1] = SET_NUMBERLB(aIndex);
	pMsg.dir = lpMsg->dir;
	pMsg.action = lpMsg->action;
	pMsg.target[0] = lpMsg->index[0];
	pMsg.target[1] = lpMsg->index[1];
	for(int n=0;n < MAX_VIEWPORT;n++)
	{
		if(lpObj->VpPlayer2[n].type == OBJECT_USER)
		{
			if(lpObj->VpPlayer2[n].state != OBJECT_EMPTY && lpObj->VpPlayer2[n].state != OBJECT_DIECMD && lpObj->VpPlayer2[n].state != OBJECT_DIED)
			{
				DataSend(lpObj->VpPlayer2[n].index,(BYTE*)&pMsg,pMsg.header.size);
			}
		}
	}
}

void CGEventRemainTimeRecv(PMSG_EVENT_REMAIN_TIME_RECV* lpMsg,int aIndex) // OK
{
	LPOBJ lpObj = &gObj[aIndex];

	if(gObjIsConnectedGP(aIndex) == 0)
	{
		return;
	}

	PMSG_EVENT_REMAIN_TIME_SEND pMsg;
	pMsg.header.set(0x91,sizeof(pMsg));
	pMsg.EventType = lpMsg->EventType;
	pMsg.RemainTimeH = 0;
	pMsg.EnteredUser = 0;
	pMsg.RemainTimeL = 0;
	if(lpMsg->EventType == 4)
	{
		lpMsg->ItemLevel = gChaosCastle.GetUserAbleLevel(&gObj[aIndex]);
	}
	else
	{
		lpMsg->ItemLevel = ((lpMsg->EventType==2)?lpMsg->ItemLevel:(lpMsg->ItemLevel-1));
	}

	switch(lpMsg->EventType)
	{
		case 1:
		if(DS_LEVEL_RANGE(lpMsg->ItemLevel) != 0)
		{
			if(gDevilSquare.GetState(lpMsg->ItemLevel) == DS_STATE_EMPTY)
			{
				if(gDevilSquare.GetEnterEnabled(lpMsg->ItemLevel) == 0)
				{
					pMsg.RemainTimeH = gDevilSquare.GetRemainTime(lpMsg->ItemLevel);
				}
				else
				{
					pMsg.EnteredUser = gDevilSquare.GetEnteredUserCount(lpMsg->ItemLevel);
				}
			}
			else
			{
				pMsg.RemainTimeH = gDevilSquare.GetRemainTime(lpMsg->ItemLevel);
			}
		}
		break;
		case 2:
		if(BC_LEVEL_RANGE(lpMsg->ItemLevel) != 0)
		{
			if(gBloodCastle.GetState(lpMsg->ItemLevel) == BC_STATE_EMPTY)
			{
				if(gBloodCastle.GetEnterEnabled(lpMsg->ItemLevel) == 0)
				{
					pMsg.RemainTimeH = gBloodCastle.GetRemainTime(lpMsg->ItemLevel);
				}
				else
				{
					pMsg.EnteredUser = gBloodCastle.GetEnteredUserCount(lpMsg->ItemLevel);
				}
			}
			else
			{
				pMsg.RemainTimeH = gBloodCastle.GetRemainTime(lpMsg->ItemLevel);
			}
		}
		break;
		case 4:
		if(CC_LEVEL_RANGE(lpMsg->ItemLevel) != 0)
		{
			if(gChaosCastle.GetState(lpMsg->ItemLevel) == CC_STATE_EMPTY)
			{
				if(gChaosCastle.GetEnterEnabled(lpMsg->ItemLevel) == 0)
				{
					pMsg.RemainTimeL = gChaosCastle.GetRemainTime(lpMsg->ItemLevel);
				}
				else
				{
					pMsg.EnteredUser = gChaosCastle.GetEnteredUserCount(lpMsg->ItemLevel);
				}
			}
			else
			{
				pMsg.RemainTimeL = gChaosCastle.GetRemainTime(lpMsg->ItemLevel);
			}
		}
		break;
		case 5:
		if(IT_LEVEL_RANGE(lpMsg->ItemLevel) != 0)
		{
			if(gIllusionTemple.GetState(lpMsg->ItemLevel) == IT_STATE_EMPTY)
			{
				if(gIllusionTemple.GetEnterEnabled(lpMsg->ItemLevel) == 0)
				{
					pMsg.RemainTimeH = gIllusionTemple.GetRemainTime(lpMsg->ItemLevel);
				}
				else
				{
					pMsg.EnteredUser = gIllusionTemple.GetEnteredUserCount(lpMsg->ItemLevel);
				}
			}
			else
			{
				pMsg.RemainTimeH = gIllusionTemple.GetRemainTime(lpMsg->ItemLevel);
			}
		}
		break;
	}
	DataSend(aIndex,(BYTE*)&pMsg,pMsg.header.size);
}

void CGPetItemCommandRecv(PMSG_PET_ITEM_COMMAND_RECV* lpMsg,int aIndex) // OK
{
	if(gObjIsConnectedGP(aIndex) == 0)
	{
		return;
	}

	if(lpMsg->type != 0)
	{
		return;
	}

	int bIndex = MAKE_NUMBERW(lpMsg->index[0],lpMsg->index[1]);
	gDarkSpirit[aIndex].ChangeCommand(lpMsg->command,bIndex);
}

void CGPetItemInfoRecv(PMSG_PET_ITEM_INFO_RECV* lpMsg,int aIndex) // OK
{
	LPOBJ lpObj = &gObj[aIndex];

	if(gObjIsConnectedGP(aIndex) == 0)
	{
		return;
	}

	if(lpMsg->type != 0 && lpMsg->type != 1)
	{
		return;
	}

	if(lpMsg->flag == 0) // Inventory
	{
		if(INVENTORY_RANGE(lpMsg->slot) == 0)
		{
			return;
		}

		GCPetItemInfoSend(aIndex,lpMsg->type,lpMsg->flag,lpMsg->slot,lpObj->Inventory[lpMsg->slot].m_PetItemLevel,lpObj->Inventory[lpMsg->slot].m_PetItemExp,(BYTE)lpObj->Inventory[lpMsg->slot].m_Durability);
	}
	else if(lpMsg->flag == 1) // Warehouse
	{
		if(WAREHOUSE_RANGE(lpMsg->slot) == 0)
		{
			return;
		}

		if(lpObj->LoadWarehouse == 0)
		{
			return;
		}

		GCPetItemInfoSend(aIndex,lpMsg->type,lpMsg->flag,lpMsg->slot,lpObj->Warehouse[lpMsg->slot].m_PetItemLevel,lpObj->Warehouse[lpMsg->slot].m_PetItemExp,(BYTE)lpObj->Warehouse[lpMsg->slot].m_Durability);
	}
	else if(lpMsg->flag == 2) // Trade
	{
		if(TRADE_RANGE(lpMsg->slot) == 0)
		{
			return;
		}

		if(lpObj->Interface.use == 0 || lpObj->Interface.type != INTERFACE_TRADE || lpObj->Interface.state == 0)
		{
			return;
		}

		GCPetItemInfoSend(aIndex,lpMsg->type,lpMsg->flag,lpMsg->slot,lpObj->Trade[lpMsg->slot].m_PetItemLevel,lpObj->Trade[lpMsg->slot].m_PetItemExp,(BYTE)lpObj->Trade[lpMsg->slot].m_Durability);
	}
	else if(lpMsg->flag == 3) // Target Trade
	{
		if(TRADE_RANGE(lpMsg->slot) == 0)
		{
			return;
		}

		if(lpObj->Interface.use == 0 || lpObj->Interface.type != INTERFACE_TRADE || lpObj->Interface.state == 0)
		{
			return;
		}

		if(OBJECT_RANGE(lpObj->TargetNumber) == 0)
		{
			return;
		}

		LPOBJ lpTarget = &gObj[lpObj->TargetNumber];

		if(lpTarget->Interface.use == 0 || lpTarget->Interface.type != INTERFACE_TRADE || lpTarget->Interface.state == 0)
		{
			return;
		}

		GCPetItemInfoSend(aIndex,lpMsg->type,lpMsg->flag,lpMsg->slot,lpTarget->Trade[lpMsg->slot].m_PetItemLevel,lpTarget->Trade[lpMsg->slot].m_PetItemExp,(BYTE)lpTarget->Trade[lpMsg->slot].m_Durability);
	}
	else if(lpMsg->flag == 4) // Chaos Box
	{
		if(CHAOS_BOX_RANGE(lpMsg->slot) == 0)
		{
			return;
		}

		GCPetItemInfoSend(aIndex,lpMsg->type,lpMsg->flag,lpMsg->slot,lpObj->ChaosBox[lpMsg->slot].m_PetItemLevel,lpObj->ChaosBox[lpMsg->slot].m_PetItemExp,(BYTE)lpObj->ChaosBox[lpMsg->slot].m_Durability);
	}
	else if(lpMsg->flag == 5) // Target Personal Shop
	{
		if(INVENTORY_SHOP_RANGE(lpMsg->slot) == 0)
		{
			return;
		}

		if(OBJECT_RANGE(lpObj->PShopDealerIndex) == 0)
		{
			return;
		}
		LPOBJ lpTarget = &gObj[lpObj->PShopDealerIndex];
		GCPetItemInfoSend(aIndex,lpMsg->type,lpMsg->flag,lpMsg->slot,lpTarget->Inventory[lpMsg->slot].m_PetItemLevel,lpTarget->Inventory[lpMsg->slot].m_PetItemExp,(BYTE)lpTarget->Inventory[lpMsg->slot].m_Durability);
	}
}

void CGMapServerMoveAuthRecv(PMSG_MAP_SERVER_MOVE_AUTH_RECV* lpMsg,int aIndex) // OK
{
	LPOBJ lpObj = &gObj[aIndex];

	if(lpObj->Connected != OBJECT_CONNECTED)
	{
		CloseClient(aIndex);
		return;
	}

	if(memcmp(gServerInfo.m_ServerVersion,lpMsg->ClientVersion,sizeof(lpMsg->ClientVersion)) != 0)
	{
		GCMapServerMoveAuthSend(aIndex,6);
		return;
	}

	if(memcmp(gServerInfo.m_ServerSerial,lpMsg->ClientSerial,sizeof(lpMsg->ClientSerial)) != 0)
	{
		GCMapServerMoveAuthSend(aIndex,6);
		return;
	}

	if(lpObj->LoginMessageSend == 0)
	{
		lpObj->LoginMessageSend++;
		lpObj->LoginMessageCount++;
		lpObj->ConnectTickCount = GetTickCount();
		lpObj->ClientTickCount = lpMsg->TickCount;
		lpObj->ServerTickCount = GetTickCount();

		char account[11] = {0};
		PacketArgumentDecrypt(account,lpMsg->account,sizeof(account));
		lpObj->MapServerMoveRequest = 1;
		GJMapServerMoveAuthSend(aIndex,account,lpMsg->name,lpMsg->AuthCode1,lpMsg->AuthCode2,lpMsg->AuthCode3,lpMsg->AuthCode4);
	}
}

void CGMoveRecv(PMSG_MOVE_RECV* lpMsg,int aIndex) // OK
{
	LPOBJ lpObj = &gObj[aIndex];

	if(gObjIsConnectedGS(aIndex) == 0)
	{
		return;
	}

	if(lpObj->RegenOk > 0)
	{
		return;
	}

	if(lpObj->Teleport != 0)
	{
		return;
	}

	if(gObjCheckMapTile(lpObj,1) != 0)
	{
		return;
	}

	if((GetTickCount()-lpObj->LastMoveTime) < 100)
	{
		return;
	}

	if(gEffectManager.CheckStunEffect(lpObj) != 0 || gEffectManager.CheckImmobilizeEffect(lpObj) != 0)
	{
		return;
	}

	if(lpObj->SkillSummonPartyTime != 0)
	{
		lpObj->SkillSummonPartyTime = 0;
		gNotice.GCNoticeSend(lpObj->Index,1,0,0,0,0,0,gMessage.GetMessage(272));
	}

	lpObj->Dir = lpMsg->path[0] >> 4;
	lpObj->Rest = 0;
	lpObj->PathCur = 0;
	lpObj->PathCount = lpMsg->path[0] & 0x0F;
	lpObj->LastMoveTime = GetTickCount();
	memset(lpObj->PathX,0,sizeof(lpObj->PathX));
	memset(lpObj->PathY,0,sizeof(lpObj->PathY));
	memset(lpObj->PathOri,0,sizeof(lpObj->PathOri));
	lpObj->TX = lpMsg->x;
	lpObj->TY = lpMsg->y;
	lpObj->PathCur = ((lpObj->PathCount>0)?1:0);
	lpObj->PathCount = ((lpObj->PathCount>0)?(lpObj->PathCount+1):lpObj->PathCount);
	lpObj->PathStartEnd = 1;
	lpObj->PathX[0] = lpMsg->x;
	lpObj->PathY[0] = lpMsg->y;
	lpObj->PathDir[0] = lpObj->Dir;

	for(int n=1;n < lpObj->PathCount;n++)
	{
		if((n%2) == 0)
		{
			lpObj->TX = lpObj->PathX[n-1]+RoadPathTable[((lpMsg->path[((n+1)/2)] & 0x0F)*2)+0];
			lpObj->TY = lpObj->PathY[n-1]+RoadPathTable[((lpMsg->path[((n+1)/2)] & 0x0F)*2)+1];
			lpObj->PathX[n] = lpObj->PathX[n-1]+RoadPathTable[((lpMsg->path[((n+1)/2)] & 0x0F)*2)+0];
			lpObj->PathY[n] = lpObj->PathY[n-1]+RoadPathTable[((lpMsg->path[((n+1)/2)] & 0x0F)*2)+1];
			lpObj->PathOri[n-1] = lpMsg->path[((n+1)/2)] & 0x0F;
			lpObj->PathDir[n+0] = lpMsg->path[((n+1)/2)] & 0x0F;
		}
		else
		{
			lpObj->TX = lpObj->PathX[n-1]+RoadPathTable[((lpMsg->path[((n+1)/2)] / 0x10)*2)+0];
			lpObj->TY = lpObj->PathY[n-1]+RoadPathTable[((lpMsg->path[((n+1)/2)] / 0x10)*2)+1];
			lpObj->PathX[n] = lpObj->PathX[n-1]+RoadPathTable[((lpMsg->path[((n+1)/2)] / 0x10)*2)+0];
			lpObj->PathY[n] = lpObj->PathY[n-1]+RoadPathTable[((lpMsg->path[((n+1)/2)] / 0x10)*2)+1];
			lpObj->PathOri[n-1] = lpMsg->path[((n+1)/2)] / 0x10;
			lpObj->PathDir[n+0] = lpMsg->path[((n+1)/2)] / 0x10;
		}
	}

	if(lpObj->TX < (lpObj->X-15) || lpObj->TX > (lpObj->X+15) || lpObj->TY < (lpObj->Y-15) || lpObj->TY > (lpObj->Y+15))
	{
		lpObj->PathCur = 0;
		lpObj->PathCount = 0;
		lpObj->PathStartEnd = 0;
		memset(lpObj->PathX,0,sizeof(lpObj->PathX));
		memset(lpObj->PathY,0,sizeof(lpObj->PathY));
		memset(lpObj->PathOri,0,sizeof(lpObj->PathOri));
		gObjSetPosition(lpObj->Index,lpObj->X,lpObj->Y);
		return;
	}
	
	if(lpObj->PathCount > 0 && (gMap[lpObj->Map].CheckAttr(lpObj->TX,lpObj->TY,4) != 0 || gMap[lpObj->Map].CheckAttr(lpObj->TX,lpObj->TY,8) != 0))
	{
		lpObj->PathCur = 0;
		lpObj->PathCount = 0;
		lpObj->PathStartEnd = 0;
		memset(lpObj->PathX,0,sizeof(lpObj->PathX));
		memset(lpObj->PathY,0,sizeof(lpObj->PathY));
		memset(lpObj->PathOri,0,sizeof(lpObj->PathOri));
		gObjSetPosition(lpObj->Index,lpObj->X,lpObj->Y);
		return;
	}

	gMap[lpObj->Map].DelStandAttr(lpObj->OldX,lpObj->OldY);

	lpObj->X = lpMsg->x;
	lpObj->Y = lpMsg->y;
	lpObj->TX = lpObj->TX;
	lpObj->TY = lpObj->TY;
	lpObj->OldX = lpObj->TX;
	lpObj->OldY = lpObj->TY;
	lpObj->ViewState = 0;
	gMap[lpObj->Map].SetStandAttr(lpObj->TX,lpObj->TY);
	PMSG_MOVE_SEND pMsg;
	pMsg.header.set(PROTOCOL_CODE1,sizeof(pMsg));
	pMsg.index[0] = SET_NUMBERHB(aIndex);
	pMsg.index[1] = SET_NUMBERLB(aIndex);
	pMsg.x = (BYTE)lpObj->TX;
	pMsg.y = (BYTE)lpObj->TY;
	pMsg.dir = lpObj->Dir << 4;
	if(lpObj->Type == OBJECT_USER)
	{
		DataSend(aIndex,(BYTE*)&pMsg,pMsg.header.size);
	}

	for(int n=0;n < MAX_VIEWPORT;n++)
	{
		if(lpObj->VpPlayer2[n].type == OBJECT_USER)
		{
			if(lpObj->VpPlayer2[n].state != OBJECT_EMPTY && lpObj->VpPlayer2[n].state != OBJECT_DIECMD && lpObj->VpPlayer2[n].state != OBJECT_DIED)
			{
				DataSend(lpObj->VpPlayer2[n].index,(BYTE*)&pMsg,pMsg.header.size);
			}
		}
	}
}

void CGConnectAccountRecv(PMSG_CONNECT_ACCOUNT_RECV* lpMsg,int aIndex) // OK
{
	LPOBJ lpObj = &gObj[aIndex];

	if(lpObj->Connected != OBJECT_CONNECTED)
	{
		CloseClient(aIndex);
		return;
	}

	if(memcmp(gServerInfo.m_ServerVersion,lpMsg->ClientVersion,sizeof(lpMsg->ClientVersion)) != 0)
	{
		GCConnectAccountSend(aIndex,6);
		return;
	}

	if(memcmp(gServerInfo.m_ServerSerial,lpMsg->ClientSerial,sizeof(lpMsg->ClientSerial)) != 0)
	{
		GCConnectAccountSend(aIndex,6);
		return;
	}

	#if(OFFLINE_MODE == TRUE)
	char account[11] = { 0 };
	PacketArgumentDecrypt(account, lpMsg->account, (sizeof(account) - 1));
	char password[11] = { 0 };
	PacketArgumentDecrypt(password, lpMsg->password, (sizeof(password) - 1));

	for (int i = OBJECT_START_USER; i < MAX_OBJECT; i++)
	{
		LPOBJ sObj = &gObj[i];
		if (sObj->Connected == OBJECT_ONLINE)
		{
			if (account[0] == sObj->Account[0])
			{
				if (!strncmp(account, sObj->Account, 10))
				{
					if (sObj->m_OfflineMode == 1)
					{
						sObj->m_OfflineMode = 0;
						gObjDel(i);
					}
					#if USE_FAKE_ONLINE == TRUE
					if (sObj->IsFakeOnline == 1)
					{
						s_FakeOnline.OnAttackAlreadyConnected(&gObj[i]);
						gObjDel(i); // them fix van thanh
					}
					#endif
					break;
				}
			}
		}
	}

	if (lpObj->LoginMessageSend == 0)
	{
		lpObj->LoginMessageSend++;
		lpObj->LoginMessageCount++;
		lpObj->ConnectTickCount = GetTickCount();
		lpObj->ClientTickCount = lpMsg->TickCount;
		lpObj->ServerTickCount = GetTickCount();

		lpObj->MapServerMoveRequest = 0;
		lpObj->LastServerCode = -1;
		lpObj->DestMap = -1;
		lpObj->DestX = 0;
		lpObj->DestY = 0;

		GJConnectAccountSend(aIndex, account, password, lpObj->IpAddr);
	}
	#else
	if (lpObj->LoginMessageSend == 0)
	{
		lpObj->LoginMessageSend++;
		lpObj->LoginMessageCount++;
		lpObj->ConnectTickCount = GetTickCount();
		lpObj->ClientTickCount = lpMsg->TickCount;
		lpObj->ServerTickCount = GetTickCount();

		char account[11] = { 0 };
		PacketArgumentDecrypt(account, lpMsg->account, (sizeof(account) - 1));
		char password[11] = { 0 };
		PacketArgumentDecrypt(password, lpMsg->password, (sizeof(password) - 1));
		lpObj->MapServerMoveRequest = 0;
		lpObj->LastServerCode = -1;
		lpObj->DestMap = -1;
		lpObj->DestX = 0;
		lpObj->DestY = 0;
		GJConnectAccountSend(aIndex, account, password, lpObj->IpAddr);
	}
	#endif
}
#if(REGISTER_INGAME)
bool isSafeString(const std::string& str) 
{
	const std::string dangerousChars = "'\";--";

	for (size_t i = 0; i < str.length(); ++i) {
		char c = str[i];
		if (dangerousChars.find(c) != std::string::npos) {
			return false;
		}
	}

	const char* dangerousStrings[] = { "delete", "drop", " " }; //

	for (size_t i = 0; i < sizeof(dangerousStrings) / sizeof(dangerousStrings[0]); ++i) 
	{
		const char* dangerousStr = dangerousStrings[i];
		if (str.find(dangerousStr) != std::string::npos) {
			return false; 
		}
	}
	return true;
}
void CGRegisterAccountRecv(PMSG_REGISTER_ACCOUNT_RECV* lpMsg, int aIndex)
{
	LPOBJ lpObj = &gObj[aIndex];

	if (lpObj->Connected != OBJECT_CONNECTED)
	{
		CloseClient(aIndex);
		return;
	}

	if (memcmp(gServerInfo.m_ServerVersion, lpMsg->ClientVersion, sizeof(lpMsg->ClientVersion)) != 0)
	{
		GCConnectAccountSend(aIndex, 6);
		return;
	}

	if (memcmp(gServerInfo.m_ServerSerial, lpMsg->ClientSerial, sizeof(lpMsg->ClientSerial)) != 0)
	{
		GCConnectAccountSend(aIndex, 6);
		return;
	}

	if (lpObj->LoginMessageSend == 0)
	{
		lpObj->LoginMessageSend++;
		lpObj->LoginMessageCount++;
		lpObj->ConnectTickCount = GetTickCount();
		lpObj->ClientTickCount = lpMsg->TickCount;
		lpObj->ServerTickCount = GetTickCount();
		char account[11] = { 0 };
		PacketArgumentDecrypt(account, lpMsg->account, (sizeof(account) - 1));
		char password[11] = { 0 };
		PacketArgumentDecrypt(password, lpMsg->password, (sizeof(password) - 1));
		char PersonalCode[11] = { 0 };
		PacketArgumentDecrypt(PersonalCode, lpMsg->PersonalCode, (sizeof(PersonalCode) - 1));
		char PersonalEmail[50] = { 0 };
		PacketArgumentDecrypt(PersonalEmail, lpMsg->szEmail, (sizeof(PersonalEmail) - 1));
		lpObj->MapServerMoveRequest = 0;
		lpObj->LastServerCode = -1;
		lpObj->DestMap = -1;
		lpObj->DestX = 0;
		lpObj->DestY = 0;

		if (!isSafeString(account) || !isSafeString(password) || !isSafeString(PersonalCode) || !isSafeString(PersonalEmail))
		{
			LogAdd(LOG_RED, "[ErrorInject] [%d][%s] ", lpObj->Index, lpObj->IpAddr);
			CloseClient(lpObj->Index);
			return;
		}
		GJRegisterAccountSend(aIndex, account, password, PersonalCode, PersonalEmail);
	}
}
#endif
void CGCloseClientRecv(PMSG_CLOSE_CLIENT_RECV* lpMsg, int aIndex) // OK
{
	LPOBJ lpObj = &gObj[aIndex];
	if(lpObj->Connected != OBJECT_LOGGED && lpObj->Connected != OBJECT_ONLINE)
	{
		return;
	}

	gObjectManager.CharacterGameCloseSet(aIndex,lpMsg->type);
}

void CGCharacterListRecv(int aIndex) // OK
{
	LPOBJ lpObj = &gObj[aIndex];

	if(lpObj->Connected != OBJECT_LOGGED)
	{
		return;
	}
	GDCharacterListSend(aIndex);
}

void CGCharacterCreateRecv(PMSG_CHARACTER_CREATE_RECV* lpMsg, int aIndex) // OK
{
	LPOBJ lpObj = &gObj[aIndex];

	if(lpObj->Connected != OBJECT_LOGGED)
	{
		return;
	}
	PMSG_CHARACTER_CREATE_SEND pMsg;
	pMsg.header.set(0xF3, 0x01, sizeof(pMsg));
	pMsg.result = 0;
	memcpy(pMsg.name,lpMsg->name,sizeof(pMsg.name));
	if(gServerInfo.m_CharacterCreateSwitch == 0)
	{
		DataSend(aIndex,(BYTE*)&pMsg,pMsg.header.size);
		return;
	}
	if(lpMsg->Class != DB_CLASS_DW && lpMsg->Class != DB_CLASS_DK && lpMsg->Class != DB_CLASS_FE && lpMsg->Class != DB_CLASS_MG && lpMsg->Class != DB_CLASS_DL && lpMsg->Class != DB_CLASS_SU && lpMsg->Class != DB_CLASS_RF)
	{
		DataSend(aIndex,(BYTE*)&pMsg,pMsg.header.size);
		return;
	}
	if(lpMsg->Class == DB_CLASS_SU && (lpObj->ClassCode & 1) == 0)
	{
		DataSend(aIndex,(BYTE*)&pMsg,pMsg.header.size);
		return;
	}
	if(lpMsg->Class == DB_CLASS_MG && (lpObj->ClassCode & 4) == 0)
	{
		DataSend(aIndex,(BYTE*)&pMsg,pMsg.header.size);
		return;
	}
	if(lpMsg->Class == DB_CLASS_DL && (lpObj->ClassCode & 2) == 0)
	{
		DataSend(aIndex,(BYTE*)&pMsg,pMsg.header.size);
		return;
	}

	if(lpMsg->Class == DB_CLASS_RF && (lpObj->ClassCode & 8) == 0)
	{
		DataSend(aIndex,(BYTE*)&pMsg,pMsg.header.size);
		return;
	}
	char name[11] = {0};
	memcpy(name,lpMsg->name,sizeof(lpMsg->name));
	GDCharacterCreateSend(aIndex, name, lpMsg->Class);
}

void CGCharacterDeleteRecv(PMSG_CHARACTER_DELETE_RECV* lpMsg,int aIndex) // OK
{
	LPOBJ lpObj = &gObj[aIndex];
	if(lpObj->Connected != OBJECT_LOGGED)
	{
		return;
	}

	if(lpObj->EnableDelCharacter == 0)
	{
		return;
	}
	PMSG_CHARACTER_DELETE_SEND pMsg;
	pMsg.header.set(0xF3,0x02,sizeof(pMsg));
	pMsg.result = 0;
	if(gObjCheckPersonalCode(aIndex,lpMsg->PersonalCode) == 0)
	{
		pMsg.result = 2;
		DataSend(aIndex,(BYTE*)&pMsg,pMsg.header.size);
		return;
	}
	if(lpObj->Lock > 0)
	{
		gNotice.GCNoticeSend(lpObj->Index,1,0,0,0,2,0,gMessage.GetMessage(778));
		pMsg.result = 2;
		DataSend(aIndex,(BYTE*)&pMsg,pMsg.header.size);
		return;
	}
	char name[11] = {0};
	memcpy(name,lpMsg->name,sizeof(lpMsg->name));
	GDCharacterDeleteSend(aIndex,name);
}

void CGCharacterInfoRecv(PMSG_CHARACTER_INFO_RECV* lpMsg,int aIndex) // OK
{
	LPOBJ lpObj = &gObj[aIndex];

	if(lpObj->Connected != OBJECT_LOGGED)
	{
		return;
	}

	lpObj->EnableDelCharacter = 0;
	char name[11] = {0};
	memcpy(name,lpMsg->name,sizeof(lpMsg->name));
	GDCharacterInfoSend(aIndex,name);
}

void CGLevelUpPointRecv(PMSG_LEVEL_UP_POINT_RECV* lpMsg,int aIndex) // OK
{
	LPOBJ lpObj = &gObj[aIndex];

	if(gObjIsConnectedGP(aIndex) == 0)
	{
		return;
	}

	PMSG_LEVEL_UP_POINT_SEND pMsg;
	pMsg.header.set(0xF3,0x06,sizeof(pMsg));
	pMsg.result = 0;
	if(gObjectManager.CharacterLevelUpPointAdd(lpObj,lpMsg->type,1) != 0)
	{
		pMsg.result = 16+lpMsg->type;

		switch(lpMsg->type)
		{
			case 2: // Vitality
				pMsg.MaxLifeAndMana = GET_MAX_WORD_VALUE((lpObj->MaxLife+lpObj->AddLife));
				break;
			case 3: // Energy
				pMsg.MaxLifeAndMana = GET_MAX_WORD_VALUE((lpObj->MaxMana+lpObj->AddMana));
				break;
		}

		pMsg.MaxBP = GET_MAX_WORD_VALUE((lpObj->MaxBP+lpObj->AddBP));

		pMsg.MaxShield = GET_MAX_WORD_VALUE((lpObj->MaxShield+lpObj->AddShield));

		#if(GAMESERVER_EXTRA==1)
		pMsg.ViewPoint = (DWORD)(lpObj->LevelUpPoint);
		pMsg.ViewMaxHP = (DWORD)(lpObj->MaxLife+lpObj->AddLife);
		pMsg.ViewMaxMP = (DWORD)(lpObj->MaxMana+lpObj->AddMana);
		pMsg.ViewMaxBP = (DWORD)(lpObj->MaxBP+lpObj->AddBP);
		pMsg.ViewMaxSD = (DWORD)(lpObj->MaxShield+lpObj->AddShield);
		pMsg.ViewStrength = lpObj->Strength;
		pMsg.ViewDexterity = lpObj->Dexterity;
		pMsg.ViewVitality = lpObj->Vitality;
		pMsg.ViewEnergy = lpObj->Energy;
		pMsg.ViewLeadership = lpObj->Leadership;
		#endif
	}
	DataSend(aIndex,(BYTE*)&pMsg,pMsg.header.size);
}

void CGCharacterMoveViewportEnableRecv(int aIndex) // OK
{
	LPOBJ lpObj = &gObj[aIndex];

	if(gObjIsConnectedGP(aIndex) == 0)
	{
		return;
	}

	lpObj->RegenOk = ((lpObj->RegenOk==1)?2:lpObj->RegenOk);
}

void CGCharacterNameCheckRecv(PMSG_CHARACTER_NAME_CHECK_RECV* lpMsg,int aIndex) // OK
{
	LPOBJ lpObj = &gObj[aIndex];

	if(lpObj->Connected != OBJECT_LOGGED)
	{
		return;
	}
	char name[11] = {0};
	memcpy(name,lpMsg->name,sizeof(lpMsg->name));
	GDCharacterNameCheckSend(aIndex,name);
}

void CGCharacterNameChangeRecv(PMSG_CHARACTER_NAME_CHANGE_RECV* lpMsg,int aIndex) // OK
{
	LPOBJ lpObj = &gObj[aIndex];
	if(lpObj->Connected != OBJECT_LOGGED)
	{
		return;
	}

	char OldName[11] = {0};
	char NewName[11] = {0};
	memcpy(OldName,lpMsg->OldName,sizeof(lpMsg->OldName));
	memcpy(NewName,lpMsg->NewName,sizeof(lpMsg->NewName));
	GDCharacterNameChangeSend(aIndex,OldName,NewName);
}

void CGOptionChangeSkinRecv(PMSG_OPTION_CHANGE_SKIN_RECV* lpMsg,int aIndex) // OK
{

}

void CGOptionDataRecv(PMSG_OPTION_DATA_RECV* lpMsg,int aIndex) // OK
{
	LPOBJ lpObj = &gObj[aIndex];

	if(gObjIsConnected(aIndex) == 0)
	{
		return;
	}

	if (lpObj->RenameActive == 0)
	{
		GDOptionDataSaveSend(aIndex,lpMsg->SkillKey,lpMsg->GameOption,lpMsg->QKey,lpMsg->WKey,lpMsg->EKey,lpMsg->ChatWindow,lpMsg->RKey,ntohl(lpMsg->QWERLevel));
	}
}

void CGClientSecurityBreachRecv(PMSG_CLIENT_SECURITY_BREACH_RECV* lpMsg,int aIndex) // OK
{
	
}

void CGAcheronEnterRecv(int aIndex) // OK
{

}

void CGSNSDataRecv(PMSG_SNS_DATA_RECV* lpMsg,int aIndex) // OK
{

}

void CGSNSDataLogRecv(PMSG_SNS_DATA_LOG_RECV* lpMsg,int aIndex) // OK
{

}


void ChatSend(LPOBJ lpObj, char* szChat)
{
	int size = strlen(szChat);
	size = ((size>MAX_CHAT_MESSAGE_SIZE) ? MAX_CHAT_MESSAGE_SIZE : size);
	PMSG_CHAT_TARGET_SEND pMsg;
	pMsg.header.set(0x01, (sizeof(pMsg)-(sizeof(pMsg.message) - (size + 1))));
	pMsg.index[0] = SET_NUMBERHB(lpObj->Index);
	pMsg.index[1] = SET_NUMBERLB(lpObj->Index);
	memcpy(pMsg.message, szChat, size);
	pMsg.message[size] = 0;
	MsgSendV2(lpObj, (BYTE*)&pMsg, pMsg.header.size);
}


void GCChatTargetSend(LPOBJ lpObj,int aIndex,char* message) // OK
{
	int size = strlen(message);
	size = ((size>MAX_CHAT_MESSAGE_SIZE)?MAX_CHAT_MESSAGE_SIZE:size);
	PMSG_CHAT_TARGET_SEND pMsg;
	pMsg.header.set(0x01,(sizeof(pMsg)-(sizeof(pMsg.message)-(size+1))));
	pMsg.index[0] = SET_NUMBERHB(aIndex);
	pMsg.index[1] = SET_NUMBERLB(aIndex);
	memcpy(pMsg.message,message,size);
	pMsg.message[size] = 0;
	DataSend(lpObj->Index,(BYTE*)&pMsg,pMsg.header.size);
	#if(GuardMessage)
	MsgSendV2(lpObj, (BYTE*)&pMsg,pMsg.header.size); // Guard Message
	#endif
}

void GCChatTargetNewSend(LPOBJ lpObj,int aIndex,char* message,...) // OK
{
	char buff[256] = {0};
	va_list arg;
	va_start(arg,message);
	vsprintf_s(buff,message,arg);
	va_end(arg);
	int size = strlen(buff);
	size = ((size>MAX_CHAT_MESSAGE_SIZE)?MAX_CHAT_MESSAGE_SIZE:size);
	PMSG_CHAT_TARGET_SEND pMsg;
	pMsg.header.set(0x01,(sizeof(pMsg)-(sizeof(pMsg.message)-(size+1))));
	pMsg.index[0] = SET_NUMBERHB(aIndex);
	pMsg.index[1] = SET_NUMBERLB(aIndex);
	memcpy(pMsg.message,buff,size);
	pMsg.message[size] = 0;
	DataSend(lpObj->Index,(BYTE*)&pMsg,pMsg.header.size);
}

void GCChatWhisperSend(int aIndex,char* name,char* message) // OK
{
	int size = strlen(message);
	size = ((size>MAX_CHAT_MESSAGE_SIZE)?MAX_CHAT_MESSAGE_SIZE:size);
	PMSG_CHAT_WHISPER_SEND pMsg;
	pMsg.header.set(0x02,(sizeof(pMsg)-(sizeof(pMsg.message)-(size+1))));
	memcpy(pMsg.name,name,sizeof(pMsg.name));
	memcpy(pMsg.message,message,size);
	pMsg.message[size] = 0;
	DataSend(aIndex,(BYTE*)&pMsg,pMsg.header.size);
}

void GCMainCheckSend(int aIndex) // OK
{
	LPOBJ lpObj = &gObj[aIndex];
	lpObj->CheckSumTableNum = GetLargeRand()%MAX_CHECKSUM_KEY;
	lpObj->CheckSumTime = GetTickCount();
	PMSG_MAIN_CHECK_SEND pMsg;
	pMsg.header.set(0x03,sizeof(pMsg));
	pMsg.key = (((GetLargeRand()%64)*16) | ((lpObj->CheckSumTableNum & 0x3F0)*64) | ((lpObj->CheckSumTableNum & 0x0F)) ^ 0xB479);
	DataSend(aIndex,(BYTE*)&pMsg,pMsg.header.size);
}

void GCEventStateSend(int aIndex,BYTE state,BYTE event) // OK
{
	PMSG_EVENT_STATE_SEND pMsg;
	pMsg.header.set(0x0B,sizeof(pMsg));
	pMsg.state = state;
	pMsg.event = event;
	DataSend(aIndex,(BYTE*)&pMsg,pMsg.header.size);
}

void GCEventStateSendToAll(BYTE state,BYTE event) // OK
{
	PMSG_EVENT_STATE_SEND pMsg;
	pMsg.header.set(0x0B,sizeof(pMsg));
	pMsg.state = state;
	pMsg.event = event;
	DataSendAll((BYTE*)&pMsg,pMsg.header.size);
}
#if SO_LUONG_BOSS
void GCMapEventStateSend(int map, BYTE state, BYTE event) // OK
{
	PMSG_EVENT_STATE_SEND pMsg;
	pMsg.header.set(0x0B, sizeof(pMsg));
	pMsg.state = state;
	pMsg.event = event;
	for (int n = OBJECT_START_USER; n < MAX_OBJECT; n++)
	{
		if (gObjIsConnected(n) != 0 && map == gObj[n].Map)
		{
			DataSend(n, (BYTE*)&pMsg, pMsg.header.size);
		}
	}
}
#endif
void GCServerMsgSend(int aIndex,BYTE msg) // OK
{
	PMSG_SERVER_MSG_SEND pMsg;
	pMsg.header.set(0x0C,sizeof(pMsg));
	pMsg.MsgNumber = msg;
	DataSend(aIndex,(BYTE*)&pMsg,pMsg.header.size);
}

void GCWeatherSend(int aIndex,BYTE weather) // OK
{
	PMSG_WEATHER_SEND pMsg;
	pMsg.header.set(0x0F,sizeof(pMsg));
	pMsg.weather = weather;
	DataSend(aIndex,(BYTE*)&pMsg,pMsg.header.size);
}

void GCDamageSend(int aIndex,int bIndex,BYTE flag,float damage,int type,float ShieldDamage) // OK
{
	PMSG_DAMAGE_SEND pMsg;
	pMsg.header.set(PROTOCOL_CODE2,sizeof(pMsg));
	pMsg.index[0] = SET_NUMBERHB(bIndex) | (flag*0x80);
	pMsg.index[1] = SET_NUMBERLB(bIndex);
	pMsg.damage[0] = SET_NUMBERHB(GET_MAX_WORD_VALUE(damage));
	pMsg.damage[1] = SET_NUMBERLB(GET_MAX_WORD_VALUE(damage));
	pMsg.type = type;
	pMsg.ShieldDamage[0] = SET_NUMBERHB(GET_MAX_WORD_VALUE(ShieldDamage));
	pMsg.ShieldDamage[1] = SET_NUMBERLB(GET_MAX_WORD_VALUE(ShieldDamage));
	pMsg.ViewCurHP = (DWORD)gObj[bIndex].Life;
	pMsg.ViewCurSD = (DWORD)gObj[bIndex].Shield;
	pMsg.ViewDamageHP = (QWORD)damage;
	pMsg.ViewDamageSD = (QWORD)ShieldDamage;

	if(gObj[aIndex].Type == OBJECT_USER)
	{
		DataSend(aIndex,(BYTE*)&pMsg,pMsg.header.size);
	}
	if(gObj[bIndex].Type == OBJECT_USER)
	{
		DataSend(bIndex,(BYTE*)&pMsg,pMsg.header.size);
	}
}

void GCUserDieSend(LPOBJ lpObj,int aIndex,int skill,int bIndex) // OK
{
	PMSG_USER_DIE_SEND pMsg;
	pMsg.header.set(0x17,sizeof(pMsg));
	pMsg.index[0] = SET_NUMBERHB(aIndex);
	pMsg.index[1] = SET_NUMBERLB(aIndex);
	pMsg.skill[0] = SET_NUMBERHB(skill);
	pMsg.skill[1] = SET_NUMBERLB(skill);
	pMsg.killer[0] = SET_NUMBERHB(bIndex);
	pMsg.killer[1] = SET_NUMBERLB(bIndex);

	if(lpObj->Type == OBJECT_USER)
	{
		DataSend(lpObj->Index,(BYTE*)&pMsg,pMsg.header.size);
	}
	MsgSendV2(lpObj,(BYTE*)&pMsg,pMsg.header.size);
}

void GCActionSend(LPOBJ lpObj,BYTE action,int aIndex,int bIndex) // OK
{
	PMSG_ACTION_SEND pMsg;
	pMsg.header.set(0x18,sizeof(pMsg));
	pMsg.index[0] = SET_NUMBERHB(aIndex);
	pMsg.index[1] = SET_NUMBERLB(aIndex);
	pMsg.dir = lpObj->Dir;
	pMsg.action = action;
	pMsg.target[0] = SET_NUMBERHB(bIndex);
	pMsg.target[1] = SET_NUMBERLB(bIndex);
	MsgSendV2(lpObj,(BYTE*)&pMsg,pMsg.header.size);
}

void GCMoneySend(int aIndex,DWORD money) // OK
{
	PMSG_ITEM_GET_SEND pMsg;
	pMsg.header.setE(0x22,sizeof(pMsg));
	pMsg.result = 0xFE;
	memset(pMsg.ItemInfo,0,sizeof(pMsg.ItemInfo));
	pMsg.ItemInfo[0] = SET_NUMBERHB(SET_NUMBERHW(money));
	pMsg.ItemInfo[1] = SET_NUMBERLB(SET_NUMBERHW(money));
	pMsg.ItemInfo[2] = SET_NUMBERHB(SET_NUMBERLW(money));
	pMsg.ItemInfo[3] = SET_NUMBERLB(SET_NUMBERLW(money));
	DataSend(aIndex,(BYTE*)&pMsg,pMsg.header.size);
}

void GCLifeSend(int aIndex,BYTE type,int life,int shield) // OK
{
	PMSG_LIFE_SEND pMsg;
	pMsg.header.set(0x26,sizeof(pMsg));
	pMsg.type = type;
	pMsg.life[0] = SET_NUMBERHB(GET_MAX_WORD_VALUE(life));
	pMsg.life[1] = SET_NUMBERLB(GET_MAX_WORD_VALUE(life));
	pMsg.flag = 0;
	pMsg.shield[0] = SET_NUMBERHB(GET_MAX_WORD_VALUE(shield));
	pMsg.shield[1] = SET_NUMBERLB(GET_MAX_WORD_VALUE(shield));
	pMsg.ViewHP = life;
	pMsg.ViewSD = shield;
	DataSend(aIndex,(BYTE*)&pMsg,pMsg.header.size);
}

void GCManaSend(int aIndex,BYTE type,int mana,int bp) // OK
{
	PMSG_MANA_SEND pMsg;
	pMsg.header.set(0x27,sizeof(pMsg));
	pMsg.type = type;
	pMsg.mana[0] = SET_NUMBERHB(GET_MAX_WORD_VALUE(mana));
	pMsg.mana[1] = SET_NUMBERLB(GET_MAX_WORD_VALUE(mana));
	pMsg.bp[0] = SET_NUMBERHB(GET_MAX_WORD_VALUE(bp));
	pMsg.bp[1] = SET_NUMBERLB(GET_MAX_WORD_VALUE(bp));
	pMsg.ViewMP = mana;
	pMsg.ViewBP = bp;
	DataSend(aIndex,(BYTE*)&pMsg,pMsg.header.size);
}

void GCItemUseSpecialTimeSend(int aIndex,BYTE number,int time) // OK
{
	PMSG_ITEM_SPECIAL_TIME_SEND pMsg;
	pMsg.header.setE(0x29,sizeof(pMsg));
	pMsg.number = number;
	pMsg.time = time;
	DataSend(aIndex,(BYTE*)&pMsg,pMsg.header.size);
}

void GCMapAttrSend(int aIndex,BYTE type,BYTE attr,BYTE flag,BYTE count,PMSG_MAP_ATTR* lpInfo) // OK
{
	BYTE send[256];
	PMSG_MAP_ATTR_SEND pMsg;
	pMsg.header.set(0x46,0);
	int size = sizeof(pMsg);
	pMsg.type = type;
	pMsg.attr = attr;
	pMsg.flag = flag;
	pMsg.count = count;
	PMSG_MAP_ATTR info;

	for(int n=0;n < count;n++)
	{
		info.x = lpInfo[n].x;
		info.y = lpInfo[n].y;
		info.tx = lpInfo[n].tx;
		info.ty = lpInfo[n].ty;

		memcpy(&send[size],&info,sizeof(info));
		size += sizeof(info);
	}
	pMsg.header.size = size;
	memcpy(send,&pMsg,sizeof(pMsg));
	DataSend(aIndex,send,size);
}

void GCPartyItemInfoSend(int aIndex,CItem* lpItem) // OK
{
	LPOBJ lpObj = &gObj[aIndex];
	if(OBJECT_RANGE(lpObj->PartyNumber) == 0)
	{
		return;
	}

	PMSG_PARTY_ITEM_INFO_SEND pMsg;
	pMsg.header.set(0x47,sizeof(pMsg));
	pMsg.index[0] = SET_NUMBERHB(aIndex);
	pMsg.index[1] = SET_NUMBERLB(aIndex);
	pMsg.ItemInfo = lpItem->m_Index & 0x1FFF;
	if(lpItem->m_Option1 != 0)
	{
		pMsg.ItemInfo |= 0x2000;
	}

	if(lpItem->m_Option2 != 0)
	{
		pMsg.ItemInfo |= 0x4000;
	}

	if(lpItem->m_Option3 != 0)
	{
		pMsg.ItemInfo |= 0x8000;
	}

	if(lpItem->IsExcItem() != 0)
	{
		pMsg.ItemInfo |= 0x10000;
	}

	if(lpItem->IsSetItem() != 0)
	{
		pMsg.ItemInfo |= 0x20000;
	}

	if(lpItem->Is380Item() != 0)
	{
		pMsg.ItemInfo |= 0x40000;
	}
	pMsg.level = (BYTE)lpItem->m_Level;
	for(int n=0;n < MAX_PARTY_USER;n++)
	{
		int index = gParty.m_PartyInfo[lpObj->PartyNumber].Index[n];

		if(OBJECT_RANGE(index) != 0)
		{
			DataSend(index,(BYTE*)&pMsg,pMsg.header.size);
		}
	}
}

void GCEffectInfoSend(int aIndex,BYTE effect) // OK
{
	PMSG_EFFECT_INFO_SEND pMsg;
	pMsg.header.set(0x48,sizeof(pMsg));
	pMsg.index[0] = SET_NUMBERHB(aIndex);
	pMsg.index[1] = SET_NUMBERLB(aIndex);
	pMsg.effect = effect;

	if(effect == 17)
	{
		DataSend(aIndex,(BYTE*)&pMsg,pMsg.header.size);
	}
	MsgSendV2(&gObj[aIndex],(BYTE*)&pMsg,pMsg.header.size);
}

void GCGuildMasterQuestionSend(int aIndex) // OK
{
	LPOBJ lpObj = &gObj[aIndex];

	if(lpObj->Interface.use != 0)
	{
		return;
	}

	PBMSG_HEAD pMsg;
	pMsg.set(0x54,sizeof(pMsg));
	DataSend(aIndex,(BYTE*)&pMsg,pMsg.size);
}

void GCMonsterSkillSend(LPOBJ lpObj,LPOBJ lpTarget,int skill) // OK
{
	PMSG_MONSTER_SKILL_SEND pMsg;
	pMsg.header.set(0x69,sizeof(pMsg));
	pMsg.skill[0] = SET_NUMBERHB(skill);
	pMsg.skill[1] = SET_NUMBERLB(skill);
	pMsg.index = lpObj->Index;
	pMsg.target = lpTarget->Index;
	if(lpObj->Index == OBJECT_USER)
	{
		DataSend(lpObj->Index,(BYTE*)&pMsg,pMsg.header.size);
	}
	MsgSendV2(lpObj,(BYTE*)&pMsg,pMsg.header.size);
}

void GCMonsterDieSend(int aIndex,int bIndex,int experience,float damage,BYTE flag) // OK
{
	PMSG_REWARD_EXPERIENCE_SEND pMsg;
	pMsg.header.setE(0x9C,sizeof(pMsg));
	pMsg.index[0] = SET_NUMBERHB(bIndex) | (flag*0x80);
	pMsg.index[1] = SET_NUMBERLB(bIndex);
	pMsg.experience[0] = SET_NUMBERHW(experience);
	pMsg.experience[1] = SET_NUMBERLW(experience);
	pMsg.damage[0] = SET_NUMBERHB(GET_MAX_WORD_VALUE(damage));
	pMsg.damage[1] = SET_NUMBERLB(GET_MAX_WORD_VALUE(damage));
	pMsg.ViewDamageHP = (QWORD)damage;		//mo dame nghin ti
	DataSend(aIndex,(BYTE*)&pMsg,pMsg.header.size);
}

void GCRewardExperienceSend(int aIndex,int experience) // OK
{
	PMSG_REWARD_EXPERIENCE_SEND pMsg;
	pMsg.header.setE(0x9C,sizeof(pMsg));
	pMsg.index[0] = 0xFF;
	pMsg.index[1] = 0xFF;
	pMsg.experience[0] = SET_NUMBERHW(experience);
	pMsg.experience[1] = SET_NUMBERLW(experience);
	pMsg.damage[0] = 0;
	pMsg.damage[1] = 0;
	pMsg.ViewDamageHP = 0;
	DataSend(aIndex,(BYTE*)&pMsg,pMsg.header.size);
}

void GCEventEnterCountSend(int aIndex,BYTE EventType,BYTE EnterCount) // OK
{
	PMSG_EVENT_ENTER_COUNT_SEND pMsg;
	pMsg.header.setE(0x9F,sizeof(pMsg));
	pMsg.EventType = EventType;
	pMsg.EnterCount = EnterCount;
	DataSend(aIndex,(BYTE*)&pMsg,pMsg.header.size);
}

void GCPetItemInfoSend(int aIndex,BYTE type,BYTE flag,BYTE slot,BYTE level,DWORD experience,BYTE durability) // OK
{
	PMSG_PET_ITEM_INFO_SEND pMsg;
	pMsg.header.set(0xA9,sizeof(pMsg));
	pMsg.type = type;
	pMsg.flag = flag;
	pMsg.slot = slot;
	pMsg.level = level;
	pMsg.experience = experience;
	pMsg.durability = durability;
	DataSend(aIndex,(BYTE*)&pMsg,pMsg.header.size);
}

void GCMapServerMoveAuthSend(int aIndex,BYTE result) // OK
{
	PMSG_MAP_SERVER_MOVE_AUTH_SEND pMsg;
	pMsg.header.set(0xB1,0x01,sizeof(pMsg));
	pMsg.result = result;
	DataSend(aIndex,(BYTE*)&pMsg,pMsg.header.size);
}

void GCTaxInfoSend(int aIndex,BYTE type,BYTE rate) // OK
{
	PMSG_TAX_INFO_SEND pMsg;
	pMsg.header.set(0xB2,0x1A,sizeof(pMsg));
	pMsg.type = type;
	pMsg.rate = rate;
	DataSend(aIndex,(BYTE*)&pMsg,pMsg.header.size);
}

void GCCreationStateSend(int aIndex,BYTE state) // OK
{
	PMSG_CREATION_STATE_SEND pMsg;
	pMsg.header.set(0xB9,0x01,sizeof(pMsg));
	pMsg.index[0] = SET_NUMBERHB(aIndex);
	pMsg.index[1] = SET_NUMBERLB(aIndex);
	pMsg.state = state;
	MsgSendV2(&gObj[aIndex],(BYTE*)&pMsg,pMsg.header.size);
}

void GCMonsterAreaSkillSend(int aIndex,int MonsterClass,int sx,int sy,int tx,int ty) // OK
{
	PMSG_MONSTER_AREA_SKILL_SEND pMsg;
	pMsg.header.set(0xBD,0x0C,sizeof(pMsg));
	pMsg.MonsterClass[0] = SET_NUMBERHB(MonsterClass);
	pMsg.MonsterClass[1] = SET_NUMBERLB(MonsterClass);
	pMsg.sx = sx;
	pMsg.sy = sy;
	pMsg.tx = tx;
	pMsg.ty = ty;
	DataSend(aIndex,(BYTE*)&pMsg,pMsg.header.size);
}

void GCElementalDamageSend(int aIndex,int bIndex,BYTE attribute,int damage) // OK
{
}

void GCCharacterCreationEnableSend(int aIndex,BYTE flag,BYTE result) // OK
{
	PMSG_CHARACTER_CREATION_ENABLE_SEND pMsg;
	pMsg.header.set(0xDE,sizeof(pMsg));
	pMsg.flag = flag;
	pMsg.result = result;
	DataSend(aIndex,(BYTE*)&pMsg,pMsg.header.size);
}

void GCLifeUpdateSend(LPOBJ lpObj) // OK
{
}

void GCCharacterAttackSpeedSend(LPOBJ lpObj) // OK
{
}

void GCConnectClientSend(int aIndex,BYTE result) // OK
{
	PMSG_CONNECT_CLIENT_SEND pMsg;
	pMsg.header.set(0xF1,0x00,sizeof(pMsg));
	pMsg.result = result;
	pMsg.index[0] = SET_NUMBERHB(aIndex);
	pMsg.index[1] = SET_NUMBERLB(aIndex);
	pMsg.ClientVersion[0] = gServerInfo.m_ServerVersion[0];
	pMsg.ClientVersion[1] = gServerInfo.m_ServerVersion[1];
	pMsg.ClientVersion[2] = gServerInfo.m_ServerVersion[2];
	pMsg.ClientVersion[3] = gServerInfo.m_ServerVersion[3];
	pMsg.ClientVersion[4] = gServerInfo.m_ServerVersion[4];
	DataSend(aIndex,(BYTE*)&pMsg,pMsg.header.size);
	gObj[aIndex].ConnectTickCount = GetTickCount();
}
#if(REGISTER_INGAME)
void GCRegisterAccountSend(int aIndex, BYTE result) // OK
{
	PMSG_CONNECT_ACCOUNT_SEND pMsg;
	pMsg.header.set(0xF1, 0x12, sizeof(pMsg));
	pMsg.result = result;
	DataSend(aIndex, (BYTE*)&pMsg, pMsg.header.size);
}
#endif
void GCConnectAccountSend(int aIndex, BYTE result) // OK
{
	PMSG_CONNECT_ACCOUNT_SEND pMsg;
	pMsg.header.set(0xF1,0x01,sizeof(pMsg));
	pMsg.result = result;
	DataSend(aIndex,(BYTE*)&pMsg,pMsg.header.size);
}

void GCConnectAccountSend(int aIndex,BYTE result,SOCKET socket) // OK
{
	PMSG_CONNECT_ACCOUNT_SEND pMsg;
	pMsg.header.set(0xF1,0x01,sizeof(pMsg));
	pMsg.result = result;
	DataSendSocket(socket,(BYTE*)&pMsg,pMsg.header.size);
}

void GCCloseClientSend(int aIndex,BYTE result) // OK
{
	PMSG_CLOSE_CLIENT_SEND pMsg;
	pMsg.header.setE(0xF1,0x02,sizeof(pMsg));
	pMsg.result = result;
	DataSend(aIndex,(BYTE*)&pMsg,pMsg.header.size);
}

void GCCharacterRegenSend(LPOBJ lpObj) // OK
{
	PMSG_CHARACTER_REGEN_SEND pMsg;
	pMsg.header.setE(0xF3,0x04,sizeof(pMsg));
	pMsg.X = (BYTE)lpObj->X;
	pMsg.Y = (BYTE)lpObj->Y;
	pMsg.Map = lpObj->Map;
	pMsg.Dir = lpObj->Dir;
	pMsg.Life = GET_MAX_WORD_VALUE(lpObj->Life);
	pMsg.Mana = GET_MAX_WORD_VALUE(lpObj->Mana);
	pMsg.Shield = GET_MAX_WORD_VALUE(lpObj->Shield);
	pMsg.BP = GET_MAX_WORD_VALUE(lpObj->BP);

	if(gMasterSkillTree.CheckMasterLevel(lpObj) == 0)
	{
		pMsg.Experience[0] = SET_NUMBERHB(SET_NUMBERHW(SET_NUMBERHDW(lpObj->Experience)));
		pMsg.Experience[1] = SET_NUMBERLB(SET_NUMBERHW(SET_NUMBERHDW(lpObj->Experience)));
		pMsg.Experience[2] = SET_NUMBERHB(SET_NUMBERLW(SET_NUMBERHDW(lpObj->Experience)));
		pMsg.Experience[3] = SET_NUMBERLB(SET_NUMBERLW(SET_NUMBERHDW(lpObj->Experience)));
		pMsg.Experience[4] = SET_NUMBERHB(SET_NUMBERHW(SET_NUMBERLDW(lpObj->Experience)));
		pMsg.Experience[5] = SET_NUMBERLB(SET_NUMBERHW(SET_NUMBERLDW(lpObj->Experience)));
		pMsg.Experience[6] = SET_NUMBERHB(SET_NUMBERLW(SET_NUMBERLDW(lpObj->Experience)));
		pMsg.Experience[7] = SET_NUMBERLB(SET_NUMBERLW(SET_NUMBERLDW(lpObj->Experience)));
	}
	else
	{
		pMsg.Experience[0] = SET_NUMBERHB(SET_NUMBERHW(SET_NUMBERHDW(lpObj->MasterExperience)));
		pMsg.Experience[1] = SET_NUMBERLB(SET_NUMBERHW(SET_NUMBERHDW(lpObj->MasterExperience)));
		pMsg.Experience[2] = SET_NUMBERHB(SET_NUMBERLW(SET_NUMBERHDW(lpObj->MasterExperience)));
		pMsg.Experience[3] = SET_NUMBERLB(SET_NUMBERLW(SET_NUMBERHDW(lpObj->MasterExperience)));
		pMsg.Experience[4] = SET_NUMBERHB(SET_NUMBERHW(SET_NUMBERLDW(lpObj->MasterExperience)));
		pMsg.Experience[5] = SET_NUMBERLB(SET_NUMBERHW(SET_NUMBERLDW(lpObj->MasterExperience)));
		pMsg.Experience[6] = SET_NUMBERHB(SET_NUMBERLW(SET_NUMBERLDW(lpObj->MasterExperience)));
		pMsg.Experience[7] = SET_NUMBERLB(SET_NUMBERLW(SET_NUMBERLDW(lpObj->MasterExperience)));
	}

	pMsg.Money = lpObj->Money;
	pMsg.ViewCurHP = (DWORD)(lpObj->Life);
	pMsg.ViewCurMP = (DWORD)(lpObj->Mana);
	pMsg.ViewCurBP = (DWORD)(lpObj->BP);
	pMsg.ViewCurSD = (DWORD)(lpObj->Shield);
	DataSend(lpObj->Index,(BYTE*)&pMsg,pMsg.header.size);
}

void GCLevelUpSend(LPOBJ lpObj) // OK
{
	PMSG_LEVEL_UP_SEND pMsg;
	pMsg.header.set(0xF3,0x05,sizeof(pMsg));
	pMsg.Level = lpObj->Level;
	pMsg.LevelUpPoint = lpObj->LevelUpPoint;
	pMsg.MaxLife = GET_MAX_WORD_VALUE((lpObj->MaxLife+lpObj->AddLife));
	pMsg.MaxMana = GET_MAX_WORD_VALUE((lpObj->MaxMana+lpObj->AddMana));
	pMsg.MaxShield = GET_MAX_WORD_VALUE((lpObj->MaxShield+lpObj->AddShield));
	pMsg.MaxBP = GET_MAX_WORD_VALUE((lpObj->MaxBP+lpObj->AddBP));
	pMsg.FruitAddPoint = lpObj->FruitAddPoint;
	pMsg.MaxFruitAddPoint = gFruit.GetMaxFruitPoint(lpObj);
	pMsg.FruitSubPoint = lpObj->FruitSubPoint;
	pMsg.MaxFruitSubPoint = gFruit.GetMaxFruitPoint(lpObj);
	pMsg.ViewPoint = (DWORD)(lpObj->LevelUpPoint);
	pMsg.ViewMaxHP = (DWORD)(lpObj->MaxLife+lpObj->AddLife);
	pMsg.ViewMaxMP = (DWORD)(lpObj->MaxMana+lpObj->AddMana);
	pMsg.ViewMaxBP = (DWORD)(lpObj->MaxBP+lpObj->AddBP);
	pMsg.ViewMaxSD = (DWORD)(lpObj->MaxShield+lpObj->AddShield);
	pMsg.ViewExperience = lpObj->Experience;
	pMsg.ViewNextExperience = lpObj->NextExperience;
	DataSend(lpObj->Index,(BYTE*)&pMsg,pMsg.header.size);
	GCEffectInfoSend(lpObj->Index,16);
}

void GCMonsterDamageSend(int aIndex,float damage,float ShieldDamage) // OK
{
	PMSG_MONSTER_DAMAGE_SEND pMsg;
	pMsg.header.set(0xF3,0x07,sizeof(pMsg));
	pMsg.damage[0] = SET_NUMBERHB(GET_MAX_WORD_VALUE(damage));
	pMsg.damage[1] = SET_NUMBERLB(GET_MAX_WORD_VALUE(damage));
	pMsg.ShieldDamage[0] = SET_NUMBERHB(GET_MAX_WORD_VALUE(ShieldDamage));
	pMsg.ShieldDamage[1] = SET_NUMBERLB(GET_MAX_WORD_VALUE(ShieldDamage));
	pMsg.ViewCurHP = (DWORD)gObj[aIndex].Life;
	pMsg.ViewCurSD = (DWORD)gObj[aIndex].Shield;
	pMsg.ViewDamageHP = (QWORD)damage;				//mo dame nghin ti
	pMsg.ViewDamageSD = (QWORD)ShieldDamage;
	if(gObj[aIndex].Type == OBJECT_USER)
	{
		DataSend(aIndex,(BYTE*)&pMsg,pMsg.header.size);
	}
}

void GCPKLevelSend(int aIndex,int PKLevel) // OK
{
	PMSG_PK_LEVEL_SEND pMsg;
	pMsg.header.set(0xF3,0x08,sizeof(pMsg));
	pMsg.index[0] = SET_NUMBERHB(aIndex);
	pMsg.index[1] = SET_NUMBERLB(aIndex);
	pMsg.PKLevel = PKLevel;
	DataSend(aIndex,(BYTE*)&pMsg,pMsg.header.size);
	MsgSendV2(&gObj[aIndex],(BYTE*)&pMsg,pMsg.header.size);
}

void GCSummonLifeSend(int aIndex,int life,int MaxLife) // OK
{
	if(MaxLife <= 0)
	{
		return;
	}
	PMSG_SUMMON_LIFE_SEND pMsg;
	pMsg.header.set(0xF3,0x20,sizeof(pMsg));
	pMsg.life = (life*100)/MaxLife;
	DataSend(aIndex,(BYTE*)&pMsg,pMsg.header.size);
}

void GCTimeViewSend(int aIndex,int time) // OK
{
	PMSG_TIME_VIEW_SEND pMsg;
	pMsg.header.set(0xF3,0x22,sizeof(pMsg));
	pMsg.time = time;
	DataSend(aIndex,(BYTE*)&pMsg,pMsg.header.size);
}

void GCMathAuthenticatorSend(int aIndex) // OK
{
}

void GCFireworksSend(LPOBJ lpObj,int x,int y) // OK
{
	PMSG_FIREWORKS_SEND pMsg;
	pMsg.header.set(0xF3,0x40,sizeof(pMsg));
	pMsg.type = 0;
	pMsg.x = x;
	pMsg.y = y;
	DataSend(lpObj->Index,(BYTE*)&pMsg,pMsg.header.size);
	MsgSendV2(lpObj,(BYTE*)&pMsg,pMsg.header.size);
}

void GCServerCommandSend(int aIndex,BYTE type,BYTE cmd1,BYTE cmd2) // OK
{
	PMSG_SERVER_COMMAND_SEND pMsg;
	pMsg.header.set(0xF3,0x40,sizeof(pMsg));
	pMsg.type = type;
	pMsg.cmd1 = cmd1;
	pMsg.cmd2 = cmd2;
	DataSend(aIndex,(BYTE*)&pMsg,pMsg.header.size);
}

void GCNewCharacterInfoSend(LPOBJ lpObj) // OK
{
	PMSG_NEW_CHARACTER_INFO_SEND pMsg;
	pMsg.header.set(0xF3,0xE0,sizeof(pMsg));
	pMsg.Level = lpObj->Level;
	pMsg.LevelUpPoint = lpObj->LevelUpPoint;
	pMsg.Experience = lpObj->Experience;
	pMsg.NextExperience = lpObj->NextExperience; //tue fix
	pMsg.Strength = GET_MAX_WORD_VALUE(lpObj->Strength);
	pMsg.Dexterity = GET_MAX_WORD_VALUE(lpObj->Dexterity);
	pMsg.Vitality = GET_MAX_WORD_VALUE(lpObj->Vitality);
	pMsg.Energy = GET_MAX_WORD_VALUE(lpObj->Energy);
	pMsg.Leadership = GET_MAX_WORD_VALUE(lpObj->Leadership);
	pMsg.Life = GET_MAX_WORD_VALUE(lpObj->Life);
	pMsg.MaxLife = GET_MAX_WORD_VALUE((lpObj->MaxLife+lpObj->AddLife));
	pMsg.Mana = GET_MAX_WORD_VALUE(lpObj->Mana);
	pMsg.MaxMana = GET_MAX_WORD_VALUE((lpObj->MaxMana+lpObj->AddMana));
	pMsg.BP = GET_MAX_WORD_VALUE(lpObj->BP);
	pMsg.MaxBP = GET_MAX_WORD_VALUE((lpObj->MaxBP+lpObj->AddBP));
	pMsg.Shield = GET_MAX_WORD_VALUE(lpObj->Shield);
	pMsg.MaxShield = GET_MAX_WORD_VALUE((lpObj->MaxShield+lpObj->AddShield));
	pMsg.FruitAddPoint = lpObj->FruitAddPoint;
	pMsg.MaxFruitAddPoint = gFruit.GetMaxFruitPoint(lpObj);
	pMsg.FruitSubPoint = lpObj->FruitSubPoint;
	pMsg.MaxFruitSubPoint = gFruit.GetMaxFruitPoint(lpObj);
	pMsg.ViewReset = (DWORD)(lpObj->Reset);
	pMsg.ViewPoint = (DWORD)(lpObj->LevelUpPoint);
	pMsg.ViewCurHP = (DWORD)(lpObj->Life);
	pMsg.ViewMaxHP = (DWORD)(lpObj->MaxLife+lpObj->AddLife);
	pMsg.ViewCurMP = (DWORD)(lpObj->Mana);
	pMsg.ViewMaxMP = (DWORD)(lpObj->MaxMana+lpObj->AddMana);
	pMsg.ViewCurBP = (DWORD)(lpObj->BP);
	pMsg.ViewMaxBP = (DWORD)(lpObj->MaxBP+lpObj->AddBP);
	pMsg.ViewCurSD = (DWORD)(lpObj->Shield);
	pMsg.ViewMaxSD = (DWORD)(lpObj->MaxShield+lpObj->AddShield);
	pMsg.ViewStrength = lpObj->Strength;
	pMsg.ViewDexterity = lpObj->Dexterity;
	pMsg.ViewVitality = lpObj->Vitality;
	pMsg.ViewEnergy = lpObj->Energy;
	pMsg.ViewLeadership = lpObj->Leadership;
	DataSend(lpObj->Index,(BYTE*)&pMsg,pMsg.header.size);
}

void GCNewCharacterCalcSend(LPOBJ lpObj) // OK
{
	PMSG_NEW_CHARACTER_CALC_SEND pMsg;
	pMsg.header.set(0xF3,0xE1,sizeof(pMsg));
	pMsg.ViewCurHP = (DWORD)(lpObj->Life);
	pMsg.ViewMaxHP = (DWORD)(lpObj->MaxLife+lpObj->AddLife);
	pMsg.ViewCurMP = (DWORD)(lpObj->Mana);
	pMsg.ViewMaxMP = (DWORD)(lpObj->MaxMana+lpObj->AddMana);
	pMsg.ViewCurBP = (DWORD)(lpObj->BP);
	pMsg.ViewMaxBP = (DWORD)(lpObj->MaxBP+lpObj->AddBP);
	pMsg.ViewCurSD = (DWORD)(lpObj->Shield);
	pMsg.ViewMaxSD = (DWORD)(lpObj->MaxShield+lpObj->AddShield); //tue add
	pMsg.ViewAddStrength = lpObj->AddStrength;
	pMsg.ViewAddDexterity = lpObj->AddDexterity;
	pMsg.ViewAddVitality = lpObj->AddVitality;
	pMsg.ViewAddEnergy = lpObj->AddEnergy;
	pMsg.ViewAddLeadership = lpObj->AddLeadership;
	gAttack.GetPreviewPhysiDamage(lpObj,&pMsg.ViewPhysiDamageMin,&pMsg.ViewPhysiDamageMax,&pMsg.ViewMulPhysiDamage,&pMsg.ViewDivPhysiDamage);
	gAttack.GetPreviewMagicDamage(lpObj,&pMsg.ViewMagicDamageMin,&pMsg.ViewMagicDamageMax,&pMsg.ViewMulMagicDamage,&pMsg.ViewDivMagicDamage,&pMsg.ViewMagicDamageRate);
	gAttack.GetPreviewCurseDamage(lpObj,&pMsg.ViewCurseDamageMin,&pMsg.ViewCurseDamageMax,&pMsg.ViewMulCurseDamage,&pMsg.ViewDivCurseDamage,&pMsg.ViewCurseDamageRate);
	pMsg.ViewPhysiSpeed = lpObj->PhysiSpeed;
	pMsg.ViewMagicSpeed = lpObj->MagicSpeed;
	pMsg.ViewAttackSuccessRate = lpObj->AttackSuccessRate;
	pMsg.ViewAttackSuccessRate += lpObj->EffectOption.AddAttackSuccessRate;
	pMsg.ViewAttackSuccessRate += (pMsg.ViewAttackSuccessRate*lpObj->EffectOption.MulAttackSuccessRate)/100;
	pMsg.ViewAttackSuccessRate -= (pMsg.ViewAttackSuccessRate*lpObj->EffectOption.DivAttackSuccessRate)/100;
	pMsg.ViewAttackSuccessRatePvP = lpObj->AttackSuccessRatePvP;
	gAttack.GetPreviewDefense(lpObj,&pMsg.ViewDefense);
	pMsg.ViewDefenseSuccessRate = lpObj->DefenseSuccessRate;
	pMsg.ViewDefenseSuccessRate += lpObj->EffectOption.AddDefenseSuccessRate;
	pMsg.ViewDefenseSuccessRate += (pMsg.ViewDefenseSuccessRate*lpObj->EffectOption.MulDefenseSuccessRate)/100;
	pMsg.ViewDefenseSuccessRate -= (pMsg.ViewDefenseSuccessRate*lpObj->EffectOption.DivDefenseSuccessRate)/100;
	pMsg.ViewDefenseSuccessRatePvP = lpObj->DefenseSuccessRatePvP;
	gAttack.GetPreviewDamageMultiplier(lpObj,&pMsg.ViewDamageMultiplier,&pMsg.ViewRFDamageMultiplierA,&pMsg.ViewRFDamageMultiplierB,&pMsg.ViewRFDamageMultiplierC);
	gDarkSpirit[lpObj->Index].GetPreviewAttackDamage(lpObj,&pMsg.ViewDarkSpiritAttackDamageMin,&pMsg.ViewDarkSpiritAttackDamageMax,&pMsg.ViewDarkSpiritAttackSpeed,&pMsg.ViewDarkSpiritAttackSuccessRate);
	DataSend(lpObj->Index,(BYTE*)&pMsg,pMsg.header.size);
}

void GCNewHealthBarSend(LPOBJ lpObj) // OK
{
	if(gServerInfo.m_MonsterHealthBarSwitch == 0 && gServerInfo.m_UserHealthBarSwitch == 0)
	{
		return;
	}
	BYTE send[4096];
	PMSG_NEW_HEALTH_BAR_SEND pMsg;
	pMsg.header.set(0xF3,0xE2,0);
	int size = sizeof(pMsg);
	pMsg.count = 0;
	PMSG_NEW_HEALTH_BAR info;
	for(int n=0;n < MAX_VIEWPORT;n++)
	{
		if(lpObj->VpPlayer[n].state != VIEWPORT_SEND && lpObj->VpPlayer[n].state != VIEWPORT_WAIT)
		{
			continue;
		}

		if(lpObj->VpPlayer[n].type == OBJECT_MONSTER && gServerInfo.m_MonsterHealthBarSwitch == 0)
		{
			continue;
		}

		if(lpObj->VpPlayer[n].type == OBJECT_USER && gServerInfo.m_UserHealthBarSwitch == 0)
		{
			continue;
		}

		if(OBJECT_RANGE(lpObj->VpPlayer[n].index) == 0)
		{
			continue;
		}

		LPOBJ lpTarget = &gObj[lpObj->VpPlayer[n].index];

		if(lpTarget->Live == 0 || lpTarget->State != OBJECT_PLAYING || OBJECT_RANGE(lpTarget->SummonIndex) != 0 || CC_MAP_RANGE(lpTarget->Map) != 0)
		{
			continue;
		}

		info.index = lpTarget->Index;
		info.type = (BYTE)lpTarget->Type;
		info.rate = (BYTE)((lpTarget->Life*100) / (lpTarget->MaxLife+lpTarget->AddLife));
		info.Level = lpTarget->Level;
		info.Life = lpTarget->Life;
		info.MonsID = lpTarget->Class;

		if(lpObj->VpPlayer[n].type == OBJECT_USER)
		{
			info.rate2 = (BYTE)((lpTarget->Shield*100)/(lpTarget->MaxShield+lpTarget->AddShield));
		}
		else
		{
			info.rate2 = 0;
		}

		memcpy(&send[size],&info,sizeof(info));
		size += sizeof(info);
		pMsg.count++;
	}
	pMsg.header.size[0] = SET_NUMBERHB(size);
	pMsg.header.size[1] = SET_NUMBERLB(size);
	memcpy(send,&pMsg,sizeof(pMsg));
	DataSend(lpObj->Index,send,size);
}

void GCNewGensBattleInfoSend(LPOBJ lpObj) // OK
{
	PMSG_NEW_GENS_BATTLE_INFO_SEND pMsg;
	pMsg.header.set(0xF3,0xE3,sizeof(pMsg));
	pMsg.GensBattleMapCount = gGensSystem.GetGensBattleMap(pMsg.GensBattleMap);
	pMsg.GensMoveIndexCount = gGensSystem.GetGensMoveIndex(pMsg.GensMoveIndex);
	DataSend(lpObj->Index,(BYTE*)&pMsg,pMsg.header.size);
}

void GCNewMessageSend(LPOBJ lpObj,char* message) // OK
{
	PMSG_NEW_MESSAGE_SEND pMsg;
	pMsg.header.set(0xF3,0xE4,sizeof(pMsg));
	memcpy(pMsg.message,message,sizeof(pMsg.message));
	DataSend(lpObj->Index,(BYTE*)&pMsg,pMsg.header.size);
}

void CGReqCastleSiegeState(PMSG_REQ_CASTLESIEGESTATE * lpMsg, int iIndex) 
{
	#if(GAMESERVER_TYPE==1)
	GS_GDReqOwnerGuildMaster(gMapServerManager.GetMapServerGroup(), iIndex);
	#endif
}

void GCAnsCastleSiegeState(int iIndex, int iResult, LPSTR lpszGuildName, LPSTR lpszGuildMaster)
{
	#if (GAMESERVER_TYPE==1)
	if( (lpszGuildName == NULL) || (lpszGuildMaster == NULL) )
	{
		return;
	}

	PMSG_ANS_CASTLESIEGESTATE pMsgResult;
	pMsgResult.h.set(0xB2, 0x00, sizeof(pMsgResult));
	pMsgResult.btResult = iResult;
	memcpy(pMsgResult.cOwnerGuild, lpszGuildName, sizeof(pMsgResult.cOwnerGuild));
	memcpy(pMsgResult.cOwnerGuildMaster, lpszGuildMaster, sizeof(pMsgResult.cOwnerGuildMaster));
	SYSTEMTIME tmStateStartDate = {0};
	SYSTEMTIME tmStateEndDate = {0};
	SYSTEMTIME tmSiegeStartDate = {0};
	int iCastleState = gCastleSiege.GetCastleState();
	int bRETVAL = gCastleSiege.GetCastleStateTerm(&tmStateStartDate, &tmStateEndDate);
	tmSiegeStartDate = gCastleSiege.GetCastleLeftSiegeDate();

	if( bRETVAL == FALSE )
	{
		iCastleState = -1;
	}
	int iStateLeftSec = gCastleSiege.GetCurRemainSec();
	pMsgResult.cCastleSiegeState = iCastleState;

	pMsgResult.btStartYearH = SET_NUMBERHB(tmStateStartDate.wYear);
	pMsgResult.btStartYearL = SET_NUMBERLB(tmStateStartDate.wYear);
	pMsgResult.btStartMonth = (BYTE)tmStateStartDate.wMonth;
	pMsgResult.btStartDay   = (BYTE)tmStateStartDate.wDay;
	pMsgResult.btStartHour  = (BYTE)tmStateStartDate.wHour;
	pMsgResult.btStartMinute= (BYTE)tmStateStartDate.wMinute;

	pMsgResult.btEndYearH = SET_NUMBERHB(tmStateEndDate.wYear);
	pMsgResult.btEndYearL = SET_NUMBERLB(tmStateEndDate.wYear);
	pMsgResult.btEndMonth = (BYTE)tmStateEndDate.wMonth;
	pMsgResult.btEndDay   = (BYTE)tmStateEndDate.wDay;
	pMsgResult.btEndHour  = (BYTE)tmStateEndDate.wHour;
	pMsgResult.btEndMinute= (BYTE)tmStateEndDate.wMinute;

	pMsgResult.btSiegeStartYearH = SET_NUMBERHB(tmSiegeStartDate.wYear);
	pMsgResult.btSiegeStartYearL = SET_NUMBERLB(tmSiegeStartDate.wYear);
	pMsgResult.btSiegeStartMonth = (BYTE)tmSiegeStartDate.wMonth;
	pMsgResult.btSiegeStartDay   = (BYTE)tmSiegeStartDate.wDay;
	pMsgResult.btSiegeStartHour  = (BYTE)tmSiegeStartDate.wHour;
	pMsgResult.btSiegeStartMinute= (BYTE)tmSiegeStartDate.wMinute;

	pMsgResult.btStateLeftSec1 = SET_NUMBERHB(SET_NUMBERHW(iStateLeftSec));
	pMsgResult.btStateLeftSec2 = SET_NUMBERLB(SET_NUMBERHW(iStateLeftSec));
	pMsgResult.btStateLeftSec3 = SET_NUMBERHB(SET_NUMBERLW(iStateLeftSec));
	pMsgResult.btStateLeftSec4 = SET_NUMBERLB(SET_NUMBERLW(iStateLeftSec));

	DataSend(iIndex, (LPBYTE)&pMsgResult, pMsgResult.h.size);
	#endif
}

void CGReqRegCastleSiege(PMSG_REQ_REGCASTLESIEGE * lpMsg, int iIndex) 
{
	#if (GAMESERVER_TYPE==1)
	if( gCastleSiege.GetCastleState() != CASTLESIEGE_STATE_REGSIEGE )
	{
		GCAnsRegCastleSiege(iIndex, 7, "");
		return;
	}
	if( strcmp(gObj[iIndex].GuildName, "") == 0 )
	{
		GCAnsRegCastleSiege(iIndex, 6, "");
		return;
	}
	if( gCastleSiege.CheckUnionGuildMaster(iIndex) == FALSE )
	{
		GCAnsRegCastleSiege(iIndex, 0, "");
	}
	else if( (gCastleSiege.CheckCastleOwnerMember(iIndex) != FALSE) || 
		(gCastleSiege.CheckCastleOwnerUnionMember(iIndex) != FALSE) )
	{
		GCAnsRegCastleSiege(iIndex, 3, "");
	}
	else
	{
		GS_GDReqRegAttackGuild(gMapServerManager.GetMapServerGroup(), iIndex);
	}
	#endif
}

void GCAnsRegCastleSiege(int iIndex, int iResult, LPSTR lpszGuildName) 
{
	#if(GAMESERVER_TYPE==1)
	PMSG_ANS_REGCASTLESIEGE pMsgResult;

	if( lpszGuildName == NULL )
	{
		return;
	}

	if( OBJECT_RANGE(iIndex) == FALSE )
	{
		return;
	}

	pMsgResult.h.set(0xB2, 0x01, sizeof(pMsgResult));
	pMsgResult.btResult = iResult;
	memcpy(&pMsgResult.szGuildName, lpszGuildName, sizeof(pMsgResult.szGuildName));

	DataSend(iIndex, (LPBYTE)&pMsgResult, pMsgResult.h.size);

	if( ::gObjIsConnected(iIndex) )
	{
		LogAdd(LOG_BLACK,"[CastleSiege] [%s][%s] Registered Castle Siege (GUILD:%s) - Result:%d",	gObj[iIndex].Account, gObj[iIndex].Name, gObj[iIndex].GuildName, iResult);
	}
	#endif
}

void CGReqGiveUpCastleSiege(PMSG_REQ_GIVEUPCASTLESIEGE * lpMsg, int iIndex) 
{
	#if(GAMESERVER_TYPE==1)
	if( (gCastleSiege.GetCastleState() < CASTLESIEGE_STATE_REGSIEGE) || (gCastleSiege.GetCastleState() > CASTLESIEGE_STATE_REGMARK) ) //Fixed
	{
		GCAnsGiveUpCastleSiege(iIndex, 3, 0, 0, "");
		return;
	}

	if( strcmp(gObj[iIndex].GuildName, "") == 0 )
	{
		return;
	}
	
	if( gCastleSiege.CheckUnionGuildMaster(iIndex) == FALSE )
	{
		return;
	}

	GS_GDReqGuildSetGiveUp(gMapServerManager.GetMapServerGroup(), iIndex, lpMsg->btGiveUp);
	#endif
}

void GCAnsGiveUpCastleSiege(int iIndex, int iResult, int bGiveUp, int iMarkCount, LPSTR lpszGuildName)
{
	#if(GAMESERVER_TYPE==1)
	PMSG_ANS_GIVEUPCASTLESIEGE pMsgResult;

	if( lpszGuildName == NULL )
	{
		return;
	}

	if( OBJECT_RANGE(iIndex) == FALSE )
	{
		return;
	}

	pMsgResult.h.set(0xB2, 0x02, sizeof(pMsgResult));
	pMsgResult.btResult = iResult;
	pMsgResult.btIsGiveUp = bGiveUp;

	memcpy(&pMsgResult.szGuildName, lpszGuildName, sizeof(pMsgResult.szGuildName));

	if( (pMsgResult.btResult == 1) && (iMarkCount > 0) )
	{
		gObj[iIndex].Money += (iMarkCount*3000);

		if( gObj[iIndex].Money > MAX_MONEY ) 
		{
			gObj[iIndex].Money = MAX_MONEY;
		}
		GCMoneySend(iIndex, gObj[iIndex].Money);
	}
	DataSend(iIndex, (LPBYTE)&pMsgResult, pMsgResult.h.size);

	if( gObjIsConnected(iIndex) )
	{
		LogAdd(LOG_BLACK,"[CastleSiege] [%s][%s] GiveUp Castle Siege (GUILD:%s) - Result:%d", gObj[iIndex].Account, gObj[iIndex].Name, gObj[iIndex].GuildName, iResult);
	}
	#endif
}

void CGReqGuildRegInfo(PMSG_REQ_GUILDREGINFO * lpMsg, int iIndex) 
{
	#if (GAMESERVER_TYPE==1)
	if( lpMsg == NULL )
	{
		return;
	}

	if( strcmp(gObj[iIndex].GuildName, "") == 0 )
	{
		return;
	}

	GS_GDReqGuildMarkRegInfo(gMapServerManager.GetMapServerGroup(), iIndex);
	#endif
}

void GCAnsGuildRegInfo(int iIndex, int iResult, CSP_ANS_GUILDREGINFO* lpMsgResult)
{
	#if (GAMESERVER_TYPE==1)
	PMSG_ANS_GUILDREGINFO pMsgResult;

	if( lpMsgResult == NULL )
	{
		return;
	}

	if( OBJECT_RANGE(iIndex) == FALSE )
	{
		return;
	}

	pMsgResult.h.set(0xB2, 3, sizeof(pMsgResult));

	pMsgResult.btResult = iResult;
	pMsgResult.btGuildMark1 = SET_NUMBERHB(SET_NUMBERHW(lpMsgResult->iRegMarkCount));
	pMsgResult.btGuildMark2 = SET_NUMBERLB(SET_NUMBERHW(lpMsgResult->iRegMarkCount));
	pMsgResult.btGuildMark3 = SET_NUMBERHB(SET_NUMBERLW(lpMsgResult->iRegMarkCount));
	pMsgResult.btGuildMark4 = SET_NUMBERLB(SET_NUMBERLW(lpMsgResult->iRegMarkCount));
	pMsgResult.btRegRank = lpMsgResult->btRegRank;
	pMsgResult.btIsGiveUp = lpMsgResult->bIsGiveUp;
	memcpy(&pMsgResult.szGuildName, lpMsgResult->szGuildName, sizeof(pMsgResult.szGuildName));

	DataSend(iIndex, (LPBYTE)&pMsgResult, pMsgResult.h.size);
	#endif
}

void CGReqRegGuildMark(PMSG_REQ_REGGUILDMARK * lpMsg, int iIndex) 
{
	#if (GAMESERVER_TYPE==1)
	if( lpMsg == NULL )
	{
		return;
	}
	
	if( OBJECT_RANGE(iIndex) == FALSE )
	{
		return;
	}
	
	if( gObj[iIndex].UseEventServer == TRUE )
	{
		return;
	}

	gObj[iIndex].UseEventServer = TRUE;

	if( gCastleSiege.GetCastleState() != CASTLESIEGE_STATE_REGMARK )
	{
		gObj[iIndex].UseEventServer = FALSE;
		return;
	}
	if( strcmp( gObj[iIndex].GuildName, "") == 0)
	{
		gObj[iIndex].UseEventServer = FALSE;
		return;
	}
	
	if(lpMsg->btItemPos < 0 || lpMsg->btItemPos >= 64)
	{
		PMSG_ANS_REGGUILDMARK pMsgResult = {0};
		pMsgResult.h.set(0xB2, 0x04, sizeof(pMsgResult));
		pMsgResult.btResult = 3;		
		DataSend(iIndex, (LPBYTE)&pMsgResult, pMsgResult.h.size);
		gObj[iIndex].UseEventServer = FALSE;
	}
	else
	{
		int iItemPos = (lpMsg->btItemPos + 12);

		if( gObj[iIndex].Inventory[iItemPos].IsItem() == FALSE )
		{
			PMSG_ANS_REGGUILDMARK pMsgResult = {0};
			pMsgResult.h.set(0xB2, 0x04, sizeof(pMsgResult));
			pMsgResult.btResult = 3;		
			DataSend(iIndex, (LPBYTE)&pMsgResult, pMsgResult.h.size);
			gObj[iIndex].UseEventServer = FALSE;
		}
		else
		{
			if( gObj[iIndex].Inventory[iItemPos].m_Index != GET_ITEM(14,21) || 
				gObj[iIndex].Inventory[iItemPos].m_Level != 3 || 
				gObj[iIndex].Inventory[iItemPos].m_Durability <= 0.0f )
			{
				PMSG_ANS_REGGUILDMARK pMsgResult = {0};
				pMsgResult.h.set(0xB2, 0x04, sizeof(pMsgResult));
				pMsgResult.btResult = 3;		
				DataSend(iIndex, (LPBYTE)&pMsgResult, pMsgResult.h.size);
				gObj[iIndex].UseEventServer = FALSE;
			}
			else
			{
				GS_GDReqRegGuildMark(gMapServerManager.GetMapServerGroup(), iIndex, iItemPos);
			}
		}
	}
	#endif
}

void GCAnsRegGuildMark(int iIndex, int iResult, CSP_ANS_GUILDREGMARK* lpMsgResult) 
{
	#if (GAMESERVER_TYPE==1)
	if( lpMsgResult == NULL )
	{
		return;
	}

	if( OBJECT_RANGE(iIndex) == FALSE )
	{
		return;
	}

	if( gObj[iIndex].Inventory[lpMsgResult->iItemPos].IsItem() == FALSE )
	{	
		PMSG_ANS_REGGUILDMARK pMsgResult ={0};
		pMsgResult.h.set(0xB2, 0x04, sizeof(pMsgResult));
		pMsgResult.btResult = 3;
		DataSend(iIndex, (LPBYTE)&pMsgResult, pMsgResult.h.size);
		gObj[iIndex].UseEventServer = 0;
	}
	else
	{
		if( gObj[iIndex].Inventory[lpMsgResult->iItemPos].m_Index != GET_ITEM(14,21) && 
			gObj[iIndex].Inventory[lpMsgResult->iItemPos].m_Level == 3 || 
			gObj[iIndex].Inventory[lpMsgResult->iItemPos].m_Durability <= 0.0f ) 
		{
			PMSG_ANS_REGGUILDMARK pMsgResult = {0};
			pMsgResult.h.set(0xB2, 0x04, sizeof(pMsgResult));
			pMsgResult.btResult = 3;
			DataSend(iIndex, (LPBYTE)&pMsgResult, pMsgResult.h.size);
			gObj[iIndex].UseEventServer = FALSE;
		}
		else
		{
			if( (gObj[iIndex].Inventory[lpMsgResult->iItemPos].m_Durability -= 1.0) > 0.0f )
			{
				gItemManager.GCItemDurSend(iIndex, lpMsgResult->iItemPos, (BYTE)gObj[iIndex].Inventory[lpMsgResult->iItemPos].m_Durability, 0);
			}
			else
			{
				gItemManager.InventoryDelItem(iIndex, lpMsgResult->iItemPos);
				gItemManager.GCItemDeleteSend(iIndex,lpMsgResult->iItemPos,1);
			}
			
			PMSG_ANS_REGGUILDMARK pMsgResult;
			pMsgResult.h.set(0xB2, 0x04, sizeof(pMsgResult));
			
			pMsgResult.btResult = iResult;
			pMsgResult.btGuildMark1 = SET_NUMBERHB(SET_NUMBERHW(lpMsgResult->iRegMarkCount));
			pMsgResult.btGuildMark2 = SET_NUMBERLB(SET_NUMBERHW(lpMsgResult->iRegMarkCount));
			pMsgResult.btGuildMark3 = SET_NUMBERHB(SET_NUMBERLW(lpMsgResult->iRegMarkCount));
			pMsgResult.btGuildMark4 = SET_NUMBERLB(SET_NUMBERLW(lpMsgResult->iRegMarkCount));
			memcpy(&pMsgResult.szGuildName, lpMsgResult->szGuildName, sizeof(pMsgResult.szGuildName));
			DataSend(iIndex, (LPBYTE)&pMsgResult, pMsgResult.h.size);
			gObj[iIndex].UseEventServer = 0;
			
		}
	}
	#endif
}

void CGReqNpcBuy(PMSG_REQ_NPCBUY * lpMsg, int iIndex) 
{
	#if (GAMESERVER_TYPE==1)
	if( lpMsg == NULL )
	{
		return;
	}

	if( ::gObjIsConnected(iIndex) == FALSE )
	{
		return;
	}

	if( strcmp(gObj[iIndex].GuildName, "") == 0 )
	{
		return;
	}

	if( gCastleSiege.CheckGuildOwnCastle(gObj[iIndex].GuildName) == FALSE || (gObj[iIndex].GuildStatus != G_MASTER && gObj[iIndex].GuildStatus != G_SUB_MASTER) )
	{
		LogAdd(LOG_RED, "[CastleSiege] CGReqNpcBuy() ERROR - Authority Fail [%s][%s], Guild:(%s)(%d), Npc:(CLS:%d, IDX:%d)",	gObj[iIndex].Account, gObj[iIndex].Name, gObj[iIndex].GuildName, gObj[iIndex].GuildStatus,lpMsg->iNpcNumber,lpMsg->iNpcIndex);
	}
	else
	{
		BYTE btResult = 0;

		BOOL bRETVAL = gCastleSiege.CheckAddDbNPC(iIndex, lpMsg->iNpcNumber, lpMsg->iNpcIndex, btResult);

		if( bRETVAL == FALSE )
		{
			GCAnsNpcBuy(iIndex, btResult, lpMsg->iNpcNumber, lpMsg->iNpcIndex);
			LogAdd(LOG_RED, "[CastleSiege] CGReqNpcBuy() ERROR - CCastleSiege::CheckAddDbNPC() [%s][%s], Guild:(%s)(%d), Npc:(CLS:%d, IDX:%d)", gObj[iIndex].Account, gObj[iIndex].Name, gObj[iIndex].GuildName, gObj[iIndex].GuildStatus,lpMsg->iNpcNumber,lpMsg->iNpcIndex);
		}
		else
		{
			LogAdd(LOG_RED, "[CastleSiege] CGReqNpcBuy() OK - [%s][%s], Guild:(%s)(%d), Npc:(CLS:%d, IDX:%d)", gObj[iIndex].Account, gObj[iIndex].Name, gObj[iIndex].GuildName, gObj[iIndex].GuildStatus,lpMsg->iNpcNumber,lpMsg->iNpcIndex);
		}
	}
	#endif
}

void GCAnsNpcBuy(int iIndex, int iResult, int iNpcNumber, int iNpcIndex) 
{
	#if(GAMESERVER_TYPE==1)
	PMSG_ANS_NPCBUY pMsgResult;

	pMsgResult.h.set(0xB2, 0x05, sizeof(pMsgResult));
	pMsgResult.btResult = iResult;
	pMsgResult.iNpcNumber = iNpcNumber;
	pMsgResult.iNpcIndex = iNpcIndex;
	DataSend(iIndex, (LPBYTE)&pMsgResult, pMsgResult.h.size);
	#endif
}

void CGReqNpcRepair(PMSG_REQ_NPCREPAIR * lpMsg, int iIndex) 
{
	#if (GAMESERVER_TYPE==1)
	if( lpMsg == NULL )
	{
		return;
	}

	if( ::gObjIsConnected(iIndex) == FALSE )
	{
		return;
	}

	if( strcmp(gObj[iIndex].GuildName, "") == 0 )
	{
		return;
	}

	if( (gCastleSiege.CheckGuildOwnCastle(gObj[iIndex].GuildName) == FALSE) || (gObj[iIndex].GuildStatus != G_MASTER) && (gObj[iIndex].GuildStatus != G_SUB_MASTER) )
	{
		LogAdd(LOG_RED, "[CastleSiege] CGReqNpcRepair() ERROR - Authority Fail [%s][%s], Guild:(%s)(%d)", gObj[iIndex].Account, gObj[iIndex].Name, gObj[iIndex].GuildName, gObj[iIndex].GuildStatus);
		return;
	}

	_CS_NPC_DATA pNpcData;
	BOOL bRETVAL;
	int iNpcIndex;
	int iRepairCost;

	bRETVAL = gCastleSiege.GetNpcData(lpMsg->iNpcNumber, lpMsg->iNpcIndex,pNpcData);

	if( bRETVAL == FALSE )
	{
		GCAnsNpcRepair(iIndex, 0, lpMsg->iNpcNumber, lpMsg->iNpcIndex, 0, 0);
		LogAdd(LOG_RED, "[CastleSiege] CGReqNpcRepair() ERROR - CL Request Fail [%s][%s], Guild:(%s)(%d), Npc:(CLS:%d, IDX:%d)",	gObj[iIndex].Account, gObj[iIndex].Name, gObj[iIndex].GuildName, gObj[iIndex].GuildStatus, lpMsg->iNpcNumber,lpMsg->iNpcIndex);
		return;
	}
	else
	{
		iNpcIndex = pNpcData.m_iNPC_OBJINDEX;

		if( gObjIsConnected(iNpcIndex) == FALSE )
		{
			GCAnsNpcRepair(iIndex, 0, lpMsg->iNpcNumber, lpMsg->iNpcIndex, 0, 0);
			LogAdd(LOG_RED,"[CastleSiege] CGReqNpcRepair() ERROR - Npc Alive Data Mismatch [%s][%s], Guild:(%s)(%d), Npc:(CLS:%d, IDX:%d)", gObj[iIndex].Account, gObj[iIndex].Name, gObj[iIndex].GuildName, gObj[iIndex].GuildStatus, pNpcData.m_iNPC_NUM, pNpcData.m_iNPC_INDEX);
			return;
		}
		else if( gObj[iNpcIndex].Live == 0 )
		{
			GCAnsNpcRepair(iIndex, 0, lpMsg->iNpcNumber, lpMsg->iNpcIndex, 0, 0);
			LogAdd(LOG_RED, "[CastleSiege] CGReqNpcRepair() ERROR - Npc Alive Data Mismatch [%s][%s], Guild:(%s)(%d), Npc:(CLS:%d, IDX:%d)",	gObj[iIndex].Account, gObj[iIndex].Name, gObj[iIndex].GuildName, gObj[iIndex].GuildStatus, pNpcData.m_iNPC_NUM, pNpcData.m_iNPC_INDEX);
			return;
		}
		else
		{
			if( (gObj[iNpcIndex].MaxLife == gObj[iNpcIndex].Life) || (gObj[iNpcIndex].MaxLife < gObj[iNpcIndex].Life) )
			{
				gObj[iNpcIndex].Life = gObj[iNpcIndex].MaxLife;
				GCAnsNpcRepair(iIndex, 0, lpMsg->iNpcNumber, lpMsg->iNpcIndex, 0, 0);
				return;
			}
			else
			{
				iRepairCost = 0;

				switch( lpMsg->iNpcNumber )
				{
					case 277:
						iRepairCost	= (int)(float((gObj[iNpcIndex].MaxLife - gObj[iNpcIndex].Life) * 0.5f) + float(gObj[iNpcIndex].CsNpcDfLevel * 1000000));
						break;
					case 283:
						iRepairCost	= (int)((gObj[iNpcIndex].MaxLife - gObj[iNpcIndex].Life)* 0.3f + float((gObj[iNpcIndex].CsNpcDfLevel + gObj[iNpcIndex].CsNpcRgLevel) * 1000000));
						break;
					default:
						GCAnsNpcRepair(iIndex, 0, lpMsg->iNpcNumber, lpMsg->iNpcIndex, 0, 0);
						return;
				}

				if( gObj[iIndex].Money < ((DWORD)iRepairCost) )
				{
					GCAnsNpcRepair(iIndex, 3, lpMsg->iNpcNumber, lpMsg->iNpcIndex, 0, 0);
					LogAdd(LOG_RED, "[CastleSiege] CGReqNpcRepair() ERROR - Money isn't enough [%s][%s], Guild:(%s)(%d), Npc:(CLS:%d, IDX:%d)", gObj[iIndex].Account, gObj[iIndex].Name, gObj[iIndex].GuildName, gObj[iIndex].GuildStatus, pNpcData.m_iNPC_NUM, pNpcData.m_iNPC_INDEX);
					return;
				}
				else
				{
					LogAdd(LOG_RED, "[CastleSiege] CGReqNpcRepair() OK - [%s][%s], Guild:(%s)(%d), Npc:(CLS:%d, IDX:%d)", gObj[iIndex].Account, gObj[iIndex].Name, gObj[iIndex].GuildName, gObj[iIndex].GuildStatus, pNpcData.m_iNPC_NUM, pNpcData.m_iNPC_INDEX);
					GS_GDReqCastleNpcRepair(gMapServerManager.GetMapServerGroup(), iIndex, lpMsg->iNpcNumber, lpMsg->iNpcIndex, iRepairCost);
					return;
				}
			}
		}
	}
	#endif
}

void GCAnsNpcRepair(int iIndex, int iResult, int iNpcNumber, int iNpcIndex, int iNpcHP, int iNpcMaxHP) 
{
	#if(GAMESERVER_TYPE==1)
	PMSG_ANS_NPCREPAIR pMsgResult;

	pMsgResult.h.set(0xB2, 0x06, sizeof(pMsgResult));
	pMsgResult.btResult = iResult;
	pMsgResult.iNpcNumber = iNpcNumber;
	pMsgResult.iNpcIndex = iNpcIndex;
	pMsgResult.iNpcHP = iNpcHP;
	pMsgResult.iNpcMaxHP = iNpcMaxHP;

	DataSend(iIndex, (LPBYTE)&pMsgResult, pMsgResult.h.size);
	#endif
}

void CGReqNpcUpgrade(PMSG_REQ_NPCUPGRADE * lpMsg, int iIndex) 
{
	#if (GAMESERVER_TYPE==1)

	if( lpMsg == NULL )
	{
		return;
	}

	if( ::gObjIsConnected(iIndex) == FALSE )
	{
		return;
	}

	int iNEED_GEMOFDEFEND = 0; 
	int	iNEED_MONEY = 0;
	int	iCUR_UPLVL = 0;
	int	iNXT_UPLVL = 0; 
	int	iNXT_UPVALUE = 0;
	int	bENABLE_UPGRADE = FALSE;

	if( strcmp(gObj[iIndex].GuildName, "") == 0 )
		return;

	if( (gCastleSiege.CheckGuildOwnCastle(gObj[iIndex].GuildName) == FALSE) || (gObj[iIndex].GuildStatus != G_MASTER) )
	{
		LogAdd(LOG_BLACK,"[CastleSiege] CGReqNpcUpgrade() ERROR - Authority Fail [%s][%s], Guild:(%s)(%d)", gObj[iIndex].Account, gObj[iIndex].Name, gObj[iIndex].GuildName, gObj[iIndex].GuildStatus);
		return;
	}

	switch( lpMsg->iNpcNumber )
	{
		case 277:
		{
			_CS_NPC_DATA m_NpcData;
			BOOL bResult = gCastleSiege.GetNpcData(lpMsg->iNpcNumber, lpMsg->iNpcIndex, m_NpcData);

			if( bResult )
			{
				switch( lpMsg->iNpcUpType )
				{
					case 1:
					{
						iCUR_UPLVL = m_NpcData.m_iNPC_DF_LEVEL;
						iNXT_UPLVL = lpMsg->iNpcUpValue;

						if( (iNXT_UPLVL >= 1) && (iNXT_UPLVL <= 3) && (iNXT_UPLVL == (iCUR_UPLVL+1)) )
						{
							bENABLE_UPGRADE = TRUE;
							iNXT_UPVALUE = iNXT_UPLVL;
							iNEED_GEMOFDEFEND =  g_iNpcUpDfLevel_CGATE[(iNXT_UPLVL-1)][1];
							iNEED_MONEY = g_iNpcUpDfLevel_CGATE[(iNXT_UPLVL-1)][2];
						}
					}
					break;

					case 3:
					{
						iCUR_UPLVL = 0;

						for( int i = 0; i < 3; i++ )
						{
							if( m_NpcData.m_iNPC_MAXHP >= *g_iNpcUpMaxHP_CGATE[i] )
							{
								iCUR_UPLVL = (i+1);
							}
						}
						iNXT_UPLVL = 0;

						for(int i = 0; i < 3; i++ )
						{
							if( lpMsg->iNpcUpValue >= *g_iNpcUpMaxHP_CGATE[i] )
							{
								iNXT_UPLVL = (i+1);
							}
						}

						if( (iNXT_UPLVL >= 1) && (iNXT_UPLVL <= 3) && (iNXT_UPLVL == (iCUR_UPLVL+1)) )
						{
							bENABLE_UPGRADE = TRUE;
							iNXT_UPVALUE = g_iNpcUpMaxHP_CGATE[(iNXT_UPLVL-1)][0];
							iNEED_GEMOFDEFEND = g_iNpcUpMaxHP_CGATE[(iNXT_UPLVL-1)][1];
							iNEED_MONEY = g_iNpcUpMaxHP_CGATE[(iNXT_UPLVL-1)][2];
						}
					}
					break;
					default:
						{
							GCAnsNpcUpgrade(iIndex, 5, lpMsg->iNpcNumber, lpMsg->iNpcIndex, lpMsg->iNpcUpType, lpMsg->iNpcUpValue);
							return;
						}
						break;
				}
			}
			else
			{
				GCAnsNpcUpgrade(iIndex, 7, lpMsg->iNpcNumber, lpMsg->iNpcIndex, lpMsg->iNpcUpType, lpMsg->iNpcUpValue);
				return;
			}
		}
		break;
		case 283:
		{
			_CS_NPC_DATA m_NpcData;

			BOOL bResult;
			
			bResult	= gCastleSiege.GetNpcData(lpMsg->iNpcNumber, lpMsg->iNpcIndex, m_NpcData);

			if( bResult )
			{
				switch( lpMsg->iNpcUpType )
				{
					case 1:
						{
							iCUR_UPLVL = m_NpcData.m_iNPC_DF_LEVEL;
							iNXT_UPLVL = lpMsg->iNpcUpValue;

							if( (iNXT_UPLVL >= 1) && (iNXT_UPLVL <= 3) && (iNXT_UPLVL == (iCUR_UPLVL+1)) )
							{
								bENABLE_UPGRADE = TRUE;
								iNXT_UPVALUE = iNXT_UPLVL;
								iNEED_GEMOFDEFEND = g_iNpcUpDfLevel_CSTATUE[(iNXT_UPLVL-1)][1];
								iNEED_MONEY = g_iNpcUpDfLevel_CSTATUE[(iNXT_UPLVL-1)][2];
							}
						}
						break;
					case 2:
						{
							iCUR_UPLVL = m_NpcData.m_iNPC_RG_LEVEL;
							iNXT_UPLVL = lpMsg->iNpcUpValue;
							
							if( (iNXT_UPLVL >= 1) && (iNXT_UPLVL <= 3) && (iNXT_UPLVL == (iCUR_UPLVL+1)) )
							{
								bENABLE_UPGRADE = TRUE;
								iNXT_UPVALUE = iNXT_UPLVL;
								iNEED_GEMOFDEFEND = g_iNpcUpRgLevel_CSTATUE[(iNXT_UPLVL-1)][1];
								iNEED_MONEY = g_iNpcUpRgLevel_CSTATUE[(iNXT_UPLVL-1)][2];
							}
						}
						break;
					case 3:
						{
							iCUR_UPLVL = 0;

							for( int i = 0; i < 3; i++ )
							{
								if( m_NpcData.m_iNPC_MAXHP >= *g_iNpcUpMaxHP_CSTATUE[i] )
								{
									iCUR_UPLVL = (i+1);
								}
							}

							iNXT_UPLVL = 0;

							for(int i = 0; i < 3; i++ )
							{
								if( lpMsg->iNpcUpValue >= *g_iNpcUpMaxHP_CSTATUE[i] )
								{
									iNXT_UPLVL = (i+1);
								}
							}

							if( (iNXT_UPLVL >= 1) && (iNXT_UPLVL <= 3) && (iNXT_UPLVL == (iCUR_UPLVL+1)) )
							{
								bENABLE_UPGRADE = TRUE;
								iNXT_UPVALUE = g_iNpcUpMaxHP_CSTATUE[(iNXT_UPLVL-1)][0];
								iNEED_GEMOFDEFEND = g_iNpcUpMaxHP_CSTATUE[(iNXT_UPLVL-1)][1];
								iNEED_MONEY = g_iNpcUpMaxHP_CSTATUE[(iNXT_UPLVL-1)][2];
							}
						}
						break;
					default:
						{
							GCAnsNpcUpgrade(iIndex, 5, lpMsg->iNpcNumber, lpMsg->iNpcIndex, lpMsg->iNpcUpType, lpMsg->iNpcUpValue);
							LogAdd(LOG_BLACK,"[CastleSiege] CGReqNpcUpgrade() ERROR - UpType doesn't Exist [%s][%s], Guild:(%s)(%d), NPC CLS:%d, IDX:%d, UPTYPE:%d, UPVAL:%d", gObj[iIndex].Account,gObj[iIndex].Name,gObj[iIndex].GuildName,gObj[iIndex].GuildNumber,lpMsg->iNpcNumber,lpMsg->iNpcIndex,lpMsg->iNpcUpType,lpMsg->iNpcUpValue);
							return;
						}
						break;
				}
			}
			else
			{
				GCAnsNpcUpgrade(iIndex, 7, lpMsg->iNpcNumber, lpMsg->iNpcIndex, lpMsg->iNpcUpType, lpMsg->iNpcUpValue);		
				return;
			}
		}
		break;
	}

	int iGEMOFDEFEND_COUNT;
	int x;
	int bRETVAL;

	if( bENABLE_UPGRADE == FALSE )
	{
		GCAnsNpcUpgrade(iIndex, 6, lpMsg->iNpcNumber, lpMsg->iNpcIndex, lpMsg->iNpcUpType, lpMsg->iNpcUpValue);		
	}
	else 
	{
		if( gObj[iIndex].Money < ((DWORD)iNEED_MONEY) )
		{
			GCAnsNpcUpgrade(iIndex, 3, lpMsg->iNpcNumber, lpMsg->iNpcIndex, lpMsg->iNpcUpType, lpMsg->iNpcUpValue);
			LogAdd(LOG_BLACK,"[CastleSiege] CGReqNpcUpgrade() ERROR - Money is Low [%s][%s], Guild:(%s)(%d), NPC CLS:%d, IDX:%d, UPTYPE:%d, UPVAL:%d, NXTLV:%d, Money:%d, Need Money:%d", gObj[iIndex].Account,gObj[iIndex].Name,gObj[iIndex].GuildName,gObj[iIndex].GuildNumber,lpMsg->iNpcNumber,lpMsg->iNpcIndex,lpMsg->iNpcUpType,lpMsg->iNpcUpValue,iNXT_UPLVL,gObj[iIndex].Money,iNEED_MONEY);
		}
		else
		{
			for( iGEMOFDEFEND_COUNT = 0, x = 0; x < INVENTORY_MAIN_SIZE; x++ )
			{
				if( gObj[iIndex].Inventory[x].IsItem() == TRUE )
				{
					if( gObj[iIndex].Inventory[x].m_Index == GET_ITEM(14,31) )
					{
						iGEMOFDEFEND_COUNT += 1;
					}
				}
			}

			if( iGEMOFDEFEND_COUNT < iNEED_GEMOFDEFEND )
			{
				GCAnsNpcUpgrade(iIndex, 4, lpMsg->iNpcNumber, lpMsg->iNpcIndex, lpMsg->iNpcUpType, lpMsg->iNpcUpValue);
				LogAdd(LOG_BLACK,"[CastleSiege] CGReqNpcUpgrade() ERROR - Gem is Low [%s][%s], Guild:(%s)(%d), NPC CLS:%d, IDX:%d, UPTYPE:%d, UPVAL:%d, NXTLV:%d, Gem:%d, Need Gem:%d",	gObj[iIndex].Account,gObj[iIndex].Name,gObj[iIndex].GuildName,gObj[iIndex].GuildNumber,lpMsg->iNpcNumber,lpMsg->iNpcIndex,lpMsg->iNpcUpType,lpMsg->iNpcUpValue,iNXT_UPLVL,iGEMOFDEFEND_COUNT,iNEED_GEMOFDEFEND);
			}
			else
			{
				bRETVAL = gCastleSiege.PayForUpgradeDbNpc(iIndex,lpMsg->iNpcNumber,lpMsg->iNpcIndex,lpMsg->iNpcUpType,lpMsg->iNpcUpValue,(iNXT_UPLVL-1));
				
				if( bRETVAL == FALSE )
				{
				}
				else
				{
					gObj[iIndex].IsCastleNPCUpgradeCompleted = TRUE;
					LogAdd(LOG_BLACK,"[CastleSiege] [0xB2][0x07] CGReqNpcUpgrade() - Pay For Npc Upgrade (CLS:%d, IDX:%d, UPTYPE:%d, UPVAL:%d)", lpMsg->iNpcNumber,lpMsg->iNpcIndex,lpMsg->iNpcUpType,iNXT_UPVALUE);
					GS_GDReqCastleNpcUpgrade(gMapServerManager.GetMapServerGroup(),iIndex,lpMsg->iNpcNumber,lpMsg->iNpcIndex,lpMsg->iNpcUpType,iNXT_UPVALUE,(iNXT_UPLVL-1));
					LogAdd(LOG_BLACK,"[CastleSiege] CGReqNpcUpgrade() REQ OK - [%s][%s], Guild:(%s)(%d), NPC CLS:%d, IDX:%d, UPTYPE:%d, UPVAL:%d, NXTLV:%d", gObj[iIndex].Account,gObj[iIndex].Name,gObj[iIndex].GuildName,gObj[iIndex].GuildNumber,lpMsg->iNpcNumber,lpMsg->iNpcIndex,lpMsg->iNpcUpType,lpMsg->iNpcUpValue,iNXT_UPLVL);
				}	
			}
		}

	}
	#endif
}

void GCAnsNpcUpgrade(int iIndex, int iResult, int iNpcNumber, int iNpcIndex, int iNpcUpType, int iNpcUpValue) 
{
	#if(GAMESERVER_TYPE==1)
	PMSG_ANS_NPCUPGRADE pMsgResult;

	pMsgResult.h.set(0xB2, 0x07, sizeof(pMsgResult));
	pMsgResult.btResult = iResult;
	pMsgResult.iNpcNumber = iNpcNumber;
	pMsgResult.iNpcIndex = iNpcIndex;
	pMsgResult.iNpcUpType = iNpcUpType;
	pMsgResult.iNpcUpValue = iNpcUpValue;

	DataSend(iIndex, (LPBYTE)&pMsgResult, pMsgResult.h.size);
	#endif
}

void CGReqTaxMoneyInfo(PMSG_REQ_TAXMONEYINFO * lpMsg, int iIndex) 
{
	#if(GAMESERVER_TYPE==1)
	if( lpMsg == NULL )
	{
		return;
	}

	if( strcmp(gObj[iIndex].GuildName, "") == 0 )
	{
		return;
	}
	
	if( (gCastleSiege.CheckGuildOwnCastle(gObj[iIndex].GuildName) == FALSE) || (gObj[iIndex].GuildStatus != G_MASTER) )
	{
		LogAdd(LOG_RED,"[CastleSiege] CGReqTaxMoneyInfo() ERROR - Authority Fail [%s][%s], Guild:(%s)(%d)", gObj[iIndex].Account, gObj[iIndex].Name, gObj[iIndex].GuildName, gObj[iIndex].GuildStatus);
	}
	else
	{
		GS_GDReqTaxInfo(gMapServerManager.GetMapServerGroup(), iIndex);
		LogAdd(LOG_RED,"[CastleSiege] CGReqTaxMoneyInfo() REQ OK - [%s][%s], Guild:(%s)(%d)",	gObj[iIndex].Account, gObj[iIndex].Name, gObj[iIndex].GuildName, gObj[iIndex].GuildStatus);
	}
	#endif
}

void GCAnsTaxMoneyInfo(int iIndex, int iResult, BYTE btTaxRateChaos, BYTE btTaxRateStore, __int64 i64Money) 
{
	#if (GAMESERVER_TYPE==1)
	PMSG_ANS_TAXMONEYINFO pMsgResult;

	pMsgResult.h.set(0xB2, 0x08, sizeof(pMsgResult));
	pMsgResult.btResult = iResult;
	pMsgResult.btTaxRateChaos = btTaxRateChaos;
	pMsgResult.btTaxRateStore = btTaxRateStore;
	pMsgResult.btMoney1 = SET_NUMBERHB(SET_NUMBERHW(SET_NUMBERHDW(i64Money)));
	pMsgResult.btMoney2 = SET_NUMBERLB(SET_NUMBERHW(SET_NUMBERHDW(i64Money)));
	pMsgResult.btMoney3 = SET_NUMBERHB(SET_NUMBERLW(SET_NUMBERHDW(i64Money)));
	pMsgResult.btMoney4 = SET_NUMBERLB(SET_NUMBERLW(SET_NUMBERHDW(i64Money)));
	pMsgResult.btMoney5 = SET_NUMBERHB( SET_NUMBERHW( SET_NUMBERLDW( i64Money ) ) ) ;
	pMsgResult.btMoney6 = SET_NUMBERLB( SET_NUMBERHW( SET_NUMBERLDW( i64Money ) ) ) ;
	pMsgResult.btMoney7 = SET_NUMBERHB( SET_NUMBERLW( SET_NUMBERLDW( i64Money ) ) ) ;
	pMsgResult.btMoney8 = SET_NUMBERLB( SET_NUMBERLW( SET_NUMBERLDW( i64Money ) ) ) ;

	DataSend(iIndex, (LPBYTE)&pMsgResult, pMsgResult.h.size);
	#endif
}

void CGReqTaxRateChange(PMSG_REQ_TAXRATECHANGE * lpMsg, int iIndex) 
{
	#if(GAMESERVER_TYPE==1)

	int iMaxTaxRate;
	int iTaxRate;

	if( lpMsg == NULL )
	{
		return;
	}

	if( strcmp(gObj[iIndex].GuildName, "") == 0 )
	{
		return;
	}

	if( (gCastleSiege.CheckGuildOwnCastle(gObj[iIndex].GuildName) == FALSE) || (gObj[iIndex].GuildStatus != G_MASTER) )
	{
		LogAdd(LOG_RED,"[CastleSiege] CGReqTaxRateChange() ERROR - Authority Fail [%s][%s], Guild:(%s)(%d)",	gObj[iIndex].Account, gObj[iIndex].Name, gObj[iIndex].GuildName, gObj[iIndex].GuildStatus);
	}
	else
	{
		iMaxTaxRate = 0;

		switch( lpMsg->btTaxType )
		{
		case 1:
			iMaxTaxRate = 3;
			break;
		case 2:
			iMaxTaxRate = 3;
			break;
		case 3:
			iMaxTaxRate = 300000;
			break;
		}

		iTaxRate = MAKE_NUMBERDW(MAKE_NUMBERW(lpMsg->btTaxRate1, lpMsg->btTaxRate2), MAKE_NUMBERW(lpMsg->btTaxRate3, lpMsg->btTaxRate4)); 

		if( (iTaxRate < 0) || (iTaxRate > iMaxTaxRate) )
		{
			GCAnsTaxRateChange(iIndex,0,0,0);
			LogAdd(LOG_BLACK,"[CastleSiege] CGReqTaxRateChange() ERROR - Tax Rate Out of Range [%s][%s], Guild:(%s)(%d), TaxType:%d, TaxRate:%d", gObj[iIndex].Account, gObj[iIndex].Name, gObj[iIndex].GuildName, gObj[iIndex].GuildStatus, lpMsg->btTaxType, iTaxRate);
		}
		else
		{
			GS_GDReqTaxRateChange(gMapServerManager.GetMapServerGroup(), iIndex, lpMsg->btTaxType, iTaxRate);
			LogAdd(LOG_BLACK,"[CastleSiege] CGReqTaxRateChange() REQ OK - [%s][%s], Guild:(%s)(%d), TaxType:%d, TaxRate:%d", gObj[iIndex].Account, gObj[iIndex].Name, gObj[iIndex].GuildName, gObj[iIndex].GuildStatus, lpMsg->btTaxType, iTaxRate);
		}
	}
	#endif
}

void GCAnsTaxRateChange(int iIndex, int iResult, BYTE btTaxType, int iTaxRate) 
{
	#if (GAMESERVER_TYPE==1)
	PMSG_ANS_TAXRATECHANGE pMsgResult;
	pMsgResult.h.set(0xB2, 0x09, sizeof(pMsgResult));
	pMsgResult.btResult = iResult;
	pMsgResult.btTaxType = btTaxType;
	pMsgResult.btTaxRate1 = SET_NUMBERHB(SET_NUMBERHW(iTaxRate));
	pMsgResult.btTaxRate2 = SET_NUMBERLB(SET_NUMBERHW(iTaxRate));
	pMsgResult.btTaxRate3 = SET_NUMBERHB(SET_NUMBERLW(iTaxRate));
	pMsgResult.btTaxRate4 = SET_NUMBERLB(SET_NUMBERLW(iTaxRate));
	DataSend(iIndex, (LPBYTE)&pMsgResult, pMsgResult.h.size);
	#endif
}

void CGReqMoneyDrawOut(PMSG_REQ_MONEYDRAWOUT * lpMsg, int iIndex) 
{
	#if(GAMESERVER_TYPE==1)
	int iMoneyChange;
	BOOL bRETVAL;

	if( lpMsg == NULL )
	{
		return;
	}

	if( strcmp(gObj[iIndex].GuildName, "") == 0 )
	{
		return;
	}
	
	if( (gCastleSiege.CheckGuildOwnCastle(gObj[iIndex].GuildName) == FALSE) || (gObj[iIndex].GuildStatus != G_MASTER) )
	{
		LogAdd(LOG_RED,"[CastleSiege] CGReqMoneyDrawOut() ERROR - Authority Fail [%s][%s], Guild:(%s)(%d)", gObj[iIndex].Account, gObj[iIndex].Name, gObj[iIndex].GuildName, gObj[iIndex].GuildStatus);
	}
	else
	{
		iMoneyChange = MAKE_NUMBERDW(MAKE_NUMBERW(lpMsg->btMoney4, lpMsg->btMoney3), MAKE_NUMBERW(lpMsg->btMoney2, lpMsg->btMoney1));

		if( iMoneyChange <= 0 )
		{
			GCAnsMoneyDrawOut(iIndex, 0, 0);
			LogAdd(LOG_BLACK,"[CastleSiege] CGReqMoneyDrawOut() ERROR - Req Money < 0 [%s][%s], Guild:(%s)(%d), Money:%d, TotMoney:%d",	gObj[iIndex].Account, gObj[iIndex].Name, gObj[iIndex].GuildName, gObj[iIndex].GuildStatus, iMoneyChange, gObj[iIndex].Money);
		}
		else
		{
			bRETVAL = gCastleSiege.CheckCastleHasMoney(iMoneyChange);

			if( bRETVAL == FALSE )
			{
				GCAnsMoneyDrawOut(iIndex, 0, 0);
				LogAdd(LOG_BLACK,"[CastleSiege] CGReqMoneyDrawOut() ERROR - Castle Money is Low [%s][%s], Guild:(%s)(%d), Money:%d, TotMoney:%d", gObj[iIndex].Account, gObj[iIndex].Name, gObj[iIndex].GuildName, gObj[iIndex].GuildStatus, iMoneyChange, gObj[iIndex].Money);
			}
			else
			{
				if( (gObj[iIndex].Money + iMoneyChange) > MAX_MONEY )
				{
					GCAnsMoneyDrawOut(iIndex, 0, 0);
				}
				else
				{
					GS_GDReqCastleMoneyChange(gMapServerManager.GetMapServerGroup(), iIndex, -iMoneyChange);
					LogAdd(LOG_BLACK,"[CastleSiege] CGReqMoneyDrawOut() REQ OK - [%s][%s], Guild:(%s)(%d), Money:%d, TotMoney:%d", gObj[iIndex].Account, gObj[iIndex].Name, gObj[iIndex].GuildName, gObj[iIndex].GuildStatus, iMoneyChange, gObj[iIndex].Money);
				}
			}
		}
	}
	#endif
}

void GCAnsMoneyDrawOut(int iIndex, int iResult, __int64 i64Money) 
{
	#if(GAMESERVER_TYPE==1)
	PMSG_ANS_MONEYDRAWOUT pMsgResult;
	pMsgResult.h.set(0xB2, 0x10, sizeof(pMsgResult));
	pMsgResult.btResult = iResult;
	pMsgResult.btMoney1 = SET_NUMBERHB(SET_NUMBERHW(SET_NUMBERHDW(i64Money)));
	pMsgResult.btMoney2 = SET_NUMBERLB(SET_NUMBERHW(SET_NUMBERHDW(i64Money)));
	pMsgResult.btMoney3 = SET_NUMBERHB(SET_NUMBERLW(SET_NUMBERHDW(i64Money)));
	pMsgResult.btMoney4 = SET_NUMBERLB(SET_NUMBERLW(SET_NUMBERHDW(i64Money)));
	pMsgResult.btMoney5 = SET_NUMBERHB( SET_NUMBERHW( SET_NUMBERLDW( i64Money ) ) ) ;
	pMsgResult.btMoney6 = SET_NUMBERLB( SET_NUMBERHW( SET_NUMBERLDW( i64Money ) ) ) ;
	pMsgResult.btMoney7 = SET_NUMBERHB( SET_NUMBERLW( SET_NUMBERLDW( i64Money ) ) ) ;
	pMsgResult.btMoney8 = SET_NUMBERLB( SET_NUMBERLW( SET_NUMBERLDW( i64Money ) ) ) ;
	
	DataSend(iIndex, (LPBYTE)&pMsgResult, pMsgResult.h.size);
	#endif
}

void GCAnsCsGateState(int iIndex, int iResult, int iGateIndex) 
{
	#if(GAMESERVER_TYPE==1)
	PMSG_ANS_CSGATESTATE pMsgResult;
	pMsgResult.h.set(0xB2, 0x11, sizeof(pMsgResult));
	pMsgResult.btResult = iResult;
	pMsgResult.btIndex1 = SET_NUMBERHB( (iGateIndex &0xffff) );
	pMsgResult.btIndex2 = SET_NUMBERLB( (iGateIndex &0xffff) );
	DataSend(iIndex, (LPBYTE)&pMsgResult, pMsgResult.h.size);
	#endif
}

void CGReqCsGateOperate(PMSG_REQ_CSGATEOPERATE * lpMsg, int iIndex) 
{
	#if(GAMESERVER_TYPE==1)
	int iGateIndex;

	if( strcmp(gObj[iIndex].GuildName, "") == 0 )
	{
		return;
	}
	
	if( gCastleSiege.GetCastleState() == CASTLESIEGE_STATE_STARTSIEGE )
	{
		if( gObj[iIndex].CsJoinSide != 1 )
		{
			LogAdd(LOG_BLACK,"[CastleSiege] CGReqCsGateOperate() ERROR - Authority Fail [%s][%s], Guild:(%s)(%d)", gObj[iIndex].Account, gObj[iIndex].Name, gObj[iIndex].GuildName, gObj[iIndex].GuildStatus);
			return;
		}
	}
	else
	{
		if( (gCastleSiege.CheckCastleOwnerMember(iIndex) == FALSE) && (gCastleSiege.CheckCastleOwnerUnionMember(iIndex) == FALSE) )
		{
			LogAdd(LOG_BLACK,"[CastleSiege] CGReqCsGateOperate() ERROR - Authority Fail [%s][%s], Guild:(%s)(%d)", gObj[iIndex].Account, gObj[iIndex].Name, gObj[iIndex].GuildName, gObj[iIndex].GuildStatus);
			return;
		}
	}
	
	iGateIndex = MAKE_NUMBERW(lpMsg->btIndex1,lpMsg->btIndex2);

	if( gCastleSiege.CheckCsGateAlive(iGateIndex) == TRUE ) 
	{
		int iOperateGate = gCastleSiege.OperateGate(iGateIndex, gObj[iGateIndex].CsNpcExistVal, lpMsg->btOperate);

		if( iOperateGate == 0 )
		{
			GCAnsCsGateOperate(iIndex, 0, -1, 0);
			LogAdd(LOG_BLACK,"[CastleSiege] CGReqCsGateOperate() ERROR - Operate Gate Failed [%s][%s], Guild:(%s)(%d)",	gObj[iIndex].Account, gObj[iIndex].Name, gObj[iIndex].GuildName, gObj[iIndex].GuildStatus);	
		}
		else
		{
			GCAnsCsGateOperate(iIndex, 1, iGateIndex, lpMsg->btOperate);
			gCastleSiege.SendCsGateStateViewPort(iGateIndex, lpMsg->btOperate);
			LogAdd(LOG_BLACK,"[CastleSiege] CGReqCsGateOperate() OK - [%s][%s], Guild:(%s)(%d), DOOR:(%d)(X:%d,Y:%d)(STATUE:%d)", gObj[iIndex].Account, gObj[iIndex].Name, gObj[iIndex].GuildName, gObj[iIndex].GuildNumber,iGateIndex, gObj[iGateIndex].X, gObj[iGateIndex].Y, gObj[iGateIndex].CsGateOpen);
		}
	}
	else
	{
		GCAnsCsGateOperate(iIndex, 2, -1, 0);
		LogAdd(LOG_BLACK,"[CastleSiege] CGReqCsGateOperate() ERROR - Gate Doesn't Exist [%s][%s], Guild:(%s)(%d)", gObj[iIndex].Account, gObj[iIndex].Name, gObj[iIndex].GuildName, gObj[iIndex].GuildStatus);	
	}
	#endif
}

void GCAnsCsGateOperate(int iIndex, int iResult, int iGateIndex, int iGateOperate) 
{
	#if(GAMESERVER_TYPE==1)
	PMSG_ANS_CSGATEOPERATE pMsgResult;

	pMsgResult.h.set(0xB2, 0x12, sizeof(pMsgResult));
	pMsgResult.btResult = iResult;
	pMsgResult.btOperate = iGateOperate;
	pMsgResult.btIndex1 = SET_NUMBERHB(iGateIndex & 0xffff);
	pMsgResult.btIndex2 = SET_NUMBERLB(iGateIndex & 0xffff);
	DataSend(iIndex, (LPBYTE)&pMsgResult, pMsgResult.h.size);
	#endif
}

void GCAnsCsGateCurState(int iIndex, int iGateIndex, int iGateOperate) 
{
	#if(GAMESERVER_TYPE==1)
	PMSG_ANS_CSGATECURSTATE pMsgResult;
	pMsgResult.h.set(0xB2, 0x13, sizeof(pMsgResult));
	pMsgResult.btOperate = iGateOperate;
	pMsgResult.btIndex1 = SET_NUMBERHB(iGateIndex & 0xffff);
	pMsgResult.btIndex2 = SET_NUMBERLB(iGateIndex & 0xffff);
	DataSend(iIndex, (LPBYTE)&pMsgResult, pMsgResult.h.size);
	#endif
}

void GCAnsCsAccessSwitchState(int iIndex, int iSwitchIndex, int iSwitchUserIndex, BYTE btSwitchState) 
{
	#if(GAMESERVER_TYPE==1)
	PMSG_ANS_NOTIFYSWITCHPROC pMsgResult;
	pMsgResult.h.set(0xB2, 0x14, sizeof(pMsgResult));
	pMsgResult.btIndex1 = SET_NUMBERHB(iSwitchIndex & 0xffff);
	pMsgResult.btIndex2 = SET_NUMBERLB(iSwitchIndex & 0xffff);
	pMsgResult.btUserIndex1 = SET_NUMBERHB(iSwitchUserIndex & 0xffff);
	pMsgResult.btUserIndex2 = SET_NUMBERLB(iSwitchUserIndex & 0xffff);
	pMsgResult.btSwitchState = btSwitchState;
	DataSend(iIndex, (LPBYTE)&pMsgResult, pMsgResult.h.size);
	#endif
}

void GCAnsCsAccessCrownState(int iIndex, BYTE btCrownState) 
{
	#if(GAMESERVER_TYPE==1)
	PMSG_ANS_NOTIFYCROWNPROC pMsgResult;
	pMsgResult.h.set(0xB2, 0x15, sizeof(pMsgResult));
	pMsgResult.btCrownState = btCrownState;

	if( btCrownState == 0 )
	{
		if( gObj[iIndex].AccumulatedCrownAccessTime < 0 || gObj[iIndex].AccumulatedCrownAccessTime > 61000 )
		{
			gObj[iIndex].AccumulatedCrownAccessTime = 0;
		}

		LogAdd(LOG_BLACK,"[CastleSiege] [Reg. Attempt] Accumulated Crown AccessTime : %d [%s](%s)(%s)",	gObj[iIndex].AccumulatedCrownAccessTime,gObj[iIndex].GuildName,gObj[iIndex].Account,gObj[iIndex].Name);
	}

	if( btCrownState == 1 )
	{
		LogAdd(LOG_BLACK,"[CastleSiege] [Reg. Success] Accumulated Crown AccessTime : %d [%s](%s)(%s)",	gObj[iIndex].AccumulatedCrownAccessTime,gObj[iIndex].GuildName,gObj[iIndex].Account,gObj[iIndex].Name);
		gObj[iIndex].AccumulatedCrownAccessTime = 0;
	}

	if( btCrownState == 2 )
	{
		gObj[iIndex].AccumulatedCrownAccessTime += GetTickCount() - gCastleSiege.GetCrownAccessTickCount();

		gObj[iIndex].AccumulatedCrownAccessTime -= gServerInfo.m_CastleSiegeLowerAccumulatedTimeValue;

		if( gObj[iIndex].AccumulatedCrownAccessTime < 0 || gObj[iIndex].AccumulatedCrownAccessTime > 61000 )
		{
			gObj[iIndex].AccumulatedCrownAccessTime = 0;
		}

		LogAdd(LOG_BLACK,"[CastleSiege] [Reg. Fail] Accumulated Crown AccessTime : %d [%s](%s)(%s)", gObj[iIndex].AccumulatedCrownAccessTime,gObj[iIndex].GuildName,gObj[iIndex].Account,gObj[iIndex].Name);
	}

	pMsgResult.dwAccumulatedCrownAccessTime = gObj[iIndex].AccumulatedCrownAccessTime;

	DataSend(iIndex, (LPBYTE)&pMsgResult, pMsgResult.h.size);
	#endif
}

void GCAnsCsNotifyStart(int iIndex, BYTE btStartState) 
{
	#if(GAMESERVER_TYPE==1)
	PMSG_ANS_NOTIFYCSSTART pMsgResult;
	pMsgResult.h.set(0xB2, 0x17, sizeof(pMsgResult));
	pMsgResult.btStartState = btStartState;
	DataSend(iIndex, (LPBYTE)&pMsgResult, pMsgResult.h.size);
	#endif
}

void GCAnsCsNotifyProgress(int iIndex, BYTE btCastleSiegeState, LPCSTR lpszGuildName) 
{
	if ( lpszGuildName == NULL )
	{
		return;
	}

	PMSG_ANS_NOTIFYCSPROGRESS pMsgResult;
	pMsgResult.h.set(0xB2, 0x18, sizeof(pMsgResult));
	pMsgResult.btCastleSiegeState = btCastleSiegeState;
	memset(pMsgResult.szGuildName, 0, sizeof(pMsgResult.szGuildName));
	memcpy(pMsgResult.szGuildName, lpszGuildName, sizeof(pMsgResult.szGuildName));
	DataSend(iIndex, (LPBYTE)&pMsgResult, pMsgResult.h.size);
}

void GCAnsCsMapSvrTaxInfo(int iIndex, BYTE btTaxType, BYTE btTaxRate) 
{
	PMSG_ANS_MAPSVRTAXINFO pMsgResult;
	pMsgResult.h.set(0xB2, 0x1A, sizeof(pMsgResult));
	pMsgResult.btTaxType = btTaxType;
	pMsgResult.btTaxRate = btTaxRate;
	DataSend(iIndex, (LPBYTE)&pMsgResult, pMsgResult.h.size);
}

void CGReqCsMiniMapData(PMSG_REQ_MINIMAPDATA * lpMsg, int iIndex) 
{
	#if(GAMESERVER_TYPE==1)
	if( gObjIsConnected(iIndex) == FALSE )
	{
		return;
	}

	if( (gObj[iIndex].CsJoinSide <= 0) || ( gCastleSiege.CheckUnionGuildMaster(iIndex) == FALSE ) )
	{
		GCAnsCsMiniMapData(iIndex, 3);
	}
	else if( gCastleSiege.GetCastleState() != CASTLESIEGE_STATE_STARTSIEGE )
	{
		GCAnsCsMiniMapData(iIndex, 2);
	}
	else
	{
		gCastleSiege.AddMiniMapDataReqUser(iIndex);
		GCAnsCsMiniMapData(iIndex, 1);
	}
	#endif
}

void GCAnsCsMiniMapData(int iIndex, BYTE btResult) 
{
	PMSG_ANS_MINIMAPDATA pMsgResult;
	pMsgResult.h.set(0xB2, 0x1B, sizeof(pMsgResult));
	pMsgResult.btResult = btResult;
	DataSend(iIndex, (LPBYTE)&pMsgResult, pMsgResult.h.size);
}

void CGReqStopCsMiniMapData(PMSG_REQ_STOPMINIMAPDATA * lpMsg, int iIndex) 
{
	#if(GAMESERVER_TYPE==1)
	if( gObjIsConnected(iIndex) == FALSE )
	{
		return;
	}

	gCastleSiege.DelMiniMapDataReqUser(iIndex);
	#endif
}

void CGReqCsSendCommand(PMSG_REQ_CSCOMMAND * lpMsg, int iIndex) 
{
	#if(GAMESERVER_TYPE==1)
	if( gObjIsConnected(iIndex) == FALSE )
	{
		return;
	}

	if( gObj[iIndex].CsJoinSide <= 0 ||	gCastleSiege.CheckUnionGuildMaster(iIndex) == FALSE )
	{
		return;
	}

	if( gCastleSiege.GetCastleState() != CASTLESIEGE_STATE_STARTSIEGE )
	{
		return;
	}

	GCAnsCsSendCommand(gObj[iIndex].CsJoinSide, lpMsg->btTeam, lpMsg->btX, lpMsg->btY, lpMsg->btCommand);
	#endif
}

void GCAnsCsSendCommand(int iCsJoinSize, BYTE btTeam, BYTE btX, BYTE btY, BYTE btCommand) 
{
	#if(GAMESERVER_TYPE==1)
	PMSG_ANS_CSCOMMAND pMsgResult;

	pMsgResult.h.set(0xB2, 0x1D, sizeof(pMsgResult));
	
	pMsgResult.btTeam = btTeam;
	pMsgResult.btX = btX;
	pMsgResult.btY = btY;
	pMsgResult.btCommand = btCommand;
	
	for( int iIndex = OBJECT_START_USER; iIndex < MAX_OBJECT; iIndex++ )
	{
		if( gObjIsConnected(iIndex) == FALSE ) continue;

		if( (gObj[iIndex].CsJoinSide == iCsJoinSize) && (gObj[iIndex].Map == MAP_CASTLE_SIEGE) )
		{
			DataSend(iIndex, (LPBYTE)&pMsgResult, pMsgResult.h.size);
		}
	}
	#endif
}

void GCAnsCsLeftTimeAlarm(BYTE btHour, BYTE btMinute) 
{
	PMSG_ANS_CSLEFTTIMEALARM pMsgResult;
	pMsgResult.h.set(0xB2, 0x1E, sizeof(pMsgResult));
	pMsgResult.btHour = btHour;
	pMsgResult.btMinute = btMinute;

	for (int iIndex = OBJECT_START_USER;iIndex < MAX_OBJECT;iIndex++)
	{
		if ( !gObjIsConnected(iIndex) )
		{
			continue;
		}
		DataSend(iIndex, (LPBYTE)&pMsgResult, pMsgResult.h.size);
	}
}

void GCAnsSelfCsLeftTimeAlarm(int iIndex, BYTE btHour, BYTE btMinute) 
{
	PMSG_ANS_CSLEFTTIMEALARM pMsgResult;

	pMsgResult.h.set(0xB2, 0x1E, sizeof(pMsgResult));
	pMsgResult.btHour = btHour;
	pMsgResult.btMinute = btMinute;

	if ( !gObjIsConnected(iIndex))
	{
		return;
	}

	DataSend(iIndex, (LPBYTE)&pMsgResult, pMsgResult.h.size);
}

void CGReqCsSetEnterHuntZone(PMSG_REQ_CSHUNTZONEENTER * lpMsg, int iIndex) 
{
	#if(GAMESERVER_TYPE==1)
	if( gObjIsConnected(iIndex) == FALSE )
	{
		return;
	}
	
	if( gCastleSiege.CheckCastleOwnerMember(iIndex) == FALSE )
	{
		GCAnsCsSetEnterHuntZone(iIndex, 2, lpMsg->btHuntZoneEnter);
	}
	else if( gCastleSiege.GetCastleState() == CASTLESIEGE_STATE_STARTSIEGE )
	{
		GCAnsCsSetEnterHuntZone(iIndex, 0, lpMsg->btHuntZoneEnter);
	}
	else
	{
		gCastleSiege.SetHuntZoneEnter(lpMsg->btHuntZoneEnter);
		GCAnsCsSetEnterHuntZone(iIndex, 1, lpMsg->btHuntZoneEnter);
	}

	#endif
}

void GCAnsCsSetEnterHuntZone(int iIndex, BYTE btResult, BYTE btEnterHuntZone) 
{
	PMSG_ANS_CSHUNTZONEENTER pMsgResult;
	pMsgResult.h.set(0xB2, 0x1F, sizeof(pMsgResult));
	pMsgResult.btResult = btResult;
	pMsgResult.btHuntZoneEnter = btEnterHuntZone;
	DataSend(iIndex, (LPBYTE)&pMsgResult, pMsgResult.h.size);
}

void CGReqNpcDbList(PMSG_REQ_NPCDBLIST * lpMsg, int iIndex) 
{
	#if(GAMESERVER_TYPE==1)
	PMSG_ANS_NPCDBLIST pResult;

	if( lpMsg == NULL )
	{
		return;
	}

	if( strcmp(gObj[iIndex].GuildName, "") == 0 )
	{
		return;
	}
	
	if( gCastleSiege.CheckGuildOwnCastle(gObj[iIndex].GuildName) == FALSE || ( (gObj[iIndex].GuildStatus != 128) && (gObj[iIndex].GuildStatus != 64) ) )
	{
		LogAdd(LOG_RED, "[CastleSiege] CGReqNpcDbList() ERROR - Authority Fail [%s][%s], Guild:(%s)(%d)", gObj[iIndex].Account, gObj[iIndex].Name, gObj[iIndex].GuildName, gObj[iIndex].GuildStatus);
		pResult.h.set(0xB3, sizeof(pResult));
		pResult.iCount = 0;
		pResult.btResult = 2;
		DataSend(iIndex, (LPBYTE)&pResult, sizeof(pResult));
	}
	else
	{
		switch( lpMsg->btMonsterCode )
		{
		case 1:
			gCastleSiege.SendNpcStateList(iIndex, 277);
			break;
		case 2:
			gCastleSiege.SendNpcStateList(iIndex, 283);
			break;
		}
	}
	#endif
}

void CGReqCsRegGuildList(PMSG_REQ_CSREGGUILDLIST * lpMsg, int iIndex) 
{
	#if(GAMESERVER_TYPE==1)
	if( lpMsg == NULL ) return;

	GS_GDReqAllGuildMarkRegInfo(gMapServerManager.GetMapServerGroup(), iIndex);
	#endif
}

void CGReqCsAttkGuildList(PMSG_REQ_CSATTKGUILDLIST * lpMsg, int iIndex) 
{
	#if(GAMESERVER_TYPE==1)
	char cBUFFER[1625];
	PMSG_ANS_CSATTKGUILDLIST* lpMsgSend;
	PMSG_CSATTKGUILDLIST* lpMsgSendBody;
	int iCount;

	if( lpMsg == NULL )	return;
	
	lpMsgSend = (PMSG_ANS_CSATTKGUILDLIST*)(cBUFFER);
	lpMsgSendBody = (PMSG_CSATTKGUILDLIST*)(cBUFFER+sizeof(PMSG_ANS_CSATTKGUILDLIST));

	iCount = 0;

	lpMsgSend->btResult = gCastleSiege.GetCsAttkGuildList(lpMsgSendBody, iCount);
	lpMsgSend->iCount = iCount;

	if( lpMsgSend->iCount < 0 )	 lpMsgSend->iCount = 0;
	if( lpMsgSend->iCount > 100 )lpMsgSend->iCount = 100;

	lpMsgSend->h.set( 0xB5, ((lpMsgSend->iCount*sizeof(PMSG_CSATTKGUILDLIST))+sizeof(PMSG_ANS_CSATTKGUILDLIST)));
	DataSend(iIndex, (LPBYTE)lpMsgSend, ((lpMsgSend->iCount*sizeof(PMSG_CSATTKGUILDLIST))+sizeof(PMSG_ANS_CSATTKGUILDLIST)));
	#endif
}

void CGReqGuildMarkOfCastleOwner(PMSG_REQ_GUILDMARK_OF_CASTLEOWNER * aRecv, int iIndex) 
{
	if ( !OBJECT_RANGE(iIndex) )
	{
		return;
	}
	LPOBJ lpObj = &gObj[iIndex];
	char szGuildName[9] = {0};
	memcpy(szGuildName, gCastleSiegeSync.GetCastleOwnerGuild(), 8);
	if ( szGuildName )
	{
		GUILD_INFO_STRUCT * lpGuild = gGuildClass.SearchGuild(szGuildName);
		if ( lpGuild != NULL )
		{
			PMSG_ANS_GUILDMARK_OF_CASTLEOWNER pMsg = {0};
			pMsg.h.set(0xB9, 0x02, sizeof(pMsg));
			memcpy(pMsg.GuildMarkOfCastleOwner, lpGuild->Mark, sizeof(pMsg.GuildMarkOfCastleOwner));
			DataSend(iIndex, (LPBYTE)&pMsg, sizeof(pMsg));
		}
	}
}

void CGReqCastleHuntZoneEntrance(PMSG_REQ_MOVE_TO_CASTLE_HUNTZONE * aRecv, int iIndex) 
{
	#if(GAMESERVER_TYPE==1)
	if( !OBJECT_RANGE(iIndex) )
	{
		return;
	}
	
	LPOBJ lpObj = &gObj[iIndex];

	PMSG_ANS_MOVE_TO_CASTLE_HUNTZONE pMsg = {0};

	pMsg.h.set(0xB9, 0x05, sizeof(pMsg));

	pMsg.btResult = 0;

	int iEnterTaxMoney = 0;

	iEnterTaxMoney = gCastleSiegeSync.GetTaxHuntZone(lpObj->Index, TRUE);

	if( iEnterTaxMoney < 0)
	{
		iEnterTaxMoney = 0;
	}

	BOOL bPermission = FALSE;

	if( gCastleSiege.GetHuntZoneEnter() )
	{
		bPermission = TRUE;
	}

	if( lpObj->Guild )
	{
		if( gCastleSiege.CheckCastleOwnerMember(lpObj->Index) )
		{
			bPermission = TRUE;
		}

		if( gCastleSiege.CheckCastleOwnerUnionMember(lpObj->Index) )
		{
			bPermission = TRUE;
		}
	}
	else
	{
		if( gCastleSiege.GetHuntZoneEnter() )
		{
			bPermission = TRUE;
		}
	}

	if( bPermission == TRUE )
	{
		if( lpObj->Money >= ((DWORD)iEnterTaxMoney) )
		{
			if( lpObj->Map == MAP_CASTLE_SIEGE )
			{
				if( gObjMoveGate(lpObj->Index, 95) == TRUE )
				{
					pMsg.btResult = 1;
					int iOldMoney = lpObj->Money;

					lpObj->Money -= iEnterTaxMoney;

					gCastleSiegeSync.AddTributeMoney(iEnterTaxMoney);
					
					if( lpObj->Money < 0)
					{
						lpObj->Money = 0;
					}
					
					GCMoneySend(lpObj->Index, lpObj->Money);

					LogAdd(LOG_BLACK,"[Castle HuntZone] [%s][%s] - Entrance TAX : %d - %d = %d", lpObj->Account, lpObj->Name, iOldMoney, iEnterTaxMoney, lpObj->Money);
				}
			}
		}
	}
	DataSend(iIndex, (LPBYTE)&pMsg, sizeof(pMsg));
	#endif
}
#if(ADD_POINT)
void ExtraCpanelSendInfo(int aIndex)
{
	LPOBJ lpObj = &gObj[aIndex];
	EXTRA_CPANEL_SENDINFO pInfo;
	pInfo.h.set(0xFB, 0x20, sizeof(pInfo));

	pInfo.QuickAdd = gServerInfo.m_QuickAddButton;
	pInfo.ResetStat = gServerInfo.m_ResetStatButton;
	pInfo.dStrength = gDefaultClassInfo.m_DefaultClassInfo[gObj[aIndex].Class].Strength;
	pInfo.dDexterity = gDefaultClassInfo.m_DefaultClassInfo[gObj[aIndex].Class].Dexterity;
	pInfo.dVitality = gDefaultClassInfo.m_DefaultClassInfo[gObj[aIndex].Class].Vitality;
	pInfo.dEnergy = gDefaultClassInfo.m_DefaultClassInfo[gObj[aIndex].Class].Energy;
	pInfo.dLeadership = gDefaultClassInfo.m_DefaultClassInfo[gObj[aIndex].Class].Leadership;
	for (int i = 0; i < MAX_ACCOUNT_LEVEL; i++)
	{
		pInfo.MaxStatPoint[i] = gServerInfo.m_MaxStatPoint[i];
	}
	DataSend(aIndex, (BYTE*)&pInfo, pInfo.h.size);
}

void CGStatAddRecv(PMSG_UPDATE_STATS_RECV* lpMsg, int aIndex)
{
	LPOBJ lpObj = &gObj[aIndex];
	gObjectManager.CharacterLevelUpPointAdd(lpObj, 0, lpMsg->Str);
	gObjectManager.CharacterLevelUpPointAdd(lpObj, 1, lpMsg->Agi);
	gObjectManager.CharacterLevelUpPointAdd(lpObj, 2, lpMsg->Vit);
	gObjectManager.CharacterLevelUpPointAdd(lpObj, 3, lpMsg->Ene);
	gObjectManager.CharacterLevelUpPointAdd(lpObj, 4, lpMsg->Com);
	GCNewCharacterInfoSend(lpObj);
}
//=====================================================================
#endif

#if (SPK_ITEMLOCK)
void SendStatusLockItem(int aIndex)
{
	if (!OBJMAX_RANGE(aIndex))
	{
		return;
	}
	LPOBJ lpObj = &gObj[aIndex];

	if (lpObj->Type != OBJECT_USER)
	{
		return;

	}
	CGPACKET_LOCKWINDOW pMsg;
	pMsg.header.set(0xD3, 0x1A, sizeof(pMsg));
	pMsg.Status = (lpObj->Lock != 0) ? 1 : 0;	
	pMsg.InPass = lpObj->Lock;
	DataSend(lpObj->Index, (BYTE*)&pMsg, pMsg.header.size);
}
void SendLockUnLock(CGPACKET_LOCKWINDOW* lpMsg, int aIndex)
{
	if (!OBJMAX_RANGE(aIndex))
	{
		return;
	}
	LPOBJ lpObj = &gObj[aIndex];

	if (lpObj->Type != OBJECT_USER)
	{
		return;

	}
	int Type = lpMsg->Status;
	int password = lpMsg->InPass;
	if (Type == 0)
	{
		if (lpObj->Lock > 0)
		{
			gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, gMessage.GetMessage(775));
			return;
		}


		if (password <= 0)
		{
			return;
		}

		if (password > 99999999)
		{
			gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, gMessage.GetMessage(774));
			return;
		}

		lpObj->Lock = password;

		gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, gMessage.GetMessage(773));
		GJAccountLockSaveSend(lpObj->Index, password);
		SendStatusLockItem(lpObj->Index);
	}
	else
	{
		if (lpObj->Lock <= 0)
		{
			return;
		}


		if (password <= 0)
		{
			return;
		}

		if (password > 99999999)
		{
			gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, gMessage.GetMessage(774));
			return;
		}

		if (password != lpObj->Lock)
		{
			gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, gMessage.GetMessage(777));
			return;
		}

		lpObj->Lock = 0;

		gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, gMessage.GetMessage(776));
		GJAccountLockSaveSend(lpObj->Index, 0);
		SendStatusLockItem(lpObj->Index);
	}
}
#endif
//tue add
struct PMSG_PING
{
	PBMSG_HEAD h;	// C1:71
};

void GCSendPing(int aIndex)
{
	PMSG_PING pMsgPing;
	pMsgPing.h.set(0xFD, sizeof(pMsgPing));
	DataSend(aIndex, (BYTE*)&pMsgPing, sizeof(pMsgPing));
}
//-------