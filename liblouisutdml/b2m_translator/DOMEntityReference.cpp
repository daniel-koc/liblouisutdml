// DOMEntityReference.cpp: implementation of the CDOMEntityReference class.
//
//////////////////////////////////////////////////////////////////////

#include "DOMEntityReference.h"
#include "DOMDocument.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
/*@
LPCTSTR CDOMEntityReference::s_lpszLessNotationName = "lt";
LPCTSTR CDOMEntityReference::s_lpszLessNotationValue = "<";
LPCTSTR CDOMEntityReference::s_lpszGreaterNotationName = "gt";
LPCTSTR CDOMEntityReference::s_lpszGreaterNotationValue = ">";
LPCTSTR CDOMEntityReference::s_lpszAmpersantNotationName = "amp";
LPCTSTR CDOMEntityReference::s_lpszAmpersantNotationValue = "&";
LPCTSTR CDOMEntityReference::s_lpszQuoteNotationName = "quot";
LPCTSTR CDOMEntityReference::s_lpszQuoteNotationValue = "\"";
@*/
CDOMEntityReference::CDOMEntityReference(CDOMDocument* pOwnerDocument)
: CDOMNode(pOwnerDocument, DOM_ENTITY_REFERENCE_NODE)
{
m_pStrNotationName = NULL;
}

CDOMEntityReference::~CDOMEntityReference()
{
if (m_pStrNotationName != NULL)
delete m_pStrNotationName;
}

CDOMNode* CDOMEntityReference::CopyNode()
{
CDOMEntityReference* pNewEntityReference = m_pOwnerDocument->CreateEntityReference(new CString(*m_pStrNotationName));
return pNewEntityReference;
}

void CDOMEntityReference::SetNodeName(CString* pStrName)
{
if (m_pStrNotationName != NULL)
delete m_pStrNotationName;
m_pStrNotationName = pStrName;
}  // SetNodeName

void CDOMEntityReference::SetNotationName(CString* pStrName)
{
if (m_pStrNotationName != NULL)
delete m_pStrNotationName;
m_pStrNotationName = pStrName;
}  // SetNotationName
/*@
LPCTSTR CDOMEntityReference::GetNotationValue()
{
if (m_pStrNotationName->CompareNoCase(CDOMEntityReference::s_lpszLessNotationName) == 0)
return CDOMEntityReference::s_lpszLessNotationValue;
else
if (m_pStrNotationName->CompareNoCase(CDOMEntityReference::s_lpszGreaterNotationName) == 0)
return CDOMEntityReference::s_lpszGreaterNotationValue;
else
if (m_pStrNotationName->CompareNoCase(CDOMEntityReference::s_lpszAmpersantNotationName) == 0)
return CDOMEntityReference::s_lpszAmpersantNotationValue;
else
if (m_pStrNotationName->CompareNoCase(CDOMEntityReference::s_lpszQuoteNotationName) == 0)
return CDOMEntityReference::s_lpszQuoteNotationValue;
else
return "";
}  // GetNotationValue
@*/
CString CDOMEntityReference::ToString()
{
CString str;
str = L"&";
if (m_pStrNotationName != NULL)
str += *m_pStrNotationName;
str += L";";
return str;
}
