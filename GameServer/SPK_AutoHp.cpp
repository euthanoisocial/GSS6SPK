#include "stdafx.h"
#include "CommandManager.h"
#include "SPK_AutoHp.h"
#include "GensSystem.h"
#include "Log.h"
#include "Map.h"
#include "MapManager.h"
#include "MemScript.h"
#include "Message.h"
#include "Notice.h"
#include "ServerInfo.h"
#include "Util.h"
#include "Viewport.h"
#include "User.h"

CAUTOHP gAUTOHP;

#if(SPK_AUTOHP == 1)
void CAUTOHP::AutoHp(LPOBJ lpObj) // OK
{

	if (lpObj->AUTOHP == 0)
	{
		return;
	}
	if (lpObj->Life > 0 && lpObj->Life < ((lpObj->MaxLife + lpObj->AddLife) * 95 / 100))
	{
		PMSG_ITEM_USE_RECV pMsg;

		pMsg.header.set(0x26, sizeof(pMsg));

		pMsg.SourceSlot = 0xFF;

		pMsg.SourceSlot = ((pMsg.SourceSlot == 0xFF) ? gItemManager.GetInventoryItemSlot(lpObj, GET_ITEM(14, 3), -1) : pMsg.SourceSlot);

		pMsg.SourceSlot = ((pMsg.SourceSlot == 0xFF) ? gItemManager.GetInventoryItemSlot(lpObj, GET_ITEM(14, 2), -1) : pMsg.SourceSlot);

		pMsg.SourceSlot = ((pMsg.SourceSlot == 0xFF) ? gItemManager.GetInventoryItemSlot(lpObj, GET_ITEM(14, 1), -1) : pMsg.SourceSlot);

		pMsg.TargetSlot = 0xFF;

		pMsg.type = 0;

		if (INVENTORY_FULL_RANGE(pMsg.SourceSlot) != 0)
		{
			gItemManager.CGItemUseRecv(&pMsg, lpObj->Index);
		}
	}
}

void CAUTOHP::MainProc() // OK
{
	for(int n=OBJECT_START_USER;n < MAX_OBJECT;n++)
	{
		if(gObjIsConnectedGP(n) != 0)
		{
			gAUTOHP.AutoHp(&gObj[n]);
		}
	}
}

bool CAUTOHP::CommandHp(LPOBJ lpObj)
{
	if(lpObj->Interface.use != 0 || lpObj->Teleport != 0 || lpObj->DieRegen != 0 || lpObj->PShopOpen != 0)
	{
		gNotice.GCNoticeSend(lpObj->Index,1,0,0,0,0,0,gMessage.GetMessage(659));
		return 0;
	}
	int TimeClick = 5 * 1000; // 5 giây
	if ((GetTickCount() - lpObj->ClickClientSend) < 5 * 1000)
	{
		gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, "Click ít thôi, bộ mắc click lắm hả?");
		return false;
	}
	if (lpObj->AUTOHP == 1)
	{
		lpObj->AUTOHP = 0;		
		gNotice.GCNoticeSend(lpObj->Index,1,0,0,0,0,0,"Chế độ Auto bơm máu đã tắt");
		return 0;
	}
	else
	{
		lpObj->AUTOHP == 0;
		gNotice.GCNoticeSend(lpObj->Index,1,0,0,0,0,0,"Chế độ Auto bơm máu đã Bật");
		lpObj->AUTOHP = 1;
		return 1;
	}
	lpObj->ClickClientSend = GetTickCount();
}
#endif