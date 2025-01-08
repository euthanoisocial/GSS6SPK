#include "StdAfx.h"
#include "CB_ArenaPVP.h"
#include "DSProtocol.h"
#include "GuildClass.h"
#include "Map.h"
#include "MapServerManager.h"
#include "MemScript.h"
#include "Message.h"
#include "ServerInfo.h"
#include "SkillManager.h"
#include "ScheduleManager.h"
#include "Notice.h"
#include "Util.h"
#include "CommandManager.h"
#include "DSProtocol.h"
#include "GameMain.h"
#include "Guild.h"
#include "CashShop.h"
#include "Monster.h"
#include "ItemBagManager.h"
#include "Gate.h"
#include "Union.h"
#include "ObjectManager.h"
#include "MonsterSetBase.h"
#include "User.h"
#include "ObjectManager.h"
#include "Move.h"
#include "MasterSkillTree.h"
#include "CustomRankUser.h"
#if(CB_EventArenaPvP)
DWORD GetTickDelayDrop = 0;
CBArenaPVP gCBArenaPVP;

CBArenaPVP::CBArenaPVP()
{
	this->m_TickCount = GetTickCount();
	this->m_State = 0;
	this->m_RemainTime = 0;
	this->m_StandTime = 0;
	this->m_CloseTime = 0;
	this->m_TickCount = 0;
	this->m_WarningTime = 0;
	this->m_EventTime = 0;
	this->m_MessageInfo.clear();
	this->m_StartTime.clear();
	this->m_Enable = 0;
	this->m_NPC = 0;
	this->m_NPCMap = 0;
	this->m_NPCX = 0;
	this->m_NPCY = 0;
	this->m_NPCDir = 0;

	this->StausTraoThuong = 0;
	this->mDataUsePvP.clear();
}

void CBArenaPVP::SetNPC()
{
	if (this->m_Enable)
	{
		MONSTER_SET_BASE_INFO info;
		memset(&info, 0, sizeof(info));

		info.Type = 0;
		info.MonsterClass = this->m_NPC;
		info.Map = this->m_NPCMap;
		info.Dis = 0;
		info.Dir = this->m_NPCDir;
		info.X = this->m_NPCX;
		info.Y = this->m_NPCY;

		gMonsterSetBase.SetInfo(info);
	}
}

void CBArenaPVP::Init()
{
	if (this->m_Enable == 0)
	{
		this->SetState(EVENT_STATE_BLANK);
	}
	else
	{
		this->SetState(EVENT_STATE_EMPTY);
	}
}

