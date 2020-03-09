/* liblouisutdml Braille Transcription Library

   This file may contain code borrowed from the Linux screenreader
   BRLTTY, copyright (C) 1999-2006 by
   the BRLTTY Team

   Copyright (C) 2004, 2005, 2006
   ViewPlus Technologies, Inc. www.viewplus.com
   and
   JJB Software, Inc. www.jjb-software.com
   All rights reserved

   This file is free software; you can redistribute it and/or modify it
   under the terms of the Lesser or Library GNU General Public License
   as published by the
   Free Software Foundation; either version 3, or (at your option) any
   later version.

   This file is distributed in the hope that it will be useful, but
   WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   Library GNU General Public License for more details.

   You should have received a copy of the Library GNU General Public
   License along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.

   Maintained by Daniel Kocielinski daniel.koc@gmail.com
   */

#include <ctype.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <louis.h>

#include "louisutdml.h"
#include "louisutdml_backtranslation.h"

int widecharlen(const widechar *s) {
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

void widecharncpy(widechar* to, const widechar* from, int length) {
  int k;
  if (length < 0) {
      for (k = 0; from[k]; k++)
	to[k] = from[k];
    } else {
      for (k = 0; k < length && from[k]; k++)
	to[k] = from[k];
    }
  to[k] = 0;
}

int widecharcmp(const widechar *s1, const widechar *s2) {
  widechar c1, c2;
  do {
      c1 = *s1++;
      c2 = *s2++;
      if (c2 == 0)
        return c1 - c2;
    } while (c1 == c2);
  return c1 < c2 ? -1 : 1;
}

void widecharcat(widechar* to, const widechar* from) {
  int len = widecharlen(to);
  widecharncpy(&to[len], from, -1);
}

// returns true if X and Y are same
int compare(const widechar* X, const widechar* Y) {
  while (*X && *Y) {
    if (*X != *Y)
      return 0;
    X++;
    Y++;
  }

  return (*Y == 0);
}

const widechar* widecharstr(const widechar* X, const widechar* Y) {
  while (*X != 0) {
    if ((*X == *Y) && compare(X, Y))
      return X;
    X++;
  }

  return NULL;
}


#define WIDESTRING_BUFFER_SIZE 102400
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
  inStringLen = widecharlen(inString);
  if (length < inStringLen)
    inStringLen = length;
  memcpy(newString, inString, inStringLen*CHARSIZE);
  newString[inStringLen] = 0;
  widestring_buf_len += length + 1;
  return newString;
}


#define MAX_ACTIONS 20

typedef struct {
  const char* fileName;
  FILE* in;
  int lineNumber;
  char line[5 * MAXNAMELEN];
  int actionsCount;
  char* action[MAX_ACTIONS];
  int actionLength[MAX_ACTIONS];
  char* value[MAX_ACTIONS];
  int valueLength[MAX_ACTIONS];
  widechar* wideValue;
  int wideValueLength;
} FileInfo;

static int errorCount = 0;

LMSymbol** LMsymbols = NULL;
int MaxLMsymbolsCount = 0;
int LMsymbolsCount = 0;
LMSymbol** LMTempsymbols = NULL;
int MaxLMTempsymbolsCount = 0;
int LMTempsymbolsCount = 0;

int LMsymbolsSorted = 0;

#define LMSYMBOLS_RESIZING_STEP 100

