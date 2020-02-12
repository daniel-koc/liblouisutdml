// DOMDocument.cpp: implementation of the CDOMDocument class.
//
//////////////////////////////////////////////////////////////////////

#include "DOMDocument.h"
#include "DOMElement.h"
#include "DOMDocumentFragment.h"
#include "DOMText.h"
#include "DOMComment.h"
#include "DOMCDATASection.h"
#include "DOMProcessingInstruction.h"
#include "DOMAttr.h"
#include "DOMEntityReference.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDOMDocument::CDOMDocument()
: CDOMNode(this, DOM_DOCUMENT_NODE)
{
m_pDocumentElement = NULL;
}

CDOMDocument::~CDOMDocument()
{
}

CDOMNode* CDOMDocument::CopyNode()
{
CDOMDocument* pNewDocument = new CDOMDocument();
pNewDocument->m_pDocumentElement = (CDOMElement*) m_pDocumentElement->CloneNode(true);
return pNewDocument;
}  // CopyNode

void CDOMDocument::SetDocumentElement(CDOMElement* pDocumentElement)
{
m_pDocumentElement = pDocumentElement;
AppendChild(m_pDocumentElement);
}

CDOMElement* CDOMDocument::GetDocumentElement()
{
return m_pDocumentElement;
}

CDOMElement* CDOMDocument::CreateElement(wchar_t* pStrTagName)
{
return CreateElement(new CString(pStrTagName));
}

CDOMElement* CDOMDocument::CreateElement(const wchar_t* pStrTagName)
{
return CreateElement(new CString(pStrTagName));
}

CDOMElement* CDOMDocument::CreateElement(CString* pStrTagName)
{
CDOMElement* pNewElement = new CDOMElement(this);
pNewElement->SetTagName(pStrTagName);
return pNewElement;
}  // CreateElement

CDOMDocumentFragment* CDOMDocument::CreateDocumentFragment() {
return new CDOMDocumentFragment(this);
}

CDOMText* CDOMDocument::CreateTextNode(wchar_t* pStrData)
{
return CreateTextNode(new CString(pStrData));
}

CDOMText* CDOMDocument::CreateTextNode(const wchar_t* pStrData)
{
return CreateTextNode(new CString(pStrData));
}

CDOMText* CDOMDocument::CreateTextNode(CString* pStrData)
{
CDOMText* pNewText = new CDOMText(this);
pNewText->SetData(pStrData);
return pNewText;
}  // CreateTextNode

CDOMComment* CDOMDocument::CreateComment(CString* pStrData)
{
CDOMComment* pNewComment = new CDOMComment(this);
pNewComment->SetData(pStrData);
return pNewComment;
}  // CreateComment

CDOMCDATASection* CDOMDocument::CreateCDATASection(CString* pStrData)
{
CDOMCDATASection* pNewCDATASection = new CDOMCDATASection(this);
pNewCDATASection->SetData(pStrData);
return pNewCDATASection;
}  // CreateCDATASection

CDOMProcessingInstruction* CDOMDocument::CreateProcessingInstruction(CString* pStrTarget, CString* pStrData)
{
CDOMProcessingInstruction* pNewProcessingInstruction = new CDOMProcessingInstruction(this);
pNewProcessingInstruction->SetTarget(pStrTarget);
pNewProcessingInstruction->SetData(pStrData);
return pNewProcessingInstruction;
}  // CreateProcessingInstruction

CDOMAttr* CDOMDocument::CreateAttribute(CString* pStrName)
{
CDOMAttr* pNewAttr = new CDOMAttr(this);
pNewAttr->SetName(pStrName);
return pNewAttr;
}  // CreateAttribute

CDOMEntityReference* CDOMDocument::CreateEntityReference(CString* pStrName)
{
CDOMEntityReference* pNewEntityReference = new CDOMEntityReference(this);
pNewEntityReference->SetNotationName(pStrName);
return pNewEntityReference;
}  // CreateEntityReference

CDOMNodeList* CDOMDocument::GetElementsByTagName(CString* pStrTagName)
{
return m_pDocumentElement->GetElementsByTagName(pStrTagName);
}  // GetElementsByTagName

CString CDOMDocument::ToString()
{
if (m_pDocumentElement != NULL)
return m_pDocumentElement->ToString();
else
return CDOMNode::ToString();
}
