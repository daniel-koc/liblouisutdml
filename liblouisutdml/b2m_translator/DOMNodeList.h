// DOMNodeList.h: interface for the CDOMNodeList class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(DOMNODELIST_H_)
#define DOMNODELIST_H_

#include "DOMNode.h"

typedef CDOMNode* CDOMNodePtr;

class CDOMNodeList {
 public:
  CDOMNodeList();
  CDOMNodeList(CDOMNode* pFirstNode);
  virtual ~CDOMNodeList();
  void AddItem(CDOMNode* pNode);
  CDOMNode* Item(int nIndex);
  int GetLength() { return m_nItemsCount; }

 protected:
  CDOMNodePtr* m_ppItems;
  int m_nMaxItemsCount;
  int m_nItemsCount;
};

#endif  // !defined(DOMNODELIST_H_)
