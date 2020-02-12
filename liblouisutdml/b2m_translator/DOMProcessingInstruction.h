// DOMProcessingInstruction.h: interface for the CDOMProcessingInstruction
// class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(DOMPROCESSINGINSTRUCTION_H_)
#define DOMPROCESSINGINSTRUCTION_H_

#include "DOMNode.h"

class CDOMDocument;

class CDOMProcessingInstruction : public CDOMNode {
 public:
  CDOMProcessingInstruction(CDOMDocument* pOwnerDocument);
  virtual ~CDOMProcessingInstruction();
  void SetTarget(CString* pStrTarget);
  CString* GetTarget();
  void SetData(CString* pStrData);
  CString* GetData();
  virtual CString ToString();

 protected:
  virtual CDOMNode* CopyNode();

 protected:
  CString* m_pStrTarget;
  CString* m_pStrData;
};

inline CString* CDOMProcessingInstruction::GetTarget() {
  return m_pStrTarget;
}

inline CString* CDOMProcessingInstruction::GetData() {
  return m_pStrData;
}

#endif  // !defined(DOMPROCESSINGINSTRUCTION_H_)
