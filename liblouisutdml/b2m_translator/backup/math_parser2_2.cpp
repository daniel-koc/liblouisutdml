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

const wchar_t wcNULL = L'\0';

static CDOMDocument* g_document = NULL;

static wchar_t* MATH_EXPR_SEPARATOR = L"_.";
static wchar_t* g_mathColor = L"blue";        // change it to "" (to inherit) or another color
static wchar_t* g_mathFontSize = L"1em";      // change to e.g. 1.2em for larger math
static wchar_t* g_mathFontFamily = L"mathvariant";  // change to "" to inherit (works in IE) 
                               // or another family (e.g. "arial")
static bool g_displayStyle = true;      // puts limits above and below large operators

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

TokenType LMpreviousSymbol, LMcurrentSymbol;

typedef void* LPVOID;

#define WIDESTRING_BUFFER_SIZE 8 * BUFSIZE
static wchar_t widestring_buffer[WIDESTRING_BUFFER_SIZE];
static int widestring_buf_len = 0;

static wchar_t*
alloc_widestring(const wchar_t* inString, int length)
{
  wchar_t *newString;
int inStringLen;
  if (inString == NULL)
    return NULL;
  if ((length + widestring_buf_len) >= WIDESTRING_BUFFER_SIZE)
    memoryError();
  newString = &widestring_buffer[widestring_buf_len];
inStringLen = (int) wcslen(inString);
if (length < inStringLen)
inStringLen = length;
  wcsncpy (newString, inString, inStringLen);
newString[inStringLen] = wcNULL;
  widestring_buf_len += length + 1;
  return newString;
}

ParseResult* LMparseExpr(wchar_t* str, bool rightbracket, bool matrix);

bool isIE() {
return false;
}

int CompareLMSymbolInput( LPVOID plmSymbol1, LPVOID plmSymbol2 )
{
return wcscmp( ((LMSymbol*) plmSymbol1)->input, ((LMSymbol*) plmSymbol2)->input );
}  // CompareFileEntriesByName

typedef int (*CompareItemsFunc)( LPVOID p1, LPVOID p2 );
static CompareItemsFunc CompareItems = CompareLMSymbolInput;

#define swap(index1, index2) \
{ t = x[index1]; x[index1] = x[index2]; x[index2] = t; }

#define vecswap(k, l, n) \
        e = k; f = l; \
        for (i=0; i<(n); i++, e++, f++) \
            swap(e, f);

