#include "config.h"
#include <stdio.h>
#include <stdlib.h>

#include "louisutdml.h"
#include "louisutdml_backtranslation.h"

size_t widelen(const widechar *s) {
  size_t len = 0;
  while (s[len] != 0)
  {
      if (s[++len] == 0)
        return len;
      if (s[++len] == 0)
        return len;
      if (s[++len] == 0)
        return len;
      ++len;
    }
  return len;
}

int widecmp(const widechar *s1, const widechar *s2)
{
  widechar c1, c2;
  do
    {
      c1 = *s1++;
      c2 = *s2++;
      if (c2 == 0)
        return c1 - c2;
    }
  while (c1 == c2);
  return c1 < c2 ? -1 : 1;
}


#define WIDESTRING_BUFFER_SIZE 8 * BUFSIZE
static widechar widestring_buffer[WIDESTRING_BUFFER_SIZE];
static int widestring_buf_len = 0;

static widechar* alloc_widestring(const widechar* inString, int length) {
  widechar* newString;
  int inStringLen;
  if (inString == NULL)
    return NULL;
  if ((widestring_buf_len + length) >= WIDESTRING_BUFFER_SIZE)
    memoryError();
  newString = &widestring_buffer[widestring_buf_len];
  inStringLen = (int)wcslen(inString);
  if (length < inStringLen)
    inStringLen = length;
  memcpy(newString, inString, inStringLen*CHARSIZE);
  newString[inStringLen] = 0;
  widestring_buf_len += length + 1;
  return newString;
}


typedef enum {
  DOM_ELEMENT_NODE = 1,
  DOM_ATTRIBUTE_NODE = 2,
  DOM_TEXT_NODE = 3,
  DOM_DOCUMENT_FRAGMENT = 4
} DOMNodeType;

struct DOMDocument_struct;

typedef struct DOMNode_struct
{
  struct DOMDocument_struct* m_pOwnerDocument;
  DOMNodeType m_nNodeType;
  struct DOMNode_struct* m_pParentNode;
  struct DOMNode_struct* m_pPreviousNodeSibling;
  struct DOMNode_struct* m_pNextNodeSibling;
  struct DOMNode_struct* m_pFirstNode;
  struct DOMNode_struct* m_pLastNode;
  int m_bUnderOverType;
  // DOM_ELEMENT_NODE:
  widechar* m_pStrTagName;
  struct DOMNode_struct* m_pFirstAttr;
  struct DOMNode_struct* m_pLastAttr;
  // DOM_ATTRIBUTE_NODE:
  widechar* m_pStrName;
  widechar* m_pStrValue;
} DOMNode;

typedef DOMNode* DOMNodePtr;

typedef struct DOMDocument_struct
{
  DOMNode* m_pDocumentElement;
} DOMDocument;

DOMNode* DOM_CreateNode(DOMDocument* pOwnerDocument, DOMNodeType nNodeType) {
DOMNode* node;
  if (!(node = malloc(sizeof(DOMNode))))
    memoryError();
  node->m_pOwnerDocument = pOwnerDocument;
  node->m_nNodeType = nNodeType;
  node->m_pParentNode = NULL;
  node->m_pPreviousNodeSibling = NULL;
  node->m_pNextNodeSibling = NULL;
  node->m_pFirstNode = NULL;
  node->m_pLastNode = NULL;
  node->m_bUnderOverType = 0;
  node->m_pStrTagName = NULL;
  node->m_pFirstAttr = NULL;
  node->m_pLastAttr = NULL;
  node->m_pStrName = NULL;
  node->m_pStrValue = NULL;
  return node;
}  // DOM_CreateDOMNode

DOMNode* DOM_CreateElement(DOMDocument* pOwnerDocument, const widechar* pStrTagName) {
  DOMNode* node = DOM_CreateNode(pOwnerDocument, DOM_ELEMENT_NODE);
int nLen = 0;
if (pStrTagName) {
int nLen = wcslen(pStrTagName);
  if (!(node->m_pStrTagName = malloc((nLen+1)*CHARSIZE)))
memoryError();
memcpy(node->m_pStrTagName, pStrTagName, (nLen+1)*CHARSIZE);
}
  return node;
}  // DOM_CreateElement

DOMNode* DOM_CreateAttribute(DOMDocument* pOwnerDocument, const widechar* pStrName, const widechar* pStrValue) {
DOMNode* node = DOM_CreateNode(pOwnerDocument, DOM_ATTRIBUTE_NODE);
if (pStrName) {
int nLen = wcslen(pStrName);
  if (!(node->m_pStrName = malloc((nLen+1)*CHARSIZE)))
memoryError();
memcpy(node->m_pStrName, pStrName, (nLen+1)*CHARSIZE);
}
if (pStrValue) {
int nLen = wcslen(pStrValue);
  if (!(node->m_pStrValue = malloc((nLen+1)*CHARSIZE)))
memoryError();
memcpy(node->m_pStrValue, pStrValue, (nLen+1)*CHARSIZE);
}
  return node;
}  // DOM_CreateAttribute

DOMNode* DOM_CreateTextNode(DOMDocument* pOwnerDocument, widechar* pStrValue) {
DOMNode* node = DOM_CreateNode(pOwnerDocument, DOM_TEXT_NODE);
if (pStrValue) {
int nLen = wcslen(pStrValue);
  if (!(node->m_pStrValue = malloc((nLen+1)*CHARSIZE)))
memoryError();
memcpy(node->m_pStrValue, pStrValue, (nLen+1)*CHARSIZE);
}
  return node;
}  // DOM_CreateText

DOMNode* DOM_CreateDocumentFragment(DOMDocument* pOwnerDocument) {
DOMNode* node = DOM_CreateNode(pOwnerDocument, DOM_DOCUMENT_FRAGMENT);
  return node;
}  // DOM_CreateDocumentFragment

void DOM_DestroyNode(DOMNode* node) {
DOMNode* pFirstNode = node->m_pFirstNode;
  while (pFirstNode) {
    node->m_pLastNode = pFirstNode;
    pFirstNode = pFirstNode->m_pNextNodeSibling;
    DOM_DestroyNode(node->m_pLastNode);
  }  // while
if (node->m_pStrTagName)
free(node->m_pStrTagName);
DOMNode* pFirstAttr = node->m_pFirstAttr;
  while (pFirstAttr) {
    node->m_pLastAttr = pFirstAttr;
    pFirstAttr = pFirstAttr->m_pNextNodeSibling;
    DOM_DestroyNode(node->m_pLastAttr);
  }  // while
if (node->m_pStrName)
free(node->m_pStrName);
if (node->m_pStrValue)
free(node->m_pStrValue);
free(node);
}  // DOM_DestroyNode

DOMDocument* DOM_CreateDocument() {
DOMDocument* document;
  if (!(document = malloc(sizeof(DOMDocument))))
    memoryError();
  document->m_pDocumentElement = NULL;
return document;
}  // DOM_CreateDocument

DOMDocument* DOM_GetOwnerDocument(DOMNode* node) { return node->m_pOwnerDocument; }
DOMNodeType DOM_GetNodeType(DOMNode* node) { return node->m_nNodeType; }
DOMNode* DOM_GetParentNode(DOMNode* node) { return node->m_pParentNode; }
DOMNode* DOM_GetFirstNode(DOMNode* node) { return node->m_pFirstNode; }
DOMNode* DOM_GetLastNode(DOMNode* node) { return node->m_pLastNode; }
DOMNode* DOM_GetPreviousNodeSibling(DOMNode* node) { return node->m_pPreviousNodeSibling; }
DOMNode* DOM_GetNextNodeSibling(DOMNode* node) { return node->m_pNextNodeSibling; }
int DOM_HasChildNodes(DOMNode* node) { return (node->m_pFirstNode != NULL); }
int DOM_IsTagName(DOMNode* node, const widechar* name)
    { return (node->m_pStrTagName ? (wcscmp(node->m_pStrTagName, name) == 0) : 0); }
widechar* DOM_GetNodeValue(DOMNode* node) { return node->m_pStrValue; }
void DOM_SetUnderOverType(DOMNode* node) { node->m_bUnderOverType = 1; }
int DOM_IsUnderOverType(DOMNode* node) { return node->m_bUnderOverType; }
int DOM_HasAttributes(DOMNode* node) { return (node->m_pFirstAttr != NULL); }

void DOM_SetAttribute(DOMNode* node, const widechar* pStrName, const widechar* pStrValue) {
  DOMNode* pNewAttr = DOM_CreateAttribute(node->m_pOwnerDocument, pStrName, pStrValue);
  if (!node->m_pFirstAttr) {
    node->m_pFirstAttr = node->m_pLastAttr = pNewAttr;
  } else {
    node->m_pLastAttr->m_pNextNodeSibling = pNewAttr;
    pNewAttr->m_pPreviousNodeSibling = node->m_pLastAttr;
    node->m_pLastAttr = pNewAttr;
  }  // else if
}  // DOM_SetAttribute