LMSymbol* CreateNewLMSymbol() {
  LMSymbol* symbol;
  if (!(symbol = malloc(sizeof(LMSymbol))))
    memoryError();
  memset(symbol, 0, sizeof(LMSymbol));

  symbol->input = NULL;
  symbol->rinput = NULL;
  symbol->tag = NULL;
  symbol->rtag = NULL;
  symbol->output = NULL;
  symbol->ieoutput = NULL;
  symbol->atname = NULL;
  symbol->atval = NULL;
  symbol->ieval = NULL;
  symbol->ttype = TTYPE_CONST;
  symbol->tsubtype = TSUBTYPE_UNDEFINED;
  symbol->invisible = BOOL_UNDEFINED;
  symbol->func = BOOL_UNDEFINED;
  symbol->acc = BOOL_UNDEFINED;
  symbol->codes = NULL;

  if (LMsymbols == NULL) {
    MaxLMsymbolsCount = LMSYMBOLS_RESIZING_STEP;
    if (!(LMsymbols = malloc(MaxLMsymbolsCount * sizeof(LMSymbol*))))
      memoryError();
  } else if (LMsymbolsCount == MaxLMsymbolsCount) {
    LMSymbol** tmpLMsymbols;
    int i;
    MaxLMsymbolsCount += LMSYMBOLS_RESIZING_STEP;
    if (!(tmpLMsymbols = malloc(MaxLMsymbolsCount * sizeof(LMSymbol*))))
      memoryError();
    for (i = 0; i < LMsymbolsCount; i++)
      tmpLMsymbols[i] = LMsymbols[i];
    free(LMsymbols);
    LMsymbols = tmpLMsymbols;
  }

  LMsymbols[LMsymbolsCount++] = symbol;

  return symbol;
}  // CreateNewLMSymbol

LMSymbol* CreateTempLMSymbol(widechar* l_input,
                             widechar* l_tag,
                             widechar* l_output,
                             TokenType l_ttype) {
  LMSymbol* symbol;
  if (!(symbol = malloc(sizeof(LMSymbol))))
    memoryError();
  memset(symbol, 0, sizeof(LMSymbol));

  symbol->input = l_input;
  symbol->rinput = NULL;
  symbol->tag = l_tag;
  symbol->rtag = NULL;
  symbol->output = l_output;
  symbol->ieoutput = NULL;
  symbol->atname = NULL;
  symbol->atval = NULL;
  symbol->ieval = NULL;
  symbol->ttype = l_ttype;
  symbol->tsubtype = TSUBTYPE_UNDEFINED;
  symbol->invisible = BOOL_UNDEFINED;
  symbol->func = BOOL_UNDEFINED;
  symbol->acc = BOOL_UNDEFINED;
  symbol->codes = NULL;

  if (LMTempsymbols == NULL) {
    MaxLMTempsymbolsCount = LMSYMBOLS_RESIZING_STEP;
    if (!(LMTempsymbols = malloc(MaxLMTempsymbolsCount * sizeof(LMSymbol*))))
      memoryError();
  } else if (LMTempsymbolsCount == MaxLMTempsymbolsCount) {
    LMSymbol** tmpLMsymbols;
    int i;
    MaxLMTempsymbolsCount += LMSYMBOLS_RESIZING_STEP;
    if (!(tmpLMsymbols = malloc(MaxLMTempsymbolsCount * sizeof(LMSymbol*))))
      memoryError();
    for (i = 0; i < LMTempsymbolsCount; i++)
      tmpLMsymbols[i] = LMTempsymbols[i];
    free(LMTempsymbols);
    LMTempsymbols = tmpLMsymbols;
  }

  LMTempsymbols[LMTempsymbolsCount++] = symbol;

  return symbol;
}  // CreateTempLMSymbol

void RemoveLastCreatedLMSymbol() {
  if (LMsymbolsCount > 0) {
    LMsymbolsCount--;
    free(LMsymbols[LMsymbolsCount]);
  }
}

void DestroyTempLMSymbols() {
  int i;
  if (LMTempsymbols != NULL) {
    for (i = 0; i < LMTempsymbolsCount; i++)
      free(LMTempsymbols[i]);
    free(LMTempsymbols);
    LMTempsymbols = NULL;
  }
  MaxLMTempsymbolsCount = 0;
  LMTempsymbolsCount = 0;
}

void DestroyLMSymbolsStruct() {
  int i;
  if (LMsymbols != NULL) {
    for (i = 0; i < LMsymbolsCount; i++)
      free(LMsymbols[i]);
    free(LMsymbols);
    LMsymbols = NULL;
  }
  MaxLMsymbolsCount = 0;
  LMsymbolsCount = 0;
  DestroyTempLMSymbols();
  LMsymbolsSorted = 0;
  widestring_buf_len = 0;
}  // DestroyLMSymbolsStruct

