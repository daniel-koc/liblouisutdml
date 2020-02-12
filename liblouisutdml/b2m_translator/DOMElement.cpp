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

void CList::SetDeletingAllItemsFlag() {
  m_bDeletingAllItemsFlag = true;
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
  if (m_pFirst == NULL)
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
  } while (pTmp != NULL);
  return pItems;
}  // GetItems

int CList::GetCount() {
  return m_nItemsCount;
}

bool CList::IsEmpty() {
  return ((m_nItemsCount == 0) ? true : false);
}

LISTITEM CList::Next() {
  if (m_pCur == NULL)
    return NULL;
  LISTITEM pItem = m_pCur->m_pItem;
  m_pCur = (CListItem*)m_pCur->m_pNext;
  return pItem;
}  // Next

bool CList::HasNext() {
  return ((m_pCur != NULL) ? true : false);
}

void CList::Reset() {
  m_pCur = m_pFirst;
}

LISTITEM CList::GetFirst() {
  if (m_pFirst == NULL)
    return NULL;
  return m_pFirst->m_pItem;
}  // GetFirst

LISTITEM CList::RemoveFirst() {
  if (m_pFirst == NULL)
    return NULL;
  LISTITEM pItem = m_pFirst->m_pItem;
  CListItem* pTmp = m_pFirst;
  m_pFirst = (CListItem*)m_pFirst->m_pNext;
  delete pTmp;
  m_nItemsCount--;
  return pItem;
}  // RemoveGetFirst

LISTITEM CList::GetLast() {
  if (m_pLast == NULL)
    return NULL;
  return m_pLast->m_pItem;
}  // GetLast

LISTITEM CList::RemoveLast() {
  if (m_pLast == NULL)
    return NULL;
  m_nItemsCount--;
  LISTITEM pItem;
  if (m_pFirst->m_pNext == NULL) {
    pItem = m_pFirst->m_pItem;
    delete m_pFirst;
    m_pFirst = m_pLast = NULL;
    return pItem;
  } else {
    CListItem* pTmp = m_pFirst;
    while (((CListItem*)pTmp->m_pNext)->m_pNext != NULL)
      pTmp = (CListItem*)pTmp->m_pNext;
    pTmp->m_pNext = NULL;
    pItem = ((CListItem*)m_pLast)->m_pItem;
    delete m_pLast;
    m_pLast = pTmp;
    return pItem;
  }  // else if
}  // RemoveLast

LISTITEM CList::GetAt(int nIndex) {
  if (m_nItemsCount == 0 || nIndex < 0 || nIndex >= m_nItemsCount)
    return NULL;
  CListItem* pTmp = m_pFirst;
  int i = 0;
  while (i < nIndex) {
    pTmp = (CListItem*)pTmp->m_pNext;
    i++;
  }  // while
  return pTmp->m_pItem;
}  // GetAt

LISTITEM CList::operator[](int nIndex) {
  if (m_nItemsCount == 0 || nIndex < 0 || nIndex >= m_nItemsCount)
    return NULL;
  CListItem* pTmp = m_pFirst;
  int i = 0;
  while (i < nIndex) {
    pTmp = (CListItem*)pTmp->m_pNext;
    i++;
  }  // while
  return pTmp->m_pItem;
}  // operator[]

