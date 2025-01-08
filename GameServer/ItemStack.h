// ItemStack.h: interface for the CItemStack class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

struct ITEM_STACK_INFO
{
	int Index;
	int MaxStack;
	int CreateItemIndex;
	#if(BUNDLE_ITEM)
	int ItemLevel;
	#endif
};

class CItemStack
{
public:
	CItemStack();
	virtual ~CItemStack();
	void Load(char* path);
	int GetItemMaxStack(int index);
	int GetCreateItemIndex(int index);
#if(BUNDLE_ITEM)
	std::map<int,ITEM_STACK_INFO> m_ItemStackInfo;
#else
private:
	std::map<int,ITEM_STACK_INFO> m_ItemStackInfo;
#endif
};

extern CItemStack gItemStack;