DOMNode* DOM_AppendChild(DOMNode* node, DOMNode* pNewChild) {
  if (!node->m_pFirstNode) {
    node->m_pFirstNode = node->m_pLastNode = pNewChild;
  } else {
    node->m_pLastNode->m_pNextNodeSibling = pNewChild;
    pNewChild->m_pPreviousNodeSibling = node->m_pLastNode;
    node->m_pLastNode = pNewChild;
  }  // else if
  pNewChild->m_pParentNode = node;
  return pNewChild;
}  // DOM_AppendChild

DOMNode* DOM_RemoveChild(DOMNode* node, DOMNode* pOldChild) {
  if (pOldChild) {
    DOMNode* pTmpNode = node->m_pFirstNode;
    while (pTmpNode && pTmpNode != pOldChild)
      pTmpNode = pTmpNode->m_pNextNodeSibling;
    if (pTmpNode == pOldChild) {
      if (pOldChild->m_pPreviousNodeSibling)
        pOldChild->m_pPreviousNodeSibling->m_pNextNodeSibling = pOldChild->m_pNextNodeSibling;
      else
        node->m_pFirstNode = pOldChild->m_pNextNodeSibling;
      if (pOldChild->m_pNextNodeSibling)
        pOldChild->m_pNextNodeSibling->m_pPreviousNodeSibling = pOldChild->m_pPreviousNodeSibling;
      else
        node->m_pLastNode = pOldChild->m_pPreviousNodeSibling;
pOldChild->m_pParentNode = NULL;
      return pOldChild;
    }  // if
  }    // if
  return NULL;
}  // RemoveChild

void widecharcat(widechar** ppszStr, int* pnStrLen, int* pnMaxStrLen, widechar* pszNewStr, int nStrNewLen) {
  widechar* pszCatStr;
  int nCatStrLen;
  if (!pszNewStr)
return;
  nCatStrLen = *pnStrLen + nStrNewLen;
if (nCatStrLen > *pnMaxStrLen) {
*pnMaxStrLen = nCatStrLen+30;
  if (!(pszCatStr = malloc(((*pnMaxStrLen)+1)*CHARSIZE)))
    memoryError();
      if (*pnStrLen > 0 && ppszStr) {
        memcpy(pszCatStr, *ppszStr, (*pnStrLen)*CHARSIZE);
      free(*ppszStr);
}
    *ppszStr = pszCatStr;
}
    memcpy(&(*ppszStr)[*pnStrLen], pszNewStr, nStrNewLen*CHARSIZE);
(*ppszStr)[nCatStrLen] = 0;
    *pnStrLen = nCatStrLen;
}

static widechar wcsLineSeparator[1] = {'\n'};
static widechar wcsLessSign[1] = {'<'};
static widechar wcsGreaterSign[1] = {'>'};
static widechar wcsSpaceSign[1] = {' '};
static widechar wcsLessSlashSign[2] = {'<', '/'};
static widechar wcsEqualQuoteSign[2] = {'=', '\"'};
static widechar wcsQuoteSign[1] = {'\"'};

widechar* DOM_ToString(DOMNode* node, int* pnStrLen) {
  widechar* pszStr;
if (DOM_GetNodeType(node) == DOM_TEXT_NODE && node->m_pStrValue) {
  *pnStrLen = wcslen(node->m_pStrValue);
  if (!(pszStr = malloc((*pnStrLen+1)*CHARSIZE)))
    memoryError();
memcpy(pszStr, node->m_pStrValue, (*pnStrLen+1)*CHARSIZE);
return pszStr;
}

int nMaxStrLen = 50;
  if (!(pszStr = malloc((nMaxStrLen+1)*CHARSIZE)))
    memoryError();
  pszStr[0] = 0;
  *pnStrLen = 0;
if (DOM_GetNodeType(node) == DOM_TEXT_NODE)
return pszStr;
  widechar* pszNewStr;
  int nStrNewLen;

if (DOM_GetNodeType(node) == DOM_ELEMENT_NODE) {
widecharcat(&pszStr, pnStrLen, &nMaxStrLen, wcsLessSign, 1);
widecharcat(&pszStr, pnStrLen, &nMaxStrLen, node->m_pStrTagName, wcslen(node->m_pStrTagName));
  if (node->m_pFirstAttr) {
    DOMNode* pTmpAttr = node->m_pFirstAttr;
    while (pTmpAttr) {
      widecharcat(&pszStr, pnStrLen, &nMaxStrLen, wcsSpaceSign, 1);
    pszNewStr = DOM_ToString(pTmpAttr, &nStrNewLen);
widecharcat(&pszStr, pnStrLen, &nMaxStrLen, pszNewStr, nStrNewLen);
free(pszNewStr);
      pTmpAttr = DOM_GetNextNodeSibling(pTmpAttr);
    }  // while
  }    // if
  widecharcat(&pszStr, pnStrLen, &nMaxStrLen, wcsGreaterSign, 1);
} else
if (DOM_GetNodeType(node) == DOM_ATTRIBUTE_NODE) {
  if (node->m_pStrName) {
widecharcat(&pszStr, pnStrLen, &nMaxStrLen, node->m_pStrName, wcslen(node->m_pStrName));
widecharcat(&pszStr, pnStrLen, &nMaxStrLen, wcsEqualQuoteSign, 2);
    if (node->m_pStrValue)
widecharcat(&pszStr, pnStrLen, &nMaxStrLen, node->m_pStrValue, wcslen(node->m_pStrValue));
widecharcat(&pszStr, pnStrLen, &nMaxStrLen, wcsQuoteSign, 1);
}
}

  DOMNode* pTmpNode = node->m_pFirstNode;
  if (pTmpNode && DOM_GetNodeType(pTmpNode) == DOM_ELEMENT_NODE)
    widecharcat(&pszStr, pnStrLen, &nMaxStrLen, wcsLineSeparator, 1);
  while (pTmpNode) {
    pszNewStr = DOM_ToString(pTmpNode, &nStrNewLen);
widecharcat(&pszStr, pnStrLen, &nMaxStrLen, pszNewStr, nStrNewLen);
free(pszNewStr);
    pTmpNode = DOM_GetNextNodeSibling(pTmpNode);
  }  // while

if (DOM_GetNodeType(node) == DOM_ELEMENT_NODE) {
widecharcat(&pszStr, pnStrLen, &nMaxStrLen, wcsLessSlashSign, 2);
widecharcat(&pszStr, pnStrLen, &nMaxStrLen, node->m_pStrTagName, wcslen(node->m_pStrTagName));
widecharcat(&pszStr, pnStrLen, &nMaxStrLen, wcsGreaterSign, 1);
widecharcat(&pszStr, pnStrLen, &nMaxStrLen, wcsLineSeparator, 1);
}

  return pszStr;
}  // ToString

typedef struct
{
  DOMNodePtr* m_ppItems;
  int m_nMaxItemsCount;
  int m_nItemsCount;
} DOMNodeList;

const int ITEMS_RESIZING_STEP = 20;

DOMNodeList* DOM_GetChildNodes(DOMNode* node) {
DOMNodeList* nodeList;
  if (!(nodeList = malloc(sizeof(DOMNodeList))))
    memoryError();
  nodeList->m_nMaxItemsCount = ITEMS_RESIZING_STEP;
  if (!(nodeList->m_ppItems = malloc(nodeList->m_nMaxItemsCount*sizeof(DOMNodePtr))))
    memoryError();
  memset(nodeList->m_ppItems, nodeList->m_nMaxItemsCount*sizeof(DOMNodePtr), 0);
  nodeList->m_nItemsCount = 0;

DOMNodePtr* ppItems;
DOMNode* pFirstNode = node->m_pFirstNode;
  while (pFirstNode) {
    if (nodeList->m_nItemsCount ==nodeList->m_nMaxItemsCount) {
      nodeList->m_nMaxItemsCount += ITEMS_RESIZING_STEP;
  if (!(ppItems = malloc(nodeList->m_nMaxItemsCount*sizeof(DOMNodePtr))))
    memoryError();
  memset(ppItems, nodeList->m_nMaxItemsCount*sizeof(DOMNodePtr), 0);
      memcpy(ppItems, nodeList->m_ppItems, nodeList->m_nItemsCount*sizeof(DOMNodePtr));
      free(nodeList->m_ppItems);
      nodeList->m_ppItems = ppItems;
    }
    nodeList->m_ppItems[nodeList->m_nItemsCount++] = pFirstNode;
    pFirstNode = pFirstNode->m_pNextNodeSibling;
  }  // while
return nodeList;
}  // DOM_GetChildNodes

void DOM_DestroyNodeList(DOMNodeList* nodeList) {
free(nodeList->m_ppItems);
free(nodeList);
}

int DOM_GetLengthOfList(DOMNodeList* nodeList) { return nodeList->m_nItemsCount; }

DOMNode* DOM_GetItemOfList(DOMNodeList* nodeList, int nIndex) {
  if (nIndex < 0 || nIndex >= nodeList->m_nItemsCount)
    return NULL;
  return nodeList->m_ppItems[nIndex];
}


