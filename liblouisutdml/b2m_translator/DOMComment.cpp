// DOMComment.cpp: implementation of the CDOMComment class.
//
//////////////////////////////////////////////////////////////////////

#include "DOMComment.h"
#include "DOMDocument.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDOMComment::CDOMComment(CDOMDocument* pOwnerDocument)
    : CDOMText(pOwnerDocument, DOM_COMMENT_NODE) {
}

CDOMComment::~CDOMComment() {
}

CDOMNode* CDOMComment::CopyNode() {
  CDOMComment* pNewComment = GetOwnerDocument()->CreateComment(
      m_pStrData ? new CString(*m_pStrData) : NULL);
  return pNewComment;
}  // CopyNode

CString CDOMComment::ToString() {
  CString str;
  str = L"<!--";
  if (m_pStrData)
    str += *m_pStrData;
  str += L"-->";
  return str;
}  // ToString
