#include "stdafx.h"
#include "BattleSurvivor.h"
#include "Guild.h"
#include "EffectManager.h"
#include "ItemBagManager.h"
#include "Notice.h"
#include "ObjectManager.h"
#include "CashShop.h"
#include "Gate.h"
#include "ScheduleManager.h"
#include "MemScript.h"
#include "DSProtocol.h"
#include "ItemManager.h"
#include "Message.h"
#include "ServerInfo.h"
#include "Util.h"
#if(EVENT_PK)
CBsVEvent gBsVEvent;

CBsVEvent::CBsVEvent()
{
	this->m_TickCount = GetTickCount();
	this->m_State		= 0;
	this->m_RemainTime	= 0;
	this->m_StandTime	= 0;
	this->m_CloseTime	= 0;
	this->m_TickCount	= 0;
	this->m_WarningTime = 0;
	this->m_EventTime	= 0;
	this->ScoreBlue		= 0;
	this->ScoreRed		= 0;
	this->TotalPlayer	= 0;
	this->Coin1			= 0;
	this->Coin2			= 0;
	this->Coin3			= 0;
	this->CoinCounter	= 0;
	this->CleanUser();
}

CBsVEvent::~CBsVEvent()
{
	
}

void CBsVEvent::Init()
{
	if(this->Enable == 0)
	{
		this->SetState(BSV_EVENT_STATE_BLANK);
	}
	else
	{
		this->SetState(BSV_EVENT_STATE_EMPTY);
	}
}

