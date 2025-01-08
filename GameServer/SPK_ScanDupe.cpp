#include "stdafx.h"
#include "CommandManager.h"
#include "SPK_ScanDupe.h"
#include "GensSystem.h"
#include "Log.h"
#include "Map.h"
#include "MapManager.h"
#include "MemScript.h"
#include "Message.h"
#include "Notice.h"
#include "ServerInfo.h"
#include "Util.h"
#include "Skill.h"
#include "SkillManager.h"
#include "EffectManager.h"
ScanItemDupe gScanItemDupe;

void ScanItemDupe::QuetDupe(int aIndex)
{
	if (gObj[aIndex].Type != OBJECT_USER ) {
		return;
	}
	bool CheckNameOK = true;
	int count = strlen(gObj[aIndex].Name);

	if (count > 10)
	{
		GCCloseClientSend(aIndex, 2);
		return;
	}
	for (int i = 0; i < count; ++i) {

		if (gObj[aIndex].Name[i] < 97 && gObj[aIndex].Name[i] > 122 && gObj[aIndex].Name[i] < 65 && gObj[aIndex].Name[i] > 90 && gObj[aIndex].Name[i] < 48 && gObj[aIndex].Name[i] > 57)
		{
			GCCloseClientSend(aIndex, 2);
			return;
		}
	}
	if (gObj[aIndex].Transaction == 1 && gObj[aIndex].Interface.type == 0)
	{
		gObjInventoryRollback(aIndex);
	}
}
