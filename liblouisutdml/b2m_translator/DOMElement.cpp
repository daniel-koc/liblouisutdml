// DOMElement.cpp: implementation of the CDOMElement class.
//
//////////////////////////////////////////////////////////////////////

#include "DOMElement.h"
#include "DOMDocument.h"
#include "DOMAttr.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

CListItem::CListItem(LISTITEM pItem) {
  m_pItem = pItem;
  m_pNext = NULL;
}

CListItem::~CListItem() {
}

CList::CList() {
  m_pFirst = NULL;
  m_pLast = NULL;
  m_nItemsCount = 0;
  m_bDeletingAllItemsFlag = false;
}  // CList

CList::~CList() {
  if (m_bDeletingAllItemsFlag)
    DeleteAll();
  else
    RemoveAll();
}  // ~CList

void CList::AddItem(const LISTITEM pItem) {
  CListItem* pNew = new CListItem(pItem);
  if (!m_pFirst)
    m_pCur = m_pFirst = m_pLast = pNew;
  else {
    m_pLast->m_pNext = pNew;
    m_pLast = pNew;
  }  // else if
  m_nItemsCount++;
}  // AddItem

LISTITEM* CList::GetItems() {
  if (m_nItemsCount == 0)
    return NULL;
  LISTITEM* pItems = new LISTITEM[m_nItemsCount];
  CListItem* pTmp = m_pFirst;
  int i = 0;
  do {
    pItems[i++] = pTmp->m_pItem;
    pTmp = (CListItem*)pTmp->m_pNext;
  } while (pTmp);
  return pItems;
}  // GetItems

LISTITEM CList::Next() {
  if (!m_pCur)
    return NULL;
  LISTITEM pItem = m_pCur->m_pItem;
  m_pCur = (CListItem*)m_pCur->m_pNext;
  return pItem;
}  // Next

LISTITEM CList::RemoveFirst() {
  if (!m_pFirst)
    return NULL;
  if (m_pCur == m_pFirst)
    m_pCur = NULL;
  LISTITEM pItem = m_pFirst->m_pItem;
  CListItem* pTmp = m_pFirst;
  m_pFirst = (CListItem*)m_pFirst->m_pNext;
  delete pTmp;
  m_nItemsCount--;
  return pItem;
}  // RemoveGetFirst

LISTITEM CList::RemoveLast() {
  if (!m_pLast)
    return NULL;
  if (m_pCur == m_pLast)
    m_pCur = NULL;
  m_nItemsCount--;
  if (!m_pFirst->m_pNext) {
    LISTITEM pItem = m_pFirst->m_pItem;
    delete m_pFirst;
    m_pFirst = m_pLast = NULL;
    return pItem;
  } else {
    CListItem* pTmp = m_pFirst;
    while (((CListItem*)pTmp->m_pNext)->m_pNext)
      pTmp = (CListItem*)pTmp->m_pNext;
    pTmp->m_pNext = NULL;
    LISTITEM pItem = m_pLast->m_pItem;
    delete m_pLast;
    m_pLast = pTmp;
    return pItem;
  }  // else if
}  // RemoveLast

LISTITEM CList::GetAt(int nIndex) {
  if (nIndex < 0 || nIndex >= m_nItemsCount)
    return NULL;
  CListItem* pTmp = m_pFirst;
  int i = 0;
  while (i < nIndex) {
    pTmp = (CListItem*)pTmp->m_pNext;
    i++;
  }  // while
  return pTmp->m_pItem;
}  // GetAt

void CList::RemoveAll() {
  CListItem* pTmpItem;
  while (m_pFirst) {
    pTmpItem = m_pFirst;
    m_pFirst = (CListItem*)m_pFirst->m_pNext;
    delete pTmpItem;
  }  // while
  m_pFirst = NULL;
  m_pLast = NULL;
  m_pCur = NULL;
  m_nItemsCount = 0;
}  // RemoveAll

void CList::DeleteAll() {
  CListItem* pTmpItem;
  while (m_pFirst) {
    pTmpItem = m_pFirst;
    m_pFirst = (CListItem*)m_pFirst->m_pNext;
    delete pTmpItem->m_pItem;
    delete pTmpItem;
  }  // while
  m_pFirst = NULL;
  m_pLast = NULL;
  m_pCur = NULL;
  m_nItemsCount = 0;
}  // DeleteAll

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDOMElement::CDOMElement(CDOMDocument* pOwnerDocument)
    : CDOMNode(pOwnerDocument, DOM_ELEMENT_NODE) {
  m_pStrTagName = NULL;
  m_pFirstAttr = NULL;
  m_pLastAttr = NULL;
}  // CDOMElement