void CBArenaPVP::Load(char* FilePath)
{
	this->StausTraoThuong = 0;
	pugi::xml_document file;
	pugi::xml_parse_result res = file.load_file(FilePath);
	if (res.status != pugi::status_ok) {
		ErrorMessageBox("File %s load fail. Error: %s", FilePath, res.description());
		return;
	}
	pugi::xml_node oEventArenaPVP = file.child("EventArenaPVP");
	this->m_Enable = oEventArenaPVP.attribute("Enable").as_int();
	pugi::xml_node Message = oEventArenaPVP.child("Message");
	for (pugi::xml_node msg = Message.child("Msg"); msg; msg = msg.next_sibling())
	{
		MESSAGE_INFO info;
		info.Index = msg.attribute("Index").as_int();
		strcpy_s(info.Message, msg.attribute("Text").as_string());
		this->m_MessageInfo.insert(std::pair<int, MESSAGE_INFO>(info.Index, info));
	}
	pugi::xml_node eEventTime = oEventArenaPVP.child("EventTime");
	for (pugi::xml_node rEventTime = eEventTime.child("Time"); rEventTime; rEventTime = rEventTime.next_sibling())
	{
		TIME info;
		info.Year = rEventTime.attribute("Year").as_int();
		info.Month = rEventTime.attribute("Month").as_int();
		info.Day = rEventTime.attribute("Day").as_int();
		info.DayOfWeek = rEventTime.attribute("DayOfWeek").as_int();
		info.Hour = rEventTime.attribute("Hour").as_int();
		info.Minute = rEventTime.attribute("Minute").as_int();
		info.Second = rEventTime.attribute("Second").as_int();
		this->m_StartTime.push_back(info);
	}
	//-Config
	pugi::xml_node eEvenNPC = oEventArenaPVP.child("EvenNPC");
	this->m_NPC = eEvenNPC.attribute("NPCClass").as_int();
	this->m_NPCMap = eEvenNPC.attribute("NPCMap").as_int();
	this->m_NPCX = eEvenNPC.attribute("NPCX").as_int();
	this->m_NPCY = eEvenNPC.attribute("NPCY").as_int();
	this->m_NPCDir = eEvenNPC.attribute("NPCDir").as_int();

	//-Config Time
	pugi::xml_node eEventStage = oEventArenaPVP.child("EventStage");
	this->m_WarningTime = eEventStage.attribute("WarningTime").as_int();
	this->m_StandTime = eEventStage.attribute("StandTime").as_int();
	this->m_EventTime = eEventStage.attribute("EventTime").as_int();
	this->m_CloseTime = eEventStage.attribute("CloseTime").as_int();

	//-Config ALl
	pugi::xml_node eEventConfig = oEventArenaPVP.child("EventConfig");
	this->m_MaxUser = eEventConfig.attribute("MaxUser").as_int();
	this->m_PointSet = eEventConfig.attribute("PointSet").as_int();
	this->m_ScoreKill = eEventConfig.attribute("ScoreKill").as_int();
	this->m_ScoreDie = eEventConfig.attribute("ScoreDie").as_int();
	this->m_MapEvent = eEventConfig.attribute("MapEvent").as_int();
	this->m_StartX= eEventConfig.attribute("StartX").as_int();
	this->m_StartY= eEventConfig.attribute("StartY").as_int();
	this->m_EndX= eEventConfig.attribute("EndX").as_int();
	this->m_EndY= eEventConfig.attribute("EndY").as_int();

	pugi::xml_node eEventYeuCau = oEventArenaPVP.child("EventYeuCau");
	this->ReqZen = eEventYeuCau.attribute("ReqZen").as_int();
	this->ReqWC = eEventYeuCau.attribute("ReqWC").as_int();
	this->ReqWP = eEventYeuCau.attribute("ReqWP").as_int();
	this->ReqGP = eEventYeuCau.attribute("ReqGP").as_int();
	this->ReqItemIndex = eEventYeuCau.attribute("ReqItemIndex").as_int();
	this->ReqItemLevel = eEventYeuCau.attribute("ReqItemLevel").as_int();
	this->ReqItemCount = eEventYeuCau.attribute("ReqItemCount").as_int();
	this->MinLevel = eEventYeuCau.attribute("MinLevel").as_int();
	this->MaxLevel = eEventYeuCau.attribute("MaxLevel").as_int();
	this->MinReset = eEventYeuCau.attribute("MinReset").as_int();
	this->MaxReset = eEventYeuCau.attribute("MaxReset").as_int();
	this->MinMasterReset = eEventYeuCau.attribute("MinMasterReset").as_int();
	this->MaxMasterReset = eEventYeuCau.attribute("MaxMasterReset").as_int();

	//==== Load Data Item Drop
	this->m_ItemDropInfo.clear();
	pugi::xml_node eEventItemDrop = oEventArenaPVP.child("EventItemDrop");
	this->DelayDrop = eEventItemDrop.attribute("DelayDrop").as_int();
	this->DropCount = eEventItemDrop.attribute("DropCount").as_int();

	for (pugi::xml_node rItem = eEventItemDrop.child("Item"); rItem; rItem = rItem.next_sibling())
	{
		ItemDropInfo InfoItemDrop;
		InfoItemDrop.IndexMin = rItem.attribute("IndexMin").as_int();
		InfoItemDrop.IndexMax = rItem.attribute("IndexMax").as_int();
		InfoItemDrop.LevelMax = rItem.attribute("LevelMax").as_int();
		InfoItemDrop.Skill = rItem.attribute("Skill").as_int();
		InfoItemDrop.Luck = rItem.attribute("Luck").as_int();
		InfoItemDrop.Option = rItem.attribute("Option").as_int();
		InfoItemDrop.Exc = rItem.attribute("Exc").as_int();		
				
		for (int i = InfoItemDrop.IndexMin; i <= InfoItemDrop.IndexMax; i++)
		{
			InfoItemDrop.Index = i;
			this->m_ItemDropInfo.push_back(InfoItemDrop);
		}		
	}

	this->m_EventReward.clear();
	pugi::xml_node eEventReward = oEventArenaPVP.child("EventReward");
	for (pugi::xml_node rTop = eEventReward.child("Top"); rTop; rTop = rTop.next_sibling())
	{
		EventReward_Data InfoTop;
		InfoTop.TopMin = rTop.attribute("TopMin").as_int();
		InfoTop.TopMax = rTop.attribute("TopMax").as_int();
		InfoTop.MinScore = rTop.attribute("MinScore").as_int();
		InfoTop.WC = rTop.attribute("WC").as_int();
		InfoTop.WP = rTop.attribute("WP").as_int();
		InfoTop.GP = rTop.attribute("GP").as_int();
		InfoTop.EventItemBag = rTop.attribute("EventItemBag").as_int();
		this->m_EventReward.push_back(InfoTop);

	}
}
char* CBArenaPVP::GetMessage(int index) // OK
{
	std::map<int, MESSAGE_INFO>::iterator it = this->m_MessageInfo.find(index);

	if (it == this->m_MessageInfo.end())
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
void CBArenaPVP::CheckSync() // OK
{
	if (this->m_StartTime.empty() != 0)
	{
		this->SetState(EVENT_STATE_BLANK);
		return;
	}

	CTime ScheduleTime;
	CScheduleManager ScheduleManager;

	for (std::vector<TIME>::iterator it = this->m_StartTime.begin(); it != this->m_StartTime.end(); it++)
	{
		ScheduleManager.AddSchedule(it->Year, it->Month, it->Day, it->Hour, it->Minute, it->Second, it->DayOfWeek);
	}

	if (ScheduleManager.GetSchedule(&ScheduleTime) == 0)
	{
		this->SetState(EVENT_STATE_BLANK);
		return;
	}

	this->m_RemainTime = (int)difftime(ScheduleTime.GetTime(), time(0));
	this->TargetTime = (int)ScheduleTime.GetTime();
}

void CBArenaPVP::MainProc()
{	
	DWORD elapsed = GetTickCount() - this->m_TickCount;
	if (elapsed < 1000)
	{
		return;
	}

	this->m_TickCount = GetTickCount();
	this->m_RemainTime = (int)difftime(this->TargetTime, time(0));

	if (this->m_Enable == 0)
	{
		if (gServerDisplayer.BEventArenaPvP != -1)
		{
			gServerDisplayer.BEventArenaPvP = -1;
		}
	}
	else
	{
		if (this->m_RemainTime >= 0 && (this->m_RemainTime < gServerDisplayer.BEventArenaPvP || gServerDisplayer.BEventArenaPvP <= 0))
		{
			if (this->m_State == EVENT_STATE_EMPTY)
			{
				gServerDisplayer.BEventArenaPvP = this->m_RemainTime;
			}
			else
			{
				if (gServerDisplayer.BEventArenaPvP != 0)
				{
					gServerDisplayer.BEventArenaPvP = 0;
				}
			}
		}
	}
	switch (this->m_State)
	{
		case EVENT_STATE_EMPTY:			this->ProcState_EMPTY();			break;
		case EVENT_STATE_STAND:			this->ProcState_STAND();			break;
		case EVENT_STATE_START:			this->ProcState_START();			break;
		case EVENT_STATE_CLEAN:			this->ProcState_CLEAN();			break;
	}	
}

void CBArenaPVP::ProcState_EMPTY() // Cacl Time
{
	if (this->m_RemainTime > 0 && this->m_RemainTime <= (this->m_WarningTime * 60))
	{
		if ((this->AlarmMinSave = (((this->m_RemainTime % 60) == 0) ? ((this->m_RemainTime / 60) - 1) : (this->m_RemainTime / 60))) != this->AlarmMinLeft)
		{
			this->AlarmMinLeft = this->AlarmMinSave;

			gNotice.GCNoticeSendToAll(0, 0, 0, 0, 0, 0, this->GetMessage(8), (this->AlarmMinLeft + 1));
		}	
		gServerDisplayer.BEventArenaPvP = this->m_RemainTime;
	}
	else if (this->m_RemainTime <= 0)
	{
		gNotice.GCNoticeSendToAll(0, 0, 0, 0, 0, 0, this->GetMessage(10));
		this->NoticeSendToAll(1, this->GetMessage(11), this->m_StandTime);
		this->SetState(EVENT_STATE_STAND);
	}	
}

void CBArenaPVP::ProcState_STAND()  // Reg Wait
{
	this->CheckUser();
	if (this->m_RemainTime > 0 && this->m_RemainTime <= (this->m_WarningTime * 60))
	{
		if ((this->AlarmMinSave = (((this->m_RemainTime % 60) == 0) ? ((this->m_RemainTime / 60) - 1) : (this->m_RemainTime / 60))) != this->AlarmMinLeft)
		{
			this->AlarmMinLeft = this->AlarmMinSave;

			gNotice.GCNoticeSendToAll(0, 0, 0, 0, 0, 0, this->GetMessage(14), (this->AlarmMinLeft + 1));
		}
	}

	if (this->m_RemainTime <= 0)
	{
		if (this->GetUserCount() >= 2)
		{
			this->NoticeSendToAll(0, this->GetMessage(12));
			this->SetState(EVENT_STATE_START);
		}
		else
		{
			this->SendClearInfo();
			LogAdd(LOG_RED, "Arena Tham Gia Size %d/%d", this->mDataUsePvP.size(), this->m_MaxUser);
			this->XoaDataEvent();
			this->NoticeSendToAll(0, this->GetMessage(9));
			this->SetState(EVENT_STATE_EMPTY);			
		}
	}
}

void CBArenaPVP::ProcState_START() // Action Event
{
	this->CheckUser();
	if (GetTickCount() > GetTickDelayDrop)
	{
		this->MakeDropItem();
		GetTickDelayDrop = GetTickCount() + (this->DelayDrop * 1000);
	}

	if (this->m_RemainTime <= 0 || this->GetUserCount() == 1)
	{
		this->NoticeSendToAll(0, this->GetMessage(13));
		this->SetState(EVENT_STATE_EMPTY);
	}
}
void CBArenaPVP::ProcState_CLEAN() // Cacl Time
{
	this->CheckUser();

	if (this->StausTraoThuong == 0)
	{
		this->ActionTraoThuong();
		this->StausTraoThuong = 1;
		return;
	}
	if (this->m_RemainTime <= 0)
	{
		this->SendClearInfo(); //Send Clear
		this->SetState(EVENT_STATE_EMPTY);
	}
}
void CBArenaPVP::SetState(int state) // OK
{
	switch ((this->m_State = state))
	{
	case EVENT_STATE_EMPTY:
		{
			this->EnterEnabled = 0;
			this->AlarmMinSave = -1;
			this->AlarmMinLeft = -1;
			this->CheckSync();
			this->XoaDataEvent();
		}
		break;
	case EVENT_STATE_STAND:
		{
			this->EnterEnabled = 1;
			this->AlarmMinSave = -1;
			this->AlarmMinLeft = -1;

			this->m_RemainTime = this->m_StandTime * 60;
			this->SendTimeEventAll(this->m_RemainTime, this->m_State);
			this->TargetTime = (int)(time(0) + this->m_RemainTime);
		}
		break;
	case EVENT_STATE_START:
		{
			this->EnterEnabled = 0;
			this->AlarmMinSave = -1;
			this->AlarmMinLeft = -1;

			this->m_RemainTime = this->m_EventTime * 60;
			this->SendTimeEventAll(this->m_RemainTime, this->m_State);
			this->TargetTime = (int)(time(0) + this->m_RemainTime);
		}
		break;
	case EVENT_STATE_CLEAN:
		{
			this->EnterEnabled = 0;
			this->AlarmMinSave = -1;
			this->AlarmMinLeft = -1;
			this->m_RemainTime = this->m_CloseTime * 60;
			this->SendTimeEventAll(this->m_RemainTime, this->m_State);
			this->TargetTime = (int)(time(0) + this->m_RemainTime);

			this->CalcUserRank(); //Get lai  Top
			this->SendBXHVeClient();
			this->StausTraoThuong = 0;
		}
		break;
		default:
	break;
	}
}
void CBArenaPVP::NoticeSendToAll(int type, char* message, ...) // OK
{
	char buff[256];

	va_list arg;
	va_start(arg, message);
	vsprintf_s(buff, message, arg);
	va_end(arg);

	for (std::map<int, EVENT_USER>::iterator it = this->mDataUsePvP.begin(); it != this->mDataUsePvP.end(); ++it)
	{
		if (OBJECT_RANGE(it->second.Index) != 0)
		{
			gNotice.GCNoticeSend(it->second.Index, type, 0, 0, 0, 0, 0, buff);
		}

	}
}

bool CBArenaPVP::CheckEnterEnabled(LPOBJ lpObj)
{
	if (this->m_Enable == 0 || !this->EnterEnabled)
	{
		gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, this->GetMessage(0));
		return 0;
	}
	if (OBJECT_RANGE(lpObj->PartyNumber) != 0)
	{
		gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, this->GetMessage(5));
		return 0;
	}

	if (lpObj->Guild != 0 && lpObj->Guild->WarState == 1)
	{
		gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, this->GetMessage(5));
		return 0;
	}

	if (OBJECT_RANGE(lpObj->DuelUser) != 0)
	{
		gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, this->GetMessage(5));
		return 0;
	}

	if (lpObj->PShopOpen != 0)
	{
		gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, this->GetMessage(5));
		return 0;
	}
	if (this->GetUserCount() >= this->m_MaxUser)
	{
		gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, this->GetMessage(2), this->m_MaxUser);
		return 0;
	}

	if (this->MinLevel != -1 && this->MinLevel > lpObj->Level)
	{
		gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, gMessage.GetMessage(224), this->MinLevel);
		return 0;
	}

	if (this->MaxLevel != -1 && this->MaxLevel < lpObj->Level)
	{
		gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, gMessage.GetMessage(230), this->MaxLevel);
		return 0;
	}

	if (this->MinReset != -1 && this->MinReset > lpObj->Reset)
	{
		gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, gMessage.GetMessage(231), this->MinReset);
		return 0;
	}

	if (this->MaxReset != -1 && this->MaxReset < lpObj->Reset)
	{
		gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, gMessage.GetMessage(232), this->MaxReset);
		return 0;
	}

	if (this->MinMasterReset != -1 && this->MinMasterReset > lpObj->MasterReset)
	{
		gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, gMessage.GetMessage(818), this->MinMasterReset);
		return 0;
	}

	if (this->MaxMasterReset != -1 && this->MaxMasterReset < lpObj->MasterReset)
	{
		gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, gMessage.GetMessage(819), this->MaxMasterReset);
		return 0;
	}
	if (this->CheckReqItems(lpObj) == 0)
	{
		return 0;
	}

	gEffectManager.ClearAllEffect(lpObj);

	int x, y;
	if (this->GetPosRandom(&x, &y) == 0)
	{
		gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, this->GetMessageA(1));
		return 0;
	}
	this->AddUser(lpObj->Index);
	EventTeleport(lpObj->Index, this->m_MapEvent, x, y);
	return 1;
}
bool CBArenaPVP::AddUser(int aIndex) // OK
{
	if (OBJECT_RANGE(aIndex) == 0)
	{
		return 0;
	}
	EVENT_USER* GetInfo = this->GetUser(aIndex);

	if (GetInfo)
	{
		GetInfo->Index = aIndex;
		GetInfo->Kills = 0;
		GetInfo->Deaths = 0;
		GetInfo->Rank = this->m_MaxUser;
		GetInfo->Score = 0;
		GetInfo->Active = 1;
		//===Backup Object
		memcpy(&GetInfo->BackupData, &gObj[aIndex], sizeof(OBJECTSTRUCT));
		GetInfo->BackupData.InventoryMap = new BYTE[INVENTORY_SIZE];
		GetInfo->BackupData.Inventory = new CItem[INVENTORY_SIZE];
		
		for (int n = 0; n < INVENTORY_SIZE; n++)
		{
			GetInfo->BackupData.Inventory[n] = gObj[aIndex].Inventory[n];
			GetInfo->BackupData.InventoryMap[n] = gObj[aIndex].InventoryMap[n];
		}

		GetInfo->BackupData.Skill = new CSkill[MAX_SKILL_LIST];
		for (int n = 0;n < MAX_SKILL_LIST;n++)
		{
			GetInfo->BackupData.Skill[n] = gObj[aIndex].Skill[n];
		}

		GetInfo->BackupData.Effect = new CEffect[MAX_EFFECT_LIST];
		for (int n = 0; n < MAX_EFFECT_LIST; n++)
		{
			GetInfo->BackupData.Effect[n] = gObj[aIndex].Effect[n];
		}
	}
	else
	{
		EVENT_USER info;
		info.Index = aIndex;
		info.Kills = 0;
		info.Deaths = 0;
		info.Rank = this->m_MaxUser;
		info.Score = 0;
		info.Active = 1;
		//===Backup Object
		memcpy(&info.BackupData, &gObj[aIndex], sizeof(OBJECTSTRUCT));
		info.BackupData.Skill = new CSkill[MAX_SKILL_LIST];
		for (int n = 0;n < MAX_SKILL_LIST;n++)
		{
			info.BackupData.Skill[n] = gObj[aIndex].Skill[n];
		}

		info.BackupData.InventoryMap = new BYTE[INVENTORY_SIZE];
		info.BackupData.Inventory = new CItem[INVENTORY_SIZE];					
		for (int n = 0; n < INVENTORY_SIZE; n++)
		{
			info.BackupData.Inventory[n] = gObj[aIndex].Inventory[n];
			info.BackupData.InventoryMap[n] = gObj[aIndex].InventoryMap[n];
		}

		info.BackupData.Effect = new CEffect[MAX_EFFECT_LIST];
		for (int n = 0; n < MAX_EFFECT_LIST; n++)
		{
			info.BackupData.Effect[n] = gObj[aIndex].Effect[n];
		}
		//====
		this->mDataUsePvP.insert(std::pair<int, EVENT_USER>(info.Index, info));
	}

	this->ClearDataUser(aIndex);
	return 1;
}
void CBArenaPVP::ClearDataUser(int aIndex)
{
	LPOBJ lpObj = &gObj[aIndex];
	if (gObjIsConnectedGP(aIndex) == 0)
		return;
	GDCharacterInfoSaveSend(lpObj->Index);

	lpObj->ArenaEventStatus = true;
	int n;
	int MaxValue = gItemManager.GetInventoryMaxValue(lpObj);
	for (n = 0; n < MaxValue; n++)
	{
		gItemManager.InventoryDelItem(lpObj->Index, n);
		gItemManager.GCItemDeleteSend(lpObj->Index, n, 1);
	}
	for (int o = INVENTORY_EXT4_SIZE; o < INVENTORY_FULL_SIZE; o++)
	{
		gItemManager.InventoryDelItem(lpObj->Index, o);
		gItemManager.GCItemDeleteSend(lpObj->Index, o, 1);
	}
	for (int n = 0;n < MAX_SKILL_LIST;n++)
	{
		lpObj->Skill[n].Clear();
		gSkillManager.GCSkillDelSend(lpObj->Index, n, 0, 0, 0);
	}
	for (n = 0; n < MAX_EFFECT_LIST; n++)
	{
		gEffectManager.DelEffect(lpObj, n);
	}

	lpObj->Money = 500000;

	lpObj->AddStrength = 0;
	lpObj->AddDexterity = 0;
	lpObj->AddVitality = 0;
	lpObj->AddEnergy = 0;
	lpObj->AddLeadership = 0;
	lpObj->ChangeSkin = 0;

	lpObj->Level = 400;
	lpObj->LevelUpPoint = this->m_PointSet;
	lpObj->Strength = 50;
	lpObj->Dexterity = 50;
	lpObj->Vitality = 50;
	lpObj->Energy = 50;
	lpObj->Leadership = 50;

	#if DANH_HIEU_NEW
	lpObj->rDanhHieu =0;
	#endif
	#if TU_LUYEN_NEW
	lpObj->rTuLuyen = 0;
	#endif
	#if QUAN_HAM_NEW
	lpObj->rQuanHam = 0;
	#endif
	#if NEW_VIP_NEW
	lpObj->rNewVip = 0;
	#endif
	#if B_HON_HOAN
	lpObj->m_RankTitle5 = 0;
	#endif

	gCustomRankUser.GCReqRankLevelUser(lpObj->Index, lpObj->Index);
	//===Info Item Send
	gItemManager.GCItemListSend(lpObj->Index);
	GCMoneySend(lpObj->Index, lpObj->Money);
	gObjectManager.CharacterMakePreviewCharSet(lpObj->Index);
	gItemManager.GCItemEquipmentSend(lpObj->Index);
	//===Info Char Point Send
	gObjectManager.CharacterCalcAttribute(lpObj->Index);
	GCNewCharacterInfoSend(lpObj);
	//====
	//=== Fix SKin Tào Lao
	gSkillManager.SkillChangeUse(lpObj->Index);
	gObjViewportListProtocolCreate(lpObj);
	gObjectManager.CharacterUpdateMapEffect(lpObj);
}
void CBArenaPVP::ClientRecvAction(BYTE* aRecv, int aIndex)
{
	if (!aRecv) return;
	XULY_CGPACKET* lpMsg = (XULY_CGPACKET*)aRecv;
	if (!lpMsg->ThaoTac) return;

	if (!this->m_Enable)
	{
		return;
	}
	if (gObjIsConnectedGP(aIndex) == 0)
	{
		return;
	}

	LPOBJ lpObj = &gObj[aIndex];

	if ((GetTickCount() - lpObj->ClickClientSend) < 2000)
	{
		return;
	}
	if (!this->CheckEnterEnabled(lpObj))
	{
		return;
	}
	this->SendTimeEventAll(this->m_RemainTime, this->m_State, aIndex);

	lpObj->ClickClientSend = GetTickCount();
	return;
}

