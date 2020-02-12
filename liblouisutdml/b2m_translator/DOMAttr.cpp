// DOMAttr.cpp: implementation of the CDOMAttr class.
//
//////////////////////////////////////////////////////////////////////

#include "DOMAttr.h"
#include "DOMDocument.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDOMAttr::CDOMAttr(CDOMDocument* pOwnerDocument)
    : CDOMNode(pOwnerDocument, DOM_ATTRIBUTE_NODE) {
  m_pStrName = NULL;
  m_pStrValue = NULL;
}  // CDOMAttr

CDOMAttr::~CDOMAttr() {
  if (m_pStrName)
    delete m_pStrName;
  if (m_pStrValue)
    delete m_pStrValue;
}  // ~CDOMAttr

CDOMNode* CDOMAttr::CopyNode() {
  CDOMAttr* pNewAttr = GetOwnerDocument()->CreateAttribute(
      m_pStrName ? new CString(*m_pStrName) : NULL);
  if (m_pStrValue)
    pNewAttr->SetValue(new CString(*m_pStrValue));
  pNewAttr->SetOwnerElement(m_pOwnerElement);
  return pNewAttr;
}  // CopyNode

CString CDOMAttr::ToString() {
  CString str;
  if (m_pStrName) {
    str += *m_pStrName;
    str += L"=\"";
    if (m_pStrValue)
      str += *m_pStrValue;
    str += L"\"";
  }
  return str;
}  // ToString

void CDOMAttr::SetName(CString* pStrName) {
  if (m_pStrName)
    delete m_pStrName;
  m_pStrName = pStrName;
}  // SetName

void CDOMAttr::SetValue(CString* pStrValue) {
  if (m_pStrValue)
    delete m_pStrValue;
  m_pStrValue = pStrValue;
}  // SetValue