CDOMElement::~CDOMElement() {
  if (m_pStrTagName)
    delete m_pStrTagName;
  while (m_pFirstAttr) {
    m_pLastAttr = m_pFirstAttr;
    m_pFirstAttr = (CDOMAttr*)m_pFirstAttr->GetNextNodeSibling();
    delete m_pLastAttr;
  }  // while
}  // ~CDOMElement

CDOMNamedNodeList* CDOMElement::GetAttributeNodes() {
  return new CDOMNamedNodeList(m_pFirstAttr);
}

CDOMNode* CDOMElement::CopyNode() {
  CDOMElement* pNewElement = GetOwnerDocument()->CreateElement(
      m_pStrTagName ? new CString(*m_pStrTagName) : NULL);
  CDOMAttr* pTmpAttr = m_pFirstAttr;
  while (pTmpAttr) {
    CString* pStrName = pTmpAttr->GetName();
    CString* pStrValue = pTmpAttr->GetValue();
    pNewElement->AddAttribute(
        pStrName ? new CString(*pStrName) : NULL,
        pStrValue ? new CString(*pStrValue) : NULL);
    pTmpAttr = (CDOMAttr*)pTmpAttr->GetNextNodeSibling();
  }  // while
  return pNewElement;
}  // CopyNode

CString CDOMElement::ToString() {
  CString str;
  str += L"<";
  str += *m_pStrTagName;
  if (m_pFirstAttr) {
    CDOMAttr* pTmpAttr = m_pFirstAttr;
    while (pTmpAttr) {
      str += L" ";
      str += pTmpAttr->ToString();
      pTmpAttr = (CDOMAttr*)pTmpAttr->GetNextNodeSibling();
    }  // while
  }    // if
  str += L">";
  str += CDOMNode::ToString();
  str += L"</";
  str += *m_pStrTagName;
  str += L">\r\n";
  return str;
}  // ToString

void CDOMElement::SetTagName(CString* pStrTagName) {
  if (m_pStrTagName)
    delete m_pStrTagName;
  m_pStrTagName = pStrTagName;
}  // SetTagName

bool CDOMElement::AddAttribute(CString* pStrName, CString* pStrValue) {
  if (HasAttribute(pStrName))
    return false;
  CDOMAttr* pNewAttr = GetOwnerDocument()->CreateAttribute(pStrName);
  pNewAttr->SetOwnerElement(this);
  pNewAttr->SetValue(pStrValue);
  if (!m_pFirstAttr)
    m_pFirstAttr = m_pLastAttr = pNewAttr;
  else {
    m_pLastAttr->SetNextNodeSibling(pNewAttr);
    pNewAttr->SetPreviousNodeSibling(m_pLastAttr);
    m_pLastAttr = pNewAttr;
  }  // else if
  return true;
}  // AddAttribute

bool CDOMElement::SetAttribute(wchar_t* pStrName, wchar_t* pStrValue) {
  return SetAttribute(
      pStrName ? new CString(pStrName) : NULL,
      pStrValue ? new CString(pStrValue) : NULL);
}

bool CDOMElement::SetAttribute(const wchar_t* pStrName,
                               const wchar_t* pStrValue) {
  return SetAttribute(
      pStrName ? new CString(pStrName) : NULL,
      pStrValue ? new CString(pStrValue) : NULL);
}

bool CDOMElement::SetAttribute(const wchar_t* pStrName, wchar_t* pStrValue) {
  return SetAttribute(
      pStrName ? new CString(pStrName) : NULL,
      pStrValue ? new CString(pStrValue) : NULL);
}

bool CDOMElement::SetAttribute(CString* pStrName, CString* pStrValue) {
  if (m_pFirstAttr) {
    CDOMAttr* pTmpAttr = m_pFirstAttr;
    do {
      if (*(pTmpAttr->GetName()) == *pStrName) {
        pTmpAttr->SetValue(pStrValue);
        return true;
      }  // if
      pTmpAttr = (CDOMAttr*)pTmpAttr->GetNextNodeSibling();
    } while (pTmpAttr);
  }  // if
  CDOMAttr* pNewAttr = GetOwnerDocument()->CreateAttribute(pStrName);
  pNewAttr->SetOwnerElement(this);
  pNewAttr->SetValue(pStrValue);
  if (!m_pFirstAttr)
    m_pFirstAttr = m_pLastAttr = pNewAttr;
  else {
    m_pLastAttr->SetNextNodeSibling(pNewAttr);
    pNewAttr->SetPreviousNodeSibling(m_pLastAttr);
    m_pLastAttr = pNewAttr;
  }  // else if
  return false;
}  // SetAttribute

CString* CDOMElement::GetAttribute(CString* pStrName) {
  if (m_pFirstAttr) {
    CDOMAttr* pTmpAttr = m_pFirstAttr;
    do {
      if (*(pTmpAttr->GetName()) == *pStrName)
        return pTmpAttr->GetValue();
      pTmpAttr = (CDOMAttr*)pTmpAttr->GetNextNodeSibling();
    } while (pTmpAttr);
  }  // if
  return NULL;
}  // GetAttribute