void CBsVEvent::Clear()
{
	this->ScoreBlue		= 0;
	this->ScoreRed		= 0;
	this->TotalPlayer	= 0;
	this->CleanUser();
}
void CBsVEvent::Load(char* path)
{
	pugi::xml_document file;
	pugi::xml_parse_result res = file.load_file(path);
	if (res.status != pugi::status_ok) {
		ErrorMessageBox("File %s load fail. Error: %s", path, res.description());
		return;
	}

	pugi::xml_node    oBsvEvent = file.child("SuperKhung");
	this->Enable	= oBsvEvent.attribute("Enable").as_int();	

	pugi::xml_node mTime = oBsvEvent.child("EventTime");
	this->m_WarningTime = mTime.attribute("WarningTime").as_int();
	this->m_StartTimePK = mTime.attribute("StartNow").as_int();
	this->m_StandTime   = mTime.attribute("StandTime").as_int();
	this->m_EventTime   = mTime.attribute("EventTime").as_int();
	this->m_CloseTime   = mTime.attribute("CloseTime").as_int();

	pugi::xml_node eEventTime = oBsvEvent.child("EventRunTime");
	for (pugi::xml_node rEventTime = eEventTime.child("RunTime"); rEventTime; rEventTime = rEventTime.next_sibling()) {
		BSV_TIME info;
		info.Year = rEventTime.attribute("Year").as_int();
		info.Month = rEventTime.attribute("Month").as_int();
		info.Day = rEventTime.attribute("Day").as_int();
		info.DayOfWeek = rEventTime.attribute("DayOfWeek").as_int();
		info.Hour = rEventTime.attribute("Hour").as_int();
		info.Minute = rEventTime.attribute("Minute").as_int();
		info.Second = rEventTime.attribute("Second").as_int();
		this->m_BSVStartTime.push_back(info);
	}

	pugi::xml_node rEventMain = oBsvEvent.child("GameMain");
	this->ReqItemCount		= rEventMain.attribute("ReqItemCount").as_int();
	this->ReqItemIndex		= rEventMain.attribute("ReqItemIndex").as_int();
	this->ReqItemLevel		= rEventMain.attribute("ReqItemLevel").as_int();	
	this->WaitingGate		= rEventMain.attribute("WaitingGate").as_int();
	this->BluTeamGate		= rEventMain.attribute("BluTeamGate").as_int();
	this->RedTeamGate		= rEventMain.attribute("RedTeamGate").as_int();
	this->SetSkin			= rEventMain.attribute("SetSkin").as_int();
	this->MinLevel			= rEventMain.attribute("MinLevel").as_int();
	this->MaxLevel			= rEventMain.attribute("MaxLevel").as_int();
	this->MinReset			= rEventMain.attribute("MinReset").as_int();
	this->MaxReset			= rEventMain.attribute("MaxReset").as_int();
	this->MinMasterReset	= rEventMain.attribute("MinMasterReset").as_int();
	this->MaxMasterReset	= rEventMain.attribute("MaxMasterReset").as_int();


	pugi::xml_node IsgCoin = oBsvEvent.child("MainCoin");
	this->Coin1				= IsgCoin.attribute("WcoinC").as_int();
	this->Coin2				= IsgCoin.attribute("WcoinP").as_int();
	this->Coin3				= IsgCoin.attribute("WcoinG").as_int();
	this->CoinKill			= IsgCoin.attribute("CoinKill").as_int();
	this->CoinDouble		= IsgCoin.attribute("DoubleCoin").as_int();

	pugi::xml_node IsgMap		= oBsvEvent.child("MainMap");
	this->m_BsVEventNPC			=	IsgMap.attribute("NPCTalk").as_int();
	this->m_BsVEventNPCMap		=	IsgMap.attribute("NPCMap").as_int();
	this->m_BsVEventNPCX		=	IsgMap.attribute("NPCX").as_int();
	this->m_BsVEventNPCY		=	IsgMap.attribute("NPCY").as_int();
	this->m_BsVEventMinUsers	=	IsgMap.attribute("EventBsVMinUsers").as_int();
	this->m_BsVEventMaxUsers	=	IsgMap.attribute("EventBsVMaxUsers").as_int();
	this->EventMap				=	IsgMap.attribute("EventMap").as_int();

	pugi::xml_node rItemDrop = oBsvEvent.child("DropItem");
	this->m_NhomItem		=	rItemDrop.attribute("Section").as_int();
	this->m_IndexItem1		=	rItemDrop.attribute("IndexItem1").as_int();
	this->m_IndexItem2		=	rItemDrop.attribute("IndexItem2").as_int();
	this->m_IndexItem3		=	rItemDrop.attribute("IndexItem3").as_int();
	this->m_IndexItem4		=	rItemDrop.attribute("IndexItem4").as_int();
	this->m_IndexItem5		=	rItemDrop.attribute("IndexItem5").as_int();	
}

void CBsVEvent::MainProc()
{
	if((GetTickCount()-this->m_TickCount) >= 1000)
	{
		this->m_TickCount = GetTickCount();
		this->m_RemainTime = (int)difftime(this->TargetTime,time(0));
		if(this->Enable == 0)
		{
			if (gServerDisplayer.EventBsV != -1)
			{
				gServerDisplayer.EventBsV = -1;
			}
		}
		else 
		{
			if (this->m_State == BSV_EVENT_STATE_EMPTY)
			{
				gServerDisplayer.EventBsV = this->m_RemainTime;
			}
			else 
			{
				if (gServerDisplayer.EventBsV != 0)
				{
					gServerDisplayer.EventBsV = 0;
				}
			}
		}
		switch(this->m_State)
		{
			case BSV_EVENT_STATE_BLANK:	this->ProcState_BLANK();break;
			case BSV_EVENT_STATE_EMPTY:	this->ProcState_EMPTY();break;
			case BSV_EVENT_STATE_STAND:	this->ProcState_STAND();break;
			case BSV_EVENT_STATE_START:	this->ProcState_START();break;
			case BSV_EVENT_STATE_CLEAN:	this->ProcState_CLEAN();break;
		}
	}
}

void CBsVEvent::ProcState_BLANK() // OK
{

}