static widechar g_wcDecimalSign = L',';
static widechar* g_mathExprSeparator = L"_.";
static widechar* g_mathColor =
    L"blue";  // change it to "" (to inherit) or another color
static widechar* g_mathFontSize =
    L"1em";  // change to e.g. 1.2em for larger math
static widechar* g_mathFontFamily = L"mathvariant";  // change to "" to inherit
                                                    // (works in IE) or another
                                                    // family (e.g. "arial")
static int g_displayStyle =
    1;  // puts limits above and below large operators
static int g_showAsciiFormulaOnHover = 1;
static int g_mrowElementsAvoiding = 1;

const widechar* BOX_MISSING_ARGUMENT = L"\u25A1";

int isIE() {
  return 0;
}

const widechar wcNULL = L'\0';

const widechar* MATH_ELNAME = L"math";
const widechar* MFENCED_ELNAME = L"mfenced";
const widechar* MI_ELNAME = L"mi";
const widechar* MN_ELNAME = L"mn";
const widechar* MO_ELNAME = L"mo";
const widechar* MOVER_ELNAME = L"mover";
const widechar* MROW_ELNAME = L"mrow";
const widechar* MSPACE_ELNAME = L"mspace";
const widechar* MSTYLE_ELNAME = L"mstyle";
const widechar* MSUB_ELNAME = L"msub";
const widechar* MSUBSUP_ELNAME = L"msubsup";
const widechar* MSUP_ELNAME = L"msup";
const widechar* MTABLE_ELNAME = L"mtable";
const widechar* MTD_ELNAME = L"mtd";
const widechar* MTR_ELNAME = L"mtr";
const widechar* MUNDER_ELNAME = L"munder";
const widechar* MUNDEROVER_ELNAME = L"munderover";
const widechar* SPAN_ELNAME = L"span";

const widechar* CLASS_ATTRNAME = L"class";
const widechar* CLOSE_ATTRNAME = L"close";
const widechar* COLUMNALIGN_ATTRNAME = L"columnalign";
const widechar* COLUMNSPACING_ATTRNAME = L"columnspacing";
const widechar* DISPLAYSTYLE_ATTRNAME = L"displaystyle";
const widechar* FONTFAMILY_ATTRNAME = L"fontfamily";
const widechar* HEIGHT_ATTRNAME = L"height";
const widechar* LINETHICKNESS_ATTRNAME = L"linethickness";
const widechar* LSPACE_ATTRNAME = L"lspace";
const widechar* MATHCOLOR_ATTRNAME = L"mathcolor";
const widechar* MATHSIZE_ATTRNAME = L"mathsize";
const widechar* MAXSIZE_ATTRNAME = L"maxsize";
const widechar* MINSIZE_ATTRNAME = L"minsize";
const widechar* OPEN_ATTRNAME = L"open";
const widechar* RSPACE_ATTRNAME = L"rspace";
const widechar* TITLE_ATTRNAME = L"title";
const widechar* WIDTH_ATTRNAME = L"width";
const widechar* XMLNS_ATTRNAME = L"xmlns";

const widechar* CENTER_ALIGN_ATTRVALUE = L"center ";
const widechar* FALSE_ATTRVALUE = L"false";
const widechar* LEFT_ALIGN_ATTRVALUE = L"left ";
const widechar* MATHML_URL_ATTRVALUE = L"http://www.w3.org/1998/Math/MathML";
const widechar* RIGHT_ALIGN_ATTRVALUE = L"right ";
const widechar* RIGHT_CENTER_LEFT_ATTRVALUE = L"right center left";
const widechar* TRUE_ATTRVALUE = L"true";
const widechar* UNIT_ATTRVALUE = L"unit";

typedef enum {
LEFT_ALIGN,
CENTER_ALIGN,
RIGHT_ALIGN
} AlignType;

typedef struct  {
  DOMNode* node;
  widechar* str;
  widechar* tag;
} ParseResult;

ParseResult* CreateParseResult(DOMNode* n, widechar* s, widechar* t) {
  ParseResult* result;
  if (!(result = malloc(sizeof(ParseResult))))
    memoryError();
  result->node = n;
  result->str = s;
  result->tag = t;
  return result;
}

typedef void* LPVOID;

static DOMDocument* g_document = NULL;
static DOMNode* g_operatorsFrag = NULL;
static int g_gatheringOperators = 0;
static int g_gatheringLetters = 0;

TokenType LMpreviousSymbol, LMcurrentSymbol;

int isDigit(widechar wc) {
  return (L'0' <= wc && wc <= L'9');
}

int isLetter(widechar wc) {
  static const widechar* s_letters =
      L"abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZąćęłńóśźżĄĆĘŁŃÓŚŹŻ";
  return (wcschr(s_letters, wc) != NULL);
}

int CompareLMSymbolInput(LPVOID plmSymbol1, LPVOID plmSymbol2) {
  return wcscmp(((LMSymbol*)plmSymbol1)->input, ((LMSymbol*)plmSymbol2)->input);
}  // CompareLMSymbolInput

typedef int (*CompareItemsFunc)(LPVOID p1, LPVOID p2);
static CompareItemsFunc CompareItems = CompareLMSymbolInput;

#define swap(index1, index2) \
  \
{                       \
    t = x[index1];           \
    x[index1] = x[index2];   \
    x[index2] = t;           \
  }

#define vecswap(k, l, n)              \
  e = k;                              \
  f = l;                              \
  for (i = 0; i < (n); i++, e++, f++) \
    swap(e, f);

#define min_value(a, b) (((a) <= (b)) ? (a) : (b))

int med3Sort(LPVOID* x, int a, int b, int c) {
  return (CompareItems(x[a], x[b]) < 0
              ? (CompareItems(x[b], x[c]) < 0 ? b : CompareItems(x[a], x[c]) < 0
                                                        ? c
                                                        : a)
              : (CompareItems(x[b], x[c]) > 0 ? b : CompareItems(x[a], x[c]) > 0
                                                        ? c
                                                        : a));
}

void QuickSort(LPVOID* x, int off, int len) {
  if (CompareItems == NULL)
    return;

  LPVOID t;
  register int i;
  // Insertion sort on smallest arrays
  if (len < 7) {
    register int j;
    for (i = off; i < len + off; i++)
      for (j = i; j > off && CompareItems(x[j - 1], x[j]) > 0; j--)
        swap(j, j - 1);
    return;
  }

  // Choose a partition element, v
  int m = off + (len >> 1);  // Small arrays, middle element
  if (len > 7) {
    register int l = off;
    register int n = off + len - 1;
    if (len > 40) {  // Big arrays, pseudomedian of 9
      register int s = len / 8;
      l = med3Sort(x, l, l + s, l + 2 * s);
      m = med3Sort(x, m - s, m, m + s);
      n = med3Sort(x, n - 2 * s, n - s, n);
    }
    m = med3Sort(x, l, m, n);  // Mid-size, med of 3
  }
  LPVOID v = x[m];

  // Establish Invariant: v* (<v)* (>v)* v*
  register int a = off;
  register int b = a;
  register int c = off + len - 1;
  register int d = c;
  register int e, f, cmp;
  while (1) {
    while (b <= c && (cmp = CompareItems(x[b], v)) <= 0) {
      if (cmp == 0) {
        swap(a, b);
        a++;
      }
      b++;
    }
    while (c >= b && (cmp = CompareItems(x[c], v)) >= 0) {
      if (cmp == 0) {
        swap(c, d);
        d--;
      }
      c--;
    }
    if (b > c)
      break;
    swap(b, c);
    b++;
    c--;
  }

  // Swap partition elements back to middle
  register int s, n = off + len;
  s = min_value(a - off, b - a);
  vecswap(off, b - s, s);
  s = min_value(d - c, n - d - 1);
  vecswap(b, n - s, s);

  // Recursively sort non-partition-elements
  if ((s = b - a) > 1)
    QuickSort(x, off, s);
  if ((s = d - c) > 1)
    QuickSort(x, n - s, s);
}  // QuickSort

int isAmpersant(widechar* s) {
  if (!s) return 0;
  return (wcscmp(s, L"&") == 0 || wcscmp(s, L"&amp;") == 0);
}

DOMNode* createXhtmlElement(const widechar* tagName) {
  return DOM_CreateElement(g_document, tagName);
}

DOMNode* createMmlNode(const widechar* tagName, DOMNode* frag) {
  DOMNode* node = DOM_CreateElement(g_document, tagName);
  //@  if (isIE) var node = document.createElement("m:"+t);
  //@  else var node =
  //document.createElementNS("http://www.w3.org/1998/Math/MathML",t);
  if (frag)
    DOM_AppendChild(node, frag);
  return node;
}

DOMNode* createMissingArgumentMmlNode() {
  return createMmlNode(MO_ELNAME, DOM_CreateTextNode(g_document, (widechar*)BOX_MISSING_ARGUMENT));
}

widechar* LMremoveCharsAndBlanks(widechar* str, int n) {
  // remove n characters and any following blanks
  widechar* st = str + n;
  int i = 0;
  for (; st[i] != wcNULL && (st[i] <= 32 || st[i] == 0xa0); i++)
    ;
  return (st + i);
}

