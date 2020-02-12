// CString_utils.cpp: implementation of the CString_ class.
//
//////////////////////////////////////////////////////////////////////

#include "String_utils.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CString::CString() {
  this->m_pszStr = new wchar_t[1];
  this->m_pszStr[0] = L'\0';
  this->m_nStrLen = 0;
  this->m_nStrBufLen = 1;
  m_pszCountBuf = new wchar_t[50];
}

CString::CString(wchar_t* pszStr, size_t nStrLen) {
  this->m_pszStr = pszStr;
  this->m_nStrLen = nStrLen;
  this->m_nStrBufLen = nStrLen;
  m_pszCountBuf = new wchar_t[50];
}

CString::CString(size_t nStrBufLen) {
  this->m_nStrBufLen = nStrBufLen;
  if (this->m_nStrBufLen == 0) {
    this->m_pszStr = NULL;
  } else {
    this->m_pszStr = new wchar_t[m_nStrBufLen + 1];
    *this->m_pszStr = NULL;
  }  // else if
  this->m_nStrLen = 0;
  m_pszCountBuf = new wchar_t[50];
}  // CString

CString::CString(const wchar_t* pszStr) {
  if (pszStr == NULL || *pszStr == NULL) {
    this->m_pszStr = NULL;
    this->m_nStrLen = 0;
    this->m_nStrBufLen = 0;
  } else {
    this->m_nStrLen = wcslen(pszStr);
    this->m_nStrBufLen = this->m_nStrLen;
    this->m_pszStr = new wchar_t[this->m_nStrBufLen + 1];
    wcscpy(this->m_pszStr, pszStr);
  }  // else if
  m_pszCountBuf = new wchar_t[50];
}  // CString

CString::CString(wchar_t* pszStr) {
  if (pszStr == NULL || *pszStr == NULL) {
    this->m_pszStr = NULL;
    this->m_nStrLen = 0;
    this->m_nStrBufLen = 0;
  } else {
    this->m_nStrLen = wcslen(pszStr);
    this->m_nStrBufLen = this->m_nStrLen;
    this->m_pszStr = new wchar_t[this->m_nStrBufLen + 1];
    wcscpy(this->m_pszStr, pszStr);
  }  // else if
  m_pszCountBuf = new wchar_t[50];
}  // CString

CString::CString(const CString& str) {
  this->m_nStrLen = str.length();
  this->m_nStrBufLen = this->m_nStrLen;
  if (this->m_nStrLen == 0) {
    this->m_pszStr = NULL;
  } else {
    this->m_pszStr = new wchar_t[this->m_nStrLen + 1];
    wcscpy(this->m_pszStr, (const wchar_t*)str);
  }  // else if
  m_pszCountBuf = new wchar_t[50];
}  // CString

CString::~CString() {
  if (this->m_pszStr != NULL) {
    delete[] this->m_pszStr;
  }
  if (m_pszCountBuf != NULL)
    delete[] m_pszCountBuf;
}

CString& CString::operator=(const wchar_t* pszStr) {
  this->m_nStrLen = wcslen(pszStr);
  if (this->m_nStrLen == 0) {
    return *this;
  }

  if (this->m_nStrLen > this->m_nStrBufLen) {
    delete[] this->m_pszStr;
    this->m_pszStr = new wchar_t[this->m_nStrLen + 1];
    this->m_nStrBufLen = this->m_nStrLen;
  }  // else if
  wcscpy(this->m_pszStr, pszStr);
  return *this;
}  // operator=

CString& CString::operator=(wchar_t* pszStr) {
  this->m_nStrLen = wcslen(pszStr);
  if (this->m_nStrLen == 0) {
    return *this;
  }

  if (this->m_nStrLen > this->m_nStrBufLen) {
    delete[] this->m_pszStr;
    this->m_pszStr = new wchar_t[this->m_nStrLen + 1];
    this->m_nStrBufLen = this->m_nStrLen;
  }  // else if
  wcscpy(this->m_pszStr, pszStr);
  return *this;
}  // operator=

CString& CString::operator=(const CString& str) {
  this->m_nStrLen = str.length();
  if (this->m_nStrLen == 0) {
    return *this;
  }

  if (this->m_nStrLen > this->m_nStrBufLen) {
    delete[] this->m_pszStr;
    this->m_pszStr = new wchar_t[this->m_nStrLen + 1];
    this->m_nStrBufLen = this->m_nStrLen;
  }  // else if
  wcscpy(this->m_pszStr, (const wchar_t*)str);
  return *this;
}  // operator=

CString operator+(const wchar_t* pszStr, const CString& str) {
  if (pszStr == NULL || *pszStr == NULL) {
    return str;
  }
  if (str.isEmpty()) {
    return CString(pszStr);
  }
  size_t nCatStrLen = str.length() + wcslen(pszStr);
  wchar_t* pszCatStr = new wchar_t[nCatStrLen + 1];
  *pszCatStr = NULL;
  wcscpy(pszCatStr, pszStr);
  wcscat(pszCatStr, (const wchar_t*)str);
  return CString(pszCatStr, nCatStrLen);
}  // operator+

