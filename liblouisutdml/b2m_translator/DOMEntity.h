// DOMEntity.h: interface for the CDOMEntity class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(DOMENTITY_H_)
#define DOMENTITY_H_

#include "DOMNode.h"

class CDOMDocument;

class CDOMEntity : public CDOMNode {
 public:
  CDOMEntity(CDOMDocument* pOwnerDocument);
  virtual ~CDOMEntity();

 protected:
  virtual CDOMNode* CopyNode() = 0;
};

#endif  // !defined(DOMENTITY_H_)
