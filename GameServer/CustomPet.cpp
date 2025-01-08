// CustomPet.cpp: implementation of the CCustomPet class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "CustomPet.h"
#include "MemScript.h"
#include "Util.h"

CCustomPet gCustomPet;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCustomPet::CCustomPet() // OK
{
	this->m_CustomPetInfo.clear();
}

CCustomPet::~CCustomPet() // OK
{

}

void CCustomPet::Load(char* path) // OK
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

	this->m_CustomPetInfo.clear();

	try
	{
		while (true)
		{
			if (lpMemScript->GetToken() == TOKEN_END)
			{
				break;
			}

			if (strcmp("end", lpMemScript->GetString()) == 0)
			{
				break;
			}

			CUSTOM_PET_INFO info;

			memset(&info, 0, sizeof(info));

			info.Index = lpMemScript->GetNumber();

			info.ItemIndex = lpMemScript->GetAsNumber();

			info.IncLife = lpMemScript->GetAsNumber();

			info.IncMana = lpMemScript->GetAsNumber();
			
			info.IncDamageRate = lpMemScript->GetAsNumber();

			info.IncAttackSpeed = lpMemScript->GetAsNumber();

			info.IncDoubleDamageRate = lpMemScript->GetAsNumber();

			info.IncExcellentDamageRate = lpMemScript->GetAsNumber();

			info.IncExperience = lpMemScript->GetAsNumber();

			info.IncCriticalDamageRate = lpMemScript->GetAsNumber();

			info.IncReflectRate = lpMemScript->GetAsNumber();

			info.Recovery = lpMemScript->GetAsNumber();

			this->m_CustomPetInfo.insert(std::pair<int, CUSTOM_PET_INFO>(info.ItemIndex, info));
		}
	}
	catch (...)
	{
		ErrorMessageBox(lpMemScript->GetLastError());
	}

	delete lpMemScript;
}



bool CCustomPet::CheckCustomPetByItem(int ItemIndex) // OK
{
	std::map<int, CUSTOM_PET_INFO>::iterator it = this->m_CustomPetInfo.find(ItemIndex);

	if (it != this->m_CustomPetInfo.end())
	{
		return 1;
	}

	return 0;
}


bool CCustomPet::PetIsInmortal(int ItemIndex) // OK
{
	std::map<int, CUSTOM_PET_INFO>::iterator it = this->m_CustomPetInfo.find(ItemIndex);

	if (it != this->m_CustomPetInfo.end())
	{
		if (it->second.Recovery != 0)
		{
			return 1;
		}
	}

	return 0;
}

int CCustomPet::GetCustomPetDamageRate(int ItemIndex) // OK
{
	std::map<int, CUSTOM_PET_INFO>::iterator it = this->m_CustomPetInfo.find(ItemIndex);

	if (it != this->m_CustomPetInfo.end())
	{
		return it->second.IncDamageRate;
	}

	return 0;
}

void CCustomPet::CalcCustomPetOption(LPOBJ lpObj, bool flag)
{
	if (flag != 0)
	{
		return;
	}

	CItem* pEquipet = &lpObj->Inventory[8];


	std::map<int, CUSTOM_PET_INFO>::iterator it = this->m_CustomPetInfo.find(pEquipet->m_Index);

	if (it != this->m_CustomPetInfo.end())
	{
		if (pEquipet->m_Durability > 0)
		{
			lpObj->AddLife					+= it->second.IncLife;
			lpObj->AddMana					+= it->second.IncMana;

			lpObj->MagicDamageMin			+= it->second.IncDamageRate;
			lpObj->MagicDamageMax			+= it->second.IncDamageRate;
			lpObj->PhysiDamageMinLeft		+= it->second.IncDamageRate;
			lpObj->PhysiDamageMinRight		+= it->second.IncDamageRate;
			lpObj->PhysiDamageMaxLeft		+= it->second.IncDamageRate;
			lpObj->PhysiDamageMaxRight		+= it->second.IncDamageRate;

			lpObj->PhysiSpeed				+= it->second.IncAttackSpeed;
			lpObj->MagicSpeed				+= it->second.IncAttackSpeed;
			lpObj->DoubleDamageRate			+= it->second.IncDoubleDamageRate;
			lpObj->ExcellentDamageRate		+= it->second.IncExcellentDamageRate;
			lpObj->ExperienceRate			+= it->second.IncExperience;
			lpObj->MasterExperienceRate		+= it->second.IncExperience;
			lpObj->CriticalDamageRate		+= it->second.IncCriticalDamageRate;
			lpObj->DamageReflect			+= it->second.IncReflectRate;
		}
	}
}
#if(SLOTPET2)
void CCustomPet::CalcCustomPet2Option(LPOBJ lpObj, bool flag)
{
	if (flag != 0)
	{
		return;
	}

	CItem* pEquipet = &lpObj->EquipInventory[12];//Pet2

	std::map<int, CUSTOM_PET_INFO>::iterator it = this->m_CustomPetInfo.find(pEquipet->m_Index);

	if (it != this->m_CustomPetInfo.end())
	{
		if (pEquipet->m_Durability > 0)
		{
			lpObj->AddLife					+= it->second.IncLife;
			lpObj->AddMana					+= it->second.IncMana;

			lpObj->MagicDamageMin			+= it->second.IncDamageRate;
			lpObj->MagicDamageMax			+= it->second.IncDamageRate;
			lpObj->PhysiDamageMinLeft		+= it->second.IncDamageRate;
			lpObj->PhysiDamageMinRight		+= it->second.IncDamageRate;
			lpObj->PhysiDamageMaxLeft		+= it->second.IncDamageRate;
			lpObj->PhysiDamageMaxRight		+= it->second.IncDamageRate;

			lpObj->PhysiSpeed				+= it->second.IncAttackSpeed;
			lpObj->MagicSpeed				+= it->second.IncAttackSpeed;
			lpObj->DoubleDamageRate			+= it->second.IncDoubleDamageRate;
			lpObj->ExcellentDamageRate		+= it->second.IncExcellentDamageRate;
			lpObj->ExperienceRate			+= it->second.IncExperience;
			lpObj->MasterExperienceRate		+= it->second.IncExperience;
			lpObj->CriticalDamageRate		+= it->second.IncCriticalDamageRate;
			lpObj->DamageReflect			+= it->second.IncReflectRate;
		}
	}
}
#endif