// DOMNamedNodeList.h: interface for the CDOMNamedNodeList class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(DOMNAMEDNODELIST_H_)
#define DOMNAMEDNODELIST_H_

#include "DOMNodeList.h"

class CDOMNamedNodeList: public CDOMNodeList
{
public:
CDOMNamedNodeList();
CDOMNamedNodeList(CDOMNode* pFirstNode);
virtual ~CDOMNamedNodeList();
CDOMNode* GetNamedItem(CString* pStrName);
CDOMNode* RemoveNamedItem(CString* pStrName);

};

#endif // !defined(DOMNAMEDNODELIST_H_)