void CBsVEvent::ProcState_EMPTY() // OK
{
	if(this->m_RemainTime > 0 && this->m_RemainTime <= (this->m_WarningTime*60))
	{
		this->CheckUser();
		this->EnterEnabled = 1;
		if((this->AlarmMinSave=(((this->m_RemainTime%60)==0)?((this->m_RemainTime/60)-1):(this->m_RemainTime/60))) != this->AlarmMinLeft)
		{
			this->AlarmMinLeft = this->AlarmMinSave;
			gNotice.GCNoticeSendToAll(0,0,0,0,0,0,"Loạn Chiến PK sẽ đóng sau %d phút!",(this->AlarmMinLeft+1));
		}
	}
	if(this->m_RemainTime <= 0)
	{
		gNotice.GCNoticeSendToAll(0,0,0,0,0,0,"Loạn Chiến PK đã đóng lại!");
		this->NoticeSendToAll(1,"Trận PK sẽ bắt đầu trong %d phút nữa!",this->m_StandTime);
		this->SetState(BSV_EVENT_STATE_STAND);
	}
}

void CBsVEvent::ProcState_STAND() // OK
{
	this->CheckUser();
	if(this->GetUserCount() < this->m_BsVEventMinUsers)
	{
		this->NoticeSendToAll(0,"Loạn chiến PK đã kết thúc X");
		this->SetState(BSV_EVENT_STATE_EMPTY);
		return;
	}
	if(this->m_RemainTime <= 0)
	{
		if (this->TotalPlayer >= this->m_BsVEventMinUsers)
		{
			this->DivisionTeam();
			this->NoticeSendToAll(0,"Loạn chiến PK đã bắt đầu!");
			this->SetState(BSV_EVENT_STATE_START);
		}
		else
		{
			this->NoticeSendToAll(0,"Loạn chiến PK đã kết thúc!");
			this->SetState(BSV_EVENT_STATE_EMPTY);
		}
	}
}

void CBsVEvent::ProcState_START() // OK
{
	this->CheckUser();
	if(this->GetUserCount() == 0)
	{
		this->SetState(BSV_EVENT_STATE_EMPTY);
		return;
	}
	if(this->GetUserCount() == 1)
	{
		this->SetState(BSV_EVENT_STATE_CLEAN);
		return;
	}
	if(this->m_RemainTime <= 0)
	{
		this->NoticeSendToAll(0,"Loạn chiến PK đã kết thúc!");
		this->SetState(BSV_EVENT_STATE_CLEAN);
	}
}

void CBsVEvent::ProcState_CLEAN() // OK
{
	this->CheckUser();
	if(this->m_RemainTime <= 0)
	{
		this->SetState(BSV_EVENT_STATE_EMPTY);
	}
}

void CBsVEvent::SetState(int state) // OK
{
	switch((this->m_State=state))
	{
		case BSV_EVENT_STATE_BLANK:	this->SetState_BLANK();		break;
		case BSV_EVENT_STATE_EMPTY:	this->SetState_EMPTY();		break;
		case BSV_EVENT_STATE_STAND:	this->SetState_STAND();		break;
		case BSV_EVENT_STATE_START:	this->SetState_START();		break;
		case BSV_EVENT_STATE_CLEAN:	this->SetState_CLEAN();		break;
	}
}

void CBsVEvent::SetState_BLANK() // OK
{

}

void CBsVEvent::SetState_EMPTY() // OK
{
	this->EnterEnabled = 0;
	this->AlarmMinSave = -1;
	this->AlarmMinLeft = -1;
	this->ClearUser();
	this->ScoreUser = 0;
	//this->ScoreRed = 0;
	this->CheckSync();
}

void CBsVEvent::SetState_STAND() // OK
{
	this->EnterEnabled = 0;
	this->AlarmMinSave = -1;
	this->AlarmMinLeft = -1;
	this->m_RemainTime = this->m_StandTime*60;
	this->TargetTime = (int)(time(0)+this->m_RemainTime);
}

