// DOMNamedNodeList.cpp: implementation of the CDOMNamedNodeList class.
//
//////////////////////////////////////////////////////////////////////

#include "DOMNamedNodeList.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDOMNamedNodeList::CDOMNamedNodeList() : CDOMNodeList() {
}

CDOMNamedNodeList::CDOMNamedNodeList(CDOMNode* pFirstNode)
    : CDOMNodeList(pFirstNode) {
}

CDOMNamedNodeList::~CDOMNamedNodeList() {
}

CDOMNode* CDOMNamedNodeList::GetNamedItem(CString* pStrName) {
  CDOMNodeListItem* pTmpNodeListItem = m_pFirstNodeListItem;
  CString* pStrNodeName;
  while (pTmpNodeListItem != NULL) {
    pStrNodeName = pTmpNodeListItem->GetNode()->GetNodeName();
    if (pStrNodeName != NULL && *pStrNodeName == *pStrName)
      return pTmpNodeListItem->GetNode();
    pTmpNodeListItem = pTmpNodeListItem->GetNextNodeListItem();
  }  // while
  return NULL;
}  // GetNamedItem

CDOMNode* CDOMNamedNodeList::RemoveNamedItem(CString* pStrName) {
  CDOMNodeListItem* pTmpNodeListItem = m_pFirstNodeListItem;
  CDOMNodeListItem* pTNodeListItem;
  CString* pStrNodeName;
  CDOMNode* pTmpNode;
  while (pTmpNodeListItem != NULL) {
    pStrNodeName = pTmpNodeListItem->GetNode()->GetNodeName();
    if (pStrNodeName != NULL && *pStrNodeName == *pStrName) {
      pTmpNode = pTmpNodeListItem->GetNode();
      if (m_pFirstNodeListItem == pTmpNodeListItem) {
        m_pFirstNodeListItem = pTmpNodeListItem->GetNextNodeListItem();
        if (m_pFirstNodeListItem == NULL)
          m_pLastNodeListItem = NULL;
      } else {
        pTNodeListItem = m_pFirstNodeListItem;
        while (pTNodeListItem->GetNextNodeListItem() != pTmpNodeListItem)
          pTNodeListItem = pTNodeListItem->GetNextNodeListItem();
        pTNodeListItem->SetNextNodeListItem(
            pTmpNodeListItem->GetNextNodeListItem());
        if (pTNodeListItem->GetNextNodeListItem() == NULL)
          m_pLastNodeListItem = pTNodeListItem;
      }  // else if
      delete pTmpNodeListItem;
      m_nItemsCount--;
      return pTmpNode;
    }  // if
    pTmpNodeListItem = pTmpNodeListItem->GetNextNodeListItem();
  }  // while
  return NULL;
}  // RemoveNamedItem
