#include "config.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
extern "C" {
#include "../louisutdml.h"
}

#include "string_utils.h"
#include "DOMAttr.h"
#include "DOMDocument.h"
#include "DOMDocumentFragment.h"
#include "DOMElement.h"
#include "DOMText.h"

static CDOMDocument* g_document = NULL;

static wchar_t* MATH_EXPR_SEPARATOR = L"_.";
static wchar_t* g_mathColor = L"blue";        // change it to "" (to inherit) or another color
static wchar_t* g_mathFontSize = L"1em";      // change to e.g. 1.2em for larger math
static wchar_t* g_mathFontFamily = L"serif";  // change to "" to inherit (works in IE) 
                               // or another family (e.g. "arial")
static bool g_displayStyle = true;      // puts limits above and below large operators

typedef enum {
CONST = 0,
UNARY = 1,
BINARY = 2,
INFIX = 3,
LEFTBRACKET = 4,
 RIGHTBRACKET = 5,
SPACE = 6,
UNDEROVER = 7,
DEFINITION = 8,
LEFTRIGHT = 9,
TEXT = 10,
BIG = 11,
LONG = 12,
STRETCHY = 13,
MATRIX = 14
} TokenType;

typedef struct {
wchar_t* input;
wchar_t* tag;
wchar_t* output;
wchar_t* atname;
wchar_t atval;
TokenType ttype;
bool acc;  // accent
} LMSymbol;