CString operator+(const CString& str, const wchar_t* pszStr) {
  if (pszStr == NULL || *pszStr == NULL) {
    return str;
  }
  if (str.isEmpty()) {
    return CString(pszStr);
  }
  size_t nCatStrLen = str.length() + wcslen(pszStr);
  wchar_t* pszCatStr = new wchar_t[nCatStrLen + 1];
  *pszCatStr = NULL;
  wcscpy(pszCatStr, (const wchar_t*)str);
  wcscat(pszCatStr, pszStr);
  return CString(pszCatStr, nCatStrLen);
}  // operator+

CString CString::operator+(const wchar_t* pszStr) const {
  if (pszStr == NULL || *pszStr == NULL) {
    return *this;
  }
  size_t nCatStrLen = this->m_nStrLen + wcslen(pszStr);
  wchar_t* pszCatStr = new wchar_t[nCatStrLen + 1];
  *pszCatStr = NULL;
  if (this->m_pszStr != NULL && *(this->m_pszStr) != NULL) {
    wcscpy(pszCatStr, this->m_pszStr);
  }
  wcscat(pszCatStr, pszStr);
  return CString(pszCatStr, nCatStrLen);
}  // operator+

CString CString::operator+(wchar_t* pszStr) const {
  if (pszStr == NULL || *pszStr == NULL) {
    return *this;
  }
  size_t nCatStrLen = this->m_nStrLen + wcslen(pszStr);
  wchar_t* pszCatStr = new wchar_t[nCatStrLen + 1];
  *pszCatStr = NULL;
  if (this->m_pszStr != NULL && *(this->m_pszStr) != NULL) {
    wcscpy(pszCatStr, this->m_pszStr);
  }
  wcscat(pszCatStr, pszStr);
  return CString(pszCatStr, nCatStrLen);
}  // operator+

CString CString::operator+(const CString& str) const {
  if (str.length() == 0) {
    return *this;
  }
  size_t nCatStrLen = this->m_nStrLen + str.length();
  wchar_t* pszCatStr = new wchar_t[nCatStrLen + 1];
  *pszCatStr = NULL;
  if (this->m_pszStr != NULL && *(this->m_pszStr) != NULL) {
    wcscpy(pszCatStr, this->m_pszStr);
  }
  wcscat(pszCatStr, (const wchar_t*)str);
  return CString(pszCatStr, nCatStrLen);
}  // operator+

CString CString::operator+(CString& str) const {
  if (str.length() == 0) {
    return *this;
  }
  size_t nCatStrLen = this->m_nStrLen + str.length();
  wchar_t* pszCatStr = new wchar_t[nCatStrLen + 1];
  *pszCatStr = NULL;
  if (this->m_pszStr != NULL && *(this->m_pszStr) != NULL) {
    wcscpy(pszCatStr, this->m_pszStr);
  }
  wcscat(pszCatStr, (const wchar_t*)str);
  return CString(pszCatStr, nCatStrLen);
}  // operator+

CString CString::operator+(CString* pStr) const {
  if (pStr->length() == 0) {
    return *this;
  }
  size_t nCatStrLen = this->m_nStrLen + pStr->length();
  wchar_t* pszCatStr = new wchar_t[nCatStrLen + 1];
  *pszCatStr = NULL;
  if (this->m_pszStr != NULL && *(this->m_pszStr) != NULL) {
    wcscpy(pszCatStr, this->m_pszStr);
  }
  wcscat(pszCatStr, (const wchar_t*)*pStr);
  return CString(pszCatStr, nCatStrLen);
}  // operator+

CString CString::operator+(signed char nCount) const {
  _swprintf(m_pszCountBuf, L"%d", nCount);
  size_t nCatStrLen = this->m_nStrLen + wcslen(m_pszCountBuf);
  wchar_t* pszCatStr = new wchar_t[nCatStrLen + 1];
  *pszCatStr = NULL;
  if (this->m_pszStr != NULL && *(this->m_pszStr) != NULL) {
    wcscpy(pszCatStr, this->m_pszStr);
  }
  if (*m_pszCountBuf != NULL) {
    wcscat(pszCatStr, m_pszCountBuf);
  }
  return CString(pszCatStr, nCatStrLen);
}  // operator+

CString CString::operator+(unsigned char nCount) const {
  _swprintf(m_pszCountBuf, L"%d", nCount);
  size_t nCatStrLen = this->m_nStrLen + wcslen(m_pszCountBuf);
  wchar_t* pszCatStr = new wchar_t[nCatStrLen + 1];
  *pszCatStr = NULL;
  if (this->m_pszStr != NULL && *(this->m_pszStr) != NULL) {
    wcscpy(pszCatStr, this->m_pszStr);
  }
  if (*m_pszCountBuf != NULL) {
    wcscat(pszCatStr, m_pszCountBuf);
  }
  return CString(pszCatStr, nCatStrLen);
}  // operator+

CString CString::operator+(wchar_t wcChar) const {
  size_t nCatStrLen = this->m_nStrLen + 1;
  wchar_t* pszCatStr = new wchar_t[nCatStrLen + 1];
  *pszCatStr = NULL;
  if (this->m_pszStr != NULL && *(this->m_pszStr) != NULL) {
    wcscpy(pszCatStr, this->m_pszStr);
  }
  *(pszCatStr + nCatStrLen - 1) = wcChar;
  *(pszCatStr + nCatStrLen) = NULL;
  return CString(pszCatStr, nCatStrLen);
}  // operator+