void CBsVEvent::SetState_START() // OK
{
	this->EnterEnabled = 0;
	this->AlarmMinSave = -1;
	this->AlarmMinLeft = -1;
	this->m_RemainTime = this->m_EventTime*60;
	this->TargetTime = (int)(time(0)+this->m_RemainTime);
}

void CBsVEvent::SetState_CLEAN() // OK
{
	this->EnterEnabled = 0;
	this->AlarmMinSave = -1;
	this->AlarmMinLeft = -1;
	for(int n=0;n < MAX_BSVEVENT_USER;n++)
	{
		if(OBJECT_RANGE(this->User[n].Index) != 0)
		{
			gObjViewportListProtocolCreate(&gObj[this->User[n].Index]);
			gObjectManager.CharacterUpdateMapEffect(&gObj[this->User[n].Index]);
			LPOBJ lpObj = &gObj[this->User[n].Index];
			if(this->CoinDouble == 1)
			{
				LPOBJ lpTarget = &gObj[this->User[n].Index];
				BSV_EVENT_USER* lpUserB = this->GetUser(lpTarget->Index);
				if(lpUserB == 0)
				{
					return;
				}
				lpUserB->Kills++;
				if (lpUserB->Kills > 0)
				{
					this->CoinCounter = lpUserB->Kills * this->CoinKill;
					if (this->CoinCounter)
					{
						this->AddCoin = this->Coin1 + this->CoinCounter;
						if (this->AddCoin)
						{
							GDSetCoinSend(lpObj->Index, AddCoin, this->Coin2, this->Coin3, 0, 0, "Loan Chien");
						}
					}
				}
				else
				{
					GDSetCoinSend(lpObj->Index, this->Coin1, this->Coin2, this->Coin3, 0, 0, "Loan Chien");
				}
			}
			else
			{
				GDSetCoinSend(lpObj->Index, this->Coin1, this->Coin2, this->Coin3, 0, 0,"Loan Chien");
			}
			GDRankingBsVEventSaveSend(lpObj->Index, this->User[n].Kills, this->User[n].Deaths);
		}
	}
	this->m_RemainTime = this->m_CloseTime*60;
	this->TargetTime = (int)(time(0)+this->m_RemainTime);
}

void CBsVEvent::CheckSync() // OK
{
	if(this->m_BSVStartTime.empty() != 0)
	{
		this->SetState(BSV_EVENT_STATE_BLANK);
		return;
	}

	CTime ScheduleTime;
	CScheduleManager ScheduleManager;
	for(std::vector<BSV_TIME>::iterator it=this->m_BSVStartTime.begin();it != this->m_BSVStartTime.end();it++)	
	{
		ScheduleManager.AddSchedule(it->Year,it->Month,it->Day,it->Hour,it->Minute,it->Second,it->DayOfWeek);
	}
	if(ScheduleManager.GetSchedule(&ScheduleTime) == 0)
	{
		this->SetState(BSV_EVENT_STATE_BLANK);
		return;
	}
	this->m_RemainTime = (int)difftime(ScheduleTime.GetTime(),time(0));
	this->TargetTime = (int)ScheduleTime.GetTime();
}

int CBsVEvent::GetState()
{
	return this->m_State;
}

bool CBsVEvent::Dialog(LPOBJ lpObj, LPOBJ lpNpc)
{
	if (!this->Enable) return false;

	if (lpNpc->Class	== this->m_BsVEventNPC		&&
		lpNpc->Map		== this->m_BsVEventNPCMap	&&
		lpNpc->X		== this->m_BsVEventNPCX		&&
		lpNpc->Y		== this->m_BsVEventNPCY)
	{
		GCChatTargetSend(lpObj, lpNpc->Index, "Chưa đủ điều kiện tham giam gia!");
		this->CheckEnterEnabled(lpObj);
		return 1;
	}
	return false;
}