int positionInLMsymbols(const widechar* str, int n) {
  // return position >=n where str appears or would be inserted
  // assumes LMSymbols is sorted
  int i = 0;
  if (n == 0) {
    int h, m;
    n = -1;
    h = LMsymbolsCount;
    while (n + 1 < h) {
      m = (n + h) >> 1;
      if (wcscmp(LM_SYMBOL_NAME(m), str) < 0)
        n = m;
      else
        h = m;
    }
    return h;
  } else
    for (i = n; i < LMsymbolsCount && wcscmp(LM_SYMBOL_NAME(i), str) < 0; i++)
      ;
  return i;  // i=LMsymbolsCount || LM_SYMBOL_NAME(i) >= str
}  // positionInLMsymbols

LMSymbol* LMgetSymbol(const widechar* str) {
  // return maximal initial substring of str that appears in names
  // return NULL if there is none
  int k = 0;  // new pos
  int j = 0;  // old pos
  int mk;  // match pos
  const int MAX_ST_LEN = 200;
  widechar st[201];
  widechar* tagst;
  widechar* match = NULL;
  int more = 1;
  int strLen, symbolNameLen, i;

  if (str == NULL || *str == wcNULL)
    return NULL;

  strLen = (int)wcslen(str);
  if (strLen > MAX_ST_LEN)
    strLen = MAX_ST_LEN;
  for (i = 1; i <= strLen && more; i++) {
    wcsncpy(st, str, i);  // initial substring of length i
    st[i] = wcNULL;
    j = k;
    k = positionInLMsymbols(st, j);
    if (k < LMsymbolsCount) {
      symbolNameLen = wcslen(LM_SYMBOL_NAME(k));
      wcsncpy(st, str, symbolNameLen);
      st[symbolNameLen] = wcNULL;
      if (wcscmp(st, LM_SYMBOL_NAME(k)) == 0) {
        match = LM_SYMBOL_NAME(k);
        mk = k;
        i = symbolNameLen;
      }
      more = wcscmp(st, LM_SYMBOL_NAME(k)) >= 0;
    } else
      more = 0;
  }
  LMpreviousSymbol = LMcurrentSymbol;
  if (match != NULL) {
    LMcurrentSymbol = LMsymbols[mk]->ttype;
    return LMsymbols[mk];
  }
  // if str[0] is a digit or - return maxsubstring of digits.
  LMcurrentSymbol = TTYPE_CONST;

  k = 1;
  widechar wc = str[0];
  if (isDigit(wc)) {
    do {
      wc = str[k];
      k++;
    } while (isDigit(wc) && k <= strLen);
    while (wc == g_wcDecimalSign) {
      wc = str[k];
      if (isDigit(wc)) {
        k++;
        do {
          wc = str[k];
          k++;
        } while (isDigit(wc) && k <= strLen);
      }  // if
    }    // while (wc == g_wcDecimalSign)
    k--;
    tagst = (widechar*)MN_ELNAME;
  }  // if (isDigit(wc))
  else if (isLetter(wc)) {
    if (g_gatheringLetters) {
      do {
        wc = str[k];
        k++;
      } while (isLetter(wc) && k <= strLen);
      k--;
    }
    tagst = (widechar*)MI_ELNAME;
  }  // if (isLetter(wc))
  else {
    if (wc == L'&') {
      wc = str[k];
      if (L'a' <= wc && wc <= L'z') {
        do {
          k++;
          wc = str[k];
        } while (L'a' <= wc && wc <= L'z');
        if (wc == L';')
          k++;
        else
          k = 1;
      }  // if (L'a' <= wc && wc <= L'z')
    }    // if (wc == L'&')
    tagst = (widechar*)MO_ELNAME;
  }  // else if

  // Commented out by DRW (not fully understood, but probably to do with
  // use of "/" as an INFIX version of "\\frac", which we don't want):
  // if (st=="-" && LMpreviousSymbol==INFIX) {
  //  LMcurrentSymbol = INFIX;  //trick "/" into recognizing "-" on second parse
  //  return {input:st, tag:tagst, output:st, ttype:UNARY, func:true};
  //}

  widechar* inout = alloc_widestring(str, k);

  return CreateTempLMSymbol(inout, tagst, inout, TTYPE_CONST);
}  // LMgetSymbol

ParseResult* LMparseExpr(widechar* str, int rightbracket, int matrix);

// Parsing ASCII math expressions with the following grammar
// v ::= [A-Za-z] | greek letters | numbers | other constant symbols
// u ::= sqrt | text | bb | other unary symbols for font commands
// b ::= frac | root | stackrel	binary symbols
// l ::= { | \left			left brackets
// r ::= } | \right		right brackets
// S ::= v | lEr | uS | bSS	Simple expression
// I ::= S_S | S^S | S_S^S | S	Intermediate expression
// E ::= IE | I/I			Expression
// Each terminal symbol is translated into a corresponding mathml node.

ParseResult* LMbuildUnaryAccent(widechar* str,
                               LMSymbol* symbol,
                               ParseResult* result) {
  DOMNode* node = createMmlNode(symbol->tag, result->node);
  widechar* output = symbol->output;
  /*
          if (isIE) {
                  if (symbol.input == "\\hat")
                          output = "\u0302";
                  else if (symbol.input == "\\widehat")
                          output = "\u005E";
                  else if (symbol.input == "\\bar")
                          output = "\u00AF";
                  else if (symbol.input == "\\grave")
                          output = "\u0300";
                  else if (symbol.input == "\\tilde")
                          output = "\u0303";
          }
  */
  DOMNode* node1 =
      createMmlNode(MO_ELNAME, DOM_CreateTextNode(g_document, output));
  if (wcscmp(symbol->input, L"\\vec") == 0 ||
      wcscmp(symbol->input, L"\\check") == 0)
    // don't allow to stretch
    DOM_SetAttribute(node1, MAXSIZE_ATTRNAME, L"1.2");
  // why doesn't "1" work?  \vec nearly disappears in firefox
  if (isIE() && wcscmp(symbol->input, L"\\bar") == 0)
    DOM_SetAttribute(node1, MAXSIZE_ATTRNAME, L"0.5");
  if (wcscmp(symbol->input, L"\\underbrace") == 0 ||
      wcscmp(symbol->input, L"\\underline") == 0)
    DOM_SetAttribute(node1, L"accentunder", L"true");
  else
    DOM_SetAttribute(node1, L"accent", L"true");
  DOM_AppendChild(node, node1);
  if (wcscmp(symbol->input, L"\\overbrace") == 0 ||
      wcscmp(symbol->input, L"\\underbrace") == 0)
    DOM_SetUnderOverType(node);

  return CreateParseResult(node, str, symbol->tag);
}  // LMbuildUnaryAccent