CString CString::operator+(signed short int nCount) const {
  _swprintf(m_pszCountBuf, L"%d", nCount);
  size_t nCatStrLen = this->m_nStrLen + wcslen(m_pszCountBuf);
  wchar_t* pszCatStr = new wchar_t[nCatStrLen + 1];
  *pszCatStr = NULL;
  if (this->m_pszStr != NULL && *(this->m_pszStr) != NULL) {
    wcscpy(pszCatStr, this->m_pszStr);
  }
  if (*m_pszCountBuf != NULL) {
    wcscat(pszCatStr, m_pszCountBuf);
  }
  return CString(pszCatStr, nCatStrLen);
}  // operator+

CString CString::operator+(unsigned short int nCount) const {
  _swprintf(m_pszCountBuf, L"%d", nCount);
  size_t nCatStrLen = this->m_nStrLen + wcslen(m_pszCountBuf);
  wchar_t* pszCatStr = new wchar_t[nCatStrLen + 1];
  *pszCatStr = NULL;
  if (this->m_pszStr != NULL && *(this->m_pszStr) != NULL) {
    wcscpy(pszCatStr, this->m_pszStr);
  }
  if (*m_pszCountBuf != NULL) {
    wcscat(pszCatStr, m_pszCountBuf);
  }
  return CString(pszCatStr, nCatStrLen);
}  // operator+

CString CString::operator+(signed int nCount) const {
  _swprintf((wchar_t*)m_pszCountBuf, L"%d", nCount);
  size_t nCatStrLen = this->m_nStrLen + wcslen(m_pszCountBuf);
  wchar_t* pszCatStr = new wchar_t[nCatStrLen + 1];
  *pszCatStr = NULL;
  if (this->m_pszStr != NULL && *(this->m_pszStr) != NULL) {
    wcscpy(pszCatStr, this->m_pszStr);
  }
  if (*m_pszCountBuf != NULL) {
    wcscat(pszCatStr, m_pszCountBuf);
  }
  return CString(pszCatStr, nCatStrLen);
}  // operator+

CString CString::operator+(unsigned int nCount) const {
  _swprintf((wchar_t*)m_pszCountBuf, L"%d", nCount);
  size_t nCatStrLen = this->m_nStrLen + wcslen(m_pszCountBuf);
  wchar_t* pszCatStr = new wchar_t[nCatStrLen + 1];
  *pszCatStr = NULL;
  if (this->m_pszStr != NULL && *(this->m_pszStr) != NULL) {
    wcscpy(pszCatStr, this->m_pszStr);
  }
  if (*m_pszCountBuf != NULL) {
    wcscat(pszCatStr, m_pszCountBuf);
  }
  return CString(pszCatStr, nCatStrLen);
}  // operator+

CString CString::operator+(signed long int nCount) const {
  _swprintf((wchar_t*)m_pszCountBuf, L"%ld", nCount);
  size_t nCatStrLen = this->m_nStrLen + wcslen(m_pszCountBuf);
  wchar_t* pszCatStr = new wchar_t[nCatStrLen + 1];
  *pszCatStr = NULL;
  if (this->m_pszStr != NULL && *(this->m_pszStr) != NULL) {
    wcscpy(pszCatStr, this->m_pszStr);
  }
  if (*m_pszCountBuf != NULL) {
    wcscat(pszCatStr, m_pszCountBuf);
  }
  return CString(pszCatStr, nCatStrLen);
}  // operator+

CString CString::operator+(unsigned long int nCount) const {
  _swprintf((wchar_t*)m_pszCountBuf, L"%ld", nCount);
  size_t nCatStrLen = this->m_nStrLen + wcslen(m_pszCountBuf);
  wchar_t* pszCatStr = new wchar_t[nCatStrLen + 1];
  *pszCatStr = NULL;
  if (this->m_pszStr != NULL && *(this->m_pszStr) != NULL) {
    wcscpy(pszCatStr, this->m_pszStr);
  }
  if (*m_pszCountBuf != NULL) {
    wcscat(pszCatStr, m_pszCountBuf);
  }
  return CString(pszCatStr, nCatStrLen);
}  // operator+

CString CString::operator+(float nFloat) const {
  _swprintf((wchar_t*)m_pszCountBuf, L"%.30f", nFloat);
  size_t nLastNoZeroIndex = -1;
  size_t nPeriodIndex = -1;
  for (size_t i = wcslen(m_pszCountBuf) - 1; i >= 0; i--) {
    if (m_pszCountBuf[i] == '.') {
      nPeriodIndex = i;
      break;
    } else if (nLastNoZeroIndex == -1 && m_pszCountBuf[i] != '0') {
      nLastNoZeroIndex = i;
    }
  }  // for
  if (nPeriodIndex != -1) {
    if (nLastNoZeroIndex == -1) {
      m_pszCountBuf[nPeriodIndex] = NULL;
    } else {
      m_pszCountBuf[nLastNoZeroIndex + 1] = NULL;
    }
  }
  size_t nCatStrLen = this->m_nStrLen + wcslen(m_pszCountBuf);
  wchar_t* pszCatStr = new wchar_t[nCatStrLen + 1];
  *pszCatStr = NULL;
  if (this->m_pszStr != NULL && *(this->m_pszStr) != NULL) {
    wcscpy(pszCatStr, this->m_pszStr);
  }
  if (*m_pszCountBuf != NULL) {
    wcscat(pszCatStr, m_pszCountBuf);
  }
  return CString(pszCatStr, nCatStrLen);
}  // operator+

