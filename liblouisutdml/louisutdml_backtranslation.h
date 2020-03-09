#ifndef liblouisutdml_forback_h
#define liblouisutdml_forback_h

#include <libxml/parser.h>
#include "liblouisutdml.h"
#include <louis.h>

int widecharlen(const widechar *s);
void widecharncpy(widechar* to, const widechar* from, int length);
int widecharcmp(const widechar *s1, const widechar *s2);
void widecharcat(widechar* to, const widechar* from);
const widechar* widecharstr(const widechar* X, const widechar* Y);

typedef enum {
BOOL_UNDEFINED = -1,
BOOL_FALSE = 0,
BOOL_TRUE = 1
} BOOLEAN;

typedef enum
{
  TTYPE_CONST = 0,
  TTYPE_UNARY = 1,
  TTYPE_BINARY = 2,
  TTYPE_INFIX = 3,
  TTYPE_LEFTBRACKET = 4,
  TTYPE_RIGHTBRACKET = 5,
  TTYPE_SPACE = 6,
  TTYPE_UNDEROVER = 7,
  TTYPE_DEFINITION = 8,
  TTYPE_LEFTRIGHT = 9,
  TTYPE_TEXT = 10,
  TTYPE_BIG = 11,
  TTYPE_LONG = 12,
  TTYPE_STRETCHY = 13,
  TTYPE_MATRIX = 14,
  TTYPE_OPERATOR = 15,
  TTYPE_REVERTUNARY = 16,
  TTYPE_FENCED = 17
} TokenType;

typedef enum{
TSUBTYPE_UNDEFINED = 0,
TSUBTYPE_VERT = 1,
TSUBTYPE_LEFT = 2,
TSUBTYPE_DOT = 3,
TSUBTYPE_SUBSCRIPT = 4,
TSUBTYPE_SUPERSCRIPT = 5,
TSUBTYPE_UNDERSCRIPT = 6,
TSUBTYPE_OVERSCRIPT = 7,
TSUBTYPE_EQNARRAY = 8,
TSUBTYPE_ARRAY = 9,
TSUBTYPE_VEC = 10,
TSUBTYPE_CHECK = 11,
TSUBTYPE_BAR = 12,
TSUBTYPE_UNDERBRACE = 13,
TSUBTYPE_OVERBRACE = 14,
TSUBTYPE_UNDERLINE = 15,
TSUBTYPE_OVERLINE = 16,
TSUBTYPE_UNITSYMBOL = 17,
TSUBTYPE_SQRT = 18,
TSUBTYPE_SFRAC = 19,
TSUBTYPE_FRAC = 20,
TSUBTYPE_ROOT = 21,
TSUBTYPE_STACKREL = 22,
TSUBTYPE_ATOP = 23,
TSUBTYPE_CHOOSE = 24,
} TokenSubtype;

typedef struct
{
widechar* input;
widechar* rinput;
widechar* tag;
widechar* rtag;
widechar* output;
widechar* ieoutput;
widechar* atname;
widechar* atval;
widechar* ieval;
TokenType ttype;
TokenSubtype tsubtype;
BOOLEAN invisible;
BOOLEAN func;
BOOLEAN acc;  // accent
widechar* codes;
} LMSymbol;

extern LMSymbol** LMsymbols;
extern int LMsymbolsCount;
extern int LMsymbolsSorted;

#define LM_SYMBOL_NAME(index) LMsymbols[index]->input

LMSymbol* CreateNewLMSymbol();
LMSymbol* CreateTempLMSymbol(widechar* l_input, widechar* l_tag, widechar* l_output, TokenType l_ttype);
void DestroyTempLMSymbols();

#endif /*louisutdml_forback_h */