#define min(a, b) (((a) <= (b)) ? (a) : (b))

    int med3Sort(LPVOID* x, int a, int b, int c)
{
        return (CompareItems(x[a], x[b]) < 0 ?
                (CompareItems(x[b], x[c]) < 0 ? b : CompareItems(x[a], x[c]) < 0 ? c : a) :
                (CompareItems(x[b], x[c]) > 0 ? b : CompareItems(x[a], x[c]) > 0 ? c : a));
}

    void QuickSort( LPVOID* x, int off, int len )
    {
if (CompareItems == NULL)
{
return;
}

        LPVOID t;
        register int i;
        // Insertion sort on smallest arrays
        if (len < 7) {
            register int j;
            for (i=off; i<len+off; i++)
                for (j=i; j>off && CompareItems(x[j-1],x[j]) > 0; j--)
                    swap(j, j-1);
            return;
        }

        // Choose a partition element, v
        int m = off + (len >> 1);       // Small arrays, middle element
        if (len > 7) {
            register int l = off;
            register int n = off + len - 1;
            if (len > 40) {        // Big arrays, pseudomedian of 9
                register int s = len/8;
                l = med3Sort(x, l,     l+s, l+2*s);
                m = med3Sort(x, m-s,   m,   m+s);
                n = med3Sort(x, n-2*s, n-s, n);
            }
            m = med3Sort(x, l, m, n); // Mid-size, med of 3
        }
        LPVOID v = x[m];

        // Establish Invariant: v* (<v)* (>v)* v*
        register int a = off;
        register int b = a;
        register int c = off + len - 1;
        register int d = c;
        register int e, f, cmp;
        while(1) {
            while (b <= c && (cmp = CompareItems(x[b], v)) <= 0) {
                if (cmp == 0)
                {
                    swap(a, b);
                    a++;
                }
                b++;
            }
            while (c >= b && (cmp = CompareItems(x[c], v)) >= 0) {
                if (cmp == 0)
                {
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
        s = min(a-off, b-a  );  vecswap(off, b-s, s);
        s = min(d-c,   n-d-1);  vecswap(b,   n-s, s);

        // Recursively sort non-partition-elements
        if ((s = b-a) > 1)
            QuickSort(x, off, s);
        if ((s = d-c) > 1)
            QuickSort(x, n-s, s);
    }  // QuickSort

CDOMElement* createXhtmlElement(wchar_t* tagName) {
return g_document->CreateElement(new CString(tagName));
}

CDOMElement* createMmlNode(wchar_t* tagName, CDOMNode* frag) {
CDOMElement* node = g_document->CreateElement(new CString(tagName));
//@  if (isIE) var node = document.createElement("m:"+t);
//@  else var node = document.createElementNS("http://www.w3.org/1998/Math/MathML",t);
  if (frag)
  node->AppendChild(frag);
  return node;
}

wchar_t* LMremoveCharsAndBlanks(wchar_t* str, int n) {
//remove n characters and any following blanks
  wchar_t* st = str + n;
int i = 0;
  for (; st[i] != wcNULL && st[i] <= 32; i++);
return (st + i);
}

int positionInLMsymbols(wchar_t* str, int n) { 
// return position >=n where str appears or would be inserted
// assumes arr is sorted
int i = 0;
  if (n==0) {
    int h, m;
    n = -1;
    h = LMsymbolsCount;
    while (n+1<h) {
      m = (n+h) >> 1;
      if (wcscmp(LM_SYMBOL_NAME(m), str) < 0)
n = m;
else
h = m;
    }
    return h;
  } else
  for (i = n; i < LMsymbolsCount && wcscmp(LM_SYMBOL_NAME(i), str) < 0; i++);
  return i; // i=LMsymbolsCount || LM_SYMBOL_NAME(i) >= str
}  // positionInLMsymbols

LMSymbol* LMgetSymbol(wchar_t* str) {
//return maximal initial substring of str that appears in names
//return NULL if there is none
  int k = 0; //new pos
  int j = 0; //old pos
  int mk; //match pos
  const int MAX_ST_LEN = 200;
  wchar_t st[MAX_ST_LEN+1];
  wchar_t* tagst;
  wchar_t* match = NULL;
  bool more = true;
int strLen, symbolNameLen, i;

  if (str == NULL || *str == wcNULL)
    return NULL;

  strLen = (int) wcslen(str);
  if (strLen > MAX_ST_LEN)
    strLen = MAX_ST_LEN;
  for (i = 1; i < strLen && more; i++) {
    wcsncpy(st, str, i); //initial substring of length i
    st[i] = wcNULL;
    j = k;
    k = positionInLMsymbols(st, j);
//printf("*%d\n", k);
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
  if (match != NULL){
    LMcurrentSymbol = LMsymbols[mk]->ttype;
//wprintf(L"*%s\n", (wchar_t*)match);
    return LMsymbols[mk];
  }
  LMcurrentSymbol = TTYPE_CONST;
  k = 1;
  wchar_t wc = str[0]; //take 1 character
i = 1;
  if (L'0' <= wc && wc <= L'9') {
    tagst = L"mn";
    for (; i < strLen; i++) {
      wc = str[i];
      if (L'0' > wc || wc > L'9')
        break;
    }
  } else
  if ((L'A' <= wc && wc <= L'Z') || (L'a' <= wc && wc <= L'z')) {
    tagst = L"mi";
    for (; i < strLen; i++) {
      wc = str[i];
      if ((L'A' > wc || wc > L'Z') && (L'a' > wc || wc > L'z'))
        break;
    }
  } else
    tagst = L"mo";

// Commented out by DRW (not fully understood, but probably to do with
// use of "/" as an INFIX version of "\\frac", which we don't want):
//}
//if (st=="-" && LMpreviousSymbol==INFIX) {
//  LMcurrentSymbol = INFIX;  //trick "/" into recognizing "-" on second parse
//  return {input:st, tag:tagst, output:st, ttype:UNARY, func:true};
//}

  wchar_t* inout = alloc_widestring(str, i);

  return CreateTempLMSymbol(inout, tagst, inout, TTYPE_CONST);
}  // LMgetSymbol

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

ParseResult* LMparseSexpr(wchar_t* str) { //parses str and returns [node,tailstr,(node)tag]
//printf("LMparseSexpr\n");
  LMSymbol* symbol;
  CDOMElement* node;
  ParseResult* result;
  ParseResult* result2;
wchar_t* atval;
  int i;
  wchar_t* st;
  wchar_t* output;
wchar_t atvalStr[100];
// rightvert = false,

  CDOMDocumentFragment* newFrag = g_document->CreateDocumentFragment();
  str = LMremoveCharsAndBlanks(str,0);
  symbol = LMgetSymbol(str);             //either a token or a bracket or empty
//if (symbol != NULL && symbol->input != NULL)
//  wprintf(L"found symbol: \"%s\"\n", symbol->input);
//else printf("not found symbol\n");
  if (symbol == NULL || symbol->ttype == TTYPE_RIGHTBRACKET)
    return new ParseResult(NULL, str, NULL);
  if (symbol->ttype == TTYPE_DEFINITION) {
    int symbolInputLen = (int) wcslen(symbol->input);
    st = alloc_widestring(symbol->output, (wcslen(symbol->output) + wcslen(str) - symbolInputLen));
    wcscat(st, LMremoveCharsAndBlanks(str, symbolInputLen));
    str = st;
    symbol = LMgetSymbol(str);
    if (symbol == NULL || symbol->ttype == TTYPE_RIGHTBRACKET)
      return new ParseResult(NULL, str, NULL);
  }
  str = LMremoveCharsAndBlanks(str, wcslen(symbol->input));
//wprintf(L"before switch: \"%s\"\n", str->c_str());
  switch (symbol->ttype) {
  case TTYPE_SPACE:
    node = createMmlNode(symbol->tag, NULL);
    node->SetAttribute(new CString(symbol->atname), new CString(symbol->atval));
    return new ParseResult(node, str, symbol->tag);
  case TTYPE_UNDEROVER:
    if (isIE()) {
      if (symbol->rinput != NULL) {   // botch for missing symbols
        st = alloc_widestring(symbol->rinput, (wcslen(symbol->rinput) + wcslen(str)));
        wcscat(st, str);
        str = st;
        symbol = LMgetSymbol(str);
        if (symbol == NULL || symbol->ttype == TTYPE_RIGHTBRACKET)
          return new ParseResult(NULL, str, NULL);
        symbol->ttype = TTYPE_UNDEROVER;
        str = LMremoveCharsAndBlanks(str, wcslen(symbol->input));
      }
    }
    return new ParseResult(createMmlNode(symbol->tag,
			g_document->CreateTextNode(new CString(symbol->output))), str, symbol->tag);
  case TTYPE_CONST:
    output = symbol->output;
    if (isIE() && symbol->ieoutput != NULL)
      output = symbol->ieoutput;
    node = createMmlNode(symbol->tag,g_document->CreateTextNode(new CString(output)));
//wprintf(L"const - output: \"%s\"\n", output);
    return new ParseResult(node, str, symbol->tag);
  case TTYPE_LONG:  // added by DRW
    node = createMmlNode(symbol->tag,g_document->CreateTextNode(new CString(symbol->output)));
    node->SetAttribute(new CString(L"minsize"), new CString(L"1.5"));
    node->SetAttribute(new CString(L"maxsize"), new CString(L"1.5"));
    node = createMmlNode(L"mover", node);
    node->AppendChild(createMmlNode(L"mspace", NULL));
    return new ParseResult(node, str, symbol->tag);
  case TTYPE_STRETCHY: // added by DRW
    output = symbol->output;
    if (isIE() && symbol->ieoutput != NULL)
      symbol->output = symbol->ieoutput;	// doesn't expand, but then nor does "\u2216"
    node = createMmlNode(symbol->tag, g_document->CreateTextNode(new CString(output)));
    if (symbol->tsubtype == TSUBTYPE_VERT) {
      node->SetAttribute(new CString(L"lspace"), new CString(L"0em"));
      node->SetAttribute(new CString(L"rspace"), new CString(L"0em"));
    }
    if (symbol->atval != NULL)
    node->SetAttribute(new CString(L"maxsize"), new CString(symbol->atval));  // don't allow to stretch here
    if (symbol->rtag != NULL)
      return new ParseResult(node, str, symbol->rtag);
    else
      return new ParseResult(node, str, symbol->tag);
  case TTYPE_BIG:  // added by DRW
    atval = symbol->atval;
    symbol = LMgetSymbol(str);
    if (symbol == NULL)
      return new ParseResult(NULL, str, NULL);
    str = LMremoveCharsAndBlanks(str, wcslen(symbol->input));
    node = createMmlNode(symbol->tag,g_document->CreateTextNode(new CString(symbol->output)));
    if (isIE()) {		// to get brackets to expand
      CDOMElement* space = createMmlNode(L"mspace", NULL);
      wcscpy(atvalStr, atval);
      wcscat(atvalStr, L"ex");
      space->SetAttribute(new CString(L"height"), new CString(atvalStr));
      node = createMmlNode(L"mrow", node);
      node->AppendChild(space);
    } else {		// ignored in IE
      node->SetAttribute(new CString(L"minsize"), new CString(atval));
      node->SetAttribute(new CString(L"maxsize"), new CString(atval));
    }
    return new ParseResult(node, str, symbol->tag);
  case TTYPE_LEFTBRACKET:   //read (expr+)
    if (symbol->tsubtype == TSUBTYPE_LEFT) { // left what?
      symbol = LMgetSymbol(str);
      if (symbol != NULL) {
        if (symbol->tsubtype == TSUBTYPE_DOT)
          symbol->invisible = BOOL_TRUE;
        str = LMremoveCharsAndBlanks(str, wcslen(symbol->input));
      }
    }
    result = LMparseExpr(str, true, false);
    if (symbol == NULL ||
	symbol->invisible == BOOL_TRUE)
      node = createMmlNode(L"mrow", result->node);
    else {
      node = createMmlNode(L"mo", g_document->CreateTextNode(new CString(symbol->output)));
      node = createMmlNode(L"mrow", node);
      node->AppendChild(result->node);
    }
    result->node = node;
    return result;
/*
  case TTYPE_MATRIX:	 //read (expr+)
    if (symbol.input == "\\begin{array}") {
      var mask = "";
      symbol = LMgetSymbol(str);
      str = LMremoveCharsAndBlanks(str,0);
      if (symbol == NULL)
	mask = "l";
      else {
	str = LMremoveCharsAndBlanks(str,symbol.input.length);
	if (symbol.input != "{")
	  mask = "l";
	else do {
	  symbol = LMgetSymbol(str);
	  if (symbol != NULL) {
	    str = LMremoveCharsAndBlanks(str,symbol.input.length);
	    if (symbol.input != "}")
	      mask = mask+symbol.input;
	  }
	} while (symbol != NULL && symbol.input != "" && symbol.input != "}");
      }
      result = LMparseExpr("{"+str,true,true);
//    if (result[0] == NULL) return [createMmlNode("mo",
//			   document.createTextNode(symbol.input)),str];
      node = createMmlNode("mtable",result[0]);
      mask = mask.replace(/l/g,"left ");
      mask = mask.replace(/r/g,"right ");
      mask = mask.replace(/c/g,"center ");
      node.setAttribute("columnalign",mask);
      node.setAttribute("displaystyle","false");
      if (isIE)
	return [node,result[1], NULL];
// trying to get a *little* bit of space around the array
// (IE already includes it)
      var lspace = createMmlNode("mspace");
      lspace.setAttribute("width","0.167em");
      var rspace = createMmlNode("mspace");
      rspace.setAttribute("width","0.167em");
      var node1 = createMmlNode("mrow",lspace);
      node1.appendChild(node);
      node1.appendChild(rspace);
      return [node1,result[1], NULL];
    } else {	// eqnarray
      result = LMparseExpr("{"+str,true,true);
      node = createMmlNode("mtable",result[0]);
      if (isIE)
	node.setAttribute("columnspacing","0.25em"); // best in practice?
      else
	node.setAttribute("columnspacing","0.167em"); // correct (but ignored?)
      node.setAttribute("columnalign","right center left");
      node.setAttribute("displaystyle","true");
      node = createMmlNode("mrow",node);
      return [node,result[1], NULL];
    }
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
	node = createMmlNode("mspace");
	node.setAttribute("width","0.33em");	// was 1ex
	newFrag.appendChild(node);
      }
      str = LMremoveCharsAndBlanks(str,i+1);
      return [createMmlNode("mrow",newFrag),str, NULL];
*/
  case TTYPE_UNARY:
      result = LMparseSexpr(str);
      if (result->node == NULL) {
      delete result;
      return new ParseResult(createMmlNode(symbol->tag,
                             g_document->CreateTextNode(new CString(symbol->output))), str, NULL);
    }
/*
      if (symbol->func == BOOL_TRUE) { // functions hack
	st = str.charAt(0);
//	if (st=="^" || st=="_" || st=="/" || st=="|" || st==",") {
	if (st=="^" || st=="_" || st==",") {
	  return [createMmlNode(symbol.tag,
		    document.createTextNode(symbol.output)),str,symbol.tag];
        } else {
	  node = createMmlNode("mrow",
	   createMmlNode(symbol.tag,document.createTextNode(symbol.output)));
	  if (isIE) {
	    var space = createMmlNode("mspace");
	    space.setAttribute("width","0.167em");
	    node.appendChild(space);
	  }
	  node.appendChild(result[0]);
	  return [node,result[1],symbol.tag];
        }
      }
*/
      if (wcscmp(symbol->input, L"\\sqrt") == 0) {		// sqrt
	if (isIE()) {	// set minsize, for \surd
	  CDOMElement* space = createMmlNode(L"Lmspace", NULL);
	  space->SetAttribute(new CString(L"height"), new CString(L"1.2ex"));
	  space->SetAttribute(new CString(L"width"), new CString(L"0em"));	// probably no effect
	  node = createMmlNode(symbol->tag,result->node);
//	  node.setAttribute("minsize","1");	// ignored
//	  node = createMmlNode("mrow",node);  // hopefully unnecessary
	  node->AppendChild(space);
	  result->node = node;
	  result->tag = symbol->tag;
	  return result;
	} else {
	  result->node = createMmlNode(symbol->tag, result->node);
	  result->tag = symbol->tag;
	  return result;
	}
      } else if (symbol->acc == BOOL_TRUE) {   // accent
        node = createMmlNode(symbol->tag, result->node);
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
	CDOMElement* node1 = createMmlNode(L"mo", g_document->CreateTextNode(new CString(output)));
/*
	if (symbol.input == "\\vec" || symbol.input == "\\check")
						// don't allow to stretch
	    node1.setAttribute("maxsize","1.2");
		 // why doesn't "1" work?  \vec nearly disappears in firefox
	if (isIE && symbol.input == "\\bar")
	    node1.setAttribute("maxsize","0.5");
	if (symbol.input == "\\underbrace" || symbol.input == "\\underline")
	  node1.setAttribute("accentunder","true");
	else
	  node1.setAttribute("accent","true");
*/
	node->AppendChild(node1);
/*
	if (symbol.input == "\\overbrace" || symbol.input == "\\underbrace")
	  node.ttype = TTYPE_UNDEROVER;
*/
	result->node = node;
	result->tag = symbol->tag;
	return result;
      } else {			      // font change or displaystyle command
/*
        if (!isIE && typeof symbol.codes != "undefined") {
          for (i=0; i<result[0].childNodes.length; i++)
            if (result[0].childNodes[i].nodeName=="mi" || result[0].nodeName=="mi") {
              st = (result[0].nodeName=="mi"?result[0].firstChild.nodeValue:
                              result[0].childNodes[i].firstChild.nodeValue);
              var newst = [];
              for (var j=0; j<st.length; j++)
                if (st.charCodeAt(j)>64 && st.charCodeAt(j)<91) newst = newst +
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
        node->SetAttribute(new CString(symbol->atname), new CString(symbol->atval));
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
    result = LMparseSexpr(str);
    if (result->node == NULL) {
      delete result;
      return new ParseResult(createMmlNode(L"mo",
			   g_document->CreateTextNode(new CString(symbol->input))), str, NULL);
    }
    result2 = LMparseSexpr(result->str);
    if (result2->node == NULL) {
      delete result;
      delete result2;
      return new ParseResult(createMmlNode(L"mo",
			   g_document->CreateTextNode(new CString(symbol->input))), str, NULL);
    }
    if (wcscmp(symbol->input, L"\\root") == 0 || wcscmp(symbol->input, L"\\stackrel") == 0)
      newFrag->AppendChild(result2->node);
    newFrag->AppendChild(result->node);
    if (wcscmp(symbol->input, L"\\frac") == 0)
newFrag->AppendChild(result2->node);
    str = result2->str;
    delete result;
    delete result2;
    return new ParseResult(createMmlNode(symbol->tag, newFrag), str, symbol->tag);
	case TTYPE_INFIX:
    str = LMremoveCharsAndBlanks(str, wcslen(symbol->input));
    return new ParseResult(createMmlNode(L"mo", g_document->CreateTextNode(new CString(symbol->output))),
	str, symbol->tag);
  default:
    return new ParseResult(createMmlNode(symbol->tag,        //its a constant
	g_document->CreateTextNode(new CString(symbol->output))), str, symbol->tag);
  }
}

ParseResult* LMparseIexpr(wchar_t* str) {
//printf("LMparseIexpr\n");
LMSymbol* symbol;
LMSymbol* sym1;
LMSymbol* sym2;
CDOMNode* node;
ParseResult* result;
wchar_t* tag;
bool underover;
  str = LMremoveCharsAndBlanks(str,0);
//wprintf(L"before LMgetSymbol: \"%s\"\n", str);
  sym1 = LMgetSymbol(str);
//if (sym1 != NULL && sym1->input != NULL)
//wprintf(L"found symbol: %s\"\n", sym1->input);
//else printf("not found symbol\n");
  result = LMparseSexpr(str);
  node = result->node;
  str = result->str;
//wprintf(L"result: \"%s\"\n", str->c_str());
  tag = result->tag;
delete result;
  symbol = LMgetSymbol(str);
  if (symbol != NULL && symbol->ttype == TTYPE_INFIX) {
    str = LMremoveCharsAndBlanks(str, wcslen(symbol->input));
    result = LMparseSexpr(str);
    if (result->node == NULL) // show box in place of missing argument
      result->node = createMmlNode(L"mo", g_document->CreateTextNode(new CString(L"\u25A1")));
    str = result->str;
    tag = result->tag;
    if (wcscmp(symbol->input, L"_") == 0 || wcscmp(symbol->input, L"^") == 0) {
//printf("(wcscmp(symbol->input, L\"_\") == 0 || wcscmp(symbol->input, L\"^\") == 0)\n");
      sym2 = LMgetSymbol(str);
//if (sym2 != NULL)
//wprintf(L"$found symbol: \"%s\"\n", sym2->input);
//else printf("not found symbol!");
tag = NULL;	// no space between x^2 and a following sin, cos, etc.
// This is for \underbrace and \overbrace
      underover = false; //((sym1.ttype == TTYPE_UNDEROVER) || (node.ttype == TTYPE_UNDEROVER));
//    underover = (sym1.ttype == TTYPE_UNDEROVER);
      if (sym2 != NULL && wcscmp(symbol->input, L"_") == 0 && wcscmp(sym2->input, L"^") == 0) {
        str = LMremoveCharsAndBlanks(str, wcslen(sym2->input));
        ParseResult* res2 = LMparseSexpr(str);
	str = res2->str;
	tag = res2->tag;  // leave space between x_1^2 and a following sin etc.
        node = createMmlNode((underover ? L"munderover" : L"msubsup"), node);
        node->AppendChild(result->node);
        node->AppendChild(res2->node);
      } else if (wcscmp(symbol->input, L"_") == 0) {
	node = createMmlNode((underover ? L"munder" : L"msub"), node);
        node->AppendChild(result->node);
      } else {
	node = createMmlNode((underover ? L"mover" : L"msup"), node);
        node->AppendChild(result->node);
      }
      node = createMmlNode(L"mrow", node); // so sum does not stretch
    } else {
      node = createMmlNode(symbol->tag, node);
      if (wcscmp(symbol->input, L"\\atop") == 0 || wcscmp(symbol->input, L"\\choose") == 0)
	((CDOMElement*) node)->SetAttribute(new CString(L"linethickness"), new CString(L"0ex"));
      node->AppendChild(result->node);
      if (wcscmp(symbol->input, L"\\choose") == 0)
	node = createMmlNode(L"mfenced", node);
    }
  }
  return new ParseResult(node, str, tag);
}

ParseResult* LMparseExpr(wchar_t* str, bool rightbracket, bool matrix) {
//printf("*LMparseExpr\n");
LMSymbol* symbol;
CDOMNode* node;
int i;
wchar_t* tag;
ParseResult* result;
  CDOMDocumentFragment* newFrag = g_document->CreateDocumentFragment();
  do {
    str = LMremoveCharsAndBlanks(str, 0);
//wprintf(L"$$before LMparseIexpr: \"%s\"\n", str->c_str());
    result = LMparseIexpr(str);
//wprintf(L"$#after LMparseIexpr: \"%s\"\n", result->str->c_str());
    node = result->node;
    str = result->str;
    tag = result->tag;
    delete result;
//wprintf(L"str: %s, tag: %s\n", str->c_str(), ((result->tag != NULL) ? result->tag : L"NULL"));
    symbol = LMgetSymbol(str);
    if (node != NULL) {
/*
	if ((*tag == L"mn" || *tag == L"mi") && symbol != NULL &&
	typeof symbol.func == "boolean" && symbol.func) {
			// Add space before \sin in 2\sin x or x\sin x
	  var space = createMmlNode("mspace");
	  space.setAttribute("width","0.167em");
	  node = createMmlNode("mrow",node);
	  node.appendChild(space);
      }
*/
      newFrag->AppendChild(node);
    }
  } while (symbol != NULL && (symbol->ttype != TTYPE_RIGHTBRACKET)
        && symbol->output != NULL);
//printf("after loop\n");

tag = NULL;
  if (symbol != NULL && symbol->ttype == TTYPE_RIGHTBRACKET) {
    if (wcscmp(symbol->input, L"\\right") == 0) { // right what?
      str = LMremoveCharsAndBlanks(str, wcslen(symbol->input));
      symbol = LMgetSymbol(str);
      if (symbol != NULL && wcscmp(symbol->input, L".") == 0)
	symbol->invisible = BOOL_TRUE;
      if (symbol != NULL)
	tag = symbol->rtag;
    }
    if (symbol != NULL)
      str = LMremoveCharsAndBlanks(str, wcslen(symbol->input)); // ready to return
/*
    var len = newFrag.childNodes.length;
    if (matrix &&
      len>0 && newFrag.childNodes[len-1].nodeName == "mrow" && len>1 &&
      newFrag.childNodes[len-2].nodeName == "mo" &&
      newFrag.childNodes[len-2].firstChild.nodeValue == "&") { //matrix
	var pos = []; // positions of ampersands
        var m = newFrag.childNodes.length;
        for (i=0; matrix && i<m; i=i+2) {
          pos[i] = [];
          node = newFrag.childNodes[i];
	  for (var j=0; j<node.childNodes.length; j++)
	    if (node.childNodes[j].firstChild.nodeValue=="&")
	      pos[i][pos[i].length]=j;
        }
	var row, frag, n, k, table = document.createDocumentFragment();
	for (i=0; i<m; i=i+2) {
	  row = document.createDocumentFragment();
	  frag = document.createDocumentFragment();
	  node = newFrag.firstChild; // <mrow> -&-&...&-&- </mrow>
	  n = node.childNodes.length;
	  k = 0;
	  for (j=0; j<n; j++) {
	    if (typeof pos[i][k] != "undefined" && j==pos[i][k]){
	      node.removeChild(node.firstChild); //remove &
	      row.appendChild(createMmlNode("mtd",frag));
	      k++;
	    } else frag.appendChild(node.firstChild);
	  }
	  row.appendChild(createMmlNode("mtd",frag));
	  if (newFrag.childNodes.length>2) {
	    newFrag.removeChild(newFrag.firstChild); //remove <mrow> </mrow>
	    newFrag.removeChild(newFrag.firstChild); //remove <mo>&</mo>
	  }
	  table.appendChild(createMmlNode("mtr",row));
	}
	return [table,str];
    }
    if (typeof symbol.invisible != "boolean" || !symbol.invisible) {
      node = createMmlNode("mo",document.createTextNode(symbol.output));
      newFrag.appendChild(node);
    }
*/
  }
  return new ParseResult(newFrag, str, tag);
}

CDOMElement* parseMath(wchar_t* str) {
wprintf(L"*parseMath: \"%s\"\n", str);
// str.replace(/^\s+/g,"")
str = LMremoveCharsAndBlanks(str, 0);
ParseResult* result = LMparseExpr(str, false, false);
  CDOMDocumentFragment* frag = (CDOMDocumentFragment*) result->node;
  delete result;

  CDOMElement* node = createMmlNode(L"mstyle", frag);
  node->SetAttribute(new CString(L"mathcolor"), new CString(g_mathColor));
  node->SetAttribute(new CString(L"fontfamily"), new CString(g_mathFontFamily));
  node->SetAttribute(new CString(L"mathsize"), new CString(g_mathFontSize));
  if (g_displayStyle)
node->SetAttribute(new CString(L"displaystyle"), new CString(L"true"));
  node = createMmlNode(L"math", node);
node->SetAttribute(new CString(L"xmlns"), new CString(L"http://www.w3.org/1998/Math/MathML"));
  return node;
}

int
back_translate_math_widestring(wchar_t* text_buffer, int textLength, wchar_t* translated_buffer, int* translatedLength)
{
if (text_buffer == NULL || textLength <= 0 ||
translated_buffer == NULL || translatedLength == NULL || *translatedLength <= 0)
return 0;
if (LMsymbols == NULL)
return 0;

if (!LMsymbolsSorted) {
QuickSort((LPVOID*)LMsymbols, 0, LMsymbolsCount);
LMsymbolsSorted = 1;
}

widestring_buf_len = 0;
if (g_document == NULL)
g_document = new CDOMDocument();

text_buffer[textLength] = wcNULL;
CString str(L" ");
str += text_buffer;
CStringArray* arr = str.split(MATH_EXPR_SEPARATOR);
printf("split count: %d\n", arr->length());

  CDOMDocumentFragment* frag = g_document->CreateDocumentFragment();
CList queue;
  bool expr = false;
  for (int i=0; i<arr->length(); i++) {
CString* str = arr->at(i);
  widechar* translated_buffer;
int translatedLength = MAX_TRANS_LENGTH;
translated_buffer = (widechar*) str->c_str();
wprintf(L"#%d: \"%s\"\n", i, (wchar_t*) translated_buffer);

    if (expr) {
const wchar_t* upDigits = L"jabcdefghi";
const wchar_t* lowDigits = L"),;:/?+=(*";
CString* arrStr[200];
bool arrBrl[200];
int arrCount = 0;
int startFrac[200];
int startFracCount = 0;
int midFrac[200];
int midFracCount = 0;
int startBrl = 0, index, digitIndex;
wchar_t wcStr[2];
for (index = 0; index < str->length(); index++) {

if (str->charAt(index) == 243) {
if (index > startBrl) {
arrStr[arrCount] = new CString(L" ");
*arrStr[arrCount] += str->substring(startBrl, index);
arrBrl[arrCount++] = true;
}
arrStr[arrCount] = new CString(L"^{");
arrBrl[arrCount++] = false;
for (digitIndex = 0; digitIndex < wcslen(lowDigits); digitIndex++)
if (str->charAt(index+1) == lowDigits[digitIndex])
break;
if (digitIndex < wcslen(lowDigits)) {
while (true) {
wcStr[0] = L'0'+digitIndex;
wcStr[1] = L'\0';
*arrStr[arrCount-1] += wcStr;
index++;
for (digitIndex = 0; digitIndex < wcslen(lowDigits); digitIndex++)
if (str->charAt(index+1) == lowDigits[digitIndex])
break;
if (digitIndex == wcslen(lowDigits))
break;
}  // while
*arrStr[arrCount-1] += CString(L"}");
}  // if (digitIndex < wcslen(lowDigits)) {
startBrl = index+1;
}  // if (str->c_str(index) == 243)
else
if (str->charAt(index) == 185) {
if (index > startBrl) {
arrStr[arrCount] = new CString(L" ");
*arrStr[arrCount] += str->substring(startBrl, index);
arrBrl[arrCount++] = true;
}
arrStr[arrCount] = new CString(L"_{");
arrBrl[arrCount++] = false;
for (digitIndex = 0; digitIndex < wcslen(lowDigits); digitIndex++)
if (str->charAt(index+1) == lowDigits[digitIndex])
break;
if (digitIndex < wcslen(lowDigits)) {
while (true) {
wcStr[0] = L'0'+digitIndex;
wcStr[1] = L'\0';
*arrStr[arrCount-1] += wcStr;
index++;
for (digitIndex = 0; digitIndex < wcslen(lowDigits); digitIndex++)
if (str->charAt(index+1) == lowDigits[digitIndex])
break;
if (digitIndex == wcslen(lowDigits))
break;
}  // while
*arrStr[arrCount-1] += CString(L"}");
}  // if (digitIndex < wcslen(lowDigits))
startBrl = index+1;
}  // if (str->c_str(index) == 185)
else
if (str->charAt(index) == 234) {
if (index > startBrl) {
arrStr[arrCount] = new CString(L" ");
*arrStr[arrCount] += str->substring(startBrl, index);
arrBrl[arrCount++] = true;
}
arrStr[arrCount] = new CString(L"}");
arrBrl[arrCount++] = false;
startBrl = index+1;
}  // if (str->charAt(index) == 234) {
else
if (str->charAt(index) == L'#') {
bool createFrac = false;
int index2 = index;
for (digitIndex = 0; digitIndex < wcslen(upDigits); digitIndex++)
if (str->charAt(index2+1) == upDigits[digitIndex])
break;
if (digitIndex < wcslen(upDigits)) {
while (true) {
index2++;
for (digitIndex = 0; digitIndex < wcslen(upDigits); digitIndex++)
if (str->charAt(index2+1) == upDigits[digitIndex])
break;
if (digitIndex == wcslen(upDigits))
break;
}  // while
for (digitIndex = 0; digitIndex < wcslen(lowDigits); digitIndex++)
if (str->charAt(index2+1) == lowDigits[digitIndex])
break;
if (digitIndex < wcslen(lowDigits))
createFrac = true;
}  // if (digitIndex < wcslen(upDigits)) {
if (createFrac) {
if (index > startBrl) {
arrStr[arrCount] = new CString(L" ");
*arrStr[arrCount] += str->substring(startBrl, index);
arrBrl[arrCount++] = true;
}
arrStr[arrCount] = new CString(L"\\frac{");
arrBrl[arrCount++] = false;
while (true) {
for (digitIndex = 0; digitIndex < wcslen(upDigits); digitIndex++)
if (str->charAt(index+1) == upDigits[digitIndex])
break;
if (digitIndex >= wcslen(upDigits))
break;
wcStr[0] = L'0'+digitIndex;
wcStr[1] = L'\0';
*arrStr[arrCount-1] += wcStr;
index++;
}  // while
*arrStr[arrCount-1] += CString(L"}{");
while (true) {
for (digitIndex = 0; digitIndex < wcslen(lowDigits); digitIndex++)
if (str->charAt(index+1) == lowDigits[digitIndex])
break;
if (digitIndex >= wcslen(lowDigits))
break;
wcStr[0] = L'0'+digitIndex;
wcStr[1] = L'\0';
*arrStr[arrCount-1] += wcStr;
index++;
}  // while
*arrStr[arrCount-1] += CString(L"}");
startBrl = index+1;
}  // if (createFrac)
}  // if (str->charAt(index) == L'#')
else
if (str->charAt(index) == L';')
{
if (index > startBrl) {
arrStr[arrCount] = new CString(L" ");
*arrStr[arrCount] += str->substring(startBrl, index);
arrBrl[arrCount++] = true;
}
startFrac[startFracCount++] = arrCount;
arrStr[arrCount] = new CString(L";");
arrBrl[arrCount++] = true;
startBrl = index+1;
}  // if (str->charAt(index) == L';')
else
if (str->charAt(index) == 128)
{
if (index > startBrl) {
arrStr[arrCount] = new CString(L" ");
*arrStr[arrCount] += str->substring(startBrl, index);
arrBrl[arrCount++] = true;
}
midFrac[midFracCount++] = arrCount;
wcStr[0] = str->charAt(index);
wcStr[1] = L'\0';
arrStr[arrCount] = new CString(wcStr);
arrBrl[arrCount++] = true;
startBrl = index+1;
}  // if (str->charAt(index) == 128)
else
if (str->charAt(index) == L'<' && startFracCount > 0 && midFracCount > 0)
{
if (index > startBrl) {
arrStr[arrCount] = new CString(L" ");
*arrStr[arrCount] += str->substring(startBrl, index);
arrBrl[arrCount++] = true;
}
arrStr[startFrac[--startFracCount]] = new CString(L"\\frac{");
arrBrl[startFrac[startFracCount]] = false;
arrStr[midFrac[--midFracCount]] = new CString(L"}{");
arrBrl[midFrac[midFracCount]] = false;
arrStr[arrCount] = new CString(L"}");
arrBrl[arrCount++] = false;
startBrl = index+1;
}  // if (str->charAt(index) == L'<' && startFracCount > 0 && midFracCount > 0)

}  // for
if (index > startBrl) {
arrStr[arrCount] = new CString(L" ");
*arrStr[arrCount] += str->substring(startBrl, index);
arrBrl[arrCount++] = true;
}

CString* strTranslation = new CString(L" ");
for (index = 0; index < arrCount; index++)
if (!arrBrl[index])
{
*strTranslation += *arrStr[index];
wprintf(L"$%d: \"%s\"\n", i, arrStr[index]->c_str());
} else {
int newIndex = index;
while ((newIndex+1) < arrCount && arrBrl[newIndex+1])
{
newIndex++;
*arrStr[index] += *arrStr[newIndex];
}
translated_buffer = (widechar*) arrStr[index]->c_str();
back_translate_with_mathexpr_table((widechar*) arrStr[index]->c_str(), arrStr[index]->length(), &translated_buffer, &translatedLength);
wprintf(L"^%d: \"%s\"\n", i, (wchar_t*) translated_buffer);
*strTranslation += CString((wchar_t*) translated_buffer);
index = newIndex;
}  // else if

for (index = 0; index < arrCount; index++)
delete arrStr[index];

	CDOMElement* node = parseMath(strTranslation->c_str());
delete strTranslation;

/*
queue.AddItem(node);
while (!queue.IsEmpty())
{
CDOMElement* pTmpElement = (CDOMElement*) queue.RemoveFirst();
CDOMNode* pTmpNode = pTmpElement->GetFirstNode();
while (pTmpNode != NULL)
{
if (pTmpNode->HasChildNodes())
{
queue.AddItem(pTmpNode);
} else
if (pTmpNode->GetNodeType() == DOM_TEXT_NODE)
{
CDOMText* pTmpTextNode = (CDOMText*) pTmpNode;
CString* str = pTmpTextNode->GetData();
  widechar* translated_buffer;
int translatedLength = MAX_TRANS_LENGTH;
translated_buffer = (widechar*) str->c_str();
//wprintf(L"# \"%s\"\n",  (wchar_t*) translated_buffer);
back_translate_with_mathexpr_table((widechar*) str->c_str(), str->length(), &translated_buffer, &translatedLength);
//wprintf(L"% \"%s\"\n",  (wchar_t*) translated_buffer);
CString* strTranslation = new CString((wchar_t*) translated_buffer);
pTmpTextNode->SetData(strTranslation);
}
pTmpNode = pTmpNode->GetNextNodeSibling();
}  // while
}  // while
*/
frag->AppendChild(node);
//      frag->AppendChild(createXhtmlElement(L"math")->AppendChild(g_document->CreateTextNode(strTranslation)));
    } else {
back_translate_with_main_table((widechar*) str->c_str(), str->length(), &translated_buffer, &translatedLength);
wprintf(L"$%d: \"%s\"\n", i, (wchar_t*) translated_buffer);
CString* strTranslation = new CString((wchar_t*) translated_buffer);
      frag->AppendChild(createXhtmlElement(L"span")->AppendChild(g_document->CreateTextNode(strTranslation)));
    }
    expr = !expr;
  }
delete arr;
CString frg_string = frag->ToString();
delete frag;
wchar_t* frg_buf = frg_string.c_str();
int frg_buflen = frg_string.length();
if (frg_buflen < *translatedLength)
*translatedLength = frg_buflen;
memcpy(translated_buffer, frg_buf, (*translatedLength)*sizeof(wchar_t));

return 1;
}  // back_translate_math_widestring


extern "C"
{

int
back_translate_math_string(widechar* text_buffer, int textLength, widechar* translated_buffer, int* translatedLength)
{
  return back_translate_math_widestring((wchar_t*) text_buffer, textLength, (wchar_t*) translated_buffer, translatedLength);
}

}  // extern "C"