LMSymbol LMsymbolsArray[] = {
//Greek letters
{ L"\\alpha", L"mi", L"\u03B1", NULL, NULL, CONST, false },
{ L"\\beta", L"mi", L"\u03B2", NULL, NULL, CONST, false },
{ L"\\gamma", L"mi", L"\u03B3", NULL, NULL, CONST, false },
{ L"\\delta", L"mi", L"\u03B4", NULL, NULL, CONST, false },
{ L"\\epsilon", L"mi", L"\u03B5", NULL, NULL, CONST, false },
{ L"\\varepsilon", L"mi", L"\u025B", NULL, NULL, CONST, false },
{ L"\\zeta", L"mi", L"\u03B6", NULL, NULL, CONST, false },
{ L"\\eta", L"mi", L"\u03B7", NULL, NULL, CONST, false },
{ L"\\theta", L"mi", L"\u03B8", NULL, NULL, CONST, false },
{ L"\\vartheta", L"mi", L"\u03D1", NULL, NULL, CONST, false },
{ L"\\iota", L"mi", L"\u03B9", NULL, NULL, CONST, false },
{ L"\\kappa", L"mi", L"\u03BA", NULL, NULL, CONST, false },
{ L"\\lambda", L"mi", L"\u03BB", NULL, NULL, CONST, false },
{ L"\\mu", L"mi", L"\u03BC", NULL, NULL, CONST, false },
{ L"\\nu", L"mi", L"\u03BD", NULL, NULL, CONST, false },
{ L"\\xi", L"mi", L"\u03BE", NULL, NULL, CONST, false },
{ L"\\pi", L"mi", L"\u03C0", NULL, NULL, CONST, false },
{ L"\\varpi", L"mi", L"\u03D6", NULL, NULL, CONST, false },
{ L"\\rho", L"mi", L"\u03C1", NULL, NULL, CONST, false },
{ L"\\varrho", L"mi", L"\u03F1", NULL, NULL, CONST, false },
{ L"\\varsigma", L"mi", L"\u03C2", NULL, NULL, CONST, false },
{ L"\\sigma", L"mi", L"\u03C3", NULL, NULL, CONST, false },
{ L"\\tau", L"mi", L"\u03C4", NULL, NULL, CONST, false },
{ L"\\upsilon", L"mi", L"\u03C5", NULL, NULL, CONST, false },
{ L"\\phi", L"mi", L"\u03C6", NULL, NULL, CONST, false },
{ L"\\varphi", L"mi", L"\u03D5", NULL, NULL, CONST, false },
{ L"\\chi", L"mi", L"\u03C7", NULL, NULL, CONST, false },
{ L"\\psi", L"mi", L"\u03C8", NULL, NULL, CONST, false },
{ L"\\omega", L"mi", L"\u03C9", NULL, NULL, CONST, false },
{ L"\\Gamma", L"mo", L"\u0393", NULL, NULL, CONST, false },
{ L"\\Delta", L"mo", L"\u0394", NULL, NULL, CONST, false },
{ L"\\Theta", L"mo", L"\u0398", NULL, NULL, CONST, false },
{ L"\\Lambda", L"mo", L"\u039B", NULL, NULL, CONST, false },
{ L"\\Xi", L"mo", L"\u039E", NULL, NULL, CONST, false },
{ L"\\Pi", L"mo", L"\u03A0", NULL, NULL, CONST, false },
{ L"\\Sigma", L"mo", L"\u03A3", NULL, NULL, CONST, false },
{ L"\\Upsilon", L"mo", L"\u03A5", NULL, NULL, CONST, false },
{ L"\\Phi", L"mo", L"\u03A6", NULL, NULL, CONST, false },
{ L"\\Psi", L"mo", L"\u03A8", NULL, NULL, CONST, false },
{ L"\\Omega", L"mo", L"\u03A9", NULL, NULL, CONST, false },

//fractions
{ L"\\frac12", L"mo", L"\u00BD", NULL, NULL, CONST, false },
{ L"\\frac14", L"mo", L"\u00BC", NULL, NULL, CONST, false },
{ L"\\frac34", L"mo", L"\u00BE", NULL, NULL, CONST, false },
{ L"\\frac13", L"mo", L"\u2153", NULL, NULL, CONST, false },
{ L"\\frac23", L"mo", L"\u2154", NULL, NULL, CONST, false },
{ L"\\frac15", L"mo", L"\u2155", NULL, NULL, CONST, false },
{ L"\\frac25", L"mo", L"\u2156", NULL, NULL, CONST, false },
{ L"\\frac35", L"mo", L"\u2157", NULL, NULL, CONST, false },
{ L"\\frac45", L"mo", L"\u2158", NULL, NULL, CONST, false },
{ L"\\frac16", L"mo", L"\u2159", NULL, NULL, CONST, false },
{ L"\\frac56", L"mo", L"\u215A", NULL, NULL, CONST, false },
{ L"\\frac18", L"mo", L"\u215B", NULL, NULL, CONST, false },
{ L"\\frac38", L"mo", L"\u215C", NULL, NULL, CONST, false },
{ L"\\frac58", L"mo", L"\u215D", NULL, NULL, CONST, false },
{ L"\\frac78", L"mo", L"\u215E", NULL, NULL, CONST, false },

//binary operation symbols
{ L"\\pm", L"mo", L"\u00B1", NULL, NULL, CONST, false },
{ L"\\mp", L"mo", L"\u2213", NULL, NULL, CONST, false },
{ L"\\triangleleft", L"mo", L"\u22B2", NULL, NULL, CONST, false },
{ L"\\triangleright", L"mo", L"\u22B3", NULL, NULL, CONST, false },
{ L"\\cdot", L"mo", L"\u22C5", NULL, NULL, CONST, false },
{ L"\\star", L"mo", L"\u22C6", NULL, NULL, CONST, false },
{ L"\\ast", L"mo", L"*" /*L"\u002A"*/, NULL, NULL, CONST, false },
{ L"\\times", L"mo", L"\u00D7", NULL, NULL, CONST, false },
{ L"\\div", L"mo", L"\u00F7", NULL, NULL, CONST, false },
{ L"\\circ", L"mo", L"\u2218", NULL, NULL, CONST, false },
//{ L"\\bullet", L"mo", L"\u2219", NULL, NULL, CONST, false },
{ L"\\bullet", L"mo", L"\u2022", NULL, NULL, CONST, false },
{ L"\\oplus", L"mo", L"\u2295", NULL, NULL, CONST, false },
{ L"\\ominus", L"mo", L"\u2296", NULL, NULL, CONST, false },
{ L"\\otimes", L"mo", L"\u2297", NULL, NULL, CONST, false },
{ L"\\bigcirc", L"mo", L"\u25CB", NULL, NULL, CONST, false },
{ L"\\oslash", L"mo", L"\u2298", NULL, NULL, CONST, false },
{ L"\\odot", L"mo", L"\u2299", NULL, NULL, CONST, false },
{ L"\\land", L"mo", L"\u2227", NULL, NULL, CONST, false },
{ L"\\wedge", L"mo", L"\u2227", NULL, NULL, CONST, false },
{ L"\\lor", L"mo", L"\u2228", NULL, NULL, CONST, false },
{ L"\\vee", L"mo", L"\u2228", NULL, NULL, CONST, false },
{ L"\\cap", L"mo", L"\u2229", NULL, NULL, CONST, false },
{ L"\\cup", L"mo", L"\u222A", NULL, NULL, CONST, false },
{ L"\\sqcap", L"mo", L"\u2293", NULL, NULL, CONST, false },
{ L"\\sqcup", L"mo", L"\u2294", NULL, NULL, CONST, false },
{ L"\\uplus", L"mo", L"\u228E", NULL, NULL, CONST, false },
{ L"\\amalg", L"mo", L"\u2210", NULL, NULL, CONST, false },
{ L"\\bigtriangleup", L"mo", L"\u25B3", NULL, NULL, CONST, false },
{ L"\\bigtriangledown", L"mo", L"\u25BD", NULL, NULL, CONST, false },
{ L"\\dag", L"mo", L"\u2020", NULL, NULL, CONST, false },
{ L"\\dagger", L"mo", L"\u2020", NULL, NULL, CONST, false },
{ L"\\ddag", L"mo", L"\u2021", NULL, NULL, CONST, false },
{ L"\\ddagger", L"mo", L"\u2021", NULL, NULL, CONST, false },
{ L"\\lhd", L"mo", L"\u22B2", NULL, NULL, CONST, false },
{ L"\\rhd", L"mo", L"\u22B3", NULL, NULL, CONST, false },
{ L"\\unlhd", L"mo", L"\u22B4", NULL, NULL, CONST, false },
{ L"\\unrhd", L"mo", L"\u22B5", NULL, NULL, CONST, false },


//BIG Operators
{ L"\\sum", L"mo", L"\u2211", NULL, NULL, UNDEROVER, false },
{ L"\\prod", L"mo", L"\u220F", NULL, NULL, UNDEROVER, false },
{ L"\\ni", L"mo", L"\u220B", NULL, NULL, CONST, false },
{ L"\\owns", L"mo", L"\u220B", NULL, NULL, CONST, false },
{ L"\\propto", L"mo", L"\u221D", NULL, NULL, CONST, false },
{ L"\\vdash", L"mo", L"\u22A2", NULL, NULL, CONST, false },
{ L"\\dashv", L"mo", L"\u22A3", NULL, NULL, CONST, false },
{ L"\\models", L"mo", L"\u22A8", NULL, NULL, CONST, false },
{ L"\\perp", L"mo", L"\u22A5", NULL, NULL, CONST, false },
{ L"\\smile", L"mo", L"\u2323", NULL, NULL, CONST, false },
{ L"\\frown", L"mo", L"\u2322", NULL, NULL, CONST, false },
{ L"\\asymp", L"mo", L"\u224D", NULL, NULL, CONST, false },
{ L"\\notin", L"mo", L"\u2209", NULL, NULL, CONST, false },

//matrices
//#{ L"\\begin{eqnarray}", output:"X", MATRIX, invisible:true},
//#{ L"\\begin{array}", output:"X", MATRIX, invisible:true},
//#{ L"\\\\", output:"}&{", DEFINITION},
//#{ L"\\end{eqnarray}", output:"}}", DEFINITION},
//#{ L"\\end{array}", output:"}}", DEFINITION},

//grouping and literal brackets
//@{ L"\\big", L"mo", L"X", NULL, L"1.2", BIG, false },
//@{ L"\\Big", L"mo", L"X", NULL, L"1.6", BIG, false },
//@{ L"\\bigg", L"mo", L"X", NULL, L"2.2",  BIG, false },
//@{ L"\\Bigg", L"mo", L"X", NULL, L"2.9",  BIG, false },
{ L"\\left", L"mo", L"X", NULL, NULL, LEFTBRACKET, false },
{ L"\\right", L"mo", L"X", NULL, NULL, RIGHTBRACKET, false },
//#{ L"{", output:"{", LEFTBRACKET, false, invisible:true},
//#{ L"}", output:"}", RIGHTBRACKET, false, invisible:true},

//@{ L"(", L"mo", L"(", NULL, L"1", STRETCHY, false },
//@{ L"[", L"mo", L"[", NULL, L"1", STRETCHY, false },
//@{ L"\\lbrack", L"mo", L"[", NULL, L"1", STRETCHY, false },
//@{ L"\\{", L"mo", L"{", NULL, L"1", STRETCHY, false },
//@{ L"\\lbrace", L"mo", L"{", NULL, L"1", STRETCHY, false },
//@{ L"\\langle", L"mo", L"\u2329", NULL, L"1", STRETCHY, false },
//@{ L"\\lfloor", L"mo", L"\u230A", NULL, L"1", STRETCHY, false },
//@{ L"\\lceil", L"mo", L"\u2308", NULL, L"1", STRETCHY, false },

//commands with argument

{ L"\\sqrt", L"msqrt", L"sqrt", NULL, NULL, UNARY, false },
{ L"\\root", L"mroot", L"root", NULL, NULL, BINARY, false },
{ L"\\frac", L"mfrac", L"/", NULL, NULL, BINARY, false },
{ L"\\stackrel", L"mover", L"stackrel", NULL, NULL, BINARY, false },
{ L"\\atop", L"mfrac", L"", NULL, NULL, INFIX, false },
{ L"\\choose", L"mfrac", L"", NULL, NULL, INFIX, false },
{ L"_", L"msub", L"_", NULL, NULL, INFIX, false },
{ L"^", L"msup", L"^", NULL, NULL, INFIX, false },
{ L"\\mathrm", L"mtext", L"text", NULL, NULL, TEXT, false },
{ L"\\mbox", L"mtext", L"mbox", NULL, NULL, TEXT, false }
};