CString CString::operator+(double nDouble) const {
  _swprintf((wchar_t*)m_pszCountBuf, L"%.30Lf", nDouble);
  size_t nLastNoZeroIndex = -1;
  size_t nPeriodIndex = -1;
  for (size_t i = wcslen(m_pszCountBuf) - 1; i >= 0; i--) {
    if (m_pszCountBuf[i] == '.') {
      nPeriodIndex = i;
      break;
    } else if (nLastNoZeroIndex == -1 && m_pszCountBuf[i] != '0') {
      nLastNoZeroIndex = i;
    }
  }  // for
  if (nPeriodIndex != -1) {
    if (nLastNoZeroIndex == -1) {
      m_pszCountBuf[nPeriodIndex] = NULL;
    } else {
      m_pszCountBuf[nLastNoZeroIndex + 1] = NULL;
    }
  }
  size_t nCatStrLen = this->m_nStrLen + wcslen(m_pszCountBuf);
  wchar_t* pszCatStr = new wchar_t[nCatStrLen + 1];
  *pszCatStr = NULL;
  if (this->m_pszStr != NULL && *(this->m_pszStr) != NULL) {
    wcscpy(pszCatStr, this->m_pszStr);
  }
  if (*m_pszCountBuf != NULL) {
    wcscat(pszCatStr, m_pszCountBuf);
  }
  return CString(pszCatStr, nCatStrLen);
}  // operator+

CString CString::operator+(long double nDouble) const {
  _swprintf((wchar_t*)m_pszCountBuf, L"%.30Lf", nDouble);
  size_t nLastNoZeroIndex = -1;
  size_t nPeriodIndex = -1;
  for (size_t i = wcslen(m_pszCountBuf) - 1; i >= 0; i--) {
    if (m_pszCountBuf[i] == '.') {
      nPeriodIndex = i;
      break;
    } else if (nLastNoZeroIndex == -1 && m_pszCountBuf[i] != '0') {
      nLastNoZeroIndex = i;
    }
  }  // for
  if (nPeriodIndex != -1) {
    if (nLastNoZeroIndex == -1) {
      m_pszCountBuf[nPeriodIndex] = NULL;
    } else {
      m_pszCountBuf[nLastNoZeroIndex + 1] = NULL;
    }
  }
  size_t nCatStrLen = this->m_nStrLen + wcslen(m_pszCountBuf);
  wchar_t* pszCatStr = new wchar_t[nCatStrLen + 1];
  *pszCatStr = NULL;
  if (this->m_pszStr != NULL && *(this->m_pszStr) != NULL) {
    wcscpy(pszCatStr, this->m_pszStr);
  }
  if (*m_pszCountBuf != NULL) {
    wcscat(pszCatStr, m_pszCountBuf);
  }
  return CString(pszCatStr, nCatStrLen);
}  // operator+

void CString::operator+=(const wchar_t* pszStr) {
  if (pszStr == NULL || *pszStr == NULL) {
    return;
  }
  size_t nCatStrLen = this->m_nStrLen + wcslen(pszStr);
  if (nCatStrLen <= this->m_nStrBufLen) {
    wcscat(this->m_pszStr, pszStr);
    this->m_nStrLen = nCatStrLen;
  } else {
    wchar_t* pszCatStr = new wchar_t[nCatStrLen + 1];
    *pszCatStr = NULL;
    if (this->m_pszStr != NULL) {
      if (*(this->m_pszStr) != NULL) {
        wcscpy(pszCatStr, this->m_pszStr);
      }
      delete[] m_pszStr;
    }
    wcscat(pszCatStr, pszStr);
    this->m_pszStr = pszCatStr;
    this->m_nStrLen = nCatStrLen;
    this->m_nStrBufLen = nCatStrLen;
  }  // else if
}  // operator+=

void CString::operator+=(wchar_t* pszStr) {
  if (pszStr == NULL || *pszStr == NULL) {
    return;
  }
  size_t nCatStrLen = this->m_nStrLen + wcslen(pszStr);
  if (nCatStrLen <= this->m_nStrBufLen) {
    wcscat(this->m_pszStr, pszStr);
    this->m_nStrLen = nCatStrLen;
  } else {
    wchar_t* pszCatStr = new wchar_t[nCatStrLen + 1];
    *pszCatStr = NULL;
    if (this->m_pszStr != NULL) {
      if (*(this->m_pszStr) != NULL) {
        wcscpy(pszCatStr, this->m_pszStr);
      }
      delete[] m_pszStr;
    }
    wcscat(pszCatStr, pszStr);
    this->m_pszStr = pszCatStr;
    this->m_nStrLen = nCatStrLen;
    this->m_nStrBufLen = nCatStrLen;
  }  // else if
}  // operator+=

