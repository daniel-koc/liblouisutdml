// DOMComment.h: interface for the CDOMComment class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(DOMCOMMENT_H_)
#define DOMCOMMENT_H_

#include "DOMText.h"

class CDOMDocument;

class CDOMComment: public CDOMText
{
public:
CDOMComment(CDOMDocument* pOwnerDocument);
virtual ~CDOMComment();
virtual CString ToString();

protected:
virtual CDOMNode* CopyNode();
};

#endif // !defined(DOMCOMMENT_H_)