bool CBArenaPVP::Dialog(LPOBJ lpObj, LPOBJ lpNpc)
{
	if (!this->m_Enable) return false;

	if (lpNpc->Class == this->m_NPC &&
		lpNpc->Map == this->m_NPCMap &&
		lpNpc->X == this->m_NPCX &&
		lpNpc->Y == this->m_NPCY)
	{
		BYTE send[4096];

		PMSG_COUNTLIST pMsg;
		pMsg.header.set(0xD3, 0x1E, 0);
		int size = sizeof(pMsg);
		pMsg.Count = 0;
		pMsg.TypeSend = eSendInfoNPCJoin;
		pMsg.header.size[0] = SET_NUMBERHB(size);
		pMsg.header.size[1] = SET_NUMBERLB(size);
		memcpy(send, &pMsg, sizeof(pMsg));
		DataSend(lpObj->Index, send, size);
		return 1;
	}

	return false;
}

CBArenaPVP::EVENT_USER* CBArenaPVP::GetUser(int aIndex) // OK
{
	if (OBJECT_RANGE(aIndex) == 0)
	{
		return 0;
	}
	std::map<int, EVENT_USER>::iterator it = this->mDataUsePvP.find(aIndex);

	if (it != this->mDataUsePvP.end())
	{
		return &it->second;
	}

	return 0;
}

