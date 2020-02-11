// DOMNodeList.h: interface for the CDOMNodeList class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(DOMNODELIST_H_)
#define DOMNODELIST_H_

#include "DOMNodeListItem.h"

class CDOMNodeList  
{
public:
CDOMNodeList();
CDOMNodeList(CDOMNode* pFirstNode);
virtual ~CDOMNodeList();
void AddItem(CDOMNode* pNode);
CDOMNode* Item(int nIndex);
int GetLength();

protected:
CDOMNodeListItem* m_pFirstNodeListItem;
CDOMNodeListItem* m_pLastNodeListItem;
int m_nItemsCount;
};

#endif // !defined(DOMNODELIST_H_)