static void configureError(FileInfo* nested, char* format, ...) {
  char buffer[1024];
  va_list arguments;
  va_start(arguments, format);
#ifdef _WIN32
  _vsnprintf(buffer, sizeof(buffer), format, arguments);
#else
  vsnprintf(buffer, sizeof(buffer), format, arguments);
#endif
  va_end(arguments);
  if (nested)
    lou_logPrint("%s:%d: %s", nested->fileName, nested->lineNumber, buffer);
  else
    lou_logPrint("%s", buffer);
  errorCount++;
}

static BOOLEAN ConvertIntToBoolean(int v) {
  return (v == 0) ? BOOL_FALSE : ((v == 1) ? BOOL_TRUE : BOOL_UNDEFINED);
}

static unsigned int convertNumericValue(FileInfo* nested,
                                        const char* digits,
                                        int* length) {
  int k;
  unsigned int binaryValue = 0;
  for (k = 0; k < *length; k++) {
    unsigned int hexDigit = 0;
    if (digits[k] >= '0' && digits[k] <= '9')
      hexDigit = digits[k] - '0';
    else if (digits[k] >= 'a' && digits[k] <= 'f')
      hexDigit = digits[k] - 'a' + 10;
    else if (digits[k] >= 'A' && digits[k] <= 'F')
      hexDigit = digits[k] - 'A' + 10;
    else {
      configureError(nested, "invalid %d-digit hexadecimal number", *length);
      *length = k;
      break;
    }
    binaryValue |= hexDigit << (4 * (*length - 1 - k));
  }
  return binaryValue;
}

static int controlCharValue(FileInfo* nested, int actionIndex) {
  /*Decode centrol characters*/
  int unicodeLength;
  unsigned char* value = (unsigned char*)nested->value[actionIndex];
  static widechar decoded[100];
  int decodedLength = 0;
  int k = 0;
  while (k < nested->valueLength[actionIndex]) {
    if (value[k] == '\\') {
      k++;
      switch (value[k]) {
        case '"':
          decoded[decodedLength++] = '"';
          break;
        case '\\':
          decoded[decodedLength++] = '\\';
          break;
        case 'F':
        case 'f':
          decoded[decodedLength++] = 12;
          break;
        case 'N':
        case 'n':
          decoded[decodedLength++] = 10;
          break;
        case 'R':
        case 'r':
          decoded[decodedLength++] = 13;
          break;
        case 'U':
        case 'u':
          unicodeLength = 4;
          decoded[decodedLength++] = (widechar)convertNumericValue(
              nested, &nested->value[actionIndex][k + 1], &unicodeLength);
          k += unicodeLength;
          break;
        default:
          configureError(nested, "invalid value '%s'", value);
          return 0;
      }
      k++;
    } else {
      decoded[decodedLength++] = (widechar)value[k++];
    }
  }
  decoded[decodedLength] = 0;
  nested->wideValue = &decoded[0];
  nested->wideValueLength = decodedLength;
  return 1;
}

#ifdef TESTING_MATH_SYMBOLS
static char* controlWidecharValue(widechar* value) {
  int k = 0;
  static unsigned char buf[200];
  int bufLength = 0;
  int valueLength = widecharlen(value);
  while (k < valueLength) {
    switch (value[k]) {
      case '"':
        buf[bufLength++] = '\\';
        buf[bufLength++] = '"';
        break;
      case '\\':
        buf[bufLength++] = '\\';
        buf[bufLength++] = '\\';
        break;
      case 12:
        buf[bufLength++] = '\\';
        buf[bufLength++] = 'f';
        break;
      case 10:
        buf[bufLength++] = '\\';
        buf[bufLength++] = 'n';
        break;
      case 13:
        buf[bufLength++] = '\\';
        buf[bufLength++] = 'r';
        break;
      default:
        if (value[k] > 127) {
          unsigned int digit, binaryValue = value[k];
          buf[bufLength++] = '\\';
          buf[bufLength++] = 'u';
          digit = (binaryValue & 0xf000) >> 12;
          buf[bufLength++] = (digit < 10) ? ('0' + digit) : ('A' + digit - 10);
          digit = (binaryValue & 0x0f00) >> 8;
          buf[bufLength++] = (digit < 10) ? ('0' + digit) : ('A' + digit - 10);
          digit = (binaryValue & 0x00f0) >> 4;
          buf[bufLength++] = (digit < 10) ? ('0' + digit) : ('A' + digit - 10);
          digit = (binaryValue & 0x000f);
          buf[bufLength++] = (digit < 10) ? ('0' + digit) : ('A' + digit - 10);
        } else {
          buf[bufLength++] = (unsigned char)value[k];
        }
    }
    k++;
  }
  buf[bufLength] = 0;
  return (char*)&buf[0];
}
#endif  // TESTING_MATH_SYMBOLS

