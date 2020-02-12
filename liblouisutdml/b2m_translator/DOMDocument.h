// DOMDocument.h: interface for the CDOMDocument class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(DOMDOCUMENT_H_)
#define DOMDOCUMENT_H_

#include "DOMNode.h"

class CDOMElement;
class CDOMDocumentFragment;
class CDOMText;
class CDOMComment;
class CDOMCDATASection;
class CDOMProcessingInstruction;
class CDOMAttr;
class CDOMEntityReference;

class CDOMDocument : public CDOMNode {
 public:
  CDOMDocument();
  virtual ~CDOMDocument();
  void SetDocumentElement(CDOMElement* pDocumentElement);
  CDOMElement* GetDocumentElement();
  CDOMElement* CreateElement(wchar_t* pStrTagName);
  CDOMElement* CreateElement(const wchar_t* pStrTagName);
  CDOMElement* CreateElement(CString* pStrTagName);
  CDOMDocumentFragment* CreateDocumentFragment();
  CDOMText* CreateTextNode(wchar_t* pStrData);
  CDOMText* CreateTextNode(const wchar_t* pStrData);
  CDOMText* CreateTextNode(CString* pStrData);
  CDOMComment* CreateComment(CString* pStrData);
  CDOMCDATASection* CreateCDATASection(CString* pStrData);
  CDOMProcessingInstruction* CreateProcessingInstruction(CString* pStrTarget,
                                                         CString* pStrData);
  CDOMAttr* CreateAttribute(CString* pStrName);
  CDOMEntityReference* CreateEntityReference(CString* pStrName);
  CDOMNodeList* GetElementsByTagName(CString* pStrTagName);
  virtual CString ToString();

 protected:
  virtual CDOMNode* CopyNode();

 protected:
  CDOMElement* m_pDocumentElement;
};

#endif  // !defined(DOMDOCUMENT_H_)
