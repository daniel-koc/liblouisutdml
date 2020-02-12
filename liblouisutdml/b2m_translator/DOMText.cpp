// DOMText.cpp: implementation of the CDOMText class.
//
//////////////////////////////////////////////////////////////////////

#include "DOMText.h"
#include "DOMDocument.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDOMText::CDOMText(CDOMDocument* pOwnerDocument)
    : CDOMCharacterData(pOwnerDocument, DOM_TEXT_NODE) {
}

CDOMText::CDOMText(CDOMDocument* pOwnerDocument, EDOMNodeType nNodeType)
    : CDOMCharacterData(pOwnerDocument, nNodeType) {
}

CDOMText::~CDOMText() {
}

CDOMNode* CDOMText::CopyNode() {
  CDOMText* pNewText = GetOwnerDocument()->CreateTextNode(
      m_pStrData ? new CString(*m_pStrData) : NULL);
  return pNewText;
}  // CopyNode

CDOMText* CDOMText::SplitText(int nOffset) {
  if (!m_pStrData)
    return NULL;
  CString* pStrPart1 = new CString();
  CString* pStrPart2 = new CString();
  *pStrPart1 = m_pStrData->substring(0, nOffset);
  *pStrPart2 = m_pStrData->substring(nOffset, m_pStrData->length());
  delete m_pStrData;
  m_pStrData = pStrPart1;
  return GetOwnerDocument()->CreateTextNode(pStrPart2);
}  // SplitText
