#ifndef liblouisutdml_forback_h
#define liblouisutdml_forback_h

#include <libxml/parser.h>
#include "liblouisutdml.h"
#include <louis.h>

typedef enum
{
BOOL_UNDEFINED = -1,
BOOL_FALSE = 0,
BOOL_TRUE = 1
} BOOL;

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
  TTYPE_REVERTUNARY = 16
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
} TokenSubtype;

typedef struct
{
wchar_t* input;
wchar_t* rinput;
wchar_t* tag;
wchar_t* rtag;
wchar_t* output;
wchar_t* ieoutput;
wchar_t* atname;
wchar_t* atval;
wchar_t* ieval;
TokenType ttype;
TokenSubtype tsubtype;
BOOL invisible;
BOOL func;
BOOL acc;  // accent
wchar_t* codes;
} LMSymbol;

extern LMSymbol** LMsymbols;
extern int LMsymbolsCount;
extern int LMsymbolsSorted;

#define LM_SYMBOL_NAME(index) LMsymbols[index]->input

LMSymbol* CreateNewLMSymbol();
LMSymbol* CreateTempLMSymbol(wchar_t* l_input, wchar_t* l_tag, wchar_t* l_output, TokenType l_ttype);
void DestroyTempLMSymbols();

void
memoryError ();
char *
alloc_string (const char *inString);
char *
alloc_string_if_not (const char *inString);
int
file_exists (const char *completePath);
int
find_file (const char *fileName, char *filePath);
int
ignore_case_comp (const char *str1, const char *str2, int length);
int
find_action (const char **actions, const char *action);

#endif /*louisutdml_forback_h */