bool CDOMElement::RemoveAttribute(CString* pStrName) {
  if (m_pFirstAttr) {
    CDOMAttr* pTmpAttr = m_pFirstAttr;
    do {
      if (*(pTmpAttr->GetName()) == *pStrName) {
        if (pTmpAttr->GetPreviousNodeSibling()) {
          pTmpAttr->GetPreviousNodeSibling()->SetNextNodeSibling(
              pTmpAttr->GetNextNodeSibling());
        } else
          m_pFirstAttr = (CDOMAttr*)pTmpAttr->GetNextNodeSibling();
        if (pTmpAttr->GetNextNodeSibling()) {
          pTmpAttr->GetNextNodeSibling()->SetPreviousNodeSibling(
              pTmpAttr->GetPreviousNodeSibling());
        } else
          m_pLastAttr = (CDOMAttr*)pTmpAttr->GetPreviousNodeSibling();
        return true;
      }  // if
      pTmpAttr = (CDOMAttr*)pTmpAttr->GetNextNodeSibling();
    } while (pTmpAttr);
  }  // if
  return false;
}  // RemoveAttribute

CDOMAttr* CDOMElement::SetAttributeNode(CDOMAttr* pNewAttr) {
  RemoveAttribute(pNewAttr->GetName());
  if (!m_pFirstAttr)
    m_pFirstAttr = m_pLastAttr = pNewAttr;
  else {
    m_pLastAttr->SetNextNodeSibling(pNewAttr);
    pNewAttr->SetPreviousNodeSibling(m_pLastAttr);
    m_pLastAttr = pNewAttr;
  }  // else if
  return pNewAttr;
}  // SetAttributeNode

CDOMAttr* CDOMElement::GetAttributeNode(CString* pStrName) {
  if (m_pFirstAttr) {
    CDOMAttr* pTmpAttr = m_pFirstAttr;
    do {
      if (*(pTmpAttr->GetName()) == *pStrName)
        return pTmpAttr;
      pTmpAttr = (CDOMAttr*)pTmpAttr->GetNextNodeSibling();
    } while (pTmpAttr);
  }  // if
  return NULL;
}  // GetAttributeNode

CDOMAttr* CDOMElement::RemoveAttributeNode(CDOMAttr* pOldAttr) {
  CDOMAttr* pTmpAttr = m_pFirstAttr;
  while (pTmpAttr && pTmpAttr != pOldAttr)
    pTmpAttr = (CDOMAttr*)pTmpAttr->GetNextNodeSibling();
  if (pTmpAttr == pOldAttr) {
    if (pTmpAttr->GetPreviousNodeSibling()) {
      pTmpAttr->GetPreviousNodeSibling()->SetNextNodeSibling(
          pTmpAttr->GetNextNodeSibling());
    } else
      m_pFirstAttr = (CDOMAttr*)pTmpAttr->GetNextNodeSibling();
    if (pTmpAttr->GetNextNodeSibling()) {
      pTmpAttr->GetNextNodeSibling()->SetPreviousNodeSibling(
          pTmpAttr->GetPreviousNodeSibling());
    } else
      m_pLastAttr = (CDOMAttr*)pTmpAttr->GetPreviousNodeSibling();
    return pTmpAttr;
  }  // if
  return NULL;
}  // RemoveAttribute

bool CDOMElement::HasAttribute(CString* pStrName) {
  if (m_pFirstAttr) {
    CDOMAttr* pTmpAttr = m_pFirstAttr;
    do {
      if (*(pTmpAttr->GetName()) == *pStrName)
        return true;
      pTmpAttr = (CDOMAttr*)pTmpAttr->GetNextNodeSibling();
    } while (pTmpAttr);
  }  // if
  return false;
}  // HasAttribute

CDOMNodeList* CDOMElement::GetElementsByTagName(CString* pStrName) {
  CDOMNodeList* pNodeList = new CDOMNodeList();
  CDOMElement* pTmpElement;
  CDOMNode* pTmpNode;
  CList queue;
  queue.AddItem(this);
  short nNodeType;
  while (!queue.IsEmpty()) {
    pTmpElement = (CDOMElement*)queue.RemoveFirst();
    pTmpNode = pTmpElement->GetFirstNode();
    while (pTmpNode) {
      nNodeType = pTmpNode->GetNodeType();
      if (nNodeType == DOM_ELEMENT_NODE) {
        queue.AddItem(pTmpNode);
        if (*(((CDOMElement*)pTmpNode)->GetTagName()) == *pStrName)
          pNodeList->AddItem(pTmpNode);
      }  // if
      pTmpNode = pTmpNode->GetNextNodeSibling();
    }  // while
  }    // while
  return pNodeList;
}  // GetElementsByTagName