ParseResult* LMparseSexpr(
    widechar* str) {  // parses str and returns [node,tailstr,(node)tag]
  LMSymbol* symbol;
  DOMNode* node;
  ParseResult* result;
  ParseResult* result2;
  widechar* atval;
  int i;
  widechar* st;
  widechar* output;
  widechar atvalStr[100];

  result = CreateParseResult(NULL, NULL, NULL);
  result2 = CreateParseResult(NULL, NULL, NULL);

  DOMNode* newFrag = DOM_CreateDocumentFragment(g_document);
  str = LMremoveCharsAndBlanks(str, 0);
  symbol = LMgetSymbol(str);  // either a token or a bracket or empty
  if (symbol == NULL || symbol->ttype == TTYPE_RIGHTBRACKET)
    return CreateParseResult(NULL, str, NULL);
  if (symbol->ttype == TTYPE_DEFINITION) {
    int symbolInputLen = (int)wcslen(symbol->input);
    st = alloc_widestring(symbol->output, (wcslen(symbol->output) +
                                           wcslen(str) - symbolInputLen));
    wcscat(st, LMremoveCharsAndBlanks(str, symbolInputLen));
    str = st;
    symbol = LMgetSymbol(str);
    if (symbol == NULL || symbol->ttype == TTYPE_RIGHTBRACKET)
      return CreateParseResult(NULL, str, NULL);
  }
  str = LMremoveCharsAndBlanks(str, wcslen(symbol->input));

  switch (symbol->ttype) {
    case TTYPE_SPACE:
      node = createMmlNode(symbol->tag, NULL);
      DOM_SetAttribute(node, symbol->atname, symbol->atval);
      return CreateParseResult(node, str, symbol->tag);
    case TTYPE_UNDEROVER:
      if (isIE()) {
        if (symbol->rinput != NULL) {  // botch for missing symbols
          st = alloc_widestring(symbol->rinput,
                                (wcslen(symbol->rinput) + wcslen(str)));
          wcscat(st, str);
          str = st;
          symbol = LMgetSymbol(str);
          if (symbol == NULL || symbol->ttype == TTYPE_RIGHTBRACKET)
            return CreateParseResult(NULL, str, NULL);
          symbol->ttype = TTYPE_UNDEROVER;
          str = LMremoveCharsAndBlanks(str, wcslen(symbol->input));
        }
      }
      return CreateParseResult(createMmlNode(symbol->tag, DOM_CreateTextNode(g_document,
                                                        symbol->output)),
                         str, symbol->tag);
    case TTYPE_CONST:
    case TTYPE_OPERATOR:
      output = symbol->output;
      if (isIE() && symbol->ieoutput != NULL)
        output = symbol->ieoutput;
      if (output)
        node = createMmlNode(symbol->tag, DOM_CreateTextNode(g_document, output));
      else
        node = createMmlNode(symbol->tag, NULL);
      return CreateParseResult(node, str, symbol->tag);
    case TTYPE_LONG:  // added by DRW
      node = createMmlNode(symbol->tag,
                           DOM_CreateTextNode(g_document, symbol->output));
      DOM_SetAttribute(node, MINSIZE_ATTRNAME, L"1.5");
      DOM_SetAttribute(node, MAXSIZE_ATTRNAME, L"1.5");
      node = createMmlNode(MOVER_ELNAME, node);
      DOM_AppendChild(node, createMmlNode(MSPACE_ELNAME, NULL));
      return CreateParseResult(node, str, symbol->tag);
    case TTYPE_STRETCHY:  // added by DRW
      output = symbol->output;
      if (isIE() && symbol->ieoutput != NULL)
        symbol->output =
            symbol->ieoutput;  // doesn't expand, but then nor does "\u2216"
      node = createMmlNode(symbol->tag, DOM_CreateTextNode(g_document, output));
      if (symbol->tsubtype == TSUBTYPE_VERT) {
        DOM_SetAttribute(node, LSPACE_ATTRNAME, L"0em");
        DOM_SetAttribute(node, RSPACE_ATTRNAME, L"0em");
      }
      if (symbol->atval != NULL)
        DOM_SetAttribute(node, MAXSIZE_ATTRNAME,
                           symbol->atval);  // don't allow to stretch here
      if (symbol->rtag != NULL)
        return CreateParseResult(node, str, symbol->rtag);
      else
        return CreateParseResult(node, str, symbol->tag);
    case TTYPE_BIG:  // added by DRW
      atval = symbol->atval;
      symbol = LMgetSymbol(str);
      if (symbol == NULL)
        return CreateParseResult(NULL, str, NULL);
      str = LMremoveCharsAndBlanks(str, wcslen(symbol->input));
      node = createMmlNode(symbol->tag,
                           DOM_CreateTextNode(g_document, symbol->output));
      if (isIE()) {  // to get brackets to expand
        DOMNode* space = createMmlNode(MSPACE_ELNAME, NULL);
        wcscpy(atvalStr, atval);
        wcscat(atvalStr, L"ex");
        DOM_SetAttribute(space, HEIGHT_ATTRNAME, atvalStr);
        node = createMmlNode(MROW_ELNAME, node);
        DOM_AppendChild(node, space);
      } else {  // ignored in IE
        DOM_SetAttribute(node, MINSIZE_ATTRNAME, atval);
        DOM_SetAttribute(node, MAXSIZE_ATTRNAME, atval);
      }
      return CreateParseResult(node, str, symbol->tag);
    case TTYPE_LEFTBRACKET:  // read (expr+)
      if (symbol->tsubtype == TSUBTYPE_LEFT) {  // left what?
        symbol = LMgetSymbol(str);
        if (symbol != NULL) {
          if (symbol->tsubtype == TSUBTYPE_DOT)
            symbol->invisible = BOOL_TRUE;
          str = LMremoveCharsAndBlanks(str, wcslen(symbol->input));
        }
      }
      result = LMparseExpr(str, 1, 0);
      node = NULL;
      if (symbol == NULL || symbol->invisible == BOOL_TRUE) {
        while (DOM_GetNodeType(result->node) == DOM_DOCUMENT_FRAGMENT &&
               DOM_GetFirstNode(result->node) &&
               DOM_GetFirstNode(result->node) == DOM_GetLastNode(result->node)) {
          DOMNode* n = DOM_RemoveChild(result->node, DOM_GetFirstNode(result->node));
          DOM_DestroyNode(result->node);
          result->node = n;
        }
        if (!g_mrowElementsAvoiding ||
            DOM_GetNodeType(result->node) != DOM_ELEMENT_NODE) {
          if (!DOM_IsTagName(result->node, MROW_ELNAME))
            node = createMmlNode(MROW_ELNAME, result->node);
        }
      } else {
        node = createMmlNode(MO_ELNAME,
                             DOM_CreateTextNode(g_document, symbol->output));
        node = createMmlNode(MROW_ELNAME, node);
        DOM_AppendChild(node, result->node);
      }
      if (node)
        result->node = node;
      return result;
    case TTYPE_MATRIX:
      if (symbol->tsubtype == TSUBTYPE_ARRAY) {
        const int MAX_ALIGN_COUNT = 100;
        AlignType align[100];
        int alignCount = 0;
        int leftAlignCount = 0, centerAlignCount = 0, rightAlignCount = 0;
        int i;
        symbol = LMgetSymbol(str);
        str = LMremoveCharsAndBlanks(str,0);
        if (symbol == NULL) {
          align[alignCount++] = LEFT_ALIGN;
          leftAlignCount++;
        } else {
          str = LMremoveCharsAndBlanks(str, wcslen(symbol->input));
          if (symbol->ttype != TTYPE_LEFTBRACKET) {
            align[alignCount++] = LEFT_ALIGN;
            leftAlignCount++;
          } else
            do {
              symbol = LMgetSymbol(str);
              if (symbol != NULL) {
                str = LMremoveCharsAndBlanks(str, wcslen(symbol->input));
                if (symbol->ttype != TTYPE_RIGHTBRACKET)
                  for (i = 0; i < wcslen(symbol->input); i++)
                    switch (symbol->input[i]) {
                      case L'l':
                        align[alignCount++] = LEFT_ALIGN;
                        leftAlignCount++;
                        break;
                      case L'c':
                        align[alignCount++] = CENTER_ALIGN;
                        centerAlignCount++;
                        break;
                      case L'r':
                        align[alignCount++] = RIGHT_ALIGN;
                        rightAlignCount++;
                        break;
                      default: NULL;
                    }  // switch for
              }
            } while (symbol != NULL && symbol->input != NULL && symbol->ttype != TTYPE_RIGHTBRACKET);
        }

        widechar* columnAlign = alloc_widestring(L" ", leftAlignCount*wcslen(LEFT_ALIGN_ATTRVALUE) + centerAlignCount*wcslen(CENTER_ALIGN_ATTRVALUE) + rightAlignCount*wcslen(RIGHT_ALIGN_ATTRVALUE) + 1);
        for (i = 0; i < alignCount; i++)
          switch (align[i]) {
            case LEFT_ALIGN:
              wcscat(columnAlign, LEFT_ALIGN_ATTRVALUE);
              break;
            case CENTER_ALIGN:
              wcscat(columnAlign, CENTER_ALIGN_ATTRVALUE);
              break;
            case RIGHT_ALIGN:
              wcscat(columnAlign, RIGHT_ALIGN_ATTRVALUE);
          }  // switch for

        st = alloc_widestring(L"{", wcslen(str) + 1);
        wcscat(st, str);
        result = LMparseExpr(st, 1, 1);
        // if (result[0] == NULL)
        //   return [createMmlNode("mo", document.createTextNode(symbol.input)),str];
        node = createMmlNode(MTABLE_ELNAME, result->node);
        DOM_SetAttribute(node, COLUMNALIGN_ATTRNAME, columnAlign);
        DOM_SetAttribute(node, DISPLAYSTYLE_ATTRNAME, FALSE_ATTRVALUE);
        if (isIE())
          return CreateParseResult(node, result->str, NULL);
        // trying to get a *little* bit of space around the array
        // (IE already includes it)
        DOMNode* lspace = createMmlNode(MSPACE_ELNAME, NULL);
        DOM_SetAttribute(lspace, WIDTH_ATTRNAME, L"0.167em");
        DOMNode* rspace = createMmlNode(MSPACE_ELNAME, NULL);
        DOM_SetAttribute(rspace, WIDTH_ATTRNAME, L"0.167em");
        DOMNode* node1 = createMmlNode(MROW_ELNAME, lspace);
        DOM_AppendChild(node1, node);
        DOM_AppendChild(node1, rspace);
        return CreateParseResult(node1, result->str, NULL);
      } else {  // (symbol->tsubtype == TSUBTYPE_EQNARRAY)
        st = alloc_widestring(L"{", wcslen(str) + 1);
        wcscat(st, str);
        result = LMparseExpr(st, 1, 1);
        node = createMmlNode(MTABLE_ELNAME, result->node);
        if (isIE())
          DOM_SetAttribute(node, COLUMNSPACING_ATTRNAME, L"0.25em"); // best in practice?
        else
          DOM_SetAttribute(node, COLUMNSPACING_ATTRNAME, L"0.167em"); // correct (but ignored?)
        DOM_SetAttribute(node, COLUMNALIGN_ATTRNAME, RIGHT_CENTER_LEFT_ATTRVALUE);
        DOM_SetAttribute(node, DISPLAYSTYLE_ATTRNAME, TRUE_ATTRVALUE);
        node = createMmlNode(MROW_ELNAME, node);
        return CreateParseResult(node, result->str, NULL);
      }
/*
            case TTYPE_TEXT:
          if (str.charAt(0)=="{") i=str.indexOf("}");
          else i = 0;
          if (i==-1)
                     i = str.length;
          st = str.slice(1,i);
          if (st.charAt(0) == " ") {
            node = createMmlNode("mspace");
            node.setAttribute("width","0.33em");	// was 1ex
            newFrag.appendChild(node);
          }
          newFrag.appendChild(
            createMmlNode(symbol.tag,document.createTextNode(st)));
          if (st.charAt(st.length-1) == " ") {
            node = createMmlNode(MSPACE_ELNAME);
            node.setAttribute(WIDTH_ATTRNAME, L"0.33em");	// was 1ex
            newFrag.appendChild(node);
          }
          str = LMremoveCharsAndBlanks(str,i+1);
          return [createMmlNode("mrow",newFrag),str, NULL];
    */
    case TTYPE_UNARY:
      if (wcscmp(symbol->input, L"\\unitsymbol") == 0)  // unitsymbol
        g_gatheringLetters = 1;
      result = LMparseSexpr(str);
      g_gatheringLetters = 0;
      if (result->node == NULL) {
        return CreateParseResult(
            createMmlNode(symbol->tag,
                          DOM_CreateTextNode(g_document, symbol->output)),
            str, NULL);
      }
      if (symbol->func == BOOL_TRUE) {  // functions hack
        widechar st = str[0];
        //	if (st=="^" || st=="_" || st=="/" || st=="|" || st==",") {
        if (st == L'^' || st == L'_' || st == L',') {
          return CreateParseResult(
              createMmlNode(symbol->tag,
                            DOM_CreateTextNode(g_document, symbol->output)),
              str, symbol->tag);
        } else {
          node = createMmlNode(
              MROW_ELNAME,
              createMmlNode(symbol->tag,
                            DOM_CreateTextNode(g_document, symbol->output)));
          if (isIE()) {
            DOMNode* space = createMmlNode(MSPACE_ELNAME, NULL);
            DOM_SetAttribute(space, WIDTH_ATTRNAME, L"0.167em");
            DOM_AppendChild(node, space);
          }
          DOM_AppendChild(node, result->node);
          return CreateParseResult(node, result->str, symbol->tag);
        }
      }
      if (wcscmp(symbol->input, L"\\sqrt") == 0) {  // sqrt
        if (isIE()) {                               // set minsize, for \surd
          DOMNode* space = createMmlNode(MSPACE_ELNAME, NULL);
          DOM_SetAttribute(space, HEIGHT_ATTRNAME, L"1.2ex");
          DOM_SetAttribute(space, WIDTH_ATTRNAME, L"0em");  // probably no effect
          node = createMmlNode(symbol->tag, result->node);
          //	  node.setAttribute(MINSIZE_ATTRNAME, L"1");	// ignored
          //	  node = createMmlNode(MROW_ELNAME, node);  // hopefully
          //unnecessary
          DOM_AppendChild(node, space);
          result->node = node;
          result->tag = symbol->tag;
          return result;
        } else {
          result->node = createMmlNode(symbol->tag, result->node);
          result->tag = symbol->tag;
          return result;
        }
      } if (wcscmp(symbol->input, L"\\unitsymbol") == 0) {  // unitsymbol
        DOMNode* node = result->node;
        while (node) {
          if (DOM_GetNodeType(node) == DOM_ELEMENT_NODE)
            DOM_SetAttribute(node, CLASS_ATTRNAME, UNIT_ATTRVALUE);
          if (DOM_HasChildNodes(node) &&
              (DOM_GetNodeType(DOM_GetFirstNode(node)) == DOM_ELEMENT_NODE ||
               DOM_GetNodeType(DOM_GetFirstNode(node)) == DOM_DOCUMENT_FRAGMENT)) {
            node = DOM_GetFirstNode(node);
          } else {
            while (node) {
              if (DOM_GetNextNodeSibling(node)) {
                node = DOM_GetNextNodeSibling(node);
                break;
              } else if (node == result->node) {
                node = NULL;
                break;
              }
              node = DOM_GetParentNode(node);
            }  // while (node)
          }  // else if
        }  // while (node)
        if (g_mrowElementsAvoiding && !DOM_IsTagName(result->node, MROW_ELNAME)) {
          DOMNode* unit_group = createMmlNode(MROW_ELNAME, result->node);
          DOM_SetAttribute(unit_group, CLASS_ATTRNAME, UNIT_ATTRVALUE);
          result->node = unit_group;
        }
        result->tag = symbol->tag;
        return result;
      } else if (symbol->acc == BOOL_TRUE) {  // accent
        return LMbuildUnaryAccent(result->str, symbol, result);
      } else {  // font change or displaystyle command
                /*
                        if (!isIE && typeof symbol.codes != "undefined") {
                          for (i=0; i<result[0].childNodes.length; i++)
                            if (result[0].childNodes[i].nodeName=="mi" ||
                   result[0].nodeName=="mi") {
                              st =
                   (result[0].nodeName=="mi"?result[0].firstChild.nodeValue:
                                              result[0].childNodes[i].firstChild.nodeValue);
                              var newst = [];
                              for (var j=0; j<st.length; j++)
                                if (st.charCodeAt(j)>64 && st.charCodeAt(j)<91) newst =
                   newst +
                                  String.fromCharCode(symbol.codes[st.charCodeAt(j)-65]);
                                else newst = newst + st.charAt(j);
                              if (result[0].nodeName=="mi")
                                result[0]=createMmlNode("mo").
                                          appendChild(document.createTextNode(newst));
                              else result[0].replaceChild(createMmlNode("mo").
                          appendChild(document.createTextNode(newst)),result[0].childNodes[i]);
                            }
                        }
                */
        node = createMmlNode(symbol->tag, result->node);
        DOM_SetAttribute(node, symbol->atname, symbol->atval);
        /*
                if (symbol.input == "\\scriptstyle" ||
                    symbol.input == "\\scriptscriptstyle")
                        node.setAttribute("displaystyle","false");
        */
        result->node = node;
        result->tag = symbol->tag;
        return result;
      }
    case TTYPE_BINARY:
      if (wcscmp(symbol->input, L"\\sfrac") == 0) {
        g_operatorsFrag = DOM_CreateDocumentFragment(g_document);
        g_gatheringOperators = 1;
      }
      result = LMparseSexpr(str);
      g_gatheringOperators = 0;
      if (result->node == NULL) {
        g_operatorsFrag = NULL;
        return CreateParseResult(
            createMmlNode(MO_ELNAME, DOM_CreateTextNode(g_document, symbol->input)),
            str, NULL);
      }
      result2 = LMparseSexpr(result->str);
      if (result2->node == NULL) {
        g_operatorsFrag = NULL;
        return CreateParseResult(
            createMmlNode(MO_ELNAME, DOM_CreateTextNode(g_document, symbol->input)),
            str, NULL);
      }
      if (wcscmp(symbol->input, L"\\root") == 0 ||
          wcscmp(symbol->input, L"\\stackrel") == 0)
        DOM_AppendChild(newFrag, result2->node);
      DOM_AppendChild(newFrag, result->node);
      if (wcscmp(symbol->input, L"\\frac") == 0 ||
          wcscmp(symbol->input, L"\\sfrac") == 0)
        DOM_AppendChild(newFrag, result2->node);
      str = result2->str;
      if (g_operatorsFrag != NULL && DOM_HasChildNodes(g_operatorsFrag)) {
        DOM_AppendChild(g_operatorsFrag, createMmlNode(symbol->tag, newFrag));
        newFrag = g_operatorsFrag;
        g_operatorsFrag = NULL;
        return CreateParseResult(newFrag, str, symbol->tag);
      }
      return CreateParseResult(createMmlNode(symbol->tag, newFrag), str, symbol->tag);
    case TTYPE_INFIX:
      str = LMremoveCharsAndBlanks(str, wcslen(symbol->input));
      return CreateParseResult(
          createMmlNode(MO_ELNAME, DOM_CreateTextNode(g_document, symbol->output)),
          str, symbol->tag);
    case TTYPE_FENCED: {
const int MAX_OPEN_CLOSE_FENCED_SIZE = 20;
widechar openFenced[21] = {L'\0'};
int openFencedLen = 0;
widechar closeFenced[21] = {L'\0'};
int closeFencedLen = 0;
widechar* tag = symbol->tag;

symbol = LMgetSymbol(str);
if (!symbol || symbol->ttype != TTYPE_LEFTBRACKET) {
// show box in place of missing argument
return CreateParseResult(
createMmlNode(tag, createMissingArgumentMmlNode()),
str, NULL);
}
str = LMremoveCharsAndBlanks(str, wcslen(symbol->input));
do {
symbol = LMgetSymbol(str);
if (!symbol) { // show box in place of missing argument
return CreateParseResult(
createMmlNode(tag, createMissingArgumentMmlNode()),
str, NULL);
}
str = LMremoveCharsAndBlanks(str, wcslen(symbol->input));
if (symbol->ttype != TTYPE_RIGHTBRACKET)
if ((openFencedLen + (int)wcslen(symbol->output)) <= MAX_OPEN_CLOSE_FENCED_SIZE)
wcscat(openFenced, symbol->output);
} while (symbol != NULL && symbol->input != NULL && symbol->ttype != TTYPE_RIGHTBRACKET);

symbol = LMgetSymbol(str);
if (!symbol || symbol->ttype != TTYPE_LEFTBRACKET) {
// show box in place of missing argument
return CreateParseResult(
createMmlNode(tag, createMissingArgumentMmlNode()),
str, NULL);
}
str = LMremoveCharsAndBlanks(str, wcslen(symbol->input));
do {
symbol = LMgetSymbol(str);
if (!symbol) { // show box in place of missing argument
return CreateParseResult(
createMmlNode(tag, createMissingArgumentMmlNode()),
str, NULL);
}
str = LMremoveCharsAndBlanks(str, wcslen(symbol->input));
if (symbol->ttype != TTYPE_RIGHTBRACKET)
if ((closeFencedLen + (int)wcslen(symbol->output)) <= MAX_OPEN_CLOSE_FENCED_SIZE)
wcscat(closeFenced, symbol->output);
} while (symbol != NULL && symbol->input != NULL && symbol->ttype != TTYPE_RIGHTBRACKET);

result = LMparseSexpr(str);
if (result->node == NULL) { // show box in place of missing argument
return CreateParseResult(
createMmlNode(tag, createMissingArgumentMmlNode()),
str, NULL);
}
node = createMmlNode(tag, result->node);
DOM_SetAttribute(node, OPEN_ATTRNAME, openFenced);
DOM_SetAttribute(node, CLOSE_ATTRNAME, closeFenced);
result->node = node;
result->tag = symbol->tag;
return result;
}
    default:
      return CreateParseResult(
          createMmlNode(symbol->tag,  // its a constant
                        DOM_CreateTextNode(g_document, symbol->output)),
          str, symbol->tag);
  }
}

