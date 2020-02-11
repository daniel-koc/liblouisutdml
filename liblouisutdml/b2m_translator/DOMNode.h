// DOMNode.h: interface for the CDOMNode class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(DOMNODE_H_)
#define DOMNODE_H_

#include "string_utils.h"

class CDOMDocument;
class CDOMNodeList;
class CDOMNamedNodeList;

enum EDOMNodeType
{
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

class CDOMNode  
{
public:
CDOMNode(CDOMDocument* pOwnerDocument, EDOMNodeType nNodeType);
virtual ~CDOMNode();
CDOMDocument* GetOwnerDocument();
EDOMNodeType GetNodeType();
virtual void SetNodeName(CString* pStrName);
virtual CString* GetNodeName();
virtual void SetNodeValue(CString* pStrValue);
virtual CString* GetNodeValue();
void SetParentNode(CDOMNode* pParentNode);
CDOMNode* GetParentNode();
CDOMNode* GetFirstNode();
CDOMNode* GetLastNode();
CDOMNodeList* GetChildNodes();
void SetPreviousNodeSibling(CDOMNode* pPreviousNodeSibling);
CDOMNode* GetPreviousNodeSibling();
void SetNextNodeSibling(CDOMNode* pNextNodeSibling);
CDOMNode* GetNextNodeSibling();
CDOMNamedNodeList* GetAttributes();
CDOMNode* InsertBefore(CDOMNode* pNewChild, CDOMNode* pRefChild);
CDOMNode* ReplaceChild(CDOMNode* pNewChild, CDOMNode* pOldChild);
CDOMNode* RemoveChild(CDOMNode* pOldChild);
CDOMNode* AppendChild(CDOMNode* pNewChild);
CDOMNode* CloneNode(bool bDeep);
void Normalize();
bool HasChildNodes();
virtual bool HasAttributes();
virtual CString ToString();
void SetDocLineColumnNumbers(int nDocLineNumber, int nDocColumnNumber);
int GetDocLineNumber();
int GetDocColumnNumber();

protected:
virtual CDOMNode* CopyNode() = 0;

protected:
CDOMDocument* m_pOwnerDocument;
EDOMNodeType m_nNodeType;
CDOMNode* m_pParentNode;
CDOMNode* m_pPreviousNodeSibling;
CDOMNode* m_pNextNodeSibling;
CDOMNode* m_pFirstNode;
CDOMNode* m_pLastNode;
int m_nDocLineNumber;
int m_nDocColumnNumber;
};

inline CDOMDocument* CDOMNode::GetOwnerDocument()
{
return m_pOwnerDocument;
}

inline EDOMNodeType CDOMNode::GetNodeType()
{
return m_nNodeType;
}

inline void CDOMNode::SetNodeName(CString* pStrName)
{
}

inline CString* CDOMNode::GetNodeName()
{
return NULL;
}

inline void CDOMNode::SetNodeValue(CString* pStrValue)
{
}

inline CString* CDOMNode::GetNodeValue()
{
return NULL;
}

inline void CDOMNode::SetParentNode(CDOMNode* pParentNode)
{
m_pParentNode = pParentNode;
}

inline CDOMNode* CDOMNode::GetParentNode()
{
return m_pParentNode;
}

inline CDOMNode* CDOMNode::GetFirstNode()
{
return m_pFirstNode;
}

inline CDOMNode* CDOMNode::GetLastNode()
{
return m_pLastNode;
}

inline void CDOMNode::SetPreviousNodeSibling(CDOMNode* pPreviousNodeSibling)
{
m_pPreviousNodeSibling = pPreviousNodeSibling;
}

inline CDOMNode* CDOMNode::GetPreviousNodeSibling()
{
return m_pPreviousNodeSibling;
}

inline void CDOMNode::SetNextNodeSibling(CDOMNode* pNextNodeSibling)
{
m_pNextNodeSibling = pNextNodeSibling;
}

inline CDOMNode* CDOMNode::GetNextNodeSibling()
{
return m_pNextNodeSibling;
}

inline CDOMNamedNodeList* CDOMNode::GetAttributes()
{
return NULL;
}

inline void CDOMNode::Normalize()
{
}

inline bool CDOMNode::HasChildNodes()
{
return ((m_pFirstNode != NULL) ? true : false);
}

inline bool CDOMNode::HasAttributes()
{
return false;
}

inline int CDOMNode::GetDocLineNumber()
{
return m_nDocLineNumber;
}

inline int CDOMNode::GetDocColumnNumber()
{
return m_nDocColumnNumber;
}

#endif // !defined(DOMNODE_H_)
