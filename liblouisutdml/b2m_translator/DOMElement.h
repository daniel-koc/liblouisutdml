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
  void SetDeletingAllItemsFlag() { m_bDeletingAllItemsFlag = true; }
  void AddItem(const LISTITEM pItem);
  LISTITEM* GetItems();
  int GetCount() { return m_nItemsCount; }
  bool IsEmpty() { return (m_nItemsCount == 0); }
  LISTITEM Next();
  bool HasNext() { return (m_pCur != NULL); }
  void Reset() { m_pCur = m_pFirst; }
  LISTITEM GetFirst() { return m_pFirst ? m_pFirst->m_pItem : NULL; }
  LISTITEM RemoveFirst();
  LISTITEM GetLast() { return (m_pLast ? m_pLast->m_pItem : NULL); }
  LISTITEM RemoveLast();
  LISTITEM GetAt(int nIndex);
  LISTITEM operator[](int nIndex) { return GetAt(nIndex); }
  void RemoveAll();
  void DeleteAll();

 private:
  CListItem* m_pFirst;
  CListItem* m_pLast;
  CListItem* m_pCur;
  int m_nItemsCount;
  bool m_bDeletingAllItemsFlag;
};

class CDOMElement : public CDOMNode {
 public:
  CDOMElement(CDOMDocument* pOwnerDocument);
  virtual ~CDOMElement();

  // CDOMNode override:
  void SetNodeName(CString* pStrName) override { SetTagName(pStrName); }
  CString* GetNodeName() override { return GetTagName(); }
  CDOMNamedNodeList* GetAttributeNodes() override;
  bool HasAttributes() override { return (m_pFirstAttr != NULL); }
  CString ToString() override;

  void SetTagName(CString* pStrTagName);
  CString* GetTagName() { return m_pStrTagName; }
  bool AddAttribute(CString* pStrName, CString* pStrValue);
  bool SetAttribute(wchar_t* pStrName, wchar_t* pStrValue);
  bool SetAttribute(const wchar_t* pStrName, const wchar_t* pStrValue);
  bool SetAttribute(const wchar_t* pStrName, wchar_t* pStrValue);
  bool SetAttribute(CString* pStrName, CString* pStrValue);
  CString* GetAttribute(CString* pStrName);
  bool RemoveAttribute(CString* pStrName);
  CDOMAttr* SetAttributeNode(CDOMAttr* pNewAttr);
  CDOMAttr* GetAttributeNode(CString* pStrName);
  CDOMAttr* RemoveAttributeNode(CDOMAttr* pOldAttr);
  bool HasAttribute(CString* pStrName);
  CDOMNodeList* GetElementsByTagName(CString* pStrName);

 protected:
  // CDOMNode override:
  CDOMNode* CopyNode() override;

 private:
  CString* m_pStrTagName;
  CDOMAttr* m_pFirstAttr;
  CDOMAttr* m_pLastAttr;
};

#endif  // !defined(DOMELEMENT_H_)