ParseResult* LMparseIexpr(widechar* str) {
  LMSymbol* symbol;
  LMSymbol* symbol1;
  LMSymbol* symbol2;
  DOMNode* node;
  widechar* tag;
  int underover;
  str = LMremoveCharsAndBlanks(str, 0);
  symbol1 = LMgetSymbol(str);
  ParseResult* result = LMparseSexpr(str);
  node = result->node;
  str = result->str;
  tag = result->tag;
  symbol = LMgetSymbol(str);

  if (symbol != NULL && symbol->ttype == TTYPE_REVERTUNARY) {
    str = LMremoveCharsAndBlanks(str, wcslen(symbol->input));
    symbol2 = LMgetSymbol(str);
    if (symbol2->acc == BOOL_TRUE) {
      str = LMremoveCharsAndBlanks(str, wcslen(symbol2->input));
      ParseResult* result2 = LMbuildUnaryAccent(str, symbol2, result);
      node = result2->node;
      tag = result2->tag;
      str = result2->str;
    }
  } else
  while (symbol != NULL && symbol->ttype == TTYPE_INFIX) {
    str = LMremoveCharsAndBlanks(str, wcslen(symbol->input));
    result = LMparseSexpr(str);
    if (result->node == NULL)  // show box in place of missing argument
      result->node = createMissingArgumentMmlNode();
              str = result->str;
    tag = result->tag;
    if (symbol->tsubtype == TSUBTYPE_SUBSCRIPT ||
        symbol->tsubtype == TSUBTYPE_SUPERSCRIPT) {
      symbol2 = LMgetSymbol(str);
      tag = NULL;  // no space between x^2 and a following sin, cos, etc.
                   // This is for \underbrace and \overbrace
      underover = ((symbol1 && symbol1->ttype == TTYPE_UNDEROVER) ||
                   (node && DOM_IsUnderOverType(node)));
      //    underover = (symbol1->ttype == TTYPE_UNDEROVER);
      if (symbol2 != NULL && symbol->tsubtype == TSUBTYPE_SUBSCRIPT &&
          symbol2->tsubtype == TSUBTYPE_SUPERSCRIPT) {
        str = LMremoveCharsAndBlanks(str, wcslen(symbol2->input));
        ParseResult* result2 = LMparseSexpr(str);
        str = result2->str;
        tag =
            result2->tag;  // leave space between x_1^2 and a following sin etc.
        node = createMmlNode((underover ? MUNDEROVER_ELNAME : MSUBSUP_ELNAME),
                             node);
        DOM_AppendChild(node, result->node);
        DOM_AppendChild(node, result2->node);
      } else if (symbol->tsubtype == TSUBTYPE_SUBSCRIPT) {
        node = createMmlNode((underover ? MUNDER_ELNAME : MSUB_ELNAME), node);
        DOM_AppendChild(node, result->node);
      } else {
        node = createMmlNode((underover ? MOVER_ELNAME : MSUP_ELNAME), node);
        DOM_AppendChild(node, result->node);
      }
      if (!g_mrowElementsAvoiding)
        node = createMmlNode(MROW_ELNAME, node);  // so sum does not stretch
    } else if (symbol->tsubtype == TSUBTYPE_UNDERSCRIPT ||
               symbol->tsubtype == TSUBTYPE_OVERSCRIPT) {
      symbol2 = LMgetSymbol(str);
      tag = NULL;  // no space between x^2 and a following sin, cos, etc.
      if (symbol2 != NULL && symbol->tsubtype == TSUBTYPE_UNDERSCRIPT &&
          symbol2->tsubtype == TSUBTYPE_OVERSCRIPT) {
        str = LMremoveCharsAndBlanks(str, wcslen(symbol2->input));
        ParseResult* result2 = LMparseSexpr(str);
        str = result2->str;
        tag =
            result2->tag;  // leave space between x_1^2 and a following sin etc.
        node = createMmlNode(MUNDEROVER_ELNAME, node);
        DOM_AppendChild(node, result->node);
        DOM_AppendChild(node, result2->node);
      } else if (symbol->tsubtype == TSUBTYPE_UNDERSCRIPT) {
        node = createMmlNode(MUNDER_ELNAME, node);
        DOM_AppendChild(node, result->node);
      } else {
        node = createMmlNode(MOVER_ELNAME, node);
        DOM_AppendChild(node, result->node);
      }
      if (!g_mrowElementsAvoiding)
        node = createMmlNode(MROW_ELNAME, node);  // so sum does not stretch
    } else {
      node = createMmlNode(symbol->tag, node);
      if (wcscmp(symbol->input, L"\\atop") == 0 ||
          wcscmp(symbol->input, L"\\choose") == 0)
        DOM_SetAttribute(node, LINETHICKNESS_ATTRNAME, L"0ex");
      DOM_AppendChild(node, result->node);
      if (wcscmp(symbol->input, L"\\choose") == 0)
        node = createMmlNode(MFENCED_ELNAME, node);
    }
    symbol1 = NULL;
    symbol = LMgetSymbol(str);
  }  // while
  return CreateParseResult(node, str, tag);
}