bool CBArenaPVP::CheckReqItems(LPOBJ lpObj)
{

	if (lpObj->Money < this->ReqZen)
	{
		gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, this->GetMessage(6));
		return false;
	}
	lpObj->Money -= this->ReqZen;
	GCMoneySend(lpObj->Index, lpObj->Money);

	if (lpObj->Coin1 < this->ReqWC || lpObj->Coin2 < this->ReqWP || lpObj->Coin3 < this->ReqGP)
	{
		gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, this->GetMessage(6));
		return false;
	}
	if (this->ReqWC > 0 || this->ReqWP > 0 || this->ReqGP > 0)
	{
		GDSetCoinSend(lpObj->Index, -(this->ReqWC), -(this->ReqWP), -(this->ReqGP), 0, 0, "Arena PVP");
	}

	lpObj->ChaosLock = 1;

	int count = gItemManager.GetInventoryItemCount(lpObj, this->ReqItemIndex, this->ReqItemLevel);

	if (count < this->ReqItemCount)
	{
		gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, this->GetMessage(7));
		return false;
	}

	gItemManager.DeleteInventoryItemCount(lpObj, this->ReqItemIndex, this->ReqItemLevel, this->ReqItemCount);

	lpObj->ChaosLock = 0;
	return true;
}
void CBArenaPVP::SendTimeEventAll(int time, int State, int aIndex)
{
	BYTE send[4096];
	PMSG_COUNTLIST pMsg;
	pMsg.header.set(0xD3, 0x1E, 0);
	int size = sizeof(pMsg);
	pMsg.Count = 0;
	pMsg.TypeSend = eSendTime;
	DATA_TIME_SEND info;
	info.Map = this->m_MapEvent;
	info.TimeCount = time;
	info.State = State;
	pMsg.Count++;
	memcpy(&send[size], &info, sizeof(info));
	size += sizeof(info);
	pMsg.header.size[0] = SET_NUMBERHB(size);
	pMsg.header.size[1] = SET_NUMBERLB(size);
	memcpy(send, &pMsg, sizeof(pMsg));

	if (aIndex != -1)
	{
		if (OBJECT_RANGE(aIndex) != 0)
		{
			DataSend(aIndex, send, size);
		}
	}
	else
	{
		for (std::map<int, EVENT_USER>::iterator it = this->mDataUsePvP.begin(); it != this->mDataUsePvP.end(); ++it)
		{
			if (OBJECT_RANGE(it->second.Index) != 0)
			{
				DataSend(it->second.Index, send, size);
			}

		}
	}
}
void CBArenaPVP::gObjSendMoveViewport(LPOBJ lpObj, BYTE x, BYTE y)
{
	PMSG_MOVE_SEND pMsg;
	pMsg.header.set(PROTOCOL_CODE1, sizeof(pMsg));
	pMsg.index[0] = SET_NUMBERHB(lpObj->Index);
	pMsg.index[1] = SET_NUMBERLB(lpObj->Index);
	pMsg.x = x;
	pMsg.y = y;
	pMsg.dir = lpObj->Dir << 4;

	for (int n = 0; n < MAX_VIEWPORT; n++)
	{
		if (lpObj->VpPlayer2[n].type == OBJECT_USER)
		{
			if (lpObj->VpPlayer2[n].state != OBJECT_EMPTY && lpObj->VpPlayer2[n].state != OBJECT_DIECMD && lpObj->VpPlayer2[n].state != OBJECT_DIED)
			{
				DataSend(lpObj->VpPlayer2[n].index, (BYTE*)& pMsg, pMsg.header.size);
			}
		}
	}
}
void CBArenaPVP::EventTeleport(int aIndex, int map, int x, int y) // OK
{
	if (OBJECT_RANGE(aIndex) == 0)
	{
		return;
	}

	if (MAP_RANGE(map) == 0)
	{
		return;
	}

	LPOBJ lpObj = &gObj[aIndex];

	lpObj->State = OBJECT_DELCMD;


	if (lpObj->Map == map && abs(lpObj->X - x) <= 15 && abs(lpObj->Y - y) <= 15)
	{
		gObjSendMoveViewport(lpObj, x, y);
	}

	lpObj->X = x;
	lpObj->Y = y;
	lpObj->TX = x;
	lpObj->TY = y;
	lpObj->Map = map;
	lpObj->PathCount = 0;
	lpObj->Teleport = 0;
	lpObj->ViewState = 0;
	lpObj->MiniMapState = 0;
	lpObj->MiniMapValue = -1;
	gObjClearViewport(lpObj);
	gMove.GCTeleportSend(aIndex, 1, lpObj->Map, (BYTE)lpObj->X, (BYTE)lpObj->Y, lpObj->Dir);
	gObjViewportListProtocolCreate(lpObj);
	gObjectManager.CharacterUpdateMapEffect(lpObj);
	lpObj->RegenMapNumber = lpObj->Map;
	lpObj->RegenMapX = (BYTE)lpObj->X;
	lpObj->RegenMapY = (BYTE)lpObj->Y;
	lpObj->RegenOk = 1;
}
bool CBArenaPVP::GetPosRandom(int* x, int* y)
{
	int px, py;
	while (true)
	{
		px = this->m_StartX + (GetLargeRand() % (this->m_EndX - this->m_StartX));
		py = this->m_StartY + (GetLargeRand() % (this->m_EndY  - this->m_StartY));

		if (gMap[this->m_MapEvent].CheckAttr(px, py, 4) == 0 && gMap[this->m_MapEvent].CheckAttr(px, py, 8) == 0)
		{
			(*x) = px;
			(*y) = py;
			return 1;
			break;
		}
	}
	return 0;
}
int CBArenaPVP::GetState() // OK
{
	return this->m_State;
}
bool CBArenaPVP::DelUser(int aIndex) // OK
{
	if (this->mDataUsePvP.empty()) return 0;
	std::map<int, EVENT_USER>::iterator it = this->mDataUsePvP.find(aIndex);

	if (it != this->mDataUsePvP.end())
	{
		if (it->second.Active)
		{
			
			LPOBJ lpObj = &gObj[aIndex];
			lpObj->AddStrength = it->second.BackupData.AddStrength;
			lpObj->AddDexterity = it->second.BackupData.AddDexterity;
			lpObj->AddVitality = it->second.BackupData.AddVitality;
			lpObj->AddEnergy = it->second.BackupData.AddEnergy;
			lpObj->AddLeadership = it->second.BackupData.AddLeadership;
			lpObj->ChangeSkin = it->second.BackupData.ChangeSkin;

			lpObj->Level = it->second.BackupData.Level;
			lpObj->LevelUpPoint = it->second.BackupData.LevelUpPoint;
			lpObj->Strength = it->second.BackupData.Strength;
			lpObj->Dexterity = it->second.BackupData.Dexterity;
			lpObj->Vitality = it->second.BackupData.Vitality;
			lpObj->Energy = it->second.BackupData.Energy;
			lpObj->Leadership = it->second.BackupData.Leadership;

			#if DANH_HIEU_NEW
			lpObj->rDanhHieu = it->second.BackupData.rDanhHieu;
			#endif
			#if TU_LUYEN_NEW
			lpObj->rTuLuyen = it->second.BackupData.rTuLuyen;
			#endif
			#if QUAN_HAM_NEW
			lpObj->rQuanHam = it->second.BackupData.rQuanHam;
			#endif
			#if NEW_VIP_NEW
			lpObj->rNewVip = it->second.BackupData.rNewVip;
			#endif
			#if B_HON_HOAN
			lpObj->m_RankTitle5 = it->second.BackupData.m_RankTitle5;
			#endif

			for (int n = 0; n < INVENTORY_SIZE; n++)
			{
				lpObj->Inventory[n] = it->second.BackupData.Inventory[n];
				lpObj->InventoryMap[n] = it->second.BackupData.InventoryMap[n];
				if(it->second.BackupData.Inventory[n].IsItem()) gItemManager.InventoryAddItem(aIndex, it->second.BackupData.Inventory[n], n);
			}

			for (int n = 0;n < MAX_SKILL_LIST;n++)
			{
				lpObj->Skill[n] = it->second.BackupData.Skill[n];
				gSkillManager.AddSkill(lpObj, (int)it->second.BackupData.Skill[n].m_index, 0);
			}
			for (int n = 0; n < MAX_EFFECT_LIST; n++)
			{
				lpObj->Effect[n] = it->second.BackupData.Effect[n];
				gEffectManager.AddEffect(lpObj, 1, it->second.BackupData.Effect[n].m_index, it->second.BackupData.Effect[n].m_count, it->second.BackupData.Effect[n].m_value[0], it->second.BackupData.Effect[n].m_value[1], it->second.BackupData.Effect[n].m_value[2], it->second.BackupData.Effect[n].m_value[3]);
			}

			lpObj->ArenaEventStatus = false;

			gSkillManager.GCSkillListSend(lpObj, 0);
			gMasterSkillTree.GCMasterSkillListSend(lpObj->Index);

			gCustomRankUser.GCReqRankLevelUser(lpObj->Index, lpObj->Index);
			//===Info Item Send
			gItemManager.GCItemListSend(lpObj->Index);
			GCMoneySend(lpObj->Index, lpObj->Money);
			gObjectManager.CharacterMakePreviewCharSet(lpObj->Index);
			gItemManager.GCItemEquipmentSend(lpObj->Index);
			//===Info Char Point Send
			gObjectManager.CharacterCalcAttribute(lpObj->Index);
			GCNewCharacterInfoSend(lpObj);
			//====
			//=== Fix SKin Tào Lao
			gSkillManager.SkillChangeUse(lpObj->Index);
			gObjViewportListProtocolCreate(lpObj);
			gObjectManager.CharacterUpdateMapEffect(lpObj);
			//===
			it->second.Active = 0;
			it->second.Score = 0;
			it->second.Kills = 0;
			it->second.Deaths = 0;
			GDCharacterInfoSaveSend(lpObj->Index);
			return 1;
		}		
	}
	return 0;
}
int CBArenaPVP::GetUserCount() // OK
{
	int Count = 0;
	for (std::map<int, EVENT_USER>::iterator it = this->mDataUsePvP.begin(); it != this->mDataUsePvP.end(); ++it)
	{
		if (it->second.Active == 1)
		{
			Count++;
		}
	}
	return Count;
}
void CBArenaPVP::CheckUser() // OK
{
	if (this->mDataUsePvP.empty()) return;

	for (std::map<int, EVENT_USER>::iterator it = this->mDataUsePvP.begin(); it != this->mDataUsePvP.end(); ++it)
	{
		if (OBJECT_RANGE(it->second.Index) == 0)
		{
			continue;
		}

		if (gObjIsConnected(it->second.Index) == 0)
		{
			this->DelUser(it->second.Index);
			continue;
		}
		if (gObj[it->second.Index].X < this->m_StartX || gObj[it->second.Index].X > this->m_EndX ||
			gObj[it->second.Index].Y < this->m_StartY || gObj[it->second.Index].Y > this->m_EndY)
		{
			this->DelUser(it->second.Index);
			continue;
		}
		if (gObj[it->second.Index].Map != this->m_MapEvent )
		{
			this->DelUser(it->second.Index);
			continue;
		}
	}
}
void CBArenaPVP::XoaDataEvent() // OK
{
	if (this->mDataUsePvP.empty()) return;

	for (std::map<int, EVENT_USER>::iterator it = this->mDataUsePvP.begin(); it != this->mDataUsePvP.end(); ++it)
	{
		if (gObjIsConnected(it->second.Index))
		{
			this->DelUser(it->second.Index);
		}

	}
	this->mDataUsePvP.clear();
}
int CBArenaPVP::GetItemRand(int Min, int Max)
{
	int ItemIndexRan = 0;
	while (true)
	{
		ItemIndexRan = (Min == Max) ? Max : Min + (GetLargeRand() % (Max - Min));
		
		std::map<int, ITEM_INFO>::iterator it = gItemManager.m_ItemInfo.find(ItemIndexRan);

		if (it != gItemManager.m_ItemInfo.end())
		{
			break;
		}
	}
	return ItemIndexRan;
}

