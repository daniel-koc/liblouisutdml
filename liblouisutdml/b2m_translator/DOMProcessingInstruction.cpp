// DOMProcessingInstruction.cpp: implementation of the CDOMProcessingInstruction
// class.
//
//////////////////////////////////////////////////////////////////////

#include "DOMProcessingInstruction.h"
#include "DOMDocument.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDOMProcessingInstruction::CDOMProcessingInstruction(
    CDOMDocument* pOwnerDocument)
    : CDOMNode(pOwnerDocument, DOM_PROCESSING_INSTRUCTION_NODE) {
  m_pStrTarget = NULL;
  m_pStrData = NULL;
}  // CDOMProcessingInstruction

CDOMProcessingInstruction::~CDOMProcessingInstruction() {
  if (m_pStrTarget != NULL)
    delete m_pStrTarget;
  if (m_pStrData != NULL)
    delete m_pStrData;
}  // ~CDOMProcessingInstruction

CDOMNode* CDOMProcessingInstruction::CopyNode() {
  CDOMProcessingInstruction* pNewProcessingInstruction =
      m_pOwnerDocument->CreateProcessingInstruction(new CString(*m_pStrTarget),
                                                    new CString(*m_pStrData));
  return pNewProcessingInstruction;
}  // CopyNode

void CDOMProcessingInstruction::SetTarget(CString* pStrTarget) {
  if (m_pStrTarget != NULL)
    delete m_pStrTarget;
  m_pStrTarget = pStrTarget;
}  // SetTarget

void CDOMProcessingInstruction::SetData(CString* pStrData) {
  if (m_pStrData != NULL)
    delete m_pStrData;
  m_pStrData = pStrData;
}  // SetData

CString CDOMProcessingInstruction::ToString() {
  CString str;
  if (m_pStrTarget != NULL) {
    str = L"<!";
    str += *m_pStrTarget;
    str += L" ";
    if (m_pStrData != NULL)
      str += *m_pStrData;
    str += L">";
  }
  return str;
}
