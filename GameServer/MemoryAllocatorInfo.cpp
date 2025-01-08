
#include "stdafx.h"
#include "MemoryAllocatorInfo.h"
#include "EffectManager.h"
#include "EventInventory.h"
#include "GensSystem.h"
#include "HackPacketCheck.h"
#include "MuunSystem.h"
#include "PentagramSystem.h"
#include "Quest.h"
#include "QuestWorld.h"
#include "SkillManager.h"
#include "SocketManager.h"
#if SLOTPET2
#include "NewUIMyInventory.h"
#endif

CMemoryAllocatorInfo::CMemoryAllocatorInfo() // OK
{
	this->Clear();
}

void CMemoryAllocatorInfo::Clear() // OK
{
	this->m_Index = -1;
	this->m_Active = 0;
	this->m_ActiveTime = 0;
	this->m_SkillBackup = 0;
	this->m_Skill = 0;
	this->m_VpPlayer = 0;
	this->m_VpPlayer2 = 0;
	this->m_VpPlayerItem = 0;
	this->m_HitDamage = 0;
	this->m_Effect = 0;
	this->m_PerSocketContext = 0;
	this->m_Inventory1 = 0;
	this->m_Inventory2 = 0;
	this->m_InventoryMap1 = 0;
	this->m_InventoryMap2 = 0;
	this->m_Trade = 0;
	this->m_TradeMap = 0;
	this->m_Warehouse = 0;
	this->m_WarehouseMap = 0;
	this->m_ChaosBox = 0;
	this->m_ChaosBoxMap = 0;
	#if SLOTPET2 == 0
	#if(GAMESERVER_UPDATE>=603)
	this->m_MuunInventory = 0;
	this->m_MuunInventoryMap = 0;
	#endif
	#endif
	this->m_MasterSkill = 0;
	this->m_QuestKillCount = 0;
	this->m_QuestWorldList = 0;
	this->m_GensVictimList = 0;
	this->m_SkillDelay = 0;
	this->m_HackPacketDelay = 0;
	this->m_HackPacketCount = 0;
}

void CMemoryAllocatorInfo::Alloc() // OK
{
	this->m_SkillBackup = new CSkill[MAX_SKILL_LIST];
	this->m_Skill = new CSkill[MAX_SKILL_LIST];
	this->m_VpPlayer = new VIEWPORT_STRUCT[MAX_VIEWPORT];
	this->m_VpPlayer2 = new VIEWPORT_STRUCT[MAX_VIEWPORT];
	this->m_VpPlayerItem = new VIEWPORT_STRUCT[MAX_VIEWPORT];
	this->m_HitDamage = new HIT_DAMAGE_STRUCT[MAX_HIT_DAMAGE];
	this->m_Effect = new CEffect[MAX_EFFECT_LIST];

	if(OBJECT_USER_RANGE(this->m_Index) != 0)
	{
		this->m_PerSocketContext = new PER_SOCKET_CONTEXT;
		this->m_Inventory1 = new CItem[INVENTORY_SIZE];
		this->m_Inventory2 = new CItem[INVENTORY_SIZE];
		this->m_InventoryMap1 = new BYTE[INVENTORY_SIZE];
		this->m_InventoryMap2 = new BYTE[INVENTORY_SIZE];
		this->m_Trade = new CItem[TRADE_SIZE];
		this->m_TradeMap = new BYTE[TRADE_SIZE];
		this->m_Warehouse = new CItem[WAREHOUSE_SIZE];
		this->m_WarehouseMap = new BYTE[WAREHOUSE_SIZE];
		this->m_ChaosBox = new CItem[CHAOS_BOX_SIZE];
		this->m_ChaosBoxMap = new BYTE[CHAOS_BOX_SIZE];
		#if(SLOTPET2)
		this->m_EquipInventory1 = new CItem[EQUIP_INVENTORY_SIZE];
		this->m_EquipInventory2 = new CItem[EQUIP_INVENTORY_SIZE];
		this->m_EquipInventoryMap1 = new BYTE[EQUIP_INVENTORY_SIZE];
		this->m_EquipInventoryMap2 = new BYTE[EQUIP_INVENTORY_SIZE];
		#else
		#if(GAMESERVER_UPDATE>=603)
		this->m_MuunInventory = new CItem[MUUN_INVENTORY_SIZE];
		this->m_MuunInventoryMap = new BYTE[MUUN_INVENTORY_SIZE];
		#endif
		#endif
		this->m_MasterSkill = new CSkill[MAX_MASTER_SKILL_LIST];
		this->m_QuestKillCount = new QUEST_KILL_COUNT[MAX_QUEST_KILL_COUNT];
		this->m_QuestWorldList = new QUEST_WORLD_LIST[MAX_QUEST_WORLD_LIST];
		this->m_GensVictimList = new GENS_SYSTEM_VICTIM_LIST[MAX_GENS_SYSTEM_VICTIM];
		this->m_SkillDelay = new DWORD[MAX_SKILL];
		this->m_HackPacketDelay = new DWORD[MAX_HACK_PACKET_INFO];
		this->m_HackPacketCount = new DWORD[MAX_HACK_PACKET_INFO];
		#if(SAU_ANTIHACK)
		this->m_BUserAttack = new BAntihackDataSkill[MAX_SKILL];
		#endif
		#if(HT_THUE_FLAG)
		this->m_mInfoBuyFlag = new CInfoFlagBuy[MAX_BUYFLAG_CB];
		#endif
	}
}
