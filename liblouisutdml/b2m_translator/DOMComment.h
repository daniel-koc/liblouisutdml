// DOMComment.h: interface for the CDOMComment class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(DOMCOMMENT_H_)
#define DOMCOMMENT_H_

#include "DOMText.h"

class CDOMDocument;

class CDOMComment : public CDOMText {
 public:
  CDOMComment(CDOMDocument* pOwnerDocument);
  virtual ~CDOMComment();

  // CDOMNode override:
  CString ToString() override;

 protected:
  // CDOMNode override:
  CDOMNode* CopyNode() override;
};

#endif  // !defined(DOMCOMMENT_H_)