bool CBsVEvent::CheckEnterEnabled(LPOBJ lpObj)
{	
	if(OBJECT_RANGE(lpObj->PartyNumber) != 0)
	{
		gNotice.GCNoticeSend(lpObj->Index,1,0,0,0,0,0,"Bỏ party nhóm trước khi vào! #1");
		return 0;
	}
	if(lpObj->Guild != 0 && lpObj->Guild->WarState == 1)
	{
		gNotice.GCNoticeSend(lpObj->Index,1,0,0,0,0,0,"Bạn chưa thể vào lúc này! #2");
		return 0;
	}
	if(OBJECT_RANGE(lpObj->DuelUser) != 0)
	{
		gNotice.GCNoticeSend(lpObj->Index,1,0,0,0,0,0,"Bạn chưa thể vào lúc này! #3");
		return 0;
	}
	if(lpObj->PShopOpen != 0)
	{
		gNotice.GCNoticeSend(lpObj->Index,1,0,0,0,0,0,"Bạn chưa thể vào lúc này!#4");
		return 0;
	}
	if(this->GetEnterEnabled() == 0)
	{
		gNotice.GCNoticeSend(lpObj->Index,1,0,0,0,0,0,"Chưa đến thời gian mở cửa!");
		return 0;
	}
	if (this->CheckReqItems(lpObj) == 0)
	{
		gNotice.GCNoticeSend(lpObj->Index,1,0,0,0,0,0,"Bạn thiếu item cần thiết để vào!");
		return 0;
	}
	if (this->GetUserCount() >= this->m_BsVEventMaxUsers)
	{
		gNotice.GCNoticeSend(lpObj->Index,1,0,0,0,0,0,"Bạn chưa thể vào lúc này!#5");
		return 0;
	}
	if(this->MinLevel != -1 && this->MinLevel > lpObj->Level)
	{
		gNotice.GCNoticeSend(lpObj->Index,1,0,0,0,0,0,"Bạn cần đủ %d cấp độ để vào!",this->MinLevel);
		return 0;
	}
	if(this->MaxLevel != -1 && this->MaxLevel < lpObj->Level)
	{
		gNotice.GCNoticeSend(lpObj->Index,1,0,0,0,0,0,"Bạn phải đạt tối đa %d cấp độ để vào!",this->MaxLevel);
		return 0;
	}
	if(this->MinReset != -1 && this->MinReset > lpObj->Reset)
	{
		gNotice.GCNoticeSend(lpObj->Index,1,0,0,0,0,0,"Bạn cần đủ %d Reset để vào!",this->MinReset);
		return 0;
	}
	if(this->MaxReset != -1 && this->MaxReset < lpObj->Reset)
	{
		gNotice.GCNoticeSend(lpObj->Index,1,0,0,0,0,0,"Bạn phải đạt tối đa %d Reset để vào!",this->MaxReset);
		return 0;
	}
	if(this->MinMasterReset != -1 && this->MinMasterReset > lpObj->rResetLife)
	{
		gNotice.GCNoticeSend(lpObj->Index,1,0,0,0,0,0,"Bạn phải đủ %d Relife để vào!",this->MinMasterReset);
		return 0;
	}
	if(this->MaxMasterReset != -1 && this->MaxMasterReset < lpObj->rResetLife)
	{
		gNotice.GCNoticeSend(lpObj->Index,1,0,0,0,0,0,"Bạn phải đạt tối đa %d Relife để vào!",this->MaxMasterReset);
		return 0;
	}
	gEffectManager.ClearAllEffect(lpObj);
	return this->AddUser(lpObj->Index);
}

