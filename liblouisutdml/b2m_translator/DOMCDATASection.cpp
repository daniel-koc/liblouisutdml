// DOMCDATASection.cpp: implementation of the CDOMCDATASection class.
//
//////////////////////////////////////////////////////////////////////

#include "DOMCDATASection.h"
#include "DOMDocument.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDOMCDATASection::CDOMCDATASection(CDOMDocument* pOwnerDocument)
: CDOMText(pOwnerDocument, DOM_CDATA_SECTION_NODE)
{
}

CDOMCDATASection::~CDOMCDATASection()
{
}

CDOMNode* CDOMCDATASection::CopyNode()
{
CDOMCDATASection* pNewCDATASection = m_pOwnerDocument->CreateCDATASection(m_pStrData);
return pNewCDATASection;
}

CString CDOMCDATASection::ToString()
{
CString str;
str = L"[CDATA[";
if (m_pStrData != NULL)
str += *m_pStrData;
str += L"]]";
return str;
}
