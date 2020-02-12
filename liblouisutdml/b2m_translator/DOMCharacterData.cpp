// DOMCharacterData.cpp: implementation of the CDOMCharacterData class.
//
//////////////////////////////////////////////////////////////////////

#include "DOMCharacterData.h"
#include "DOMDocument.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDOMCharacterData::CDOMCharacterData(CDOMDocument* pOwnerDocument,
                                     EDOMNodeType nNodeType)
    : CDOMNode(pOwnerDocument, nNodeType) {
  m_pStrData = NULL;
}

CDOMCharacterData::~CDOMCharacterData() {
  if (m_pStrData != NULL)
    delete m_pStrData;
}

void CDOMCharacterData::SetData(CString* pStrData) {
  if (m_pStrData != NULL)
    delete m_pStrData;
  m_pStrData = pStrData;
}  // SetData

CString* CDOMCharacterData::SubstringData(int nOffset, int nCount) {
  CString* pStrSubstring = new CString();
  *pStrSubstring = m_pStrData->substring(nOffset, nOffset + nCount);
  return pStrSubstring;
}  // SubstringData

void CDOMCharacterData::AppendData(CString* pStrArg) {
  *m_pStrData += *pStrArg;
  delete pStrArg;
}  // AppendData

void CDOMCharacterData::InsertData(int nOffset, CString* pStrArg) {
  CString* pStrData = new CString();
  *pStrData = m_pStrData->substring(0, nOffset);
  *pStrData += *pStrArg;
  *pStrData += m_pStrData->substring(nOffset, m_pStrData->length());
  delete m_pStrData;
  delete pStrArg;
  m_pStrData = pStrData;
}  // InsertData

void CDOMCharacterData::DeleteData(int nOffset, int nCount) {
  CString* pStrData = new CString();
  *pStrData = m_pStrData->substring(0, nOffset);
  *pStrData += m_pStrData->substring(nOffset + nCount, m_pStrData->length());
  delete m_pStrData;
  m_pStrData = pStrData;
}  // DeleteData

void CDOMCharacterData::ReplaceData(int nOffset, int nCount, CString* pStrArg) {
  CString* pStrData = new CString();
  *pStrData = m_pStrData->substring(0, nOffset);
  *pStrData += pStrArg->substring(0, nCount);
  *pStrData += m_pStrData->substring(nOffset + nCount, m_pStrData->length());
  delete m_pStrData;
  m_pStrData = pStrData;
  delete pStrArg;
}  // ReplaceData

CString CDOMCharacterData::ToString() {
  CString str;
  if (m_pStrData != NULL)
    str = *m_pStrData;
  return str;
}