static int getLine(FileInfo* nested) {
  int lineLen = 0;
  int ch;
  int pch = 0;
  while ((ch = fgetc(nested->in)) != EOF) {
    if (ch == 13)
      continue;
    if (pch == '\\' && ch == 10) {
      lineLen--;
      continue;
    }
    if (ch == 10 || lineLen > (sizeof(nested->line) - 2))
      break;
    nested->line[lineLen++] = ch;
    pch = ch;
  }
  nested->line[lineLen] = 0;
  if (lineLen == 0 && ch == EOF)
    return 0;
  return 1;
}

static int parseLine(FileInfo* nested) {
  char* curchar = NULL;
  int ch = 0, i;
  nested->actionsCount = 0;
  for (i = 0; i < MAX_ACTIONS; i++) {
    nested->action[i] = "";
    nested->actionLength[i] = 0;
    nested->value[i] = "";
    nested->valueLength[i] = 0;
  }
  while (getLine(nested)) {
    nested->lineNumber++;
    curchar = nested->line;
    while ((ch = *curchar++) <= 32 && ch != 0)
      ;
    if (ch == 0 || ch == '#' || ch == '<')
      continue;
    do {
      nested->actionsCount++;
      nested->action[nested->actionsCount - 1] = curchar - 1;
      while ((ch = *curchar++) > 32 && ch != '=' && ch != ':')
        ;
      nested->actionLength[nested->actionsCount - 1] =
          curchar - nested->action[nested->actionsCount - 1] - 1;
      nested->action[nested->actionsCount -
                     1][nested->actionLength[nested->actionsCount - 1]] = 0;
      while (((ch = *curchar++) <= 32 || ch == '=' || ch == ':') && ch != 0)
        ;
      if (ch == 0)
        break;
      nested->value[nested->actionsCount - 1] = curchar - 1;
      if (*nested->value[nested->actionsCount - 1] == 34) /*quote */
      {
        nested->value[nested->actionsCount - 1]++;
        while ((ch = *curchar++) && ch != 34)
          ;
      } else {
        while ((ch = *curchar++) > 32)
          ;
      }
      nested->valueLength[nested->actionsCount - 1] =
          curchar - nested->value[nested->actionsCount - 1] - 1;
      nested->value[nested->actionsCount -
                    1][nested->valueLength[nested->actionsCount - 1]] = 0;
      if (ch == 0)
        break;
      while ((ch = *curchar++) <= 32 && ch != 0)
        ;
    } while (ch != 0 && ch != '#' && ch != '<');
    return 1;
  }
  return 0;
}

#define NOTFOUND 1000

static int checkActions(FileInfo* nested,
                        const char** actions,
                        int actionIndex) {
  int actionNum = find_action(actions, nested->action[actionIndex]);
  if (actionNum == -1) {
    configureError(nested, "Property name '%s' not recognized",
                   nested->action[actionIndex]);
    return NOTFOUND;
  }
  return actionNum;
}