void CList::RemoveAll() {
  CListItem* pTmpItem;
  while (m_pFirst != NULL) {
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
  while (m_pFirst != NULL) {
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
  if (m_pStrTagName != NULL)
    delete m_pStrTagName;
  while (m_pFirstAttr != NULL) {
    m_pLastAttr = m_pFirstAttr;
    m_pFirstAttr = (CDOMAttr*)m_pFirstAttr->GetNextNodeSibling();
    delete m_pLastAttr;
  }  // while
}  // ~CDOMElement

CDOMNode* CDOMElement::CopyNode() {
  CDOMElement* pNewElement =
      m_pOwnerDocument->CreateElement(new CString(*m_pStrTagName));
  CDOMAttr* pTmpAttr = m_pFirstAttr;
  while (pTmpAttr != NULL) {
    CString* pStrName = pTmpAttr->GetName();
    CString* pStrValue = pTmpAttr->GetValue();
    pNewElement->AddAttribute(new CString(*pStrName), new CString(*pStrValue));
    pTmpAttr = (CDOMAttr*)pTmpAttr->GetNextNodeSibling();
  }  // while
  return pNewElement;
}  // CopyNode

void CDOMElement::SetNodeName(CString* pStrName) {
  if (m_pStrTagName != NULL)
    delete m_pStrTagName;
  m_pStrTagName = pStrName;
}  // SetNodeName

void CDOMElement::SetTagName(CString* pStrTagName) {
  if (m_pStrTagName != NULL)
    delete m_pStrTagName;
  m_pStrTagName = pStrTagName;
}  // SetTagName

CDOMNamedNodeList* CDOMElement::GetAttributeNodes() {
  return new CDOMNamedNodeList(m_pFirstAttr);
}

void CDOMElement::AddAttribute(CString* pStrName, CString* pStrValue) {
  CDOMAttr* pNewAttr = m_pOwnerDocument->CreateAttribute(pStrName);
  pNewAttr->SetOwnerElement(this);
  pNewAttr->SetValue(pStrValue);
  if (m_pFirstAttr == NULL)
    m_pFirstAttr = m_pLastAttr = pNewAttr;
  else {
    m_pLastAttr->SetNextNodeSibling(pNewAttr);
    pNewAttr->SetPreviousNodeSibling(m_pLastAttr);
    m_pLastAttr = pNewAttr;
  }  // else if
}  // AddAttribute

bool CDOMElement::SetAttribute(wchar_t* pStrName, wchar_t* pStrValue) {
  return SetAttribute(new CString(pStrName), new CString(pStrValue));
}

bool CDOMElement::SetAttribute(const wchar_t* pStrName,
                               const wchar_t* pStrValue) {
  return SetAttribute(new CString(pStrName), new CString(pStrValue));
}

bool CDOMElement::SetAttribute(const wchar_t* pStrName, wchar_t* pStrValue) {
  return SetAttribute(new CString(pStrName), new CString(pStrValue));
}

bool CDOMElement::SetAttribute(CString* pStrName, CString* pStrValue) {
  if (m_pFirstAttr != NULL) {
    CDOMAttr* pTmpAttr = m_pFirstAttr;
    do {
      if (*(pTmpAttr->GetName()) == *pStrName) {
        pTmpAttr->SetValue(pStrValue);
        return true;
      }  // if
      pTmpAttr = (CDOMAttr*)pTmpAttr->GetNextNodeSibling();
    } while (pTmpAttr != NULL);
  }  // if
  CDOMAttr* pNewAttr = m_pOwnerDocument->CreateAttribute(pStrName);
  pNewAttr->SetOwnerElement(this);
  pNewAttr->SetValue(pStrValue);
  if (m_pFirstAttr == NULL)
    m_pFirstAttr = m_pLastAttr = pNewAttr;
  else {
    m_pLastAttr->SetNextNodeSibling(pNewAttr);
    pNewAttr->SetPreviousNodeSibling(m_pLastAttr);
    m_pLastAttr = pNewAttr;
  }  // else if
  return false;
}  // SetAttribute

CString* CDOMElement::GetAttribute(CString* pStrName) {
  if (m_pFirstAttr != NULL) {
    CDOMAttr* pTmpAttr = m_pFirstAttr;
    do {
      if (*(pTmpAttr->GetName()) == *pStrName)
        return pTmpAttr->GetValue();
      pTmpAttr = (CDOMAttr*)pTmpAttr->GetNextNodeSibling();
    } while (pTmpAttr != NULL);
  }  // if
  return NULL;
}  // GetAttribute

void CDOMElement::RemoveAttribute(CString* pStrName) {
  if (m_pFirstAttr != NULL) {
    CDOMAttr* pTmpAttr = m_pFirstAttr;
    do {
      if (*(pTmpAttr->GetName()) == *pStrName) {
        if (pTmpAttr->GetPreviousNodeSibling() != NULL)
          pTmpAttr->GetPreviousNodeSibling()->SetNextNodeSibling(
              pTmpAttr->GetNextNodeSibling());
        else
          m_pFirstAttr = (CDOMAttr*)pTmpAttr->GetNextNodeSibling();
        if (pTmpAttr->GetNextNodeSibling() != NULL)
          pTmpAttr->GetNextNodeSibling()->SetPreviousNodeSibling(
              pTmpAttr->GetPreviousNodeSibling());
        else
          m_pLastAttr = (CDOMAttr*)pTmpAttr->GetPreviousNodeSibling();
        return;
      }  // if
      pTmpAttr = (CDOMAttr*)pTmpAttr->GetNextNodeSibling();
    } while (pTmpAttr != NULL);
  }  // if
}  // RemoveAttribute

CDOMAttr* CDOMElement::SetAttributeNode(CDOMAttr* pNewAttr) {
  if (m_pFirstAttr == NULL)
    m_pFirstAttr = m_pLastAttr = pNewAttr;
  else {
    m_pLastAttr->SetNextNodeSibling(pNewAttr);
    pNewAttr->SetPreviousNodeSibling(m_pLastAttr);
    m_pLastAttr = pNewAttr;
  }  // else if
  return pNewAttr;
}  // SetAttributeNode

CDOMAttr* CDOMElement::GetAttributeNode(CString* pStrName) {
  if (m_pFirstAttr != NULL) {
    CDOMAttr* pTmpAttr = m_pFirstAttr;
    do {
      if (*(pTmpAttr->GetName()) == *pStrName)
        return pTmpAttr;
      pTmpAttr = (CDOMAttr*)pTmpAttr->GetNextNodeSibling();
    } while (pTmpAttr != NULL);
  }  // if
  return NULL;
}  // GetAttributeNode

CDOMAttr* CDOMElement::RemoveAttributeNode(CDOMAttr* pOldAttr) {
  CDOMAttr* pTmpAttr = m_pFirstAttr;
  while (pTmpAttr != NULL && pTmpAttr != pOldAttr)
    pTmpAttr = (CDOMAttr*)pTmpAttr->GetNextNodeSibling();
  if (pTmpAttr == pOldAttr) {
    if (pTmpAttr->GetPreviousNodeSibling() != NULL)
      pTmpAttr->GetPreviousNodeSibling()->SetNextNodeSibling(
          pTmpAttr->GetNextNodeSibling());
    else
      m_pFirstAttr = (CDOMAttr*)pTmpAttr->GetNextNodeSibling();
    if (pTmpAttr->GetNextNodeSibling() != NULL)
      pTmpAttr->GetNextNodeSibling()->SetPreviousNodeSibling(
          pTmpAttr->GetPreviousNodeSibling());
    else
      m_pLastAttr = (CDOMAttr*)pTmpAttr->GetPreviousNodeSibling();
    return pTmpAttr;
  }  // if
  return NULL;
}  // RemoveAttribute

bool CDOMElement::HasAttribute(CString* pStrName) {
  if (m_pFirstAttr != NULL) {
    CDOMAttr* pTmpAttr = m_pFirstAttr;
    do {
      if (*(pTmpAttr->GetName()) == *pStrName)
        return true;
      pTmpAttr = (CDOMAttr*)pTmpAttr->GetNextNodeSibling();
    } while (pTmpAttr != NULL);
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
    while (pTmpNode != NULL) {
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

CString CDOMElement::ToString() {
  CString str;
  str += L"<";
  str += *m_pStrTagName;
  if (m_pFirstAttr != NULL) {
    CDOMAttr* pTmpAttr = m_pFirstAttr;
    while (pTmpAttr != NULL) {
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