bool CBsVEvent::CheckReqItems(LPOBJ lpObj)
{
	lpObj->ChaosLock = 1;
	int count = gItemManager.GetInventoryItemCount(lpObj,this->ReqItemIndex, this->ReqItemLevel);
	if (count < this->ReqItemCount)
	{
		gNotice.GCNoticeSend(lpObj->Index,1,0,0,0,0,0,"Bạn thiếu item cần thiết để vào!");
		return false;
	}
	gItemManager.DeleteInventoryItemCount(lpObj,this->ReqItemIndex,this->ReqItemLevel,this->ReqItemCount);
	lpObj->ChaosLock = 0;
	return true;
}

int CBsVEvent::GetEnterEnabled() // OK
{
	return this->EnterEnabled;
}

int CBsVEvent::GetEnteredUserCount() // OK
{
	return this->GetUserCount();
}

bool CBsVEvent::CheckEnteredUser(int aIndex) // OK
{
	return ((this->GetUser(aIndex)==0)?0:1);
}

bool CBsVEvent::CheckPlayerTarget(LPOBJ lpObj) // OK
{
	if(this->GetState() == BSV_EVENT_STATE_START)
	{
		if (this->EventMap == lpObj->Map)
		{
			if(this->CheckEnteredUser(lpObj->Index) != 0)
			{
				return 1;
			}
		}
	}
	return 0;
}
bool CBsVEvent::CheckPlayerJoined(LPOBJ lpObj,LPOBJ lpTarget) // OK
{
	if(this->GetState() != BSV_EVENT_STATE_BLANK)
	{
		if (this->EventMap == lpObj->Map)
		{
			if(this->CheckEnteredUser(lpObj->Index) != 0)
			{
				return 1;
			}
		}
	}
	return 0;
}

bool CBsVEvent::CheckSelfTeam(LPOBJ lpObj,LPOBJ lpTarget)
{
	if(this->GetState() == BSV_EVENT_STATE_START)
	{
		BSV_EVENT_USER* lpUserA = this->GetUser(lpObj->Index);
		BSV_EVENT_USER* lpUserB = this->GetUser(lpTarget->Index);
		if (lpUserA == 0 || lpUserB == 0)
		{
			return 1;
		}
		if (lpUserA->Index == lpUserB->Index)
		{
			return 1;
		}
	}
	return 0;
}

bool CBsVEvent::AddUser(int aIndex) // OK
{
	if(OBJECT_RANGE(aIndex) == 0)
	{
		return 0;
	}
	if(this->GetUser(aIndex) != 0)
	{
		return 0;
	}
	for(int n=0;n < MAX_BSVEVENT_USER;n++)
	{
		if(OBJECT_RANGE(this->User[n].Index) != 0)
		{
			continue;
		}

		this->User[n].Index = aIndex;
		this->User[n].Available = 1;
		this->TotalPlayer++;
		gObjMoveGate(aIndex, this->WaitingGate);
		return 1;
	}
	return 0;
}

bool CBsVEvent::DelUser(int aIndex) // OK
{
	if(OBJECT_RANGE(aIndex) == 0)
	{
		return 0;
	}
	BSV_EVENT_USER* lpUser = this->GetUser(aIndex);
	if(lpUser == 0)
	{
		return 0;
	}
	lpUser->Reset();
	this->TotalPlayer--;
	return 1;
}

BSV_EVENT_USER* CBsVEvent::GetUser(int aIndex) // OK
{
	if(OBJECT_RANGE(aIndex) == 0)
	{
		return 0;
	}
	for(int n=0;n < MAX_BSVEVENT_USER;n++)
	{
		if(this->User[n].Index == aIndex)
		{
			return &this->User[n];
		}		
	}
	return 0;
}

void CBsVEvent::CleanUser() // OK
{
	for(int n=0;n < MAX_BSVEVENT_USER;n++)
	{
		this->User[n].Reset();
		this->TotalPlayer = 0;
	}
}

