// CustomPet.h: interface for the CCustomPet class.
//
//////////////////////////////////////////////////////////////////////
#pragma once

#include "Item.h"
#include "User.h"

#define MAX_CUSTOM_PET 512 // 500

struct CUSTOM_PET_INFO
{
	int Index;
	int ItemIndex;
	int IncDamageRate;
	int IncLife;
	int IncMana;
	int IncAttackSpeed;
	int IncDoubleDamageRate;
	int IncExcellentDamageRate;
	int IncExperience;
	int IncCriticalDamageRate;
	int IncReflectRate;
	int Recovery;
};

class CCustomPet
{
public:
	CCustomPet();
	virtual ~CCustomPet();
	void Load(char* path);
	bool CheckCustomPetByItem(int ItemIndex);
	bool PetIsInmortal(int ItemIndex);
	int GetCustomPetDamageRate(int ItemIndex);
	void CalcCustomPetOption(LPOBJ lpObj, bool flag);
	#if(SLOTPET2)
	void CalcCustomPet2Option(LPOBJ lpObj, bool flag);
	#endif
public:
	std::map<int, CUSTOM_PET_INFO> m_CustomPetInfo;
};

extern CCustomPet gCustomPet;