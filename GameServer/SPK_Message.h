// Message.h: interface for the CMessage class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

struct SPK_MESSAGE_INFO
{
	int Index;
	char Message[128];
};

class CSPKMessage
{
public:
	CSPKMessage();
	void Load(char* path);
	char* GetMessage(int index);
private:
	char m_DefaultMessage[128];
	std::map<int, SPK_MESSAGE_INFO> m_MessageInfo;
};

extern CSPKMessage gSPKMessage;
