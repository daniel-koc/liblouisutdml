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

  // CDOMNode override:
  CString ToString() override;

 protected:
  // CDOMNode override:
  CDOMNode* CopyNode() override;
};

#endif  // !defined(DOMCDATASECTION_H_)