static int checkValues(FileInfo* nested, const char** values, int actionIndex) {
  int k;
  for (k = 0; values[k]; k += 2)
    if (nested->valueLength[actionIndex] == strlen(values[k]) &&
        ignore_case_comp(values[k], nested->value[actionIndex],
                         nested->valueLength[actionIndex]) == 0)
      break;
  if (values[k] == NULL) {
    configureError(nested, "Property value '%s' not recognized",
                   nested->value[actionIndex]);
    return NOTFOUND;
  }
  return atoi(values[k + 1]);
}

static const char* tokenTypes[] = {"CONST",
                                   "0",
                                   "UNARY",
                                   "1",
                                   "BINARY",
                                   "2",
                                   "INFIX",
                                   "3",
                                   "LEFTBRACKET",
                                   "4",
                                   "RIGHTBRACKET",
                                   "5",
                                   "SPACE",
                                   "6",
                                   "UNDEROVER",
                                   "7",
                                   "DEFINITION",
                                   "8",
                                   "LEFTRIGHT",
                                   "9",
                                   "TEXT",
                                   "10",
                                   "BIG",
                                   "11",
                                   "LONG",
                                   "12",
                                   "STRETCHY",
                                   "13",
                                   "MATRIX",
                                   "14",
                                   "OPERATOR",
                                   "15",
                                   "REVERTUNARY",
                                   "16",
                                   "FENCED",
                                   "17",
                                   NULL};

static const char* tokenSubtypes[] = {"UNDEFINED",
                                      "0",
                                      "VERT",
                                      "1",
                                      "LEFT",
                                      "2",
                                      "DOT",
                                      "3",
                                      "SUBSCRIPT",
                                      "4",
                                      "SUPERSCRIPT",
                                      "5",
                                      "UNDERSCRIPT",
                                      "6",
                                      "OVERSCRIPT",
                                      "7",
                                      "EQNARRAY",
                                      "8",
                                      "ARRAY",
                                      "9",
                                      "VEC",
                                      "10",
                                      "CHECK",
                                      "11",
                                      "BAR",
                                      "12",
                                      "UNDERBRACE",
                                      "13",
                                      "OVERBRACE",
                                      "14",
                                      "UNDERLINE",
                                      "15",
                                      "OVERLINE",
                                      "16",
                                      "UNITSYMBOL",
                                      "17",
                                      "SQRT",
                                      "18",
                                      "SFRAC",
                                      "19",
                                      "FRAC",
                                      "20",
                                      "ROOT",
                                      "21",
                                      "STACKREL",
                                      "22",
                                      "ATOP",
                                      "23",
                                      "CHOOSE",
                                      "24",
                                      NULL};

