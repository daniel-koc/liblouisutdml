// DOMNode.h: interface for the CDOMDocumentFragment class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(DOM_DOCUMENT_FRAGMENT_H_)
#define DOM_DOCUMENT_FRAGMENT_H_

#include "DOMNode.h"

class CDOMDocumentFragment : public CDOMNode {
 public:
  CDOMDocumentFragment(CDOMDocument* pOwnerDocument);
  virtual ~CDOMDocumentFragment();

 protected:
  // CDOMNode override:
  CDOMNode* CopyNode() override;
};

#endif  // !defined(DOM_DOCUMENT_FRAGMENT_H_)