static LMSymbol** LMsymbols = NULL;
static int LMsymbolsCount = 0;
static wchar_t** LMnames = NULL;

typedef void* LPVOID;

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
CString* pstrTagName = new CString(tagName);
return g_document->CreateElement(pstrTagName);
}

CDOMElement* createMmlNode(wchar_t* tagName, CDOMNode* frag) {
CString* pstrTagName = new CString(tagName);
CDOMElement* node = g_document->CreateElement(pstrTagName);
  if (frag)
  node->AppendChild(frag);
  return node;
}
/*
int position(StringArray arr, String str, int n) { 
// return position >=n where str appears or would be inserted
// assumes arr is sorted
  if (n==0) {
    var h,m;
    n = -1;
    h = arr.length;
    while (n+1<h) {
      m = (n+h) >> 1;
      if (arr[m]<str) n = m; else h = m;
    }
    return h;
  } else
    for (var i=n; i<arr.length && arr[i]<str; i++);
  return i; // i=arr.length || arr[i]>=str
}

CString* LMremoveCharsAndBlanks(CString* str, int n) {
//remove n characters and any following blanks
  CString* st = str->substring(n, str->length());
int i = 0;
  for (; i<st->length() && st->charAt(i) <= 32; i++);
if (i == 0)
return st;
CString* st2 = st->substring(i, st->length());
delete st;
return st2;
}

LMSymbol* LMgetSymbol(String str) {
//return maximal initial substring of str that appears in names
//return null if there is none
  var k = 0; //new pos
  var j = 0; //old pos
  var mk; //match pos
  var st;
  var tagst;
  var match = "";
  var more = true;
  for (var i=1; i<=str.length && more; i++) {
    st = str.slice(0,i); //initial substring of length i
    j = k;
    k = position(LMnames, st, j);
    if (k<LMnames.length && str.slice(0,LMnames[k].length)==LMnames[k]){
      match = LMnames[k];
      mk = k;
      i = match.length;
    }
    more = k<LMnames.length && str.slice(0,LMnames[k].length)>=LMnames[k];
  }
  LMpreviousSymbol=LMcurrentSymbol;
  if (match!=""){
    LMcurrentSymbol=LMsymbols[mk].ttype;
    return LMsymbols[mk];
  }
  LMcurrentSymbol=CONST;
  k = 1;
  st = str.slice(0,1); //take 1 character
  if ("0"<=st && st<="9") tagst = "mn";
  else tagst = (("A">st || st>"Z") && ("a">st || st>"z")?"mo":"mi");

// Commented out by DRW (not fully understood, but probably to do with
// use of "/" as an INFIX version of "\\frac", which we don't want):
//}
//if (st=="-" && LMpreviousSymbol==INFIX) {
//  LMcurrentSymbol = INFIX;  //trick "/" into recognizing "-" on second parse
//  return {input:st, tag:tagst, output:st, ttype:UNARY, func:true};
//}

  return {input:st, tag:tagst, output:st, ttype:CONST};
}


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

var LMpreviousSymbol,LMcurrentSymbol;

function LMparseSexpr(str) { //parses str and returns [node,tailstr,(node)tag]
  var symbol, node, result, result2, i, st,// rightvert = false,
    newFrag = document.createDocumentFragment();
  str = LMremoveCharsAndBlanks(str,0);
  symbol = LMgetSymbol(str);             //either a token or a bracket or empty
  if (symbol == null || symbol.ttype == RIGHTBRACKET)
    return [null,str,null];
  if (symbol.ttype == DEFINITION) {
    str = symbol.output+LMremoveCharsAndBlanks(str,symbol.input.length);
    symbol = LMgetSymbol(str);
    if (symbol == null || symbol.ttype == RIGHTBRACKET)
      return [null,str,null];
  }
  str = LMremoveCharsAndBlanks(str,symbol.input.length);
  switch (symbol.ttype) {
  case SPACE:
    node = createMmlNode(symbol.tag);
    node.setAttribute(symbol.atname,symbol.atval);
    return [node,str,symbol.tag];
  case UNDEROVER:
    if (isIE) {
      if (symbol.input.substr(0,4) == "\\big") {   // botch for missing symbols
	str = "\\"+symbol.input.substr(4)+str;	   // make \bigcup = \cup etc.
	symbol = LMgetSymbol(str);
	symbol.ttype = UNDEROVER;
	str = LMremoveCharsAndBlanks(str,symbol.input.length);
      }
    }
    return [createMmlNode(symbol.tag,
			document.createTextNode(symbol.output)),str,symbol.tag];
  case CONST:
    var output = symbol.output;
    if (isIE) {
      if (symbol.input == "'")
	output = "\u2032";
      else if (symbol.input == "''")
	output = "\u2033";
      else if (symbol.input == "'''")
	output = "\u2033\u2032";
      else if (symbol.input == "''''")
	output = "\u2033\u2033";
      else if (symbol.input == "\\square")
	output = "\u25A1";	// same as \Box
      else if (symbol.input.substr(0,5) == "\\frac") {
						// botch for missing fractions
	var denom = symbol.input.substr(6,1);
	if (denom == "5" || denom == "6") {
	  str = symbol.input.replace(/\\frac/,"\\frac ")+str;
	  return [node,str,symbol.tag];
	}
      }
    }
    node = createMmlNode(symbol.tag,document.createTextNode(output));
    return [node,str,symbol.tag];
  case LONG:  // added by DRW
    node = createMmlNode(symbol.tag,document.createTextNode(symbol.output));
    node.setAttribute("minsize","1.5");
    node.setAttribute("maxsize","1.5");
    node = createMmlNode("mover",node);
    node.appendChild(createMmlNode("mspace"));
    return [node,str,symbol.tag];
  case STRETCHY:  // added by DRW
    if (isIE && symbol.input == "\\backslash")
	symbol.output = "\\";	// doesn't expand, but then nor does "\u2216"
    node = createMmlNode(symbol.tag,document.createTextNode(symbol.output));
    if (symbol.input == "|" || symbol.input == "\\vert" ||
	symbol.input == "\\|" || symbol.input == "\\Vert") {
	  node.setAttribute("lspace","0em");
	  node.setAttribute("rspace","0em");
    }
    node.setAttribute("maxsize",symbol.atval);  // don't allow to stretch here
    if (symbol.rtag != null)
      return [node,str,symbol.rtag];
    else
      return [node,str,symbol.tag];
  case BIG:  // added by DRW
    var atval = symbol.atval;
    symbol = LMgetSymbol(str);
    if (symbol == null)
	return [null,str,null];
    str = LMremoveCharsAndBlanks(str,symbol.input.length);
    node = createMmlNode(symbol.tag,document.createTextNode(symbol.output));
    if (isIE) {		// to get brackets to expand
      var space = createMmlNode("mspace");
      space.setAttribute("height",atval+"ex");
      node = createMmlNode("mrow",node);
      node.appendChild(space);
    } else {		// ignored in IE
      node.setAttribute("minsize",atval);
      node.setAttribute("maxsize",atval);
    }
    return [node,str,symbol.tag];
  case LEFTBRACKET:   //read (expr+)
    if (symbol.input == "\\left") { // left what?
      symbol = LMgetSymbol(str);
      if (symbol != null) {
	if (symbol.input == ".")
	  symbol.invisible = true;
	str = LMremoveCharsAndBlanks(str,symbol.input.length);
      }
    }
    result = LMparseExpr(str,true,false);
    if (symbol==null ||
	(typeof symbol.invisible == "boolean" && symbol.invisible))
      node = createMmlNode("mrow",result[0]);
    else {
      node = createMmlNode("mo",document.createTextNode(symbol.output));
      node = createMmlNode("mrow",node);
      node.appendChild(result[0]);
    }
    return [node,result[1],result[2]];
  case MATRIX:	 //read (expr+)
    if (symbol.input == "\\begin{array}") {
      var mask = "";
      symbol = LMgetSymbol(str);
      str = LMremoveCharsAndBlanks(str,0);
      if (symbol == null)
	mask = "l";
      else {
	str = LMremoveCharsAndBlanks(str,symbol.input.length);
	if (symbol.input != "{")
	  mask = "l";
	else do {
	  symbol = LMgetSymbol(str);
	  if (symbol != null) {
	    str = LMremoveCharsAndBlanks(str,symbol.input.length);
	    if (symbol.input != "}")
	      mask = mask+symbol.input;
	  }
	} while (symbol != null && symbol.input != "" && symbol.input != "}");
      }
      result = LMparseExpr("{"+str,true,true);
//    if (result[0]==null) return [createMmlNode("mo",
//			   document.createTextNode(symbol.input)),str];
      node = createMmlNode("mtable",result[0]);
      mask = mask.replace(/l/g,"left ");
      mask = mask.replace(/r/g,"right ");
      mask = mask.replace(/c/g,"center ");
      node.setAttribute("columnalign",mask);
      node.setAttribute("displaystyle","false");
      if (isIE)
	return [node,result[1],null];
// trying to get a *little* bit of space around the array
// (IE already includes it)
      var lspace = createMmlNode("mspace");
      lspace.setAttribute("width","0.167em");
      var rspace = createMmlNode("mspace");
      rspace.setAttribute("width","0.167em");
      var node1 = createMmlNode("mrow",lspace);
      node1.appendChild(node);
      node1.appendChild(rspace);
      return [node1,result[1],null];
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
      return [node,result[1],null];
    }
  case TEXT:
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
      return [createMmlNode("mrow",newFrag),str,null];
  case UNARY:
      result = LMparseSexpr(str);
      if (result[0]==null) return [createMmlNode(symbol.tag,
                             document.createTextNode(symbol.output)),str];
      if (typeof symbol.func == "boolean" && symbol.func) { // functions hack
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
      if (symbol.input == "\\sqrt") {		// sqrt
	if (isIE) {	// set minsize, for \surd
	  var space = createMmlNode("mspace");
	  space.setAttribute("height","1.2ex");
	  space.setAttribute("width","0em");	// probably no effect
	  node = createMmlNode(symbol.tag,result[0])
//	  node.setAttribute("minsize","1");	// ignored
//	  node = createMmlNode("mrow",node);  // hopefully unnecessary
	  node.appendChild(space);
	  return [node,result[1],symbol.tag];
	} else
	  return [createMmlNode(symbol.tag,result[0]),result[1],symbol.tag];
      } else if (typeof symbol.acc == "boolean" && symbol.acc) {   // accent
        node = createMmlNode(symbol.tag,result[0]);
	var output = symbol.output;
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
	var node1 = createMmlNode("mo",document.createTextNode(output));
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
	node.appendChild(node1);
	if (symbol.input == "\\overbrace" || symbol.input == "\\underbrace")
	  node.ttype = UNDEROVER;
	return [node,result[1],symbol.tag];
      } else {			      // font change or displaystyle command
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
        node = createMmlNode(symbol.tag,result[0]);
        node.setAttribute(symbol.atname,symbol.atval);
	if (symbol.input == "\\scriptstyle" ||
	    symbol.input == "\\scriptscriptstyle")
		node.setAttribute("displaystyle","false");
	return [node,result[1],symbol.tag];
      }
  case BINARY:
    result = LMparseSexpr(str);
    if (result[0]==null) return [createMmlNode("mo",
			   document.createTextNode(symbol.input)),str,null];
    result2 = LMparseSexpr(result[1]);
    if (result2[0]==null) return [createMmlNode("mo",
			   document.createTextNode(symbol.input)),str,null];
    if (symbol.input=="\\root" || symbol.input=="\\stackrel")
      newFrag.appendChild(result2[0]);
    newFrag.appendChild(result[0]);
    if (symbol.input=="\\frac") newFrag.appendChild(result2[0]);
    return [createMmlNode(symbol.tag,newFrag),result2[1],symbol.tag];
  case INFIX:
    str = LMremoveCharsAndBlanks(str,symbol.input.length);
    return [createMmlNode("mo",document.createTextNode(symbol.output)),
	str,symbol.tag];
  default:
    return [createMmlNode(symbol.tag,        //its a constant
	document.createTextNode(symbol.output)),str,symbol.tag];
  }
}

function LMparseIexpr(str) {
  var symbol, sym1, sym2, node, result, tag, underover;
  str = LMremoveCharsAndBlanks(str,0);
  sym1 = LMgetSymbol(str);
  result = LMparseSexpr(str);
  node = result[0];
  str = result[1];
  tag = result[2];
  symbol = LMgetSymbol(str);
  if (symbol.ttype == INFIX) {
    str = LMremoveCharsAndBlanks(str,symbol.input.length);
    result = LMparseSexpr(str);
    if (result[0] == null) // show box in place of missing argument
      result[0] = createMmlNode("mo",document.createTextNode("\u25A1"));
    str = result[1];
    tag = result[2];
    if (symbol.input == "_" || symbol.input == "^") {
      sym2 = LMgetSymbol(str);
      tag = null;	// no space between x^2 and a following sin, cos, etc.
// This is for \underbrace and \overbrace
      underover = ((sym1.ttype == UNDEROVER) || (node.ttype == UNDEROVER));
//    underover = (sym1.ttype == UNDEROVER);
      if (symbol.input == "_" && sym2.input == "^") {
        str = LMremoveCharsAndBlanks(str,sym2.input.length);
        var res2 = LMparseSexpr(str);
	str = res2[1];
	tag = res2[2];  // leave space between x_1^2 and a following sin etc.
        node = createMmlNode((underover?"munderover":"msubsup"),node);
        node.appendChild(result[0]);
        node.appendChild(res2[0]);
      } else if (symbol.input == "_") {
	node = createMmlNode((underover?"munder":"msub"),node);
        node.appendChild(result[0]);
      } else {
	node = createMmlNode((underover?"mover":"msup"),node);
        node.appendChild(result[0]);
      }
      node = createMmlNode("mrow",node); // so sum does not stretch
    } else {
      node = createMmlNode(symbol.tag,node);
      if (symbol.input == "\\atop" || symbol.input == "\\choose")
	node.setAttribute("linethickness","0ex");
      node.appendChild(result[0]);
      if (symbol.input == "\\choose")
	node = createMmlNode("mfenced",node);
    }
  }
  return [node,str,tag];
}

function LMparseExpr(str,rightbracket,matrix) {
  var symbol, node, result, i, tag,
  CDOMDocumentFragment* newFrag = g_document->CreateDocumentFragment();
  do {
    str = LMremoveCharsAndBlanks(str,0);
    result = LMparseIexpr(str);
    node = result[0];
    str = result[1];
    tag = result[2];
    symbol = LMgetSymbol(str);
    if (node!=undefined) {
      if ((tag == "mn" || tag == "mi") && symbol!=null &&
	typeof symbol.func == "boolean" && symbol.func) {
			// Add space before \sin in 2\sin x or x\sin x
	  var space = createMmlNode("mspace");
	  space.setAttribute("width","0.167em");
	  node = createMmlNode("mrow",node);
	  node.appendChild(space);
      }
      newFrag.appendChild(node);
    }
  } while ((symbol.ttype != RIGHTBRACKET)
        && symbol!=null && symbol.output!="");
  tag = null;
  if (symbol.ttype == RIGHTBRACKET) {
    if (symbol.input == "\\right") { // right what?
      str = LMremoveCharsAndBlanks(str,symbol.input.length);
      symbol = LMgetSymbol(str);
      if (symbol != null && symbol.input == ".")
	symbol.invisible = true;
      if (symbol != null)
	tag = symbol.rtag;
    }
    if (symbol!=null)
      str = LMremoveCharsAndBlanks(str,symbol.input.length); // ready to return
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
  }
  return [newFrag,str,tag];
}
*/
CDOMElement* parseMath(CString* str) {
// str.replace(/^\s+/g,"")
int i = 0;
while (i < str->length() && str->charAt(i) == L' ')
i++;
//if (i > 0)
//*str = str->substring(i, str->length());
//  CDOMDocumentFragment* frag = LMparseExpr(str, false, false)[0]
  CDOMDocumentFragment* frag = g_document->CreateDocumentFragment();
frag->AppendChild(g_document->CreateTextNode(str));

  CDOMElement* node = createMmlNode(L"mstyle", frag);
  node->SetAttribute(new CString(L"mathcolor"), new CString(g_mathColor));
  node->SetAttribute(new CString(L"fontfamily"), new CString(g_mathFontFamily));
  node->SetAttribute(new CString(L"mathsize"), new CString(g_mathFontSize));
  if (g_displayStyle)
node->SetAttribute(new CString(L"displaystyle"), new CString(L"true"));
  node = createMmlNode(L"math", node);
  return node;
}

