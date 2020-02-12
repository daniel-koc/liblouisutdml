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

  // CDOMNode override:
  void SetNodeName(CString* pStrName) override { SetTarget(pStrName); }
  CString* GetNodeName() override { return GetTarget(); }
  void SetNodeValue(CString* pStrValue) override { SetData(pStrValue); }
  CString* GetNodeValue() override { return GetData(); }
  CString ToString() override;

  void SetTarget(CString* pStrTarget);
  CString* GetTarget() { return m_pStrTarget; }
  void SetData(CString* pStrData);
  CString* GetData() { return m_pStrData; }

 protected:
  // CDOMNode override:
  CDOMNode* CopyNode() override;

 private:
  CString* m_pStrTarget;
  CString* m_pStrData;
};

#endif  // !defined(DOMPROCESSINGINSTRUCTION_H_)