ParseResult* LMparseExpr(widechar* str, int rightbracket, int matrix) {
  LMSymbol* operatorSymbol;
  LMSymbol* symbol;
  DOMNode* node;
  int i;
  widechar* tag;
  ParseResult* result;
  DOMNode* newFrag = DOM_CreateDocumentFragment(g_document);
  do {
    str = LMremoveCharsAndBlanks(str, 0);
    operatorSymbol = LMgetSymbol(str);
    result = LMparseIexpr(str);
    node = result->node;
    str = result->str;
    tag = result->tag;
    symbol = LMgetSymbol(str);
    if (node != NULL) {
      if ((tag &&
           (wcscmp(tag, MN_ELNAME) == 0 || wcscmp(tag, MI_ELNAME) == 0)) &&
          symbol != NULL && symbol->func == BOOL_TRUE &&
          !g_mrowElementsAvoiding) {
        // Add space before \sin in 2\sin x or x\sin x
        DOMNode* space = createMmlNode(MSPACE_ELNAME, NULL);
        DOM_SetAttribute(space, WIDTH_ATTRNAME, L"0.167em");
        node = createMmlNode(MROW_ELNAME, node);
        DOM_AppendChild(node, space);
      }
      if (g_gatheringOperators && g_operatorsFrag != NULL) {
        if (operatorSymbol->ttype == TTYPE_OPERATOR) {
          DOM_AppendChild(g_operatorsFrag, node);
        } else {
          g_gatheringOperators = 0;
          DOM_AppendChild(newFrag, node);
        }
      } else {
        DOM_AppendChild(newFrag, node);
      }
    }
  } while (symbol != NULL && symbol->ttype != TTYPE_RIGHTBRACKET &&
           (symbol->ttype == TTYPE_SPACE || symbol->output != NULL));
  // powyzszy warunek mozna poprawic usuwajac symbol->ttype == TTYPE_SPACE gdy w
  // pliku math_backtranslation_symbols.defs dodac niepuste wartosci dla pol
  // output gdy type=space
  tag = NULL;
  if (symbol != NULL && symbol->ttype == TTYPE_RIGHTBRACKET) {
    if (wcscmp(symbol->input, L"\\right") == 0) {  // right what?
      str = LMremoveCharsAndBlanks(str, wcslen(symbol->input));
      symbol = LMgetSymbol(str);
      if (symbol != NULL && wcscmp(symbol->input, L".") == 0)
        symbol->invisible = BOOL_TRUE;
      if (symbol != NULL)
        tag = symbol->rtag;
    }
    if (symbol != NULL)
      str = LMremoveCharsAndBlanks(str,
                                   wcslen(symbol->input));  // ready to return

    if (matrix) {
      DOMNodeList* newFragNodeList = DOM_GetChildNodes(newFrag);
      int m = DOM_GetLengthOfList(newFragNodeList);
if (m > 1)
      if (m > 1 && DOM_IsTagName(DOM_GetItemOfList(newFragNodeList, m-1), MROW_ELNAME) && DOM_IsTagName(DOM_GetItemOfList(newFragNodeList, m-2), MO_ELNAME) && isAmpersant(DOM_GetNodeValue(DOM_GetFirstNode(DOM_GetItemOfList(newFragNodeList, m-2))))) {
        DOMNode* table = DOM_CreateDocumentFragment(g_document);
        DOMNode *row, *frag;
        DOMNodeList* nodeList;
        int i, j, n, ampersantNodesCount;
        DOMNode** ampersantNodes;
        for (i = 0; i < m; i++) {
          if (!DOM_IsTagName(DOM_GetItemOfList(newFragNodeList, i), MROW_ELNAME))
            continue;
          row = DOM_CreateDocumentFragment(g_document);
          frag = DOM_CreateDocumentFragment(g_document);
          node = DOM_GetItemOfList(newFragNodeList, i); // <mrow> -&-&...&-&- </mrow>
          nodeList = DOM_GetChildNodes(node);
          n = DOM_GetLengthOfList(nodeList);
          if (!(ampersantNodes = malloc(sizeof(DOMNode*)*n)))
memoryError();
          ampersantNodesCount = 0;
          for (j = 0; j < n; j++) {
            if (DOM_GetFirstNode(DOM_GetItemOfList(nodeList, j)) && isAmpersant(DOM_GetNodeValue(DOM_GetFirstNode(DOM_GetItemOfList(nodeList, j))))) {
              ampersantNodes[ampersantNodesCount++] = DOM_GetItemOfList(nodeList, j);
              DOM_AppendChild(row, createMmlNode(MTD_ELNAME, frag));
            } else {
              DOM_AppendChild(frag, DOM_GetItemOfList(nodeList, j));
            }
          }
          DOM_AppendChild(row, createMmlNode(MTD_ELNAME, frag));
          for (j = 0; j < ampersantNodesCount; j++)
            DOM_DestroyNode(ampersantNodes[j]);
          free(ampersantNodes);
//          delete nodeList;
          DOM_AppendChild(table, createMmlNode(MTR_ELNAME, row));
        }
        for (i = 0; i < m; i++)
//          delete newFragNodeList->Item(i);
//        delete newFragNodeList;
        return CreateParseResult(table, str, NULL);
      }
//      delete newFragNodeList;
    }

    if (symbol->invisible != BOOL_TRUE) {
      node = createMmlNode(MO_ELNAME, DOM_CreateTextNode(g_document, symbol->output));
      DOM_AppendChild(newFrag, node);
    }
  }
  return CreateParseResult(newFrag, str, tag);
}

