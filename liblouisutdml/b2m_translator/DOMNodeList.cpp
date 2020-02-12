// DOMNodeList.cpp: implementation of the CDOMNodeList class.
//
//////////////////////////////////////////////////////////////////////

#include "DOMNodeList.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

const int ITEMS_RESIZING_STEP = 20;

CDOMNodeList::CDOMNodeList() {
  m_nMaxItemsCount = ITEMS_RESIZING_STEP;
  m_ppItems = new CDOMNodePtr[m_nMaxItemsCount];
  memset(m_ppItems, m_nMaxItemsCount*sizeof(CDOMNodePtr), 0);
  m_nItemsCount = 0;
}  // CDOMNodeList

CDOMNodeList::CDOMNodeList(CDOMNode* pFirstNode) {
  m_nMaxItemsCount = ITEMS_RESIZING_STEP;
  m_ppItems = new CDOMNodePtr[m_nMaxItemsCount];
  memset(m_ppItems, m_nMaxItemsCount*sizeof(CDOMNodePtr), 0);
  m_nItemsCount = 0;

  while (pFirstNode) {
    if (m_nItemsCount ==m_nMaxItemsCount) {
      m_nMaxItemsCount += ITEMS_RESIZING_STEP;
      CDOMNodePtr* ppItems = new CDOMNodePtr[m_nMaxItemsCount];
      memset(ppItems, m_nMaxItemsCount*sizeof(CDOMNodePtr), 0);
      memcpy(ppItems, m_ppItems, m_nItemsCount*sizeof(CDOMNodePtr));
      delete[] m_ppItems;
      m_ppItems = ppItems;
    }
    m_ppItems[m_nItemsCount++] = pFirstNode;
    pFirstNode = pFirstNode->GetNextNodeSibling();
  }  // while
}  // CDOMNodeList

CDOMNodeList::~CDOMNodeList() {
  delete[] m_ppItems;
}

void CDOMNodeList::AddItem(CDOMNode* pNode) {
  if (m_nItemsCount ==m_nMaxItemsCount) {
    m_nMaxItemsCount += ITEMS_RESIZING_STEP;
    CDOMNodePtr* ppItems = new CDOMNodePtr[m_nMaxItemsCount];
    memset(ppItems, m_nMaxItemsCount*sizeof(CDOMNodePtr), 0);
    memcpy(ppItems, m_ppItems, m_nItemsCount*sizeof(CDOMNodePtr));
    delete[] m_ppItems;
    m_ppItems = ppItems;
  }
  m_ppItems[m_nItemsCount++] = pNode;
}  // AddItem

CDOMNode* CDOMNodeList::Item(int nIndex) {
  if (nIndex < 0 || nIndex >= m_nItemsCount)
    return NULL;
  return m_ppItems[nIndex];
}  // Item
