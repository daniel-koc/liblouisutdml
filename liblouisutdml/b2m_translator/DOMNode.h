// DOMNode.h: interface for the CDOMNode class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(DOMNODE_H_)
#define DOMNODE_H_

#include "string_utils.h"

class CDOMDocument;
class CDOMNodeList;
class CDOMNamedNodeList;

enum EDOMNodeType {
  DOM_ELEMENT_NODE = 1,
  DOM_ATTRIBUTE_NODE = 2,
  DOM_TEXT_NODE = 3,
  DOM_CDATA_SECTION_NODE = 4,
  DOM_ENTITY_REFERENCE_NODE = 5,
  DOM_ENTITY_NODE = 6,
  DOM_PROCESSING_INSTRUCTION_NODE = 7,
  DOM_COMMENT_NODE = 8,
  DOM_DOCUMENT_NODE = 9,
  DOM_DOCUMENT_FRAGMENT = 10
};

class CDOMNode {
 public:
  CDOMNode(CDOMDocument* pOwnerDocument, EDOMNodeType nNodeType);
  virtual ~CDOMNode();

  virtual void SetNodeName(CString* pStrName);
  virtual CString* GetNodeName();
  virtual void SetNodeValue(CString* pStrValue);
  virtual CString* GetNodeValue();
  virtual CDOMNamedNodeList* GetAttributeNodes();
  virtual bool HasAttributes();
  virtual CString ToString();

  CDOMDocument* GetOwnerDocument() { return m_pOwnerDocument; }
  EDOMNodeType GetNodeType() { return m_nNodeType; }
  void SetParentNode(CDOMNode* pParentNode) { m_pParentNode = pParentNode; }
  CDOMNode* GetParentNode() { return m_pParentNode; }
  CDOMNode* GetFirstNode() { return m_pFirstNode; }
  CDOMNode* GetLastNode() { return m_pLastNode; }
  CDOMNodeList* GetChildNodes();
  void SetPreviousNodeSibling(CDOMNode* pPreviousNodeSibling)
    { m_pPreviousNodeSibling = pPreviousNodeSibling; }
  CDOMNode* GetPreviousNodeSibling() { return m_pPreviousNodeSibling; }
  void SetNextNodeSibling(CDOMNode* pNextNodeSibling)
    { m_pNextNodeSibling = pNextNodeSibling; }
  CDOMNode* GetNextNodeSibling() { return m_pNextNodeSibling; }
  CDOMNode* InsertBefore(CDOMNode* pNewChild, CDOMNode* pRefChild);
  CDOMNode* ReplaceChild(CDOMNode* pNewChild, CDOMNode* pOldChild);
  CDOMNode* RemoveChild(CDOMNode* pOldChild);
  CDOMNode* AppendChild(CDOMNode* pNewChild);
  CDOMNode* CloneNode(bool bDeep);
  void Normalize() {}
  bool HasChildNodes() { return (m_pFirstNode != NULL); }
  void SetDocLineColumnNumbers(int nDocLineNumber, int nDocColumnNumber);
  int GetDocLineNumber() { return m_nDocLineNumber; }
  int GetDocColumnNumber() { return m_nDocColumnNumber; }

  bool NodeNameEq(const wchar_t* name)
    { return (GetNodeName() ? (*GetNodeName() == name) : false); }
  bool NodeValueEq(const wchar_t* value)
    { return (GetNodeValue() ? (*GetNodeValue() == value) : false); }

  void SetUnderOverType() { m_bUnderOverType = true; }
  bool IsUnderOverType() { return m_bUnderOverType; }

 protected:
  virtual CDOMNode* CopyNode() = 0;

 private:
  CDOMDocument* m_pOwnerDocument;
  EDOMNodeType m_nNodeType;
  CDOMNode* m_pParentNode;
  CDOMNode* m_pPreviousNodeSibling;
  CDOMNode* m_pNextNodeSibling;
  CDOMNode* m_pFirstNode;
  CDOMNode* m_pLastNode;
  int m_nDocLineNumber;
  int m_nDocColumnNumber;
  bool m_bUnderOverType;
};

#endif  // !defined(DOMNODE_H_)
