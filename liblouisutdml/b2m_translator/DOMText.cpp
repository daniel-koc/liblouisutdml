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
  CDOMText* pNewText =
      m_pOwnerDocument->CreateTextNode(new CString(*m_pStrData));
  return pNewText;
}  // CopyNode

CDOMText* CDOMText::SplitText(int nOffset) {
  CString* pStrPart1 = new CString();
  CString* pStrPart2 = new CString();
  *pStrPart1 = m_pStrData->substring(0, nOffset);
  *pStrPart2 = m_pStrData->substring(nOffset, m_pStrData->length());
  delete m_pStrData;
  m_pStrData = pStrPart1;
  CDOMText* pNewText = m_pOwnerDocument->CreateTextNode(pStrPart2);
  return pNewText;
}  // SplitText