void CString::operator+=(CString str) {
  if (str.length() == 0) {
    return;
  }
  size_t nCatStrLen = this->m_nStrLen + str.length();
  if (nCatStrLen <= this->m_nStrBufLen) {
    wcscat(this->m_pszStr, str);
    this->m_nStrLen = nCatStrLen;
  } else {
    wchar_t* pszCatStr = new wchar_t[nCatStrLen + 1];
    *pszCatStr = NULL;
    if (this->m_pszStr != NULL) {
      if (*(this->m_pszStr) != NULL) {
        wcscpy(pszCatStr, this->m_pszStr);
      }
      delete[] m_pszStr;
    }
    wcscat(pszCatStr, str);
    this->m_pszStr = pszCatStr;
    this->m_nStrLen = nCatStrLen;
    this->m_nStrBufLen = nCatStrLen;
  }  // else if
}  // operator+=

void CString::operator+=(CString* pStr) {
  if (pStr->length() == 0) {
    return;
  }
  size_t nCatStrLen = this->m_nStrLen + pStr->length();
  if (nCatStrLen <= this->m_nStrBufLen) {
    wcscat(this->m_pszStr, (wchar_t*)*pStr);
    this->m_nStrLen = nCatStrLen;
  } else {
    wchar_t* pszCatStr = new wchar_t[nCatStrLen + 1];
    *pszCatStr = NULL;
    if (this->m_pszStr != NULL) {
      if (*(this->m_pszStr) != NULL) {
        wcscpy(pszCatStr, this->m_pszStr);
      }
      delete[] m_pszStr;
    }
    wcscat(pszCatStr, *pStr);
    this->m_pszStr = pszCatStr;
    this->m_nStrLen = nCatStrLen;
    this->m_nStrBufLen = nCatStrLen;
  }  // else if
}  // operator+=

void CString::operator+=(signed char nCount) {
  _swprintf((wchar_t*)m_pszCountBuf, L"%d", nCount);
  if (*m_pszCountBuf == NULL) {
    return;
  }
  size_t nCatStrLen = this->m_nStrLen + wcslen(m_pszCountBuf);
  if (nCatStrLen <= this->m_nStrBufLen) {
    wcscat(this->m_pszStr, m_pszCountBuf);
    this->m_nStrLen = nCatStrLen;
  } else {
    wchar_t* pszCatStr = new wchar_t[nCatStrLen + 1];
    *pszCatStr = NULL;
    if (this->m_pszStr != NULL) {
      if (*(this->m_pszStr) != NULL) {
        wcscpy(pszCatStr, this->m_pszStr);
      }
      delete[] m_pszStr;
    }
    wcscat(pszCatStr, m_pszCountBuf);
    this->m_pszStr = pszCatStr;
    this->m_nStrLen = nCatStrLen;
    this->m_nStrBufLen = nCatStrLen;
  }  // else if
}  // operator+=

void CString::operator+=(unsigned char nCount) {
  _swprintf((wchar_t*)m_pszCountBuf, L"%d", nCount);
  if (*m_pszCountBuf == NULL) {
    return;
  }
  size_t nCatStrLen = this->m_nStrLen + wcslen(m_pszCountBuf);
  if (nCatStrLen <= this->m_nStrBufLen) {
    wcscat(this->m_pszStr, m_pszCountBuf);
    this->m_nStrLen = nCatStrLen;
  } else {
    wchar_t* pszCatStr = new wchar_t[nCatStrLen + 1];
    *pszCatStr = NULL;
    if (this->m_pszStr != NULL) {
      if (*(this->m_pszStr) != NULL) {
        wcscpy(pszCatStr, this->m_pszStr);
      }
      delete[] m_pszStr;
    }
    wcscat(pszCatStr, m_pszCountBuf);
    this->m_pszStr = pszCatStr;
    this->m_nStrLen = nCatStrLen;
    this->m_nStrBufLen = nCatStrLen;
  }  // else if
}  // operator+=

void CString::operator+=(wchar_t wcChar) {
  size_t nCatStrLen = this->m_nStrLen + 1;
  if (nCatStrLen <= this->m_nStrBufLen) {
    *(this->m_pszStr + nCatStrLen - 1) = wcChar;
    *(this->m_pszStr + nCatStrLen) = NULL;
    this->m_nStrLen = nCatStrLen;
  } else {
    wchar_t* pszCatStr = new wchar_t[nCatStrLen + 1];
    *pszCatStr = NULL;
    if (this->m_pszStr != NULL) {
      if (*(this->m_pszStr) != NULL) {
        wcscpy(pszCatStr, this->m_pszStr);
      }
      delete[] m_pszStr;
    }
    *(pszCatStr + nCatStrLen - 1) = wcChar;
    *(pszCatStr + nCatStrLen) = NULL;
    this->m_pszStr = pszCatStr;
    this->m_nStrLen = nCatStrLen;
    this->m_nStrBufLen = nCatStrLen;
  }  // else if
}  // operator+=

