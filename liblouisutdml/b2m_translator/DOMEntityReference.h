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

  // CDOMNode override:
  void SetNodeName(CString* pStrName) override { SetNotationName(pStrName); }
  CString* GetNodeName() override { return GetNotationName(); }
  CString ToString() override;

  void SetNotationName(CString* pStrNotationName);
  CString* GetNotationName() { return m_pStrNotationName; }
  //@LPCTSTR GetNotationValue();

 protected:
  // CDOMNode override:
  CDOMNode* CopyNode() override;

 private:
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

#endif  // !defined(DOMENTITYREFERENCE_H_)
