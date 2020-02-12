#include "config.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
extern "C" {
#include "../louisutdml.h"
#include "../louisutdml_backtranslation.h"
}

#include "string_utils.h"
#include "DOMAttr.h"
#include "DOMDocument.h"
#include "DOMDocumentFragment.h"
#include "DOMElement.h"
#include "DOMText.h"

static wchar_t g_wcDecimalSign = L',';
static wchar_t* g_mathExprSeparator = L"_.";
static wchar_t* g_mathColor =
    L"blue";  // change it to "" (to inherit) or another color
static wchar_t* g_mathFontSize =
    L"1em";  // change to e.g. 1.2em for larger math
static wchar_t* g_mathFontFamily = L"mathvariant";  // change to "" to inherit
                                                    // (works in IE) or another
                                                    // family (e.g. "arial")
static bool g_displayStyle =
    true;  // puts limits above and below large operators
static bool g_showAsciiFormulaOnHover = true;
static bool g_mrowElementsAvoiding = true;

const wchar_t* BOX_MISSING_ARGUMENT = L"\u25A1";

bool isIE() {
  return false;
}

const wchar_t wcNULL = L'\0';

const wchar_t* MATH_ELNAME = L"math";
const wchar_t* MFENCED_ELNAME = L"mfenced";
const wchar_t* MI_ELNAME = L"mi";
const wchar_t* MN_ELNAME = L"mn";
const wchar_t* MO_ELNAME = L"mo";
const wchar_t* MOVER_ELNAME = L"mover";
const wchar_t* MROW_ELNAME = L"mrow";
const wchar_t* MSPACE_ELNAME = L"mspace";
const wchar_t* MSTYLE_ELNAME = L"mstyle";
const wchar_t* MSUB_ELNAME = L"msub";
const wchar_t* MSUBSUP_ELNAME = L"msubsup";
const wchar_t* MSUP_ELNAME = L"msup";
const wchar_t* MTABLE_ELNAME = L"mtable";
const wchar_t* MTD_ELNAME = L"mtd";
const wchar_t* MTR_ELNAME = L"mtr";
const wchar_t* MUNDER_ELNAME = L"munder";
const wchar_t* MUNDEROVER_ELNAME = L"munderover";
const wchar_t* SPAN_ELNAME = L"span";

const wchar_t* CLASS_ATTRNAME = L"class";
const wchar_t* CLOSE_ATTRNAME = L"close";
const wchar_t* COLUMNALIGN_ATTRNAME = L"columnalign";
const wchar_t* COLUMNSPACING_ATTRNAME = L"columnspacing";
const wchar_t* DISPLAYSTYLE_ATTRNAME = L"displaystyle";
const wchar_t* FONTFAMILY_ATTRNAME = L"fontfamily";
const wchar_t* HEIGHT_ATTRNAME = L"height";
const wchar_t* LINETHICKNESS_ATTRNAME = L"linethickness";
const wchar_t* LSPACE_ATTRNAME = L"lspace";
const wchar_t* MATHCOLOR_ATTRNAME = L"mathcolor";
const wchar_t* MATHSIZE_ATTRNAME = L"mathsize";
const wchar_t* MAXSIZE_ATTRNAME = L"maxsize";
const wchar_t* MINSIZE_ATTRNAME = L"minsize";
const wchar_t* OPEN_ATTRNAME = L"open";
const wchar_t* RSPACE_ATTRNAME = L"rspace";
const wchar_t* TITLE_ATTRNAME = L"title";
const wchar_t* WIDTH_ATTRNAME = L"width";
const wchar_t* XMLNS_ATTRNAME = L"xmlns";

const wchar_t* CENTER_ALIGN_ATTRVALUE = L"center ";
const wchar_t* FALSE_ATTRVALUE = L"false";
const wchar_t* LEFT_ALIGN_ATTRVALUE = L"left ";
const wchar_t* MATHML_URL_ATTRVALUE = L"http://www.w3.org/1998/Math/MathML";
const wchar_t* RIGHT_ALIGN_ATTRVALUE = L"right ";
const wchar_t* RIGHT_CENTER_LEFT_ATTRVALUE = L"right center left";
const wchar_t* TRUE_ATTRVALUE = L"true";
const wchar_t* UNIT_ATTRVALUE = L"unit";

typedef enum {
LEFT_ALIGN,
CENTER_ALIGN,
RIGHT_ALIGN
} AlignType;

struct ParseResult {
  CDOMNode* node;
  wchar_t* str;
  wchar_t* tag;

  ParseResult() {
    node = NULL;
    str = NULL;
    tag = NULL;
  }

  ParseResult(CDOMNode* n, wchar_t* s, wchar_t* t) {
    this->node = n;
    this->str = s;
    this->tag = t;
  }
};

typedef void* LPVOID;

static CDOMDocument* g_document = NULL;
static CDOMDocumentFragment* g_operatorsFrag = NULL;
static bool g_gatheringOperators = false;
static bool g_gatheringLetters = false;

TokenType LMpreviousSymbol, LMcurrentSymbol;