void CString::operator+=(signed short int nCount) {
  _swprintf((wchar_t*)m_pszCountBuf, L"%d", nCount);
  if (*m_pszCountBuf == NULL) {
    return;
  }
  size_t nCatStrLen = this->m_nStrLen + wcslen(m_pszCountBuf);
  if (nCatStrLen <= this->m_nStrBufLen) {
    wcscat(this->m_pszStr, m_pszCountBuf);
    this->m_nStrLen = nCatStrLen;
  } else {
    wchar_t* pszCatStr = new wchar_t[nCatStrLen + 1];
    *pszCatStr = NULL;
    if (this->m_pszStr != NULL) {
      if (*(this->m_pszStr) != NULL) {
        wcscpy(pszCatStr, this->m_pszStr);
      }
      delete[] m_pszStr;
    }
    wcscat(pszCatStr, m_pszCountBuf);
    this->m_pszStr = pszCatStr;
    this->m_nStrLen = nCatStrLen;
    this->m_nStrBufLen = nCatStrLen;
  }  // else if
}  // operator+=

void CString::operator+=(unsigned short int nCount) {
  _swprintf((wchar_t*)m_pszCountBuf, L"%d", nCount);
  if (*m_pszCountBuf == NULL) {
    return;
  }
  size_t nCatStrLen = this->m_nStrLen + wcslen(m_pszCountBuf);
  if (nCatStrLen <= this->m_nStrBufLen) {
    wcscat(this->m_pszStr, m_pszCountBuf);
    this->m_nStrLen = nCatStrLen;
  } else {
    wchar_t* pszCatStr = new wchar_t[nCatStrLen + 1];
    *pszCatStr = NULL;
    if (this->m_pszStr != NULL) {
      if (*(this->m_pszStr) != NULL) {
        wcscpy(pszCatStr, this->m_pszStr);
      }
      delete[] m_pszStr;
    }
    wcscat(pszCatStr, m_pszCountBuf);
    this->m_pszStr = pszCatStr;
    this->m_nStrLen = nCatStrLen;
    this->m_nStrBufLen = nCatStrLen;
  }  // else if
}  // operator+=

void CString::operator+=(signed int nCount) {
  _swprintf((wchar_t*)m_pszCountBuf, L"%d", nCount);
  if (*m_pszCountBuf == NULL) {
    return;
  }
  size_t nCatStrLen = this->m_nStrLen + wcslen(m_pszCountBuf);
  if (nCatStrLen <= this->m_nStrBufLen) {
    wcscat(this->m_pszStr, m_pszCountBuf);
    this->m_nStrLen = nCatStrLen;
  } else {
    wchar_t* pszCatStr = new wchar_t[nCatStrLen + 1];
    *pszCatStr = NULL;
    if (this->m_pszStr != NULL) {
      if (*(this->m_pszStr) != NULL) {
        wcscpy(pszCatStr, this->m_pszStr);
      }
      delete[] m_pszStr;
    }
    wcscat(pszCatStr, m_pszCountBuf);
    this->m_pszStr = pszCatStr;
    this->m_nStrLen = nCatStrLen;
    this->m_nStrBufLen = nCatStrLen;
  }  // else if
}  // operator+=

void CString::operator+=(unsigned int nCount) {
  _swprintf((wchar_t*)m_pszCountBuf, L"%d", nCount);
  if (*m_pszCountBuf == NULL) {
    return;
  }
  size_t nCatStrLen = this->m_nStrLen + wcslen(m_pszCountBuf);
  if (nCatStrLen <= this->m_nStrBufLen) {
    wcscat(this->m_pszStr, m_pszCountBuf);
    this->m_nStrLen = nCatStrLen;
  } else {
    wchar_t* pszCatStr = new wchar_t[nCatStrLen + 1];
    *pszCatStr = NULL;
    if (this->m_pszStr != NULL) {
      if (*(this->m_pszStr) != NULL) {
        wcscpy(pszCatStr, this->m_pszStr);
      }
      delete[] m_pszStr;
    }
    wcscat(pszCatStr, m_pszCountBuf);
    this->m_pszStr = pszCatStr;
    this->m_nStrLen = nCatStrLen;
    this->m_nStrBufLen = nCatStrLen;
  }  // else if
}  // operator+=

void CString::operator+=(signed long int nCount) {
  _swprintf((wchar_t*)m_pszCountBuf, L"%ld", nCount);
  if (*m_pszCountBuf == NULL) {
    return;
  }
  size_t nCatStrLen = this->m_nStrLen + wcslen(m_pszCountBuf);
  if (nCatStrLen <= this->m_nStrBufLen) {
    wcscat(this->m_pszStr, m_pszCountBuf);
    this->m_nStrLen = nCatStrLen;
  } else {
    wchar_t* pszCatStr = new wchar_t[nCatStrLen + 1];
    *pszCatStr = NULL;
    if (this->m_pszStr != NULL) {
      if (*(this->m_pszStr) != NULL) {
        wcscpy(pszCatStr, this->m_pszStr);
      }
      delete[] m_pszStr;
    }
    wcscat(pszCatStr, m_pszCountBuf);
    this->m_pszStr = pszCatStr;
    this->m_nStrLen = nCatStrLen;
    this->m_nStrBufLen = nCatStrLen;
  }  // else if
}  // operator+=

