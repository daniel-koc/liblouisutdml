// DOMAttr.cpp: implementation of the CDOMAttr class.
//
//////////////////////////////////////////////////////////////////////

#include "DOMAttr.h"
#include "DOMDocument.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDOMAttr::CDOMAttr(CDOMDocument* pOwnerDocument)
: CDOMNode(pOwnerDocument, DOM_ATTRIBUTE_NODE)
{
m_pStrName = NULL;
m_pStrValue = NULL;
}  // CDOMAttr

CDOMAttr::~CDOMAttr()
{
if (m_pStrName != NULL)
delete m_pStrName;
if (m_pStrValue != NULL)
delete m_pStrValue;
}  // ~CDOMAttr

CDOMNode* CDOMAttr::CopyNode()
{
CDOMAttr* pNewAttr = m_pOwnerDocument->CreateAttribute(new CString(*m_pStrName));
pNewAttr->SetValue(new CString(*m_pStrValue));
pNewAttr->SetOwnerElement(m_pOwnerElement);
return pNewAttr;
}  // CopyNode

void CDOMAttr::SetNodeName(CString* pStrName)
{
if (m_pStrName != NULL)
delete m_pStrName;
m_pStrName = pStrName;
}  // SetNodeName

void CDOMAttr::SetNodeValue(CString* pStrValue)
{
if (m_pStrValue != NULL)
delete m_pStrValue;
m_pStrValue = pStrValue;
}  // SetNodeValue

void CDOMAttr::SetName(CString* pStrName)
{
if (m_pStrName != NULL)
delete m_pStrName;
m_pStrName = pStrName;
}  // SetName

void CDOMAttr::SetValue(CString* pStrValue)
{
if (m_pStrValue != NULL)
delete m_pStrValue;
m_pStrValue = pStrValue;
}  // SetValue

CString CDOMAttr::ToString()
{
CString str;
if (m_pStrName != NULL) {
str += *m_pStrName;
str += L"=\"";
if (m_pStrValue != NULL)
str += *m_pStrValue;
str += L"\"";
}
return str;
}
