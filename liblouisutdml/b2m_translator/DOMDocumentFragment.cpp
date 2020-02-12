// DOMDocumentFragment.cpp: implementation of the CDOMDocumentFragment class.
//
//////////////////////////////////////////////////////////////////////

#include "DOMDocumentFragment.h"
#include "DOMDocument.h"

CDOMDocumentFragment::CDOMDocumentFragment(CDOMDocument* pOwnerDocument)
    : CDOMNode(pOwnerDocument, DOM_DOCUMENT_FRAGMENT) {
}

CDOMDocumentFragment::~CDOMDocumentFragment() {
}

CDOMNode* CDOMDocumentFragment::CopyNode() {
  CDOMDocumentFragment* pNewDocumentFragment =
      m_pOwnerDocument->CreateDocumentFragment();
  return pNewDocumentFragment;
}  // CopyNode
