// DOMNodeListItem.cpp: implementation of the CDOMNodeListItem class.
//
//////////////////////////////////////////////////////////////////////

#include "DOMNodeListItem.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDOMNodeListItem::CDOMNodeListItem(CDOMNode* pNode) {
  m_pNode = pNode;
  m_pNextNodeListItem = NULL;
}

CDOMNodeListItem::~CDOMNodeListItem() {
}
