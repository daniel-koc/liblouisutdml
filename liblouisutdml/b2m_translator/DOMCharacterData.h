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

  // CDOMNode override:
  void SetNodeValue(CString* pStrValue) override { SetData(pStrValue); }
  CString* GetNodeValue() override { return GetData(); }
  CString ToString() override;

  void SetData(CString* pStrData);
  CString* GetData() { return m_pStrData; }
  int GetLength() { return (m_pStrData ? m_pStrData->length() : 0); }
  CString* SubstringData(int nOffset, int nCount);
  void AppendData(CString* pStrArg);
  void InsertData(int nOffset, CString* pStrArg);
  void DeleteData(int nOffset, int nCount);
  void ReplaceData(int nOffset, int nCount, CString* pStrArg);

 protected:
  CString* m_pStrData;
};

#endif  // !defined(DOMCHARACTERDATA_H_)
