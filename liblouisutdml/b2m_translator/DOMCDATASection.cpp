// DOMCDATASection.cpp: implementation of the CDOMCDATASection class.
//
//////////////////////////////////////////////////////////////////////

#include "DOMCDATASection.h"
#include "DOMDocument.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDOMCDATASection::CDOMCDATASection(CDOMDocument* pOwnerDocument)
    : CDOMText(pOwnerDocument, DOM_CDATA_SECTION_NODE) {
}

CDOMCDATASection::~CDOMCDATASection() {
}

CDOMNode* CDOMCDATASection::CopyNode() {
  CDOMCDATASection* pNewCDATASection = GetOwnerDocument()->CreateCDATASection(
      m_pStrData ? new CString(*m_pStrData) : NULL);
  return pNewCDATASection;
}  // CopyNode

CString CDOMCDATASection::ToString() {
  CString str;
  str = L"[CDATA[";
  if (m_pStrData)
    str += *m_pStrData;
  str += L"]]";
  return str;
}  // ToString
