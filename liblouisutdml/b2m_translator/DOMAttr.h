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
  virtual void SetNodeName(CString* pStrName);
  virtual CString* GetNodeName();
  virtual void SetNodeValue(CString* pStrValue);
  virtual CString* GetNodeValue();
  void SetName(CString* pStrName);
  CString* GetName();
  void SetValue(CString* pStrValue);
  CString* GetValue();
  void SetOwnerElement(CDOMElement* pOwnerElement);
  CDOMElement* GetOwnerElement();
  virtual CString ToString();

 protected:
  virtual CDOMNode* CopyNode();

 protected:
  CString* m_pStrName;
  CString* m_pStrValue;
  CDOMElement* m_pOwnerElement;
};

inline CString* CDOMAttr::GetNodeName() {
  return m_pStrName;
}

inline CString* CDOMAttr::GetNodeValue() {
  return m_pStrValue;
}

inline CString* CDOMAttr::GetName() {
  return m_pStrName;
}

inline CString* CDOMAttr::GetValue() {
  return m_pStrValue;
}

inline void CDOMAttr::SetOwnerElement(CDOMElement* pOwnerElement) {
  m_pOwnerElement = pOwnerElement;
}

inline CDOMElement* CDOMAttr::GetOwnerElement() {
  return m_pOwnerElement;
}

#endif  // !defined(DOMATTR_H_)
