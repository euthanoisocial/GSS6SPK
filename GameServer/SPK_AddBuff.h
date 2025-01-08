#pragma once
#include "User.h"

struct BuffData {
    int IDBuff;
    int Timer;
    int Val1;
    int Val2;
    int Val3;
    int Val4;
};

struct IsReadData {
    BuffData Buffs[32]; // Giới hạn 32 buff
    int SkillCount; // Đếm số buff đã đọc
};

class AddBuffer {
public:
    IsReadData IsReadDataX;
    void AddBuffer::Read(char* FilePath);
    bool CommandAddBuff(LPOBJ lpObj);
};
extern AddBuffer gAddBuffer;