void CBsVEvent::ClearUser() // OK
{
	for(int n=0;n < MAX_BSVEVENT_USER;n++)
	{
		if(OBJECT_RANGE(this->User[n].Index) == 0)
		{
			continue;
		}
		gObjMoveGate(this->User[n].Index,17);
		if (this->SetSkin == 1)
		{
			gObj[this->User[n].Index].Change = -1;
		}
		gObjViewportListProtocolCreate(&gObj[this->User[n].Index]);
		gObjectManager.CharacterUpdateMapEffect(&gObj[this->User[n].Index]);
		this->User[n].Reset();
		this->TotalPlayer--;
	}
}

void CBsVEvent::CheckUser() // OK
{
	for(int n=0;n < MAX_BSVEVENT_USER;n++)
	{
		if(OBJECT_RANGE(this->User[n].Index) == 0)
		{
			continue;
		}
		if(gObjIsConnected(this->User[n].Index) == 0)
		{
			this->DelUser(this->User[n].Index);
			continue;
		}
		if (gObj[this->User[n].Index].Map != this->EventMap)
		{
			this->DelUser(this->User[n].Index);
			continue;
		}
	}	
}

int CBsVEvent::GetUserCount() // OK
{
	int count = 0;
	for(int n=0;n < MAX_BSVEVENT_USER;n++)
	{
		if(OBJECT_RANGE(this->User[n].Index) != 0)
		{
			count++;
		}
		if (gObj[n].PartyNumber >= 0)
		{
			PMSG_PARTY_DEL_MEMBER_RECV pMsg;
			int pindex = gParty.GetMemberIndex(gObj[n].PartyNumber, pMsg.number);
			if (pindex >= 0)
			{
				pMsg.number = pindex;
				gParty.CGPartyDelMemberRecv(&pMsg, gObj[n].Index);
			}
		}
	}
	return count;
}

bool CBsVEvent::GetUserRespawnLocation(LPOBJ lpObj,int* gate,int* map,int* x,int* y,int* dir,int* level) // OK
{
	if(this->Enable == 0)
	{
		return 0;
	}
	if(this->GetState() != BSV_EVENT_STATE_START)
	{
		return 0;
	}
	BSV_EVENT_USER* lpUser = this->GetUser(lpObj->Index);
	if(lpUser == 0)
	{
		return 0;
	}

	if (lpUser->Index)
	{
		if(gGate.GetGate(this->BluTeamGate,gate,map,x,y,dir,level) != 0)
		{
			return 1;
		}
	}
	else
	{
		if(gGate.GetGate(this->WaitingGate,gate,map,x,y,dir,level) != 0)
		{
			return 1;
		}
	}
	return 0;
}

void CBsVEvent::DivisionTeam()
{
	int iTeam = 1;
	int Index = 0;
	for(int i=0;i<MAX_BSVEVENT_USER;i++)
	{
		Index = this->ReturnMaxReset();
		this->User[i].Team = iTeam;
		if(iTeam == 1) iTeam = 2;
		else iTeam = 1;
	}
	
	for(int n=0;n<MAX_BSVEVENT_USER;n++)
	{
		
		if (this->User[n].Team == 1)
		{
			gObjMoveGate(this->User[n].Index, this->BluTeamGate);
			gObjViewportListProtocolCreate(&gObj[this->User[n].Index]);
			gObjectManager.CharacterUpdateMapEffect(&gObj[this->User[n].Index]);
		}
		else if (this->User[n].Team == 2)
		{
			gObjMoveGate(this->User[n].Index, this->RedTeamGate);
			gObjViewportListProtocolCreate(&gObj[this->User[n].Index]);
			gObjectManager.CharacterUpdateMapEffect(&gObj[this->User[n].Index]);
		}
	}
}

int CBsVEvent::ReturnMaxReset()
{
	int MaxRes = 0;
	int Index = 0;
	int UserIndex = 0;
	for(int n=0;n < MAX_BSVEVENT_USER;n++)
	{
		LPOBJ lpObj = &gObj[this->User[n].Index];
		if(OBJECT_RANGE(lpObj->Index) == 0)
		{
			continue;
		}

		if (!this->User[n].Available)
		{
			continue;
		}

		if(lpObj->Reset >= MaxRes)
		{
			MaxRes = lpObj->Reset;
			Index = lpObj->Index;
			UserIndex = n;
		}
	}
	this->User[UserIndex].Available = 0;
	return Index;
}