void CString::operator+=(unsigned long int nCount) {
  _swprintf((wchar_t*)m_pszCountBuf, L"%ld", nCount);
  if (*m_pszCountBuf == NULL) {
    return;
  }
  size_t nCatStrLen = this->m_nStrLen + wcslen(m_pszCountBuf);
  if (nCatStrLen <= this->m_nStrBufLen) {
    wcscat(this->m_pszStr, m_pszCountBuf);
    this->m_nStrLen = nCatStrLen;
  } else {
    wchar_t* pszCatStr = new wchar_t[nCatStrLen + 1];
    *pszCatStr = NULL;
    if (this->m_pszStr != NULL) {
      if (*(this->m_pszStr) != NULL) {
        wcscpy(pszCatStr, this->m_pszStr);
      }
      delete[] m_pszStr;
    }
    wcscat(pszCatStr, m_pszCountBuf);
    this->m_pszStr = pszCatStr;
    this->m_nStrLen = nCatStrLen;
    this->m_nStrBufLen = nCatStrLen;
  }  // else if
}  // operator+=

void CString::operator+=(float nFloat) {
  _swprintf((wchar_t*)m_pszCountBuf, L"%.30f", nFloat);
  if (*m_pszCountBuf == NULL) {
    return;
  }
  size_t nLastNoZeroIndex = -1;
  size_t nPeriodIndex = -1;
  for (size_t i = wcslen(m_pszCountBuf) - 1; i >= 0; i--) {
    if (m_pszCountBuf[i] == '.') {
      nPeriodIndex = i;
      break;
    } else if (nLastNoZeroIndex == -1 && m_pszCountBuf[i] != '0') {
      nLastNoZeroIndex = i;
    }
  }  // for
  if (nPeriodIndex != -1) {
    if (nLastNoZeroIndex == -1) {
      m_pszCountBuf[nPeriodIndex] = NULL;
    } else {
      m_pszCountBuf[nLastNoZeroIndex + 1] = NULL;
    }
  }
  size_t nCatStrLen = this->m_nStrLen + wcslen(m_pszCountBuf);
  if (nCatStrLen <= this->m_nStrBufLen) {
    wcscat(this->m_pszStr, m_pszCountBuf);
    this->m_nStrLen = nCatStrLen;
  } else {
    wchar_t* pszCatStr = new wchar_t[nCatStrLen + 1];
    *pszCatStr = NULL;
    if (this->m_pszStr != NULL) {
      if (*(this->m_pszStr) != NULL) {
        wcscpy(pszCatStr, this->m_pszStr);
      }
      delete[] m_pszStr;
    }
    wcscat(pszCatStr, m_pszCountBuf);
    this->m_pszStr = pszCatStr;
    this->m_nStrLen = nCatStrLen;
    this->m_nStrBufLen = nCatStrLen;
  }  // else if
}  // operator+=

void CString::operator+=(double nDouble) {
  _swprintf((wchar_t*)m_pszCountBuf, L"%.30Lf", nDouble);
  if (*m_pszCountBuf == NULL) {
    return;
  }
  size_t nLastNoZeroIndex = -1;
  size_t nPeriodIndex = -1;
  for (size_t i = wcslen(m_pszCountBuf) - 1; i >= 0; i--) {
    if (m_pszCountBuf[i] == '.') {
      nPeriodIndex = i;
      break;
    } else if (nLastNoZeroIndex == -1 && m_pszCountBuf[i] != '0') {
      nLastNoZeroIndex = i;
    }
  }  // for
  if (nPeriodIndex != -1) {
    if (nLastNoZeroIndex == -1) {
      m_pszCountBuf[nPeriodIndex] = NULL;
    } else {
      m_pszCountBuf[nLastNoZeroIndex + 1] = NULL;
    }
  }
  size_t nCatStrLen = this->m_nStrLen + wcslen(m_pszCountBuf);
  if (nCatStrLen <= this->m_nStrBufLen) {
    wcscat(this->m_pszStr, m_pszCountBuf);
    this->m_nStrLen = nCatStrLen;
  } else {
    wchar_t* pszCatStr = new wchar_t[nCatStrLen + 1];
    *pszCatStr = NULL;
    if (this->m_pszStr != NULL) {
      if (*(this->m_pszStr) != NULL) {
        wcscpy(pszCatStr, this->m_pszStr);
      }
      delete[] m_pszStr;
    }
    wcscat(pszCatStr, m_pszCountBuf);
    this->m_pszStr = pszCatStr;
    this->m_nStrLen = nCatStrLen;
    this->m_nStrBufLen = nCatStrLen;
  }  // else if
}  // operator+=

void CString::operator+=(long double nDouble) {
  _swprintf((wchar_t*)m_pszCountBuf, L"%.30Lf", nDouble);
  if (*m_pszCountBuf == NULL) {
    return;
  }
  size_t nLastNoZeroIndex = -1;
  size_t nPeriodIndex = -1;
  for (size_t i = wcslen(m_pszCountBuf) - 1; i >= 0; i--) {
    if (m_pszCountBuf[i] == '.') {
      nPeriodIndex = i;
      break;
    } else if (nLastNoZeroIndex == -1 && m_pszCountBuf[i] != '0') {
      nLastNoZeroIndex = i;
    }
  }  // for
  if (nPeriodIndex != -1) {
    if (nLastNoZeroIndex == -1) {
      m_pszCountBuf[nPeriodIndex] = NULL;
    } else {
      m_pszCountBuf[nLastNoZeroIndex + 1] = NULL;
    }
  }
  size_t nCatStrLen = this->m_nStrLen + wcslen(m_pszCountBuf);
  if (nCatStrLen <= this->m_nStrBufLen) {
    wcscat(this->m_pszStr, m_pszCountBuf);
    this->m_nStrLen = nCatStrLen;
  } else {
    wchar_t* pszCatStr = new wchar_t[nCatStrLen + 1];
    *pszCatStr = NULL;
    if (this->m_pszStr != NULL) {
      if (*(this->m_pszStr) != NULL) {
        wcscpy(pszCatStr, this->m_pszStr);
      }
      delete[] m_pszStr;
    }
    wcscat(pszCatStr, m_pszCountBuf);
    this->m_pszStr = pszCatStr;
    this->m_nStrLen = nCatStrLen;
    this->m_nStrBufLen = nCatStrLen;
  }  // else if
}  // operator+=

