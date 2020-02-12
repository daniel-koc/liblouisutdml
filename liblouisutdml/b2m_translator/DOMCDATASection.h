// DOMCDATASection.h: interface for the CDOMCDATASection class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(DOMCDATASECTION_H_)
#define DOMCDATASECTION_H_

#include "DOMText.h"

class CDOMDocument;

class CDOMCDATASection : public CDOMText {
 public:
  CDOMCDATASection(CDOMDocument* pOwnerDocument);
  virtual ~CDOMCDATASection();
  virtual CString ToString();

 protected:
  virtual CDOMNode* CopyNode();
};

#endif  // !defined(DOMCDATASECTION_H_)
