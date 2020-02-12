// DOMEntity.cpp: implementation of the CDOMEntity class.
//
//////////////////////////////////////////////////////////////////////

#include "DOMEntity.h"
#include "DOMDocument.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDOMEntity::CDOMEntity(CDOMDocument* pOwnerDocument)
    : CDOMNode(pOwnerDocument, DOM_ENTITY_NODE) {
}

CDOMEntity::~CDOMEntity() {
}
