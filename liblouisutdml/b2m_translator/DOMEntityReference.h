// DOMEntityReference.h: interface for the CDOMEntityReference class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(DOMENTITYREFERENCE_H_)
#define DOMENTITYREFERENCE_H_

#include "DOMNode.h"

class CDOMDocument;

class CDOMEntityReference : public CDOMNode {
 public:
  CDOMEntityReference(CDOMDocument* pOwnerDocument);
  virtual ~CDOMEntityReference();
  virtual void SetNodeName(CString* pStrName);
  virtual CString* GetNodeName();
  void SetNotationName(CString* pStrName);
  CString* GetNotationName();
  //@LPCTSTR GetNotationValue();
  virtual CString ToString();

 protected:
  virtual CDOMNode* CopyNode();

 protected:
  CString* m_pStrNotationName;
  /*@
  static LPCTSTR s_lpszLessNotationName;
  static LPCTSTR s_lpszLessNotationValue;
  static LPCTSTR s_lpszGreaterNotationName;
  static LPCTSTR s_lpszGreaterNotationValue;
  static LPCTSTR s_lpszAmpersantNotationName;
  static LPCTSTR s_lpszAmpersantNotationValue;
  static LPCTSTR s_lpszQuoteNotationName;
  static LPCTSTR s_lpszQuoteNotationValue;
  @*/
};

inline CString* CDOMEntityReference::GetNodeName() {
  return m_pStrNotationName;
}

inline CString* CDOMEntityReference::GetNotationName() {
  return m_pStrNotationName;
}

#endif  // !defined(DOMENTITYREFERENCE_H_)