#define WIDESTRING_BUFFER_SIZE 8 * BUFSIZE
static wchar_t widestring_buffer[WIDESTRING_BUFFER_SIZE];
static int widestring_buf_len = 0;

static wchar_t* alloc_widestring(const wchar_t* inString, int length) {
  wchar_t* newString;
  int inStringLen;
  if (inString == NULL)
    return NULL;
  if ((widestring_buf_len + length) >= WIDESTRING_BUFFER_SIZE)
    memoryError();
  newString = &widestring_buffer[widestring_buf_len];
  inStringLen = (int)wcslen(inString);
  if (length < inStringLen)
    inStringLen = length;
  wcsncpy(newString, inString, inStringLen);
  newString[inStringLen] = wcNULL;
  widestring_buf_len += length + 1;
  return newString;
}

bool isDigit(wchar_t wc) {
  return (L'0' <= wc && wc <= L'9');
}

bool isLetter(wchar_t wc) {
  static const wchar_t* s_letters =
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

#define min(a, b) (((a) <= (b)) ? (a) : (b))

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
  s = min(a - off, b - a);
  vecswap(off, b - s, s);
  s = min(d - c, n - d - 1);
  vecswap(b, n - s, s);

  // Recursively sort non-partition-elements
  if ((s = b - a) > 1)
    QuickSort(x, off, s);
  if ((s = d - c) > 1)
    QuickSort(x, n - s, s);
}  // QuickSort

bool isAmpersant(CString* s) {
  if (!s) return false;
  return (*s == L"&" || *s == L"&amp;");
}

CDOMElement* createXhtmlElement(const wchar_t* tagName) {
  return g_document->CreateElement(tagName);
}

CDOMElement* createMmlNode(const wchar_t* tagName, CDOMNode* frag) {
  CDOMElement* node = g_document->CreateElement(tagName);
  //@  if (isIE) var node = document.createElement("m:"+t);
  //@  else var node =
  //document.createElementNS("http://www.w3.org/1998/Math/MathML",t);
  if (frag)
    node->AppendChild(frag);
  return node;
}

CDOMNode* createMissingArgumentMmlNode() {
  return createMmlNode(MO_ELNAME, g_document->CreateTextNode(BOX_MISSING_ARGUMENT));
}

wchar_t* LMremoveCharsAndBlanks(wchar_t* str, int n) {
  // remove n characters and any following blanks
  wchar_t* st = str + n;
  int i = 0;
  for (; st[i] != wcNULL && (st[i] <= 32 || st[i] == 0xa0); i++)
    ;
  return (st + i);
}

int positionInLMsymbols(const wchar_t* str, int n) {
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

LMSymbol* LMgetSymbol(const wchar_t* str) {
  // return maximal initial substring of str that appears in names
  // return NULL if there is none
  int k = 0;  // new pos
  int j = 0;  // old pos
  int mk;  // match pos
  const int MAX_ST_LEN = 200;
  wchar_t st[MAX_ST_LEN + 1];
  wchar_t* tagst;
  wchar_t* match = NULL;
  bool more = true;
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
  wchar_t wc = str[0];
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
    tagst = (wchar_t*)MN_ELNAME;
  }  // if (isDigit(wc))
  else if (isLetter(wc)) {
    if (g_gatheringLetters) {
      do {
        wc = str[k];
        k++;
      } while (isLetter(wc) && k <= strLen);
      k--;
    }
    tagst = (wchar_t*)MI_ELNAME;
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
    tagst = (wchar_t*)MO_ELNAME;
  }  // else if

  // Commented out by DRW (not fully understood, but probably to do with
  // use of "/" as an INFIX version of "\\frac", which we don't want):
  // if (st=="-" && LMpreviousSymbol==INFIX) {
  //  LMcurrentSymbol = INFIX;  //trick "/" into recognizing "-" on second parse
  //  return {input:st, tag:tagst, output:st, ttype:UNARY, func:true};
  //}

  wchar_t* inout = alloc_widestring(str, k);

  return CreateTempLMSymbol(inout, tagst, inout, TTYPE_CONST);
}  // LMgetSymbol

ParseResult LMparseExpr(wchar_t* str, bool rightbracket, bool matrix);

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

ParseResult LMbuildUnaryAccent(wchar_t* str,
                               LMSymbol* symbol,
                               ParseResult result) {
  CDOMElement* node = createMmlNode(symbol->tag, result.node);
  wchar_t* output = symbol->output;
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
  CDOMElement* node1 =
      createMmlNode(MO_ELNAME, g_document->CreateTextNode(output));
  if (wcscmp(symbol->input, L"\\vec") == 0 ||
      wcscmp(symbol->input, L"\\check") == 0)
    // don't allow to stretch
    node1->SetAttribute(MAXSIZE_ATTRNAME, L"1.2");
  // why doesn't "1" work?  \vec nearly disappears in firefox
  if (isIE() && wcscmp(symbol->input, L"\\bar") == 0)
    node1->SetAttribute(MAXSIZE_ATTRNAME, L"0.5");
  if (wcscmp(symbol->input, L"\\underbrace") == 0 ||
      wcscmp(symbol->input, L"\\underline") == 0)
    node1->SetAttribute(L"accentunder", L"true");
  else
    node1->SetAttribute(L"accent", L"true");
  node->AppendChild(node1);
  if (wcscmp(symbol->input, L"\\overbrace") == 0 ||
      wcscmp(symbol->input, L"\\underbrace") == 0)
    node->SetUnderOverType();

  return ParseResult(node, str, symbol->tag);
}  // LMbuildUnaryAccent

