// DOMAttr.h: interface for the CDOMAttr class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(DOMATTR_H_)
#define DOMATTR_H_

#include "DOMNode.h"

class CDOMDocument;
class CDOMElement;

class CDOMAttr : public CDOMNode {
 public:
  CDOMAttr(CDOMDocument* pOwnerDocument);
  virtual ~CDOMAttr();

  // CDOMNode override:
  void SetNodeName(CString* pStrName) override { SetName(pStrName); }
  CString* GetNodeName() override { return GetName(); }
  void SetNodeValue(CString* pStrValue) override { SetValue(pStrValue); }
  CString* GetNodeValue() override { return GetValue(); }
  CString ToString() override;

  void SetName(CString* pStrName);
  CString* GetName() { return m_pStrName; }
  void SetValue(CString* pStrValue);
  CString* GetValue() { return m_pStrValue; }
  void SetOwnerElement(CDOMElement* pOwnerElement)
    { m_pOwnerElement = pOwnerElement; }
  CDOMElement* GetOwnerElement() { return m_pOwnerElement; }

 protected:
  // CDOMNode override:
  CDOMNode* CopyNode() override;

 private:
  CString* m_pStrName;
  CString* m_pStrValue;
  CDOMElement* m_pOwnerElement;
};

#endif  // !defined(DOMATTR_H_)