void CBsVEvent::UserDieProc(LPOBJ lpObj,LPOBJ lpTarget) // OK
{
	if (this->Enable == 0 || this->GetState() != BSV_EVENT_STATE_START)
	{
		return;
	}
	BSV_EVENT_USER* lpUserA = this->GetUser(lpObj->Index);
	if(lpUserA == 0)
	{
		return;
	}
	BSV_EVENT_USER* lpUserB = this->GetUser(lpTarget->Index);
	if(lpUserB == 0)
	{
		return;
	}

	if(lpObj->Index)
	{
		this->ScoreUser++;
	}
	else if(lpTarget->Index) 
	{
		this->ScoreUser++;
	}

	lpUserA->Deaths++;
	lpUserB->Kills++;
	gNotice.GCNoticeSend(lpUserA->Index, 1, 0, 0, 0, 0, 0, "Bạn đã bị giết %d lần!", lpUserA->Deaths);
	gNotice.GCNoticeSend(lpUserB->Index, 1, 0, 0, 0, 0, 0, "Bạn đã được cộng %d điểm!", lpUserB->Kills);

	this->NgocNgauNhien = 0;
	unsigned char ItemsIndex[5] = 
	{ 
		this->m_IndexItem1, //ID 13
		this->m_IndexItem2, //ID 14
		this->m_IndexItem3, //ID 16
		this->m_IndexItem4, //ID 22
		this->m_IndexItem5  //ID 41
	};
	this->NgocNgauNhien = ItemsIndex[rand() % 5];
	GDCreateItemSend(lpObj->Index, lpObj->Map, (BYTE)lpObj->X, (BYTE)lpObj->Y, GET_ITEM(this->m_NhomItem, this->NgocNgauNhien), 0, 0, 0, 0, 0, -1, 0, 0, 0, 0, 0, 0xFF, 0);

	for (int i=0;i<MAX_BSVEVENT_USER;i++)
	{
		if(OBJECT_RANGE(this->User[i].Index) == 0)
		{
			continue;
		}

		if(this->User[i].Index)
		{
			gNotice.GCNoticeSend(this->User[i].Index,0,0,0,0,0,0,"%s đã bị %s giết! đạt được %d điểm!",lpObj->Name,lpTarget->Name,lpUserB->Kills);
		}		
	}
}

void CBsVEvent::NoticeSendToAll(int type,char* message,...) // OK
{
	char buff[256];

	va_list arg;
	va_start(arg,message);
	vsprintf_s(buff,message,arg);
	va_end(arg);

	for(int n=0;n < MAX_BSVEVENT_USER;n++)
	{
		if(OBJECT_RANGE(this->User[n].Index) != 0)
		{
			gNotice.GCNoticeSend(this->User[n].Index,type,0,0,0,0,0,buff);
		}
	}
}

void CBsVEvent::StartBSV()
{
	time_t theTime = time(NULL);
	struct tm *aTime = localtime(&theTime);
	int hour	= aTime->tm_hour;
	int minute	= aTime->tm_min + this->m_StartTimePK;
	if (minute >= 60)
	{
		hour++;
		minute = minute-60; 
	}
	BSV_TIME info;
	info.Year = -1;
	info.Month = -1;
	info.Day = -1;
	info.DayOfWeek = -1;
	info.Hour = hour;
	info.Minute = minute;
	info.Second = 0;
	this->m_BSVStartTime.push_back(info);
	LogAdd(LOG_EVENT,"Event PK được set chạy lúc %2d:%2d:00",hour,minute);
	this->Init();
}
#endif