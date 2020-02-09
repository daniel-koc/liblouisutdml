// DOMNodeListItem.h: interface for the CDOMNodeListItem class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(DOMNODELISTITEM_H_)
#define DOMNODELISTITEM_H_

#include "DOMNode.h"

class CDOMNodeListItem {
 public:
  CDOMNodeListItem(CDOMNode* pNode);
  virtual ~CDOMNodeListItem();
  CDOMNode* GetNode();
  void SetNextNodeListItem(CDOMNodeListItem* pNextNodeListItem);
  CDOMNodeListItem* GetNextNodeListItem();

 protected:
  CDOMNode* m_pNode;
  CDOMNodeListItem* m_pNextNodeListItem;
};

inline CDOMNode* CDOMNodeListItem::GetNode() {
  return m_pNode;
}

inline void CDOMNodeListItem::SetNextNodeListItem(
    CDOMNodeListItem* pNextNodeListItem) {
  m_pNextNodeListItem = pNextNodeListItem;
}

inline CDOMNodeListItem* CDOMNodeListItem::GetNextNodeListItem() {
  return m_pNextNodeListItem;
}

#endif  // !defined(DOMNODELISTITEM_H_)
