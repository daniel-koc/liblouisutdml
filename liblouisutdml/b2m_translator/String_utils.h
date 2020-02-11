// CString_.h: interface for the CString_ class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(STRING_UTILS_H_)
#define STRING_UTILS_H_

#include "config.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*typedef enum {
false,
true
} bool;
*/

class CStringArray;

class CString
{
public:
	CString();
	CString(wchar_t* pszStr, size_t nStrLen);
	CString(size_t nStrBufLen);
	CString(const wchar_t* pszStr);
	CString(wchar_t* pszStr);
	CString(const CString& str);
	virtual ~CString();

	CString& operator=(const wchar_t* pszStr);
	CString& operator=(wchar_t* pszStr);
	CString& operator=(const CString& str);

	int length() const
	{
		return (int) m_nStrLen;
	}

	bool isEmpty() const
	{
		return (m_nStrLen == 0);
	}

wchar_t* c_str() {
	return m_pszStr;
}

	operator const wchar_t*() const
	{
		return m_pszStr;
}

	operator wchar_t*() const
	{
		return m_pszStr;
	}

	int operator==(const wchar_t* pszStr) const
	{
		return (wcscmp(m_pszStr, pszStr) == 0);
	}

	int operator==(wchar_t* pszStr) const
	{
		return (wcscmp(m_pszStr, pszStr) == 0);
	}

	int operator==(CString& str) const
	{
		return (wcscmp(m_pszStr, (const wchar_t*) str) == 0);
	}

	int operator<(const wchar_t* pszStr) const
	{
		return (wcscmp(m_pszStr, pszStr) < 0);
	}

	int operator<(wchar_t* pszStr) const
	{
		return (wcscmp(m_pszStr, pszStr) < 0);
	}

	int operator<(CString& str) const
	{
		return (wcscmp(m_pszStr, (const wchar_t*) str) < 0);
	}

	int operator>(const wchar_t* pszStr) const
	{
		return (wcscmp(m_pszStr, pszStr) > 0);
	}

	int operator>(wchar_t* pszStr) const
	{
		return (wcscmp(m_pszStr, pszStr) > 0);
	}

	int operator>(CString& str) const
	{
		return (wcscmp(m_pszStr, (const wchar_t*) str) > 0);
	}

	int operator<=(const wchar_t* pszStr) const
	{
		return (wcscmp(m_pszStr, pszStr) <= 0);
	}

	int operator<=(wchar_t* pszStr) const
	{
		return (wcscmp(m_pszStr, pszStr) <= 0);
	}

	int operator<=(CString& str) const
	{
		return (wcscmp(m_pszStr, (const wchar_t*) str) <= 0);
	}

	int operator>=(const wchar_t* pszStr) const
	{
		return (wcscmp(m_pszStr, pszStr) >= 0);
	}

	int operator>=(wchar_t* pszStr) const
	{
		return (wcscmp(m_pszStr, pszStr) >= 0);
	}

	int operator>=(CString& str) const
	{
		return (wcscmp(m_pszStr, (const wchar_t*) str) >= 0);
	}

	bool equals(const wchar_t* pszStr) const
	{
		return (wcscmp(m_pszStr, pszStr) == 0);
	}

	bool equals(wchar_t* pszStr) const
	{
		return (wcscmp(m_pszStr, pszStr) == 0);
	}

	bool equals(CString& str) const
	{
		return (wcscmp(m_pszStr, (const wchar_t*) str) == 0);
	}

	wchar_t& operator[](int nIndex)
	{
		return m_pszStr[((nIndex < 0) ? (0) : (((size_t) nIndex >= m_nStrLen) ? (m_nStrLen-1) : (nIndex)))];
	}

	wchar_t charAt(int nIndex) const
	{
		return m_pszStr[((nIndex < 0) ? (0) : (((size_t) nIndex >= m_nStrLen) ? (m_nStrLen-1) : (nIndex)))];
	}

	wchar_t getAt(int nIndex) const
	{
		return m_pszStr[((nIndex < 0) ? (0) : (((size_t) nIndex >= m_nStrLen) ? (m_nStrLen-1) : (nIndex)))];
	}

	friend CString operator+(const wchar_t* pszStr, const CString& str);
	friend CString operator+(const CString& str, const wchar_t* pszStr);

	CString operator+(const wchar_t* pszStr) const;
	CString operator+(wchar_t* pszStr) const;
	CString operator+(const CString& str) const;
	CString operator+(CString& str) const;
	CString operator+(CString* pStr) const;
	CString operator+(signed char nCount) const;
	CString operator+(unsigned char nCount) const;
	CString operator+(wchar_t wcChar) const;
	CString operator+(signed short int nCount) const;
	CString operator+(unsigned short int nCount) const;
	CString operator+(signed int nCount) const;
	CString operator+(unsigned int nCount) const;
	CString operator+(signed long int nCount) const;
	CString operator+(unsigned long int nCount) const;
	CString operator+(float nFloat) const;
	CString operator+(double nDouble) const;
	CString operator+(long double nDouble) const;

	void operator+=(const wchar_t* pszStr);
	void operator+=(wchar_t* pszStr);
	void operator+=(CString str);
	void operator+=(CString* pStr);
	void operator+=(signed char nCount);
	void operator+=(unsigned char nCount);
	void operator+=(wchar_t wcChar);
	void operator+=(signed short int nCount);
	void operator+=(unsigned short int nCount);
	void operator+=(signed int nCount);
	void operator+=(unsigned int nCount);
	void operator+=(signed long int nCount);
	void operator+=(unsigned long int nCount);
	void operator+=(float nFloat);
	void operator+=(double nDouble);
	void operator+=(long double nDouble);

	CString substring(size_t nStartIndex, size_t nEndIndex);

void remove()
{
		this->m_pszStr = NULL;
		this->m_nStrBufLen = this->m_nStrLen = 0;
}

	void clear()
	{
		if (this->m_pszStr != NULL)
			*this->m_pszStr = NULL;
		this->m_nStrLen = 0;
	}  // clear

CStringArray* split(const wchar_t* wcsSeparator);

private:
friend class CStringArray;

	wchar_t* m_pszStr;
	size_t m_nStrLen;
	size_t m_nStrBufLen;
wchar_t* m_pszCountBuf;
};  // CString

class CStringArray
{
public:
CStringArray(CString* str);
virtual ~CStringArray();

void split(const wchar_t* wcsSeparator);

int length()
{ return m_nArrayLen; }

CString* at(int nIndex) {
if (nIndex < 0 || nIndex >= m_nArrayLen)
return NULL;
return ((m_ppArray != NULL) ? m_ppArray[nIndex] : NULL);
}

private:
CString* m_pStr;
CString** m_ppArray;
int m_nArrayLen;
int m_nMaxArrayLen;
};  // CStringArray

#endif // !defined(STRING_UTILS_H_)
