// DOMElement.h: interface for the CDOMElement class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(DOMELEMENT_H_)
#define DOMELEMENT_H_

#include "DOMNode.h"
#include "DOMNamedNodeList.h"

class CDOMDocument;
class CDOMAttr;

typedef void* LISTITEM;

class CListItem {
 public:
  CListItem(LISTITEM pItem);
  virtual ~CListItem();

 public:
  LISTITEM m_pItem;
  void* m_pNext;
};

class CList {
 public:
  CList();
  virtual ~CList();
  void SetDeletingAllItemsFlag();
  void AddItem(const LISTITEM pItem);
  LISTITEM* GetItems();
  int GetCount();
  bool IsEmpty();
  LISTITEM Next();
  bool HasNext();
  void Reset();
  LISTITEM GetFirst();
  LISTITEM RemoveFirst();
  LISTITEM GetLast();
  LISTITEM RemoveLast();
  LISTITEM GetAt(int nIndex);
  LISTITEM operator[](int nIndex);
  void RemoveAll();
  void DeleteAll();

 public:
  CListItem* m_pFirst;
  CListItem* m_pLast;
  CListItem* m_pCur;
  int m_nItemsCount;

 private:
  bool m_bDeletingAllItemsFlag;
};

class CDOMElement : public CDOMNode {
 public:
  CDOMElement(CDOMDocument* pOwnerDocument);
  virtual ~CDOMElement();
  virtual void SetNodeName(CString* pStrName);
  virtual CString* GetNodeName();
  void SetTagName(CString* pStrTagName);
  CString* GetTagName();
  void AddAttribute(CString* pStrName, CString* pStrValue);
  bool SetAttribute(wchar_t* pStrName, wchar_t* pStrValue);
  bool SetAttribute(const wchar_t* pStrName, const wchar_t* pStrValue);
  bool SetAttribute(const wchar_t* pStrName, wchar_t* pStrValue);
  bool SetAttribute(CString* pStrName, CString* pStrValue);
  CString* GetAttribute(CString* pStrName);
  void RemoveAttribute(CString* pStrName);
  CDOMAttr* SetAttributeNode(CDOMAttr* pNewAttr);
  CDOMAttr* GetAttributeNode(CString* pStrName);
  CDOMAttr* RemoveAttributeNode(CDOMAttr* pOldAttr);
  bool HasAttribute(CString* pStrName);
  CDOMNamedNodeList* GetAttributeNodes();
  virtual bool HasAttributes();
  CDOMNodeList* GetElementsByTagName(CString* pStrName);
  virtual CString ToString();

 protected:
  virtual CDOMNode* CopyNode();

 protected:
  CString* m_pStrTagName;
  CDOMAttr* m_pFirstAttr;
  CDOMAttr* m_pLastAttr;
};

inline CString* CDOMElement::GetNodeName() {
  return m_pStrTagName;
}

inline CString* CDOMElement::GetTagName() {
  return m_pStrTagName;
}

inline bool CDOMElement::HasAttributes() {
  return ((m_pFirstAttr != NULL) ? true : false);
}

#endif  // !defined(DOMELEMENT_H_)
