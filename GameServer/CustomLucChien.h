#pragma once
#include "Protocol.h"

struct CHAR_DATA
{
	int LCLevel;
	int LCStr;
	int LCAgi;
	int LCVit;
	int LCEne;
	int LCCmd;
	int LCLvMaster;
	int LCAddSkillMT;
	void clear()
	{
		LCLevel = 0;
		LCStr = 0;
		LCAgi = 0;
		LCVit = 0;
		LCEne = 0;
		LCCmd = 0;
		LCLvMaster = 0;
		LCAddSkillMT = 0;
	}

};
struct ITEMDATA_LUCCHIEN
{
	int IndexMin;
	int IndexMax;
	int LCCap;
	int LCGoc;
	int LCLevel;
	int LCOptSkill;
	int LCOptLuck;
	int LCOptLife;
	int LCOptExl;
	int LCOptHarmony;
	int LCOpt380;
	int LCOptAnc;
	int LCOptSocket;
};

struct DATA_LUCCHIEN
{
	PSBMSG_HEAD header; // C3:F3:03
	DWORD LucChien;
};

class CustomLucChien
{
public:
	CustomLucChien();
	virtual ~CustomLucChien();
	void Init();
	void LoadFileXML(char* FilePath);
	int Enable;
	CHAR_DATA mLucChienDataChar;
	std::vector<ITEMDATA_LUCCHIEN> mLucChienDataItem;
	void CustomLucChien::CalcLucChien(int aIndex);
	int CustomLucChien::GetLucChienItem(CItem* Item);
	
};

extern CustomLucChien gCustomLucChien;