ParseResult LMparseSexpr(
    wchar_t* str) {  // parses str and returns [node,tailstr,(node)tag]
  LMSymbol* symbol;
  CDOMElement* node;
  ParseResult result;
  ParseResult result2;
  wchar_t* atval;
  int i;
  wchar_t* st;
  wchar_t* output;
  wchar_t atvalStr[100];

  CDOMDocumentFragment* newFrag = g_document->CreateDocumentFragment();
  str = LMremoveCharsAndBlanks(str, 0);
  symbol = LMgetSymbol(str);  // either a token or a bracket or empty
  if (symbol == NULL || symbol->ttype == TTYPE_RIGHTBRACKET)
    return ParseResult(NULL, str, NULL);
  if (symbol->ttype == TTYPE_DEFINITION) {
    int symbolInputLen = (int)wcslen(symbol->input);
    st = alloc_widestring(symbol->output, (wcslen(symbol->output) +
                                           wcslen(str) - symbolInputLen));
    wcscat(st, LMremoveCharsAndBlanks(str, symbolInputLen));
    str = st;
    symbol = LMgetSymbol(str);
    if (symbol == NULL || symbol->ttype == TTYPE_RIGHTBRACKET)
      return ParseResult(NULL, str, NULL);
  }
  str = LMremoveCharsAndBlanks(str, wcslen(symbol->input));

  switch (symbol->ttype) {
    case TTYPE_SPACE:
      node = createMmlNode(symbol->tag, NULL);
      node->SetAttribute(symbol->atname, symbol->atval);
      return ParseResult(node, str, symbol->tag);
    case TTYPE_UNDEROVER:
      if (isIE()) {
        if (symbol->rinput != NULL) {  // botch for missing symbols
          st = alloc_widestring(symbol->rinput,
                                (wcslen(symbol->rinput) + wcslen(str)));
          wcscat(st, str);
          str = st;
          symbol = LMgetSymbol(str);
          if (symbol == NULL || symbol->ttype == TTYPE_RIGHTBRACKET)
            return ParseResult(NULL, str, NULL);
          symbol->ttype = TTYPE_UNDEROVER;
          str = LMremoveCharsAndBlanks(str, wcslen(symbol->input));
        }
      }
      return ParseResult(createMmlNode(symbol->tag, g_document->CreateTextNode(
                                                        symbol->output)),
                         str, symbol->tag);
    case TTYPE_CONST:
    case TTYPE_OPERATOR:
      output = symbol->output;
      if (isIE() && symbol->ieoutput != NULL)
        output = symbol->ieoutput;
      if (output)
        node = createMmlNode(symbol->tag, g_document->CreateTextNode(output));
      else
        node = createMmlNode(symbol->tag, NULL);
      return ParseResult(node, str, symbol->tag);
    case TTYPE_LONG:  // added by DRW
      node = createMmlNode(symbol->tag,
                           g_document->CreateTextNode(symbol->output));
      node->SetAttribute(MINSIZE_ATTRNAME, L"1.5");
      node->SetAttribute(MAXSIZE_ATTRNAME, L"1.5");
      node = createMmlNode(MOVER_ELNAME, node);
      node->AppendChild(createMmlNode(MSPACE_ELNAME, NULL));
      return ParseResult(node, str, symbol->tag);
    case TTYPE_STRETCHY:  // added by DRW
      output = symbol->output;
      if (isIE() && symbol->ieoutput != NULL)
        symbol->output =
            symbol->ieoutput;  // doesn't expand, but then nor does "\u2216"
      node = createMmlNode(symbol->tag, g_document->CreateTextNode(output));
      if (symbol->tsubtype == TSUBTYPE_VERT) {
        node->SetAttribute(LSPACE_ATTRNAME, L"0em");
        node->SetAttribute(RSPACE_ATTRNAME, L"0em");
      }
      if (symbol->atval != NULL)
        node->SetAttribute(MAXSIZE_ATTRNAME,
                           symbol->atval);  // don't allow to stretch here
      if (symbol->rtag != NULL)
        return ParseResult(node, str, symbol->rtag);
      else
        return ParseResult(node, str, symbol->tag);
    case TTYPE_BIG:  // added by DRW
      atval = symbol->atval;
      symbol = LMgetSymbol(str);
      if (symbol == NULL)
        return ParseResult(NULL, str, NULL);
      str = LMremoveCharsAndBlanks(str, wcslen(symbol->input));
      node = createMmlNode(symbol->tag,
                           g_document->CreateTextNode(symbol->output));
      if (isIE()) {  // to get brackets to expand
        CDOMElement* space = createMmlNode(MSPACE_ELNAME, NULL);
        wcscpy(atvalStr, atval);
        wcscat(atvalStr, L"ex");
        space->SetAttribute(HEIGHT_ATTRNAME, atvalStr);
        node = createMmlNode(MROW_ELNAME, node);
        node->AppendChild(space);
      } else {  // ignored in IE
        node->SetAttribute(MINSIZE_ATTRNAME, atval);
        node->SetAttribute(MAXSIZE_ATTRNAME, atval);
      }
      return ParseResult(node, str, symbol->tag);
    case TTYPE_LEFTBRACKET:  // read (expr+)
      if (symbol->tsubtype == TSUBTYPE_LEFT) {  // left what?
        symbol = LMgetSymbol(str);
        if (symbol != NULL) {
          if (symbol->tsubtype == TSUBTYPE_DOT)
            symbol->invisible = BOOL_TRUE;
          str = LMremoveCharsAndBlanks(str, wcslen(symbol->input));
        }
      }
      result = LMparseExpr(str, true, false);
      node = NULL;
      if (symbol == NULL || symbol->invisible == BOOL_TRUE) {
        while (result.node->GetNodeType() == DOM_DOCUMENT_FRAGMENT &&
               result.node->GetFirstNode() &&
               result.node->GetFirstNode() == result.node->GetLastNode()) {
          CDOMNode* n = result.node->RemoveChild(result.node->GetFirstNode());
          delete result.node;
          result.node = n;
        }
        if (!g_mrowElementsAvoiding ||
            result.node->GetNodeType() != DOM_ELEMENT_NODE) {
          if (!result.node->NodeNameEq(MROW_ELNAME))
            node = createMmlNode(MROW_ELNAME, result.node);
        }
      } else {
        node = createMmlNode(MO_ELNAME,
                             g_document->CreateTextNode(symbol->output));
        node = createMmlNode(MROW_ELNAME, node);
        node->AppendChild(result.node);
      }
      if (node)
        result.node = node;
      return result;
    case TTYPE_MATRIX:
      if (symbol->tsubtype == TSUBTYPE_ARRAY) {
        const int MAX_ALIGN_COUNT = 100;
        AlignType align[MAX_ALIGN_COUNT];
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

        wchar_t* columnAlign = alloc_widestring(L" ", leftAlignCount*wcslen(LEFT_ALIGN_ATTRVALUE) + centerAlignCount*wcslen(CENTER_ALIGN_ATTRVALUE) + rightAlignCount*wcslen(RIGHT_ALIGN_ATTRVALUE) + 1);
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
        result = LMparseExpr(st, true, true);
        // if (result[0] == NULL)
        //   return [createMmlNode("mo", document.createTextNode(symbol.input)),str];
        node = createMmlNode(MTABLE_ELNAME, result.node);
        node->SetAttribute(COLUMNALIGN_ATTRNAME, columnAlign);
        node->SetAttribute(DISPLAYSTYLE_ATTRNAME, FALSE_ATTRVALUE);
        if (isIE())
          return ParseResult(node, result.str, NULL);
        // trying to get a *little* bit of space around the array
        // (IE already includes it)
        CDOMElement* lspace = createMmlNode(MSPACE_ELNAME, NULL);
        lspace->SetAttribute(WIDTH_ATTRNAME, L"0.167em");
        CDOMElement* rspace = createMmlNode(MSPACE_ELNAME, NULL);
        rspace->SetAttribute(WIDTH_ATTRNAME, L"0.167em");
        CDOMElement* node1 = createMmlNode(MROW_ELNAME, lspace);
        node1->AppendChild(node);
        node1->AppendChild(rspace);
        return ParseResult(node1, result.str, NULL);
      } else {  // (symbol->tsubtype == TSUBTYPE_EQNARRAY)
        st = alloc_widestring(L"{", wcslen(str) + 1);
        wcscat(st, str);
        result = LMparseExpr(st, true, true);
        node = createMmlNode(MTABLE_ELNAME, result.node);
        if (isIE())
          node->SetAttribute(COLUMNSPACING_ATTRNAME, L"0.25em"); // best in practice?
        else
          node->SetAttribute(COLUMNSPACING_ATTRNAME, L"0.167em"); // correct (but ignored?)
        node->SetAttribute(COLUMNALIGN_ATTRNAME, RIGHT_CENTER_LEFT_ATTRVALUE);
        node->SetAttribute(DISPLAYSTYLE_ATTRNAME, TRUE_ATTRVALUE);
        node = createMmlNode(MROW_ELNAME, node);
        return ParseResult(node, result.str, NULL);
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
        g_gatheringLetters = true;
      result = LMparseSexpr(str);
      g_gatheringLetters = false;
      if (result.node == NULL) {
        return ParseResult(
            createMmlNode(symbol->tag,
                          g_document->CreateTextNode(symbol->output)),
            str, NULL);
      }
      if (symbol->func == BOOL_TRUE) {  // functions hack
        wchar_t st = str[0];
        //	if (st=="^" || st=="_" || st=="/" || st=="|" || st==",") {
        if (st == L'^' || st == L'_' || st == L',') {
          return ParseResult(
              createMmlNode(symbol->tag,
                            g_document->CreateTextNode(symbol->output)),
              str, symbol->tag);
        } else {
          node = createMmlNode(
              MROW_ELNAME,
              createMmlNode(symbol->tag,
                            g_document->CreateTextNode(symbol->output)));
          if (isIE()) {
            CDOMElement* space = createMmlNode(MSPACE_ELNAME, NULL);
            space->SetAttribute(WIDTH_ATTRNAME, L"0.167em");
            node->AppendChild(space);
          }
          node->AppendChild(result.node);
          return ParseResult(node, result.str, symbol->tag);
        }
      }
      if (wcscmp(symbol->input, L"\\sqrt") == 0) {  // sqrt
        if (isIE()) {                               // set minsize, for \surd
          CDOMElement* space = createMmlNode(MSPACE_ELNAME, NULL);
          space->SetAttribute(HEIGHT_ATTRNAME, L"1.2ex");
          space->SetAttribute(WIDTH_ATTRNAME, L"0em");  // probably no effect
          node = createMmlNode(symbol->tag, result.node);
          //	  node.setAttribute(MINSIZE_ATTRNAME, L"1");	// ignored
          //	  node = createMmlNode(MROW_ELNAME, node);  // hopefully
          //unnecessary
          node->AppendChild(space);
          result.node = node;
          result.tag = symbol->tag;
          return result;
        } else {
          result.node = createMmlNode(symbol->tag, result.node);
          result.tag = symbol->tag;
          return result;
        }
      } if (wcscmp(symbol->input, L"\\unitsymbol") == 0) {  // unitsymbol
        CDOMNode* node = result.node;
        while (node) {
          if (node->GetNodeType() == DOM_ELEMENT_NODE)
            ((CDOMElement*) node)->SetAttribute(CLASS_ATTRNAME, UNIT_ATTRVALUE);
          if (node->HasChildNodes() &&
              (node->GetFirstNode()->GetNodeType() == DOM_ELEMENT_NODE ||
               node->GetFirstNode()->GetNodeType() == DOM_DOCUMENT_FRAGMENT)) {
            node = node->GetFirstNode();
          } else {
            while (node) {
              if (node->GetNextNodeSibling()) {
                node = node->GetNextNodeSibling();
                break;
              } else if (node == result.node) {
                node = NULL;
                break;
              }
              node = node->GetParentNode();
            }  // while (node)
          }  // else if
        }  // while (node)
        if (g_mrowElementsAvoiding && !result.node->NodeNameEq(MROW_ELNAME)) {
          CDOMElement* unit_group = createMmlNode(MROW_ELNAME, result.node);
          unit_group->SetAttribute(CLASS_ATTRNAME, UNIT_ATTRVALUE);
          result.node = unit_group;
        }
        result.tag = symbol->tag;
        return result;
      } else if (symbol->acc == BOOL_TRUE) {  // accent
        return LMbuildUnaryAccent(result.str, symbol, result);
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
        node = createMmlNode(symbol->tag, result.node);
        node->SetAttribute(symbol->atname, symbol->atval);
        /*
                if (symbol.input == "\\scriptstyle" ||
                    symbol.input == "\\scriptscriptstyle")
                        node.setAttribute("displaystyle","false");
        */
        result.node = node;
        result.tag = symbol->tag;
        return result;
      }
    case TTYPE_BINARY:
      if (wcscmp(symbol->input, L"\\sfrac") == 0) {
        g_operatorsFrag = g_document->CreateDocumentFragment();
        g_gatheringOperators = true;
      }
      result = LMparseSexpr(str);
      g_gatheringOperators = false;
      if (result.node == NULL) {
        g_operatorsFrag = NULL;
        return ParseResult(
            createMmlNode(MO_ELNAME, g_document->CreateTextNode(symbol->input)),
            str, NULL);
      }
      result2 = LMparseSexpr(result.str);
      if (result2.node == NULL) {
        g_operatorsFrag = NULL;
        return ParseResult(
            createMmlNode(MO_ELNAME, g_document->CreateTextNode(symbol->input)),
            str, NULL);
      }
      if (wcscmp(symbol->input, L"\\root") == 0 ||
          wcscmp(symbol->input, L"\\stackrel") == 0)
        newFrag->AppendChild(result2.node);
      newFrag->AppendChild(result.node);
      if (wcscmp(symbol->input, L"\\frac") == 0 ||
          wcscmp(symbol->input, L"\\sfrac") == 0)
        newFrag->AppendChild(result2.node);
      str = result2.str;
      if (g_operatorsFrag != NULL && g_operatorsFrag->HasChildNodes()) {
        g_operatorsFrag->AppendChild(createMmlNode(symbol->tag, newFrag));
        newFrag = g_operatorsFrag;
        g_operatorsFrag = NULL;
        return ParseResult(newFrag, str, symbol->tag);
      }
      return ParseResult(createMmlNode(symbol->tag, newFrag), str, symbol->tag);
    case TTYPE_INFIX:
      str = LMremoveCharsAndBlanks(str, wcslen(symbol->input));
      return ParseResult(
          createMmlNode(MO_ELNAME, g_document->CreateTextNode(symbol->output)),
          str, symbol->tag);
    case TTYPE_FENCED: {
const int MAX_OPEN_CLOSE_FENCED_SIZE = 20;
wchar_t openFenced[MAX_OPEN_CLOSE_FENCED_SIZE+1] = {L'\0'};
int openFencedLen = 0;
wchar_t closeFenced[MAX_OPEN_CLOSE_FENCED_SIZE+1] = {L'\0'};
int closeFencedLen = 0;
wchar_t* tag = symbol->tag;

symbol = LMgetSymbol(str);
if (!symbol || symbol->ttype != TTYPE_LEFTBRACKET) {
// show box in place of missing argument
return ParseResult(
createMmlNode(tag, createMissingArgumentMmlNode()),
str, NULL);
}
str = LMremoveCharsAndBlanks(str, wcslen(symbol->input));
do {
symbol = LMgetSymbol(str);
if (!symbol) { // show box in place of missing argument
return ParseResult(
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
return ParseResult(
createMmlNode(tag, createMissingArgumentMmlNode()),
str, NULL);
}
str = LMremoveCharsAndBlanks(str, wcslen(symbol->input));
do {
symbol = LMgetSymbol(str);
if (!symbol) { // show box in place of missing argument
return ParseResult(
createMmlNode(tag, createMissingArgumentMmlNode()),
str, NULL);
}
str = LMremoveCharsAndBlanks(str, wcslen(symbol->input));
if (symbol->ttype != TTYPE_RIGHTBRACKET)
if ((closeFencedLen + (int)wcslen(symbol->output)) <= MAX_OPEN_CLOSE_FENCED_SIZE)
wcscat(closeFenced, symbol->output);
} while (symbol != NULL && symbol->input != NULL && symbol->ttype != TTYPE_RIGHTBRACKET);

result = LMparseSexpr(str);
if (result.node == NULL) { // show box in place of missing argument
return ParseResult(
createMmlNode(tag, createMissingArgumentMmlNode()),
str, NULL);
}
node = createMmlNode(tag, result.node);
node->SetAttribute(OPEN_ATTRNAME, openFenced);
node->SetAttribute(CLOSE_ATTRNAME, closeFenced);
result.node = node;
result.tag = symbol->tag;
return result;
}
    default:
      return ParseResult(
          createMmlNode(symbol->tag,  // its a constant
                        g_document->CreateTextNode(symbol->output)),
          str, symbol->tag);
  }
}

ParseResult LMparseIexpr(wchar_t* str) {
  LMSymbol* symbol;
  LMSymbol* symbol1;
  LMSymbol* symbol2;
  CDOMNode* node;
  wchar_t* tag;
  bool underover;
  str = LMremoveCharsAndBlanks(str, 0);
  symbol1 = LMgetSymbol(str);
  ParseResult result = LMparseSexpr(str);
  node = result.node;
  str = result.str;
  tag = result.tag;
  symbol = LMgetSymbol(str);

  if (symbol != NULL && symbol->ttype == TTYPE_REVERTUNARY) {
    str = LMremoveCharsAndBlanks(str, wcslen(symbol->input));
    symbol2 = LMgetSymbol(str);
    if (symbol2->acc == BOOL_TRUE) {
      str = LMremoveCharsAndBlanks(str, wcslen(symbol2->input));
      ParseResult result2 = LMbuildUnaryAccent(str, symbol2, result);
      node = result2.node;
      tag = result2.tag;
      str = result2.str;
    }
  } else
  while (symbol != NULL && symbol->ttype == TTYPE_INFIX) {
    str = LMremoveCharsAndBlanks(str, wcslen(symbol->input));
    result = LMparseSexpr(str);
    if (result.node == NULL)  // show box in place of missing argument
      result.node = createMissingArgumentMmlNode();
              str = result.str;
    tag = result.tag;
    if (symbol->tsubtype == TSUBTYPE_SUBSCRIPT ||
        symbol->tsubtype == TSUBTYPE_SUPERSCRIPT) {
      symbol2 = LMgetSymbol(str);
      tag = NULL;  // no space between x^2 and a following sin, cos, etc.
                   // This is for \underbrace and \overbrace
      underover = ((symbol1 && symbol1->ttype == TTYPE_UNDEROVER) ||
                   (node && node->IsUnderOverType()));
      //    underover = (symbol1->ttype == TTYPE_UNDEROVER);
      if (symbol2 != NULL && symbol->tsubtype == TSUBTYPE_SUBSCRIPT &&
          symbol2->tsubtype == TSUBTYPE_SUPERSCRIPT) {
        str = LMremoveCharsAndBlanks(str, wcslen(symbol2->input));
        ParseResult result2 = LMparseSexpr(str);
        str = result2.str;
        tag =
            result2.tag;  // leave space between x_1^2 and a following sin etc.
        node = createMmlNode((underover ? MUNDEROVER_ELNAME : MSUBSUP_ELNAME),
                             node);
        node->AppendChild(result.node);
        node->AppendChild(result2.node);
      } else if (symbol->tsubtype == TSUBTYPE_SUBSCRIPT) {
        node = createMmlNode((underover ? MUNDER_ELNAME : MSUB_ELNAME), node);
        node->AppendChild(result.node);
      } else {
        node = createMmlNode((underover ? MOVER_ELNAME : MSUP_ELNAME), node);
        node->AppendChild(result.node);
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
        ParseResult result2 = LMparseSexpr(str);
        str = result2.str;
        tag =
            result2.tag;  // leave space between x_1^2 and a following sin etc.
        node = createMmlNode(MUNDEROVER_ELNAME, node);
        node->AppendChild(result.node);
        node->AppendChild(result2.node);
      } else if (symbol->tsubtype == TSUBTYPE_UNDERSCRIPT) {
        node = createMmlNode(MUNDER_ELNAME, node);
        node->AppendChild(result.node);
      } else {
        node = createMmlNode(MOVER_ELNAME, node);
        node->AppendChild(result.node);
      }
      if (!g_mrowElementsAvoiding)
        node = createMmlNode(MROW_ELNAME, node);  // so sum does not stretch
    } else {
      node = createMmlNode(symbol->tag, node);
      if (wcscmp(symbol->input, L"\\atop") == 0 ||
          wcscmp(symbol->input, L"\\choose") == 0)
        ((CDOMElement*)node)->SetAttribute(LINETHICKNESS_ATTRNAME, L"0ex");
      node->AppendChild(result.node);
      if (wcscmp(symbol->input, L"\\choose") == 0)
        node = createMmlNode(MFENCED_ELNAME, node);
    }
    symbol1 = NULL;
    symbol = LMgetSymbol(str);
  }  // while
  return ParseResult(node, str, tag);
}

ParseResult LMparseExpr(wchar_t* str, bool rightbracket, bool matrix) {
  LMSymbol* operatorSymbol;
  LMSymbol* symbol;
  CDOMNode* node;
  int i;
  wchar_t* tag;
  ParseResult result;
  CDOMDocumentFragment* newFrag = g_document->CreateDocumentFragment();
  do {
    str = LMremoveCharsAndBlanks(str, 0);
    operatorSymbol = LMgetSymbol(str);
    result = LMparseIexpr(str);
    node = result.node;
    str = result.str;
    tag = result.tag;
    symbol = LMgetSymbol(str);
    if (node != NULL) {
      if ((tag &&
           (wcscmp(tag, MN_ELNAME) == 0 || wcscmp(tag, MI_ELNAME) == 0)) &&
          symbol != NULL && symbol->func == BOOL_TRUE &&
          !g_mrowElementsAvoiding) {
        // Add space before \sin in 2\sin x or x\sin x
        CDOMElement* space = createMmlNode(MSPACE_ELNAME, NULL);
        space->SetAttribute(WIDTH_ATTRNAME, L"0.167em");
        node = createMmlNode(MROW_ELNAME, node);
        node->AppendChild(space);
      }
      if (g_gatheringOperators && g_operatorsFrag != NULL) {
        if (operatorSymbol->ttype == TTYPE_OPERATOR) {
          g_operatorsFrag->AppendChild(node);
        } else {
          g_gatheringOperators = false;
          newFrag->AppendChild(node);
        }
      } else {
        newFrag->AppendChild(node);
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
      CDOMNodeList* newFragNodeList = newFrag->GetChildNodes();
      int m = newFragNodeList->GetLength();
if (m > 1)
      if (m > 1 && newFragNodeList->Item(m-1)->NodeNameEq(MROW_ELNAME) && newFragNodeList->Item(m-2)->NodeNameEq(MO_ELNAME) && isAmpersant(newFragNodeList->Item(m-2)->GetFirstNode()->GetNodeValue())) {
        CDOMDocumentFragment* table = g_document->CreateDocumentFragment();
        CDOMDocumentFragment *row, *frag;
        CDOMNodeList* nodeList;
        int i, j, n, ampersantNodesCount;
        CDOMNodePtr* ampersantNodes;
        for (i = 0; i < m; i++) {
          if (!newFragNodeList->Item(i)->NodeNameEq(MROW_ELNAME))
            continue;
          row = g_document->CreateDocumentFragment();
          frag = g_document->CreateDocumentFragment();
          node = newFragNodeList->Item(i); // <mrow> -&-&...&-&- </mrow>
          nodeList = node->GetChildNodes();
          n = nodeList->GetLength();
          ampersantNodes = new CDOMNodePtr[n];
          ampersantNodesCount = 0;
          for (j = 0; j < n; j++) {
            if (nodeList->Item(j)->GetFirstNode() && isAmpersant(nodeList->Item(j)->GetFirstNode()->GetNodeValue())) {
              ampersantNodes[ampersantNodesCount++] = nodeList->Item(j);
              row->AppendChild(createMmlNode(MTD_ELNAME, frag));
            } else {
              frag->AppendChild(nodeList->Item(j));
            }
          }
          row->AppendChild(createMmlNode(MTD_ELNAME, frag));
          for (j = 0; j < ampersantNodesCount; j++)
            delete ampersantNodes[j];
          delete[] ampersantNodes;
//          delete nodeList;
          table->AppendChild(createMmlNode(MTR_ELNAME, row));
        }
        for (i = 0; i < m; i++)
//          delete newFragNodeList->Item(i);
//        delete newFragNodeList;
        return ParseResult(table, str, NULL);
      }
//      delete newFragNodeList;
    }

    if (symbol->invisible != BOOL_TRUE) {
      node = createMmlNode(MO_ELNAME, g_document->CreateTextNode(symbol->output));
      newFrag->AppendChild(node);
    }
  }
  return ParseResult(newFrag, str, tag);
}

CDOMElement* parseMath(wchar_t* str) {
  //@wprintf(L"*parseMath: \"%s\"\n", str);
  // str.replace(/^\s+/g,"")
  str = LMremoveCharsAndBlanks(str, 0);
  ParseResult result = LMparseExpr(str, false, false);
  CDOMDocumentFragment* frag = (CDOMDocumentFragment*)result.node;

  CDOMElement* node = createMmlNode(MSTYLE_ELNAME, frag);
  node->SetAttribute(MATHCOLOR_ATTRNAME, g_mathColor);
  node->SetAttribute(FONTFAMILY_ATTRNAME, g_mathFontFamily);
  node->SetAttribute(MATHSIZE_ATTRNAME, g_mathFontSize);
  if (g_displayStyle)
    node->SetAttribute(DISPLAYSTYLE_ATTRNAME, TRUE_ATTRVALUE);
  node = createMmlNode(MATH_ELNAME, node);
  node->SetAttribute(XMLNS_ATTRNAME, MATHML_URL_ATTRVALUE);
//@  if (g_showAsciiFormulaOnHover)
//@    node->SetAttribute(TITLE_ATTRNAME, str);  // does not show in Gecko
  return node;
}

#include "math_parser_correction.ci"

int back_translate_math_widestring(wchar_t* text_buffer,
                                   int textLength,
                                   wchar_t* translated_buffer,
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
    g_document = new CDOMDocument();
  CDOMDocumentFragment* frag = g_document->CreateDocumentFragment();

  text_buffer[textLength] = wcNULL;
  int nMathExprSeparatorLen = (int)wcslen(g_mathExprSeparator);
  bool mathExpression = false;
  wchar_t* pwcsTranslatingStr;
  int nTranslatingStrLen;

  wchar_t* pwcsStr = text_buffer;
  while (*pwcsStr != wcNULL) {
    widestring_buf_len = 0;
    widechar* pFragTranslatedBuffer;
    int nFragTranslatedLength;

    wchar_t* pwcsSeparatorStr = wcsstr(pwcsStr, g_mathExprSeparator);
    if (pwcsSeparatorStr == NULL) {
      nTranslatingStrLen = (int)wcslen(pwcsStr);
      pwcsTranslatingStr = alloc_widestring(pwcsStr, nTranslatingStrLen);
      if (mathExpression &&
          pwcsTranslatingStr[nTranslatingStrLen - 1] == g_mathExprSeparator[0])
        pwcsTranslatingStr[nTranslatingStrLen - 1] = wcNULL;
      pwcsStr += nTranslatingStrLen;
    } else if (pwcsSeparatorStr == pwcsStr) {
      if (pwcsStr == text_buffer)
        mathExpression = true;
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
      wchar_t* mathBrl =
          correctBeforeBackTranslation(pwcsTranslatingStr, nTranslatingStrLen);
      //@wprintf(L"$\"%s\"\n", mathBrl);
      back_translate_with_mathexpr_table(
          (widechar*)mathBrl, (int)wcslen(mathBrl), &pFragTranslatedBuffer,
          &nFragTranslatedLength);
      //@wprintf(L"^\"%s\"\n", (wchar_t*)pFragTranslatedBuffer);

      CDOMElement* node = parseMath((wchar_t*)pFragTranslatedBuffer);
      frag->AppendChild(node);
    }  // if (mathExpression)
    else {
      back_translate_with_main_table((widechar*)pwcsTranslatingStr,
                                     nTranslatingStrLen, &pFragTranslatedBuffer,
                                     &nFragTranslatedLength);
      //@wprintf(L"$\"%s\"\n", (wchar_t*)pFragTranslatedBuffer);
      frag->AppendChild(createXhtmlElement(SPAN_ELNAME)
                            ->AppendChild(g_document->CreateTextNode(
                                (wchar_t*)pFragTranslatedBuffer)));
    }  // else if (mathExpression)

    mathExpression = !mathExpression;
  }  // while (*pwcsStr != wcNULL)

  CString frg_string = frag->ToString();
  delete frag;
  wchar_t* frg_buf = frg_string.c_str();
  int frg_buflen = frg_string.length();
  if (frg_buflen < *translatedLength)
    *translatedLength = frg_buflen;
  memcpy(translated_buffer, frg_buf, (*translatedLength) * sizeof(wchar_t));

  return 1;
}  // back_translate_math_widestring

extern "C" {

int back_translate_math_string(widechar* text_buffer,
                               int textLength,
                               widechar* translated_buffer,
                               int* translatedLength) {
  return back_translate_math_widestring((wchar_t*)text_buffer, textLength,
                                        (wchar_t*)translated_buffer,
                                        translatedLength);
}

}  // extern "C"