int
back_translate_math_widestring(wchar_t* text_buffer, int textLength, wchar_t* translated_buffer, int* translatedLength)
{
if (text_buffer == NULL || textLength <= 0 ||
translated_buffer == NULL || translatedLength == NULL || *translatedLength <= 0)
return 0;

if (LMsymbols == NULL)
{
LMsymbolsCount = sizeof(LMsymbolsArray) / sizeof(LMSymbol);
LMsymbols = new LMSymbol*[LMsymbolsCount];
int i;
for (i = 0; i < LMsymbolsCount; i++)
LMsymbols[i] = &LMsymbolsArray[i];

QuickSort((LPVOID*)LMsymbols, 0, LMsymbolsCount);
char strInput[100];
LMnames = new wchar_t*[LMsymbolsCount];
for (i = 0; i < LMsymbolsCount; i++)
{
LMnames[i] = LMsymbols[i]->input;
wchar_to_utf8((widechar*) LMsymbols[i]->input, strInput, 100);
printf("%s ; %d\n", strInput, i);
}
}  // if (LMsymbols == NULL)

if (g_document == NULL)
g_document = new CDOMDocument();

text_buffer[textLength] = 0;
CString str(L" ");
str += text_buffer;
CStringArray* arr = str.split(MATH_EXPR_SEPARATOR);
printf("split count: %d\n", arr->length());

  CDOMDocumentFragment* frag = g_document->CreateDocumentFragment();
CList queue;
  bool expr = false;
  for (int i=0; i<arr->length(); i++) {
wprintf(L"%d: \"%s\"\n", i, arr->at(i)->c_str());
    if (expr) {
CDOMElement* node = parseMath(arr->at(i));

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
wprintf(L"# \"%s\"\n",  (wchar_t*) translated_buffer);
back_translate_with_mathexpr_table((widechar*) str->c_str(), str->length(), &translated_buffer, &translatedLength);
wprintf(L"% \"%s\"\n",  (wchar_t*) translated_buffer);
CString* strTranslation = new CString((wchar_t*) translated_buffer);
pTmpTextNode->SetData(strTranslation);
}
pTmpNode = pTmpNode->GetNextNodeSibling();
}  // while
}  // while

frag->AppendChild(node);
    } else {
CString* str = arr->at(i);
  widechar* translated_buffer;
int translatedLength = MAX_TRANS_LENGTH;
translated_buffer = (widechar*) str->c_str();
wprintf(L"#%d: \"%s\"\n", i, (wchar_t*) translated_buffer);
back_translate_with_main_table((widechar*) str->c_str(), str->length(), &translated_buffer, &translatedLength);
wprintf(L"$%d: \"%s\"\n", i, (wchar_t*) translated_buffer);
back_translate_with_main_table((widechar*) str->c_str(), str->length(), &translated_buffer, &translatedLength);
wprintf(L"2$%d: \"%s\"\n", i, (wchar_t*) translated_buffer);

CString* strTranslation = new CString((wchar_t*) translated_buffer);
      frag->AppendChild(createXhtmlElement(L"span")->AppendChild(g_document->CreateTextNode(strTranslation)));
    }
    expr = !expr;
  }

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
