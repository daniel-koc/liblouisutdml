// DOMNode.cpp: implementation of the CDOMNode class.
//
//////////////////////////////////////////////////////////////////////

#include "DOMNode.h"
#include "DOMNodeList.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDOMNode::CDOMNode(CDOMDocument* pOwnerDocument, EDOMNodeType nNodeType) {
  m_pOwnerDocument = pOwnerDocument;
  m_nNodeType = nNodeType;
  m_pParentNode = NULL;
  m_pPreviousNodeSibling = NULL;
  m_pNextNodeSibling = NULL;
  m_pFirstNode = NULL;
  m_pLastNode = NULL;
  m_nDocLineNumber = 0;
  m_nDocColumnNumber = 0;
  m_bUnderOverType = false;
}  // CDOCNode

CDOMNode::~CDOMNode() {
  while (m_pFirstNode) {
    m_pLastNode = m_pFirstNode;
    m_pFirstNode = m_pFirstNode->GetNextNodeSibling();
    delete m_pLastNode;
  }  // while
}  // ~CDOMNode

void CDOMNode::SetNodeName(CString* pStrName) {
}

CString* CDOMNode::GetNodeName() {
  return NULL;
}

void CDOMNode::SetNodeValue(CString* pStrValue) {
}

CString* CDOMNode::GetNodeValue() {
  return NULL;
}

CDOMNamedNodeList* CDOMNode::GetAttributeNodes() {
  return NULL;
}

bool CDOMNode::HasAttributes() {
  return false;
}

CString CDOMNode::ToString() {
  CString str;
  CDOMNode* pTmpNode = m_pFirstNode;
  if (pTmpNode && pTmpNode->GetNodeType() == DOM_ELEMENT_NODE)
    str += L"\r\n";
  while (pTmpNode) {
    str += pTmpNode->ToString();
    pTmpNode = pTmpNode->GetNextNodeSibling();
  }  // while
  return str;
}  // ToString

CDOMNodeList* CDOMNode::GetChildNodes() {
  return new CDOMNodeList(m_pFirstNode);
}

CDOMNode* CDOMNode::InsertBefore(CDOMNode* pNewChild, CDOMNode* pRefChild) {
  if (pNewChild && pRefChild) {
    CDOMNode* pTmpNode = m_pFirstNode;
    while (pTmpNode && pTmpNode != pRefChild)
      pTmpNode = pTmpNode->GetNextNodeSibling();
    if (pTmpNode == pRefChild) {
      pNewChild->SetPreviousNodeSibling(pRefChild->GetPreviousNodeSibling());
      if (pRefChild->GetPreviousNodeSibling())
        pRefChild->GetPreviousNodeSibling()->SetNextNodeSibling(pNewChild);
      else
        m_pFirstNode = pNewChild;
      pNewChild->SetNextNodeSibling(pRefChild);
      pRefChild->SetPreviousNodeSibling(pNewChild);
      pNewChild->SetParentNode(pRefChild->GetParentNode());
      return pNewChild;
    }  // if
  }    // if
  return NULL;
}  // InsertBefore

CDOMNode* CDOMNode::ReplaceChild(CDOMNode* pNewChild, CDOMNode* pOldChild) {
  if (pNewChild && pOldChild) {
    CDOMNode* pTmpNode = m_pFirstNode;
    while (pTmpNode && pTmpNode != pOldChild)
      pTmpNode = pTmpNode->GetNextNodeSibling();
    if (pTmpNode == pOldChild) {
      pNewChild->SetPreviousNodeSibling(pOldChild->GetPreviousNodeSibling());
      if (pOldChild->GetPreviousNodeSibling())
        pOldChild->GetPreviousNodeSibling()->SetNextNodeSibling(pNewChild);
      else
        m_pFirstNode = pNewChild;
      pNewChild->SetNextNodeSibling(pOldChild->GetNextNodeSibling());
      if (pOldChild->GetNextNodeSibling())
        pOldChild->GetNextNodeSibling()->SetPreviousNodeSibling(pNewChild);
      else
        m_pLastNode = pNewChild;
      pNewChild->SetParentNode(pOldChild->GetParentNode());
      return pNewChild;
    }  // if
  }    // if
  return NULL;
}  // ReplaceChild

CDOMNode* CDOMNode::RemoveChild(CDOMNode* pOldChild) {
  if (pOldChild) {
    CDOMNode* pTmpNode = m_pFirstNode;
    while (pTmpNode && pTmpNode != pOldChild)
      pTmpNode = pTmpNode->GetNextNodeSibling();
    if (pTmpNode == pOldChild) {
      if (pOldChild->GetPreviousNodeSibling()) {
        pOldChild->GetPreviousNodeSibling()->SetNextNodeSibling(
            pOldChild->GetNextNodeSibling());
      } else
        m_pFirstNode = pOldChild->GetNextNodeSibling();
      if (pOldChild->GetNextNodeSibling()) {
        pOldChild->GetNextNodeSibling()->SetPreviousNodeSibling(
            pOldChild->GetPreviousNodeSibling());
      } else
        m_pLastNode = pOldChild->GetPreviousNodeSibling();
pOldChild->SetParentNode(NULL);
      return pOldChild;
    }  // if
  }    // if
  return NULL;
}  // RemoveChild

CDOMNode* CDOMNode::AppendChild(CDOMNode* pNewChild) {
  if (!m_pFirstNode)
    m_pFirstNode = m_pLastNode = pNewChild;
  else {
    m_pLastNode->SetNextNodeSibling(pNewChild);
    pNewChild->SetPreviousNodeSibling(m_pLastNode);
    m_pLastNode = pNewChild;
  }  // else if
  pNewChild->SetParentNode(this);
  return pNewChild;
}  // AppendChild

CDOMNode* CDOMNode::CloneNode(bool bDeep) {
  CDOMNode* pNewNode = CopyNode();
  if (bDeep) {
    CDOMNode* pTmpNode = m_pFirstNode;
    while (pTmpNode) {
      pNewNode->AppendChild(pTmpNode->CloneNode(true));
      pTmpNode = pTmpNode->GetNextNodeSibling();
    }  // while
  }    // if
  return pNewNode;
}  // CloneNode

void CDOMNode::SetDocLineColumnNumbers(int nDocLineNumber,
                                       int nDocColumnNumber) {
  m_nDocLineNumber = nDocLineNumber;
  m_nDocColumnNumber = nDocColumnNumber;
}
