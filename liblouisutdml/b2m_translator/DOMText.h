// DOMText.h: interface for the CDOMText class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(DOMTEXT_H_)
#define DOMTEXT_H_

#include "DOMCharacterData.h"

class CDOMDocument;

class CDOMText: public CDOMCharacterData
{
public:
CDOMText(CDOMDocument* pOwnerDocument);
CDOMText(CDOMDocument* pOwnerDocument, EDOMNodeType nNodeType);
virtual ~CDOMText();
CDOMText* SplitText(int nOffset);

protected:
virtual CDOMNode* CopyNode();
};

#endif // !defined(DOMTEXT_H_)