static int compileSymbolsDefs(FileInfo* nested) {
  static const char* mainActions[] = {
      "input",  "1",  "rinput",   "2",  "tag",      "3",  "rtag",      "4",
      "output", "5",  "ieoutput", "6",  "atname",   "7",  "atval",     "8",
      "ieval",  "9",  "ttype",    "10", "tsubtype", "11", "invisible", "12",
      "func",   "13", "acc",      "14", "codes",    "15", NULL};

  static const char* yesNo[] = {
      "no", "0", "false", "0", "yes", "1", "true", "1", NULL};

  int mainActionNumber, actionIndex, k;
  while (parseLine(nested)) {
    LMSymbol* symbol = CreateNewLMSymbol();
    if (symbol == NULL)
      return 0;

    for (actionIndex = 0; actionIndex < nested->actionsCount; actionIndex++) {
      mainActionNumber = checkActions(nested, mainActions, actionIndex);
    choseMainAction:
      switch (mainActionNumber) {
        case NOTFOUND:
        case 0:
          break;
        case 1:  // input
          if (controlCharValue(nested, actionIndex))
            symbol->input =
                alloc_widestring(nested->wideValue, nested->wideValueLength);
          break;
        case 2:  // rinput
          if (controlCharValue(nested, actionIndex))
            symbol->rinput =
                alloc_widestring(nested->wideValue, nested->wideValueLength);
          break;
        case 3:  // tag
          if (controlCharValue(nested, actionIndex))
            symbol->tag =
                alloc_widestring(nested->wideValue, nested->wideValueLength);
          break;
        case 4:  // rtag
          if (controlCharValue(nested, actionIndex))
            symbol->rtag =
                alloc_widestring(nested->wideValue, nested->wideValueLength);
          break;
        case 5:  // output
          if (controlCharValue(nested, actionIndex))
            symbol->output =
                alloc_widestring(nested->wideValue, nested->wideValueLength);
          break;
        case 6:  // ieoutput
          if (controlCharValue(nested, actionIndex))
            symbol->ieoutput =
                alloc_widestring(nested->wideValue, nested->wideValueLength);
          break;
        case 7:  // atname
          if (controlCharValue(nested, actionIndex))
            symbol->atname =
                alloc_widestring(nested->wideValue, nested->wideValueLength);
          break;
        case 8:  // atval
          if (controlCharValue(nested, actionIndex))
            symbol->atval =
                alloc_widestring(nested->wideValue, nested->wideValueLength);
          break;
        case 9:  // ieval
          if (controlCharValue(nested, actionIndex))
            symbol->ieval =
                alloc_widestring(nested->wideValue, nested->wideValueLength);
          break;
        case 10:  // ttype
          if ((k = checkValues(nested, tokenTypes, actionIndex)) != NOTFOUND)
            symbol->ttype = k;
          break;
        case 11:  // tsubtype
          if ((k = checkValues(nested, tokenSubtypes, actionIndex)) != NOTFOUND)
            symbol->tsubtype = k;
          break;
        case 12:  // invisible
          if ((k = checkValues(nested, yesNo, actionIndex)) != NOTFOUND)
            symbol->invisible = ConvertIntToBoolean(k);
          break;
        case 13:  // func
          if ((k = checkValues(nested, yesNo, actionIndex)) != NOTFOUND)
            symbol->func = ConvertIntToBoolean(k);
          break;
        case 14:  // acc
          if ((k = checkValues(nested, yesNo, actionIndex)) != NOTFOUND)
            symbol->acc = ConvertIntToBoolean(k);
          break;
        case 15:  // codes
          if (controlCharValue(nested, actionIndex))
            symbol->codes =
                alloc_widestring(nested->wideValue, nested->wideValueLength);
          break;
        default:
          RemoveLastCreatedLMSymbol();
          configureError(nested,
                         "Program error in readbacktranslationconfig.c");
          continue;
      }
    }
  }

  return 1;
}  // compileSymbolsDefs

static int initSymbolsDefsFiles(const char* firstConfigFile, char* fileName) {
  char symbolDefsPath[MAXNAMELEN];
  int k;
  strcpy(symbolDefsPath, firstConfigFile);
  for (k = strlen(symbolDefsPath) - 1; k >= 0; k--)
    if (symbolDefsPath[k] == ud->file_separator)
      break;
  strcpy(fileName, &symbolDefsPath[k + 1]);
  if (k < 0)
    k++;
  symbolDefsPath[k] = 0;
  //@  set_paths (symbolDefsPath);
  return 1;
}

int symbols_compileDefinitions(const char* fileName) {
  FileInfo nested;
  char completePath[MAXNAMELEN];
  if (!*fileName)
    return 1; /*Probably run with defaults */
  nested.fileName = fileName;
  nested.lineNumber = 0;
  if (!find_file(fileName, completePath)) {
    configureError(NULL, "Can't find symbol definitions file '%s'", fileName);
    return 0;
  }
  if ((nested.in = fopen((char*)completePath, "rb"))) {
    compileSymbolsDefs(&nested);
    fclose(nested.in);
  } else {
    configureError(NULL, "Can't open symbol definitions file '%s'", fileName);
    return 0;
  }
  return 1;
}

