#include "stdafx.h"
#include "SPK_AddBuff.h"
#include "EffectManager.h"
#include "Notice.h"
#include "MemScript.h"
#include "Util.h"
#include <fstream>
#include <sstream>
#include <string>

AddBuffer gAddBuffer;

void AddBuffer::Read(char* FilePath) 
{
    this->IsReadDataX.SkillCount = 0;

    std::ifstream file(FilePath);
    if (!file.is_open()) 
    {
        ErrorMessageBox("Không thể mở tệp", FilePath);
        return;
    }

    std::string line;
    while (std::getline(file, line)) {
        if (line.empty() || line[0] == '/') 
        {
            continue;
        }

        if (line == "end") 
        {
            break;
        }

        std::stringstream ss(line);
        BuffData buff;

        if (ss >> buff.IDBuff >> buff.Timer >> buff.Val1 >> buff.Val2 >> buff.Val3 >> buff.Val4) 
        {
            if (this->IsReadDataX.SkillCount < 32) {
                this->IsReadDataX.Buffs[this->IsReadDataX.SkillCount] = buff;
                this->IsReadDataX.SkillCount++;
            }
            else 
            {
                LogAdd(LOG_RED, "Đã vượt quá giới hạn.");
                break;
            }
        }
    }
    file.close();
}
bool AddBuffer::CommandAddBuff(LPOBJ lpObj)
{
    int TimeClick = 5 * 1000; // 5 giây
    if ((GetTickCount() - lpObj->ClickClientSend) < 5 * 1000)
    {
        gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, "Click ít thôi, bộ mắc click lắm hả?");
        return false;
    }
    for (int i = 0; i < this->IsReadDataX.SkillCount; i++) 
    {
        BuffData& buff = this->IsReadDataX.Buffs[i];
        gEffectManager.AddEffect(lpObj, 1, buff.IDBuff, (int)(time(0) + buff.Timer), buff.Val1, buff.Val2, buff.Val3, buff.Val4);
    }

    gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, "Bạn đã được thêm Buff");
    lpObj->ClickClientSend = GetTickCount();
    return true;
}