CString CString::substring(size_t nStartIndex, size_t nEndIndex) {
  if (nStartIndex < 0) {
    nStartIndex = 0;
  }
  if (nEndIndex > this->m_nStrLen) {
    nEndIndex = this->m_nStrLen;
  }
  if (nEndIndex <= nStartIndex) {
    return CString();
  }
  size_t nNewStrLen = nEndIndex - nStartIndex;
  wchar_t* pszNewStr = new wchar_t[nNewStrLen + 1];
  wcsncpy(pszNewStr, this->m_pszStr + nStartIndex, nNewStrLen);
  *(pszNewStr + nNewStrLen) = NULL;
  return CString(pszNewStr, nNewStrLen);
}  // substring

CString CString::slice(size_t nStartIndex, size_t nLength) {
  return substring(nStartIndex, nStartIndex + nLength);
}

CString* CString::substringPtr(size_t nStartIndex, size_t nEndIndex) {
  if (nStartIndex < 0) {
    nStartIndex = 0;
  }
  if (nEndIndex > this->m_nStrLen) {
    nEndIndex = this->m_nStrLen;
  }
  if (nEndIndex <= nStartIndex) {
    return new CString();
  }
  size_t nNewStrLen = nEndIndex - nStartIndex;
  wchar_t* pszNewStr = new wchar_t[nNewStrLen + 1];
  wcsncpy(pszNewStr, this->m_pszStr + nStartIndex, nNewStrLen);
  *(pszNewStr + nNewStrLen) = NULL;
  return new CString(pszNewStr, nNewStrLen);
}  // substringPtr

CString* CString::slicePtr(size_t nStartIndex, size_t nLength) {
  return substringPtr(nStartIndex, nStartIndex + nLength);
}

CStringArray* CString::split(const wchar_t* wcsSeparator) {
  CStringArray* strArray = new CStringArray(this);
  strArray->split(wcsSeparator);
  return strArray;
}

CStringArray::CStringArray(CString* str)
    : m_pStr(str), m_ppArray(NULL), m_nArrayLen(0), m_nMaxArrayLen(0) {
}

CStringArray::~CStringArray() {
  if (m_ppArray != NULL) {
    for (int i = 0; i < m_nArrayLen; i++)
      delete m_ppArray[i];
    delete[] m_ppArray;
  }
}

void CStringArray::split(const wchar_t* wcsSeparator) {
  if (m_pStr == NULL || m_pStr->length() == 0)
    return;

  if (m_ppArray != NULL) {
    for (int i = 0; i < m_nArrayLen; i++)
      delete m_ppArray[i];
    delete[] m_ppArray;
    m_ppArray = NULL;
  }
  m_nArrayLen = m_nMaxArrayLen = 0;

  if (wcsSeparator == NULL || wcsSeparator[0] == NULL)
    return;

  m_nMaxArrayLen = 10;
  m_ppArray = new CString* [m_nMaxArrayLen];
  int nSeparatorLen = (int)wcslen(wcsSeparator);
  wchar_t* pwcsStr = m_pStr->m_pszStr;
  CString* pNewStr;
  int nNewStrLen;
  wchar_t* pwcsNewStr;
  while (*pwcsStr != NULL) {
    wchar_t* pwcsSeparatorStr = wcsstr(pwcsStr, wcsSeparator);
    if (pwcsSeparatorStr == NULL) {
      pNewStr = new CString(pwcsStr);
      pwcsStr += wcslen(pwcsStr);
    } else if (pwcsSeparatorStr == pwcsStr) {
      pwcsStr += nSeparatorLen;
      continue;
    } else {
      nNewStrLen = pwcsSeparatorStr - pwcsStr;
      pwcsNewStr = new wchar_t[nNewStrLen + 1];
      wcsncpy(pwcsNewStr, pwcsStr, nNewStrLen);
      pwcsNewStr[nNewStrLen] = NULL;
      pNewStr = new CString(pwcsNewStr, nNewStrLen);
      pwcsStr = pwcsSeparatorStr + nSeparatorLen;
    }

    if (m_nArrayLen == m_nMaxArrayLen) {
      m_nMaxArrayLen += 10;
      CString** ppNewArray = new CString* [m_nMaxArrayLen];
      for (int i = 0; i < m_nArrayLen; i++)
        ppNewArray[i] = m_ppArray[i];
      delete[] m_ppArray;
      m_ppArray = ppNewArray;
    }
    m_ppArray[m_nArrayLen++] = pNewStr;
  }  // while
}  // split
