// DOMNodeList.cpp: implementation of the CDOMNodeList class.
//
//////////////////////////////////////////////////////////////////////

#include "DOMNodeList.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDOMNodeList::CDOMNodeList()
{
m_pFirstNodeListItem = NULL;
m_pLastNodeListItem = NULL;
m_nItemsCount = 0;
}

CDOMNodeList::CDOMNodeList(CDOMNode* pFirstNode)
{
while (pFirstNode != NULL)
{
CDOMNodeListItem* pNewNodeListItem = new CDOMNodeListItem(pFirstNode);
if (m_pFirstNodeListItem == NULL)
m_pFirstNodeListItem = m_pLastNodeListItem = pNewNodeListItem;
else {
m_pLastNodeListItem->SetNextNodeListItem(pNewNodeListItem);
m_pLastNodeListItem = pNewNodeListItem;
}  // else if
m_nItemsCount++;
pFirstNode = pFirstNode->GetNextNodeSibling();
}  // while
}  // CDOMNodeList

CDOMNodeList::~CDOMNodeList()
{
while (m_pFirstNodeListItem != NULL)
{
m_pLastNodeListItem = m_pFirstNodeListItem;
m_pFirstNodeListItem = m_pFirstNodeListItem->GetNextNodeListItem();
delete m_pLastNodeListItem;
}  // while
}  // ~CDOMNodeList

void CDOMNodeList::AddItem(CDOMNode* pNode)
{
CDOMNodeListItem* pNewNodeListItem = new CDOMNodeListItem(pNode);
if (m_pFirstNodeListItem == NULL)
m_pFirstNodeListItem = m_pLastNodeListItem = pNewNodeListItem;
else {
m_pLastNodeListItem->SetNextNodeListItem(pNewNodeListItem);
m_pLastNodeListItem = pNewNodeListItem;
}  // else if
m_nItemsCount++;
}  // AddItem

CDOMNode* CDOMNodeList::Item(int nIndex)
{
if (nIndex < 0 || nIndex >= m_nItemsCount)
return NULL;
CDOMNodeListItem* pTmpNodeListItem = m_pFirstNodeListItem;
int i = 0;
while (i < nIndex)
{
pTmpNodeListItem = pTmpNodeListItem->GetNextNodeListItem();
i++;
}  // while
return pTmpNodeListItem->GetNode();
}  // Item

int CDOMNodeList::GetLength()
{
return m_nItemsCount;
}
