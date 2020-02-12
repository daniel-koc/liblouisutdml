// DOMCharacterData.h: interface for the CDOMCharacterData class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(DOMCHARACTERDATA_H_)
#define DOMCHARACTERDATA_H_

#include "DOMNode.h"

class CDOMDocument;

class CDOMCharacterData : public CDOMNode {
 public:
  CDOMCharacterData(CDOMDocument* pOwnerDocument, EDOMNodeType nNodeType);
  virtual ~CDOMCharacterData();
  void SetData(CString* pStrData);
  CString* GetData();
  int GetLength();
  CString* SubstringData(int nOffset, int nCount);
  void AppendData(CString* pStrArg);
  void InsertData(int nOffset, CString* pStrArg);
  void DeleteData(int nOffset, int nCount);
  void ReplaceData(int nOffset, int nCount, CString* pStrArg);
  virtual CString ToString();

 protected:
  CString* m_pStrData;
};

inline CString* CDOMCharacterData::GetData() {
  return m_pStrData;
}

inline int CDOMCharacterData::GetLength() {
  return m_pStrData->length();
}

#endif  // !defined(DOMCHARACTERDATA_H_)