int read_symbol_definitions_file(const char* symbolDefsFileList) {
  int k;
  char mainFile[MAXNAMELEN];
  char subFile[MAXNAMELEN];
  int listLength;
  int currentListPos = 0;
  errorCount = 0;
  widestring_buf_len = 0;
  DestroyLMSymbolsStruct();

  /*Process file list*/
  if (symbolDefsFileList == NULL) {
    //@      set_paths (NULL);
    if (!(symbols_compileDefinitions("math_backtranslation_symbols.defs")))
      return 0;
  } else {
    listLength = strlen(symbolDefsFileList);
    for (k = 0; k < listLength; k++)
      if (symbolDefsFileList[k] == ',')
        break;
    if (k == listLength || k == 0) { /* Only one file */
      initSymbolsDefsFiles(symbolDefsFileList, mainFile);
      symbols_compileDefinitions(mainFile);
    } else { /* Compile a list of files */
      strncpy(subFile, symbolDefsFileList, k);
      subFile[k] = 0;
      initSymbolsDefsFiles(subFile, mainFile);
      currentListPos = k + 1;
      symbols_compileDefinitions(mainFile);
      while (currentListPos < listLength) {
        for (k = currentListPos; k < listLength; k++)
          if (symbolDefsFileList[k] == ',')
            break;
        strncpy(subFile, &symbolDefsFileList[currentListPos],
                k - currentListPos);
        subFile[k - currentListPos] = 0;
        symbols_compileDefinitions(subFile);
        currentListPos = k + 1;
      }
    }
  }

#ifdef TESTING_MATH_SYMBOLS
  FILE* out = fopen("math_backtranslation_symbols_temp.defs", "wb");
  if (!out)
    return 1;
  for ( k = 0; k < LMsymbolsCount; k++) {
    if (LMsymbols[k]->input != NULL)
      fprintf(out, "input:\"%s\"", controlWidecharValue(LMsymbols[k]->input));
    if (LMsymbols[k]->rinput != NULL)
      fprintf(out, "\trinput:\"%s\"", controlWidecharValue(LMsymbols[k]->rinput));
    if (LMsymbols[k]->tag != NULL)
      fprintf(out, "\ttag:\"%s\"", controlWidecharValue(LMsymbols[k]->tag));
    if (LMsymbols[k]->rtag != NULL)
      fprintf(out, "\trtag:\"%s\"", controlWidecharValue(LMsymbols[k]->rtag));
    if (LMsymbols[k]->output != NULL)
      fprintf(out, "\toutput:\"%s\"", controlWidecharValue(LMsymbols[k]->output));
    if (LMsymbols[k]->ieoutput != NULL)
      fprintf(out, "\tieoutput:\"%s\"",
          controlWidecharValue(LMsymbols[k]->ieoutput));
    if (LMsymbols[k]->atname != NULL)
      fprintf(out, "\tatname:\"%s\"", controlWidecharValue(LMsymbols[k]->atname));
    if (LMsymbols[k]->atval != NULL)
      fprintf(out, "\tatval:\"%s\"", controlWidecharValue(LMsymbols[k]->atval));
    if (LMsymbols[k]->ieval != NULL)
      fprintf(out, "\tieval:\"%s\"", controlWidecharValue(LMsymbols[k]->ieval));
    fprintf(out, "\tttype:%s", tokenTypes[2 * (int)LMsymbols[k]->ttype]);
    if (LMsymbols[k]->tsubtype != TSUBTYPE_UNDEFINED)
      fprintf(out, "\ttsubtype:%s", tokenSubtypes[2 * (int)LMsymbols[k]->tsubtype]);
    if (LMsymbols[k]->invisible != BOOL_UNDEFINED)
      fprintf(out, "\tinvisible:%s", ((LMsymbols[k]->invisible == BOOL_TRUE) ?
          "true" : "false"));
    if (LMsymbols[k]->func != BOOL_UNDEFINED)
      fprintf(out, "\tfunc:%s", ((LMsymbols[k]->func == BOOL_TRUE) ? "true" :
          "false"));
    if (LMsymbols[k]->acc != BOOL_UNDEFINED)
      fprintf(out, "\tacc:%s", ((LMsymbols[k]->acc == BOOL_TRUE) ? "true" :
          "false"));
    if (LMsymbols[k]->codes != NULL)
      fprintf(out, "\tcodes:%s", controlWidecharValue(LMsymbols[k]->codes));
    fprintf(out, "\r\n");
  }
  fclose(out);
  #endif  // TESTING_MATH_SYMBOLS

  return 1;
}
