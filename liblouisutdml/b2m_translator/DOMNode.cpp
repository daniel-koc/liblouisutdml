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
  while (m_pFirstNode != NULL) {
    m_pLastNode = m_pFirstNode;
    m_pFirstNode = m_pFirstNode->GetNextNodeSibling();
    delete m_pLastNode;
  }  // while
}  // ~CDOMNode

CDOMNodeList* CDOMNode::GetChildNodes() {
  return new CDOMNodeList(m_pFirstNode);
}

CDOMNode* CDOMNode::InsertBefore(CDOMNode* pNewChild, CDOMNode* pRefChild) {
  if (pNewChild != NULL && pRefChild != NULL) {
    CDOMNode* pTmpNode = m_pFirstNode;
    while (pTmpNode != NULL && pTmpNode != pRefChild)
      pTmpNode = pTmpNode->m_pNextNodeSibling;
    if (pTmpNode == pRefChild) {
      pNewChild->m_pPreviousNodeSibling = pRefChild->m_pPreviousNodeSibling;
      if (pRefChild->m_pPreviousNodeSibling != NULL)
        pRefChild->m_pPreviousNodeSibling->m_pNextNodeSibling = pNewChild;
      else
        m_pFirstNode = pNewChild;
      pNewChild->m_pNextNodeSibling = pRefChild;
      pRefChild->m_pPreviousNodeSibling = pNewChild;
      pNewChild->SetParentNode(pRefChild->GetParentNode());
    }  // if
  }    // if
  return pNewChild;
}  // InsertBefore

CDOMNode* CDOMNode::ReplaceChild(CDOMNode* pNewChild, CDOMNode* pOldChild) {
  if (pNewChild != NULL && pOldChild != NULL) {
    CDOMNode* pTmpNode = m_pFirstNode;
    while (pTmpNode != NULL && pTmpNode != pOldChild)
      pTmpNode = pTmpNode->m_pNextNodeSibling;
    if (pTmpNode == pOldChild) {
      pNewChild->m_pPreviousNodeSibling = pOldChild->m_pPreviousNodeSibling;
      if (pOldChild->m_pPreviousNodeSibling != NULL)
        pOldChild->m_pPreviousNodeSibling->m_pNextNodeSibling = pNewChild;
      else
        m_pFirstNode = pNewChild;
      pNewChild->m_pNextNodeSibling = pOldChild->m_pNextNodeSibling;
      if (pOldChild->m_pNextNodeSibling != NULL)
        pOldChild->m_pNextNodeSibling->m_pPreviousNodeSibling = pNewChild;
      else
        m_pLastNode = pNewChild;
      pNewChild->SetParentNode(pOldChild->GetParentNode());
    }  // if
  }    // if
  return pNewChild;
}  // ReplaceChild

CDOMNode* CDOMNode::RemoveChild(CDOMNode* pOldChild) {
  if (pOldChild != NULL) {
    CDOMNode* pTmpNode = m_pFirstNode;
    while (pTmpNode != NULL && pTmpNode != pOldChild)
      pTmpNode = pTmpNode->m_pNextNodeSibling;
    if (pTmpNode == pOldChild) {
      if (pOldChild->m_pPreviousNodeSibling != NULL)
        pOldChild->m_pPreviousNodeSibling->m_pNextNodeSibling =
            pOldChild->m_pNextNodeSibling;
      else
        m_pFirstNode = pOldChild->m_pNextNodeSibling;
      if (pOldChild->m_pNextNodeSibling != NULL)
        pOldChild->m_pNextNodeSibling->m_pPreviousNodeSibling =
            pOldChild->m_pPreviousNodeSibling;
      else
        m_pLastNode = pOldChild->m_pPreviousNodeSibling;
    }  // if
  }    // if
  return pOldChild;
}  // RemoveChild

CDOMNode* CDOMNode::AppendChild(CDOMNode* pNewChild) {
  if (m_pFirstNode == NULL)
    m_pFirstNode = m_pLastNode = pNewChild;
  else {
    m_pLastNode->m_pNextNodeSibling = pNewChild;
    pNewChild->m_pPreviousNodeSibling = m_pLastNode;
    m_pLastNode = pNewChild;
  }  // else if
  pNewChild->SetParentNode(this);
  return pNewChild;
}  // AppendChild

CDOMNode* CDOMNode::CloneNode(bool bDeep) {
  CDOMNode* pNewNode = CopyNode();
  if (bDeep) {
    CDOMNode* pTmpNode = m_pFirstNode;
    while (pTmpNode != NULL) {
      pNewNode->AppendChild(pTmpNode->CloneNode(true));
      pTmpNode = pTmpNode->m_pNextNodeSibling;
    }  // while
  }    // if
  return pNewNode;
}  // CloneNode

CString CDOMNode::ToString() {
  CString str;
  CDOMNode* pTmpNode = m_pFirstNode;
  if (pTmpNode != NULL && pTmpNode->GetNodeType() == DOM_ELEMENT_NODE)
    str += L"\r\n";
  while (pTmpNode != NULL) {
    str += pTmpNode->ToString();
    pTmpNode = pTmpNode->GetNextNodeSibling();
  }  // while
  return str;
}

void CDOMNode::SetDocLineColumnNumbers(int nDocLineNumber,
                                       int nDocColumnNumber) {
  m_nDocLineNumber = nDocLineNumber;
  m_nDocColumnNumber = nDocColumnNumber;
}