DOMNode* parseMath(widechar* str) {
  //@wprintf(L"*parseMath: \"%s\"\n", str);
  // str.replace(/^\s+/g,"")
  str = LMremoveCharsAndBlanks(str, 0);
  ParseResult* result = LMparseExpr(str, 0, 0);
  DOMNode* frag = result->node;

  DOMNode* node = createMmlNode(MSTYLE_ELNAME, frag);
  DOM_SetAttribute(node, MATHCOLOR_ATTRNAME, g_mathColor);
  DOM_SetAttribute(node, FONTFAMILY_ATTRNAME, g_mathFontFamily);
  DOM_SetAttribute(node, MATHSIZE_ATTRNAME, g_mathFontSize);
  if (g_displayStyle)
    DOM_SetAttribute(node, DISPLAYSTYLE_ATTRNAME, TRUE_ATTRVALUE);
  node = createMmlNode(MATH_ELNAME, node);
  DOM_SetAttribute(node, XMLNS_ATTRNAME, MATHML_URL_ATTRVALUE);
//@  if (g_showAsciiFormulaOnHover)
//@    DOM_SetAttribute(node, TITLE_ATTRNAME, str);  // does not show in Gecko
  return node;
}

#include "math_parser_correction.ci"

int back_translate_math_string(widechar* text_buffer,
                               int textLength,
                               widechar* translated_buffer,
                               int* translatedLength) {
  if (text_buffer == NULL || textLength <= 0 || translated_buffer == NULL ||
      translatedLength == NULL || *translatedLength <= 0)
    return 0;
  if (LMsymbols == NULL)
    return 0;

  if (!LMsymbolsSorted) {
    QuickSort((LPVOID*)LMsymbols, 0, LMsymbolsCount);
    LMsymbolsSorted = 1;
  }
  DestroyTempLMSymbols();

  widestring_buf_len = 0;
  if (g_document == NULL)
    g_document = DOM_CreateDocument();
  DOMNode* frag = DOM_CreateDocumentFragment(g_document);

  text_buffer[textLength] = wcNULL;
  int nMathExprSeparatorLen = (int)wcslen(g_mathExprSeparator);
  int mathExpression = 0;
  widechar* pwcsTranslatingStr;
  int nTranslatingStrLen;

  widechar* pwcsStr = text_buffer;
  while (*pwcsStr != wcNULL) {
    widestring_buf_len = 0;
    widechar* pFragTranslatedBuffer;
    int nFragTranslatedLength;

    widechar* pwcsSeparatorStr = wcsstr(pwcsStr, g_mathExprSeparator);
    if (pwcsSeparatorStr == NULL) {
      nTranslatingStrLen = (int)wcslen(pwcsStr);
      pwcsTranslatingStr = alloc_widestring(pwcsStr, nTranslatingStrLen);
      if (mathExpression &&
          pwcsTranslatingStr[nTranslatingStrLen - 1] == g_mathExprSeparator[0])
        pwcsTranslatingStr[nTranslatingStrLen - 1] = wcNULL;
      pwcsStr += nTranslatingStrLen;
    } else if (pwcsSeparatorStr == pwcsStr) {
      if (pwcsStr == text_buffer)
        mathExpression = 1;
      pwcsStr += nMathExprSeparatorLen;
      continue;
    } else {
      nTranslatingStrLen = pwcsSeparatorStr - pwcsStr;
      pwcsTranslatingStr = alloc_widestring(pwcsStr, nTranslatingStrLen);
      pwcsStr = pwcsSeparatorStr + nMathExprSeparatorLen;
    }

    pFragTranslatedBuffer = NULL;
    nFragTranslatedLength = MAX_TRANS_LENGTH;
    if (mathExpression) {
      widechar* mathBrl =
          correctBeforeBackTranslation(pwcsTranslatingStr, nTranslatingStrLen);
      //@wprintf(L"$\"%s\"\n", mathBrl);
      back_translate_with_mathexpr_table(
          mathBrl, (int)wcslen(mathBrl), &pFragTranslatedBuffer,
          &nFragTranslatedLength);
      //@wprintf(L"^\"%s\"\n", (widechar*)pFragTranslatedBuffer);

      DOMNode* node = parseMath((widechar*)pFragTranslatedBuffer);
      DOM_AppendChild(frag, node);
    }  // if (mathExpression)
    else {
      back_translate_with_main_table(pwcsTranslatingStr,
                                     nTranslatingStrLen, &pFragTranslatedBuffer,
                                     &nFragTranslatedLength);
      //@wprintf(L"$\"%s\"\n", (widechar*)pFragTranslatedBuffer);
      DOM_AppendChild(frag,
                            DOM_AppendChild(createXhtmlElement(SPAN_ELNAME), DOM_CreateTextNode(g_document,
                                (widechar*)pFragTranslatedBuffer)));
    }  // else if (mathExpression)

    mathExpression = !mathExpression;
  }  // while (*pwcsStr != wcNULL)

  int frg_buflen;
  widechar* frg_buf = DOM_ToString(frag, &frg_buflen);
  DOM_DestroyNode(frag);
  if (frg_buflen < *translatedLength)
    *translatedLength = frg_buflen;
  memcpy(translated_buffer, frg_buf, (*translatedLength) * sizeof(widechar));

  return 1;
}  // back_translate_math_string
