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
  if (m_nItemsCount == 0)
    return NULL;
  for (int i = 0; i < m_nItemsCount; i++) {
    CString* pStrNodeName = m_ppItems[i]->GetNodeName();
    if (pStrNodeName && *pStrNodeName == *pStrName)
      return m_ppItems[i];
  }  // for
  return NULL;
}  // GetNamedItem

CDOMNode* CDOMNamedNodeList::RemoveNamedItem(CString* pStrName) {
  if (m_nItemsCount == 0)
    return NULL;
  int i = 0;
  for (; i < m_nItemsCount; i++) {
    CString* pStrNodeName = m_ppItems[i]->GetNodeName();
    if (pStrNodeName && *pStrNodeName == *pStrName)
      break;
  }  // for
  if (i == m_nItemsCount)
    return NULL;
  CDOMNode* pNode = m_ppItems[i];
  for (int j = i+1; j < m_nItemsCount; j++)
    m_ppItems[j-1] = m_ppItems[j];
  m_nItemsCount--;
  return pNode;
}  // RemoveNamedItem
