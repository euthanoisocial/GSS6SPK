// CustomBauCua.h: interface for the CCustomBauCua class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include "User.h"

#define MAX_CUSTOM_BauCua 100


enum eCustomBauCuaKQ
{
	CUSTOM_Bau = 0,
	CUSTOM_Cua = 1,
	CUSTOM_Tom = 2,
	CUSTOM_Ca = 3,
	CUSTOM_Ga = 4,
	CUSTOM_Nai = 5,
};

struct PSBMSG_BauCua_REVC
{
	PSBMSG_HEAD header; // C1:F3:F4

	int cNai;
	int cBau;
	int cGa;
	int cCa;
	int cCua;
	int cTom;

	int WCOINC;
	int WCOINP;
	int Bless;
	int Soul;
	int Life;
	int Chaos;
};
struct PSBMSG_BauCua_SENDC
{
	PSBMSG_HEAD h; // C1:F3:F4
	int hot1;
	int hot2;
	int hot3;
	int tongtrathuong;

};

struct CUSTOM_BauCua_START_TIME
{
	int Year;
	int Month;
	int Day;
	int DayOfWeek;
	int Hour;
	int Minute;
	int Second;
};
struct CUSTOM_BauCua_USER
{
	void Reset() // OK
	{
		this->Index = -1;
		this->Cuoc_bless_bau = 0;
		this->Cuoc_bless_cua = 0;
		this->Cuoc_bless_tom = 0;
		this->Cuoc_bless_ca = 0;
		this->Cuoc_bless_ga = 0;
		this->Cuoc_bless_nai = 0;
		//
		this->Cuoc_soul_bau = 0;
		this->Cuoc_soul_cua = 0;
		this->Cuoc_soul_tom = 0;
		this->Cuoc_soul_ca = 0;
		this->Cuoc_soul_ga = 0;
		this->Cuoc_soul_nai = 0;
		//
		this->Cuoc_chao_bau = 0;
		this->Cuoc_chao_cua = 0;
		this->Cuoc_chao_tom = 0;
		this->Cuoc_chao_ca = 0;
		this->Cuoc_chao_ga = 0;
		this->Cuoc_chao_nai = 0;
		//
		this->Cuoc_life_bau = 0;
		this->Cuoc_life_cua = 0;
		this->Cuoc_life_tom = 0;
		this->Cuoc_life_ca = 0;
		this->Cuoc_life_ga = 0;
		this->Cuoc_life_nai = 0;
		//
		this->Cuoc_cre_bau = 0;
		this->Cuoc_cre_cua = 0;
		this->Cuoc_cre_tom = 0;
		this->Cuoc_cre_ca = 0;
		this->Cuoc_cre_ga = 0;
		this->Cuoc_cre_nai = 0;
		//
		this->Cuoc_long_bau = 0;
		this->Cuoc_long_cua = 0;
		this->Cuoc_long_tom = 0;
		this->Cuoc_long_ca = 0;
		this->Cuoc_long_ga = 0;
		this->Cuoc_long_nai = 0;
		//
		//
		this->Cuoc_wc_bau = 0;
		this->Cuoc_wc_cua = 0;
		this->Cuoc_wc_tom = 0;
		this->Cuoc_wc_ca = 0;
		this->Cuoc_wc_ga = 0;
		this->Cuoc_wc_nai = 0;
	}

	int Index;
	int Cuoc_bless_bau;
	int Cuoc_bless_cua;
	int Cuoc_bless_tom;
	int Cuoc_bless_ca;
	int Cuoc_bless_ga;
	int Cuoc_bless_nai;
	//
	int Cuoc_soul_bau;
	int Cuoc_soul_cua;
	int Cuoc_soul_tom;
	int Cuoc_soul_ca;
	int Cuoc_soul_ga;
	int Cuoc_soul_nai;
	//
	int Cuoc_chao_bau;
	int Cuoc_chao_cua;
	int Cuoc_chao_tom;
	int Cuoc_chao_ca;
	int Cuoc_chao_ga;
	int Cuoc_chao_nai;
	//
	int Cuoc_life_bau;
	int Cuoc_life_cua;
	int Cuoc_life_tom;
	int Cuoc_life_ca;
	int Cuoc_life_ga;
	int Cuoc_life_nai;
	//
	int Cuoc_cre_bau;
	int Cuoc_cre_cua;
	int Cuoc_cre_tom;
	int Cuoc_cre_ca;
	int Cuoc_cre_ga;
	int Cuoc_cre_nai;
	//
	int Cuoc_long_bau;
	int Cuoc_long_cua;
	int Cuoc_long_tom;
	int Cuoc_long_ca;
	int Cuoc_long_ga;
	int Cuoc_long_nai;
	//
	//
	int Cuoc_wc_bau;
	int Cuoc_wc_cua;
	int Cuoc_wc_tom;
	int Cuoc_wc_ca;
	int Cuoc_wc_ga;
	int Cuoc_wc_nai;
};
struct CUSTOM_BauCua_RULE_INFO
{
	chr Name[32];
	int AlarmTime;
	int EventTime;
};

struct CUSTOM_BauCua_INFO
{
	int Index;
	int State;
	int RemainTime;
	int TargetTime;
	int TickCount;
	int AlarmMinSave;
	int AlarmMinLeft;
	CUSTOM_BauCua_RULE_INFO RuleInfo;
	CUSTOM_BauCua_USER User[100];
	std::vector<CUSTOM_BauCua_START_TIME> StartTime;
};

struct CUSTOMBauCua_QUESTION
{
	int Index;
	int Value1;
	int Value2;
	int Value3;
	char Question[128];
	char Answer[128];
};

class CCustomBauCua
{
public:
	CCustomBauCua();
	virtual ~CCustomBauCua();
	void Load();
	void XuLyBauCua(PSBMSG_BauCua_REVC* lpMsg, int aIndex);
	CUSTOM_BauCua_USER* GetUser(CUSTOM_BauCua_INFO* lpInfo, int aIndex);

	int TatMoBauCua;
	int TyLeWinBauCua;
	int TyLeBau;
	int TyLeCua;
	int TyLeTom;
	int TyLeCa;
	int TyLeGa;
	int TyLeNai;

private:
	std::map<int, CUSTOMBauCua_QUESTION> m_CustomBauCua;

	CUSTOM_BauCua_INFO m_CustomBauCuaInfo[MAX_CUSTOM_BauCua];
	int Active;
	int IndexSelected;
	int IndexInfo;

};

extern CCustomBauCua gCustomBauCua;