void CBArenaPVP::MakeDropItem()
{
	if (this->m_ItemDropInfo.empty())
	{
		return;
	}
	if (this->m_RemainTime > gServerInfo.m_ItemDropTime + this->DelayDrop)
	{
		for (int i = 0; i < this->DropCount; i++)
		{
			ItemDropInfo* item = &this->m_ItemDropInfo[GetLargeRand() % this->m_ItemDropInfo.size()];

			short x, y;
			while (true)
			{
				x = this->m_StartX + (GetLargeRand() % (this->m_EndX - this->m_StartX));
				y = this->m_StartY + (GetLargeRand() % (this->m_EndY - this->m_StartY));

				int ItemIndexRan = item->Index;

				int LevelRan = (item->LevelMax == 0) ? 0 : GetLargeRand() % item->LevelMax;
				int SkillRan = item->Skill;
				int LucklRan = item->Luck;
				int OptionRan = item->Option;
				int ExcRan = item->Exc;

				if (item->Skill == -1) SkillRan = GetLargeRand() % 1;
				if (item->Luck == -1) LucklRan = GetLargeRand() % 1;
				if (item->Option == -1) OptionRan = GetLargeRand() % 7;
				if (item->Exc == -1) ExcRan = GetLargeRand() % 63;

				if (gMap[this->m_MapEvent].CheckAttr(x, y, 4) == 0 && gMap[this->m_MapEvent].CheckAttr(x, y, 8) == 0)
				{
					GDCreateItemSend(OBJECT_START_USER, this->m_MapEvent, x, y, ItemIndexRan, (BYTE)LevelRan, 0, SkillRan, LucklRan, OptionRan, -1, ExcRan, 0, 0, 0, 0, 0xFF, (DWORD)time(0) + (this->m_EventTime * 1000));//Gioi han item nv 30p
					break;
				}
			}
		}
	}	
}
bool CBArenaPVP::CheckEnteredUser(int aIndex) // OK
{
	return ((this->GetUser(aIndex) == 0) ? 0 : 1);
}
bool CBArenaPVP::CheckPlayerJoined(LPOBJ lpObj, LPOBJ lpTarget)
{
	if (lpObj->Index == lpTarget->Index)
	{
		return 0;
	}
	if (this->GetState() < EVENT_STATE_STAND)
	{
		return 0;
	}
	else if (this->m_MapEvent == lpObj->Map )
	{
		if (lpObj->X < this->m_StartX || lpObj->X > this->m_EndX ||
			lpObj->Y < this->m_StartY || lpObj->Y > this->m_EndY)
		{
			return 0;
		}
		std::map<int, EVENT_USER>::iterator TeamA = this->mDataUsePvP.find(lpObj->Index);
		std::map<int, EVENT_USER>::iterator TeamB = this->mDataUsePvP.find(lpTarget->Index);

		if (TeamA != this->mDataUsePvP.end() && TeamB != this->mDataUsePvP.end())
		{
			return 1;
		}
	}
	return 0;
}
void CBArenaPVP::ClearCacheBXH()
{
	for (int i = 0; i < 10; i++)
	{
		ZeroMemory(&this->CacheTopBXH[i],sizeof(this->CacheTopBXH[i]));
	}
}
void CBArenaPVP::CalcUserRank() // OK
{
	this->ClearCacheBXH();
	for (std::map<int, EVENT_USER>::iterator itA = this->mDataUsePvP.begin(); itA != this->mDataUsePvP.end(); ++itA)
	{
		if (OBJECT_RANGE(itA->second.Index) == 0)
		{
			continue;
		}

		int rank = this->mDataUsePvP.size();
		int CountA = std::distance(this->mDataUsePvP.begin(), itA);

		std::map<int, EVENT_USER>::iterator itB = this->mDataUsePvP.begin();
		while (itB != this->mDataUsePvP.end())
		{
			if (OBJECT_RANGE(itB->second.Index) == 0 || itB->second.Active == 0)
			{
				--rank;
				goto Next;
			}

			if (itA->second.Index == itB->second.Index || itA->second.Score > itB->second.Score || (itA->second.Score == itB->second.Score && CountA < std::distance(this->mDataUsePvP.begin(), itB)))
			{
				--rank;
				goto Next;
			}
Next:
			++itB;
		}

		itA->second.Rank = (rank + 1);
		if (itA->second.Rank >= 1 && itA->second.Rank <= 10)
		{
			memcpy(&this->CacheTopBXH[itA->second.Rank - 1].Name, gObj[itA->second.Index].Name, sizeof(this->CacheTopBXH[itA->second.Rank - 1].Name));
			this->CacheTopBXH[itA->second.Rank - 1].Score = itA->second.Score;
			this->CacheTopBXH[itA->second.Rank - 1].Kill = itA->second.Kills;
		}
	}
}
void CBArenaPVP::UserDieProc(LPOBJ lpObj, LPOBJ lpTarget) // OK
{
	if (this->m_Enable == 0)
	{
		return;
	}

	if (this->GetState() != EVENT_STATE_START)
	{
		return;
	}
	std::map<int, EVENT_USER>::iterator TeamA = this->mDataUsePvP.find(lpObj->Index);
	std::map<int, EVENT_USER>::iterator TeamB = this->mDataUsePvP.find(lpTarget->Index); //Nguoi Kill
	if (TeamA == this->mDataUsePvP.end() || TeamB == this->mDataUsePvP.end())
	{
		return;
	}
	if (TeamA->second.Active == 1 && TeamB->second.Active == 1)
	{
		TeamA->second.Deaths++;
		TeamA->second.Score = (TeamA->second.Score > this->m_ScoreDie ? TeamA->second.Score - this->m_ScoreDie : 0);

		TeamB->second.Kills++;
		TeamB->second.Score += this->m_ScoreKill;

		this->CalcUserRank();
		this->SendBXHVeClient();
		this->SendInfoKillUser(TeamA->second.Index);
		this->SendInfoKillUser(TeamB->second.Index);
	}
}
void CBArenaPVP::SendInfoKillUser(int aIndex)
{

	BYTE send[4096];
	PMSG_COUNTLIST pMsg;
	pMsg.header.set(0xD3, 0x1E, 0);
	int size = sizeof(pMsg);
	pMsg.Count = 0;
	pMsg.TypeSend = eSendInfoUser;
	EVENT_USER* lpUser = this->GetUser(aIndex);

	if (lpUser == 0)
	{
		return;
	}
	//===
	DATA_INFOUSER_SEND info;
	info.Score = lpUser->Score;
	info.Kills = lpUser->Kills;
	info.Deaths = lpUser->Deaths;
	info.Rank = lpUser->Rank;

	pMsg.Count++;
	memcpy(&send[size], &info, sizeof(info));
	size += sizeof(info);
	pMsg.header.size[0] = SET_NUMBERHB(size);
	pMsg.header.size[1] = SET_NUMBERLB(size);
	// ---
	memcpy(send, &pMsg, sizeof(pMsg));

	if (OBJECT_RANGE(aIndex) != 0)
	{
		DataSend(aIndex, send, size);
	}
}
int CBArenaPVP::CheckRankIsReward(int Rank, int Score)
{
	int CountR = 0;
	for (std::vector<EventReward_Data>::iterator it = this->m_EventReward.begin(); it != this->m_EventReward.end(); it++)
	{
		if (Rank >= it->TopMin && Rank <= it->TopMax && Score >= it->MinScore)
		{
			return CountR;
		}
		CountR++;
	}
	return -1;
}
void CBArenaPVP::ActionTraoThuong()
{
	if (this->mDataUsePvP.empty()) return;
	for (std::map<int, EVENT_USER>::iterator it = this->mDataUsePvP.begin(); it != this->mDataUsePvP.end(); ++it)
	{
		if (OBJECT_RANGE(it->second.Index) == 0)
		{
			continue;
		}	

		int GetCRW = this->CheckRankIsReward(it->second.Rank, it->second.Score);

		if (GetCRW == -1)
		{
			gObjMoveGate(it->second.Index, 17);
			continue;
		}

		if (this->m_EventReward[GetCRW].WC > 0 || this->m_EventReward[GetCRW].WP > 0 || this->m_EventReward[GetCRW].GP > 0)
		{
			GDSetCoinSend(it->second.Index, this->m_EventReward[GetCRW].WC, this->m_EventReward[GetCRW].WP, this->m_EventReward[GetCRW].GP, 0, 0, "ArenaPvPCongCoin");
		}
		if (this->m_EventReward[GetCRW].EventItemBag != -1)
		{
			gItemBagManager.DropItemByIndexItemBag(this->m_EventReward[GetCRW].EventItemBag, &gObj[it->second.Index]); //Gui Item cho NV
		}
		gNotice.GCNoticeSend(it->second.Index, 0, 0, 0, 0, 0, 0, this->GetMessage(3), it->second.Rank);

		gObjMoveGate(it->second.Index, 17);

		this->DelUser(it->second.Index);
	}
}
void CBArenaPVP::SendClearInfo()
{

	BYTE send[4096];
	PMSG_COUNTLIST pMsg;
	pMsg.header.set(0xD3, 0x1E, 0);
	int size = sizeof(pMsg);
	pMsg.Count = 0;
	pMsg.TypeSend = eSendClearInfo;
	pMsg.header.size[0] = SET_NUMBERHB(size);
	pMsg.header.size[1] = SET_NUMBERLB(size);
	memcpy(send, &pMsg, sizeof(pMsg));

	for (std::map<int, EVENT_USER>::iterator it = this->mDataUsePvP.begin(); it != this->mDataUsePvP.end(); ++it)
	{
		if (OBJECT_RANGE(it->second.Index) != 0)
		{
			DataSend(it->second.Index, send, size);
		}

	}
}
void CBArenaPVP::SendBXHVeClient()
{
	BYTE send[4096];
	PMSG_COUNTLIST pMsg;
	pMsg.header.set(0xD3, 0x1E, 0);
	int size = sizeof(pMsg);
	pMsg.Count = 0;
	pMsg.TypeSend = eSendBXHKill;
	DATA_BXH_KILL info;
	for (int rTop = 0; rTop < 10; rTop++)
	{
		memcpy(&send[size], &this->CacheTopBXH[rTop], sizeof(this->CacheTopBXH[rTop]));
		size += sizeof(info);
		pMsg.Count++;
	}

	pMsg.header.size[0] = SET_NUMBERHB(size);
	pMsg.header.size[1] = SET_NUMBERLB(size);
	memcpy(send, &pMsg, sizeof(pMsg));

	for (std::map<int, EVENT_USER>::iterator it = this->mDataUsePvP.begin(); it != this->mDataUsePvP.end(); ++it)
	{
		if (OBJECT_RANGE(it->second.Index) != 0)
		{
			DataSend(it->second.Index, send, size);
		}

	}
}
//==Hoi SInh
bool CBArenaPVP::GetUserRespawnLocation(LPOBJ lpObj, int* gate, int* map, int* x, int* y, int* dir, int* level) // OK
{
	if (this->m_Enable == 0)
	{
		return 0;
	}

	if (this->m_State < EVENT_STATE_STAND)
	{
		return 0;
	}

	EVENT_USER* lpUser = this->GetUser(lpObj->Index);

	if (lpUser == 0 || lpUser->Active == 0)
	{
		return 0;
	}

	int xX, yY;
	if (this->GetPosRandom(&xX, &yY) == 0)
	{
		return 0;
	}
	(*gate) = 0;
	(*map) = this->m_MapEvent;
	(*x) = xX;
	(*y) = yY;
	(*dir) = 3;
	(*level) = 0;
	return 1;
}
void CBArenaPVP::StartEvent()
{
	time_t theTime = time(NULL);
	struct tm* aTime = localtime(&theTime);

	int hour = aTime->tm_hour;
	int minute = aTime->tm_min + 1;

	if (minute >= 60)
	{
		hour++;
		minute = minute - 60;
	}

	TIME info;
	info.Year = -1;
	info.Month = -1;
	info.Day = -1;
	info.DayOfWeek = -1;
	info.Hour = hour;
	info.Minute = minute;
	info.Second = 0;
	this->m_StartTime.push_back(info);
	LogAdd(LOG_EVENT, "[Set EventArenaPVP] At %02d:%02d", hour, minute);
	this->Init();
}
#endif