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

   Maintained by John J. Boyer john.boyer@jjb-software.com
   */

#include "config.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "louisutdml.h"
#include <libxml/HTMLparser.h>

UserData *ud = NULL;

#define MAX_VERSION_SIZE 60
#define MAX_CONFIG_FILE_LIST_SIZE 1024
#define MAX_FILE_NAME_SIZE 512
#define MAX_SETTINGS_STRING_SIZE 2048
#define MAX_INBUF_SIZE 10000

static char s_configFileList[MAX_CONFIG_FILE_LIST_SIZE];
static char s_inFileName[MAX_FILE_NAME_SIZE];
static char s_outFileName[MAX_FILE_NAME_SIZE];
static char s_logFileName[MAX_FILE_NAME_SIZE];
static char s_settingsString[MAX_SETTINGS_STRING_SIZE];
static char s_inbuf[MAX_INBUF_SIZE];

_DLL_EXPORT widechar *EXPORT_CALL lbu_versionW ()
{
  static widechar s_wcsVersion[MAX_VERSION_SIZE];
  char* version = lbu_version();
int versionInSize = strlen(version);
int versionOutSize = MAX_VERSION_SIZE;

s_wcsVersion[0] = 0;
utf8_string_to_wc(version, &versionInSize, s_wcsVersion, &versionOutSize);

return s_wcsVersion;
}

_DLL_EXPORT char *EXPORT_CALL
lbu_version ()
{
  static char *version = PACKAGE_VERSION;
  static char bothVersions[MAX_VERSION_SIZE];
  strcpy (bothVersions, version);
  strcat (bothVersions, " ");
  strcat (bothVersions, lou_version ());
  return bothVersions;
}

void
libxml_errors (void *ctx ATTRIBUTE_UNUSED, const char *msg, ...)
{
  va_list args;
  char buffer[MAXNAMELEN];
  va_start (args, msg);
  memset (buffer, 0, sizeof (buffer));
  vsnprintf (buffer, sizeof (buffer) - 4, msg, args);
  va_end (args);
  lou_logPrint ("%s", buffer);
}

static xmlParserCtxt *ctxt;
static int libxml2_initialized = 0;

static void
cleanupLibxml ()
{
  destroy_semantic_table ();
  if (ud->doc != NULL)
    xmlFreeDoc (ud->doc);
  if (!libxml2_initialized)
    return;
  xmlCleanupParser ();
  initGenericErrorDefaultFunc (NULL);
  xmlFreeParserCtxt (ctxt);
}

static void
freeEverything ()
{
  lou_logEnd ();
  cleanupLibxml ();
  lbu_free ();
}

static int
processXmlDocument (const char *inputDoc, int length)
{
  /*This function does all processing of xml documents as such.
   * If length is 0 the document is assumed to be a file.
   * If length is not 0 it is assumed to be in memory.
   * Sort of hackish, but only hackers will see it. */
  xmlNode *rootElement = NULL;
  int haveSemanticFile;
  if (!libxml2_initialized)
    {
      libxml2_initialized = 1;
      LIBXML_TEST_VERSION xmlKeepBlanksDefault (0);
      xmlSubstituteEntitiesDefault (1);
      xmlThrDefIndentTreeOutput (1);
      xmlThrDefKeepBlanksDefaultValue (0);
      xmlThrDefLineNumbersDefaultValue (1);
    }
  ud->doc = NULL;
  ctxt = xmlNewParserCtxt ();
  xmlSetGenericErrorFunc (ctxt, libxml_errors);
  if (length == 0)
    {
      if ((ud->mode & htmlDoc))
	ud->doc = htmlParseFile (inputDoc, NULL);
      else
	{
	  if (ud->internet_access)
	    ud->doc = xmlCtxtReadFile (ctxt, inputDoc, NULL,
				       XML_PARSE_DTDVALID | XML_PARSE_NOENT);
	  else
	    ud->doc = xmlParseFile (inputDoc);
	  if (ud->doc == NULL)
	    {
	      lou_logPrint
		("Document could not be processed; may be \
malformed or contain illegal characters");
	      cleanupLibxml ();
	      return 0;
	    }
	}
    }
  else
    ud->doc = xmlParseMemory (inputDoc, length);
  if (ud->doc == NULL)
    {
      lou_logPrint ("Document could not be processed, probably  malformed");
      cleanupLibxml ();
      return 0;
    }
  if (ud->doc->encoding == NULL)
    {
      lou_logPrint ("Encoding, preferably UTF-8,  must be specified");
      cleanupLibxml ();
      return 0;
    }
  if (ud->format_for >= utd && ignore_case_comp (ud->doc->encoding,
						 "UTF-8", 5) != 0)
    {
      lou_logPrint ("UTDML requires UTF-8 encoding, not '%s'",
		    ud->doc->encoding);
      cleanupLibxml ();
      return 0;
    }
  rootElement = xmlDocGetRootElement (ud->doc);
  if (rootElement == NULL)
    {
      lou_logPrint ("Document is empty");
      cleanupLibxml ();
      return 0;
    }
  if (ud->mode & convertOnly)
    convert_utd ();
  else
    {
      haveSemanticFile = compile_semantic_table (rootElement);
      do_xpath_expr ();
      examine_document (rootElement);
      append_new_entries ();
      /* This will generate a new semantic-action file if none exists 
       * and newEntries is  yes in the configuration file.
       * Otherwise it generates a new_enhtries file.*/
      if (!haveSemanticFile)
	{
	  cleanupLibxml ();
	  return 0;
	}
      if (!transcribe_document (rootElement))
	{
	  lou_logPrint ("Document could not be transcribed");
	  cleanupLibxml ();
	  return 0;
	}
    }
  cleanupLibxml ();
  return 1;
}

_DLL_EXPORT void *EXPORT_CALL lbu_initializeW (
	widechar *configFileList,
	widechar *logFileName,
	widechar *settingsString)
{
int configFileListInSize = 0;
int configFileListOutSize = MAX_CONFIG_FILE_LIST_SIZE;
int logFileNameInSize = 0;
int logFileNameOutSize = MAX_FILE_NAME_SIZE;
int settingsStringInSize = 0;
int settingsStringOutSize = MAX_SETTINGS_STRING_SIZE;

if (configFileList != 0)
{
configFileListInSize = wcslen(configFileList);
wc_string_to_utf8(configFileList, &configFileListInSize, s_configFileList, &configFileListOutSize);
}
if (logFileName != 0)
{
logFileNameInSize = wcslen(logFileName);
wc_string_to_utf8(logFileName, &logFileNameInSize, s_logFileName, &logFileNameOutSize);
}
if (settingsString != 0)
{
settingsStringInSize = wcslen(settingsString);
wc_string_to_utf8(settingsString, &settingsStringInSize, s_settingsString, &settingsStringOutSize);
}

return lbu_initialize (
	((configFileList != 0) ? (const char *)s_configFileList : 0),
	((logFileName != 0) ? (const char *)s_logFileName : 0),
	((settingsString != 0) ? (const char *)s_settingsString : 0));
}

_DLL_EXPORT void *EXPORT_CALL
lbu_initialize (const char *configFileList,
		const char *logFileName, const char *settingsString)
{
  if (!read_configuration_file (configFileList, logFileName,
				settingsString, 0))
    return NULL;
  return (void *) ud;
}

_DLL_EXPORT int EXPORT_CALL lbu_translateStringW(
	widechar *configFileList,
	widechar *inbuf,
	 int inlen,
	widechar *outbuf,
	int *outlen,
	widechar *logFileName,
	widechar *settingsString,
	unsigned int mode)
{
int configFileListInSize = 0;
int configFileListOutSize = MAX_CONFIG_FILE_LIST_SIZE;
int inbufInSize = 0;
int inbufOutSize = MAX_INBUF_SIZE;
int logFileNameInSize = 0;
int logFileNameOutSize = MAX_FILE_NAME_SIZE;
int settingsStringInSize = 0;
int settingsStringOutSize = MAX_SETTINGS_STRING_SIZE;

if (configFileList != 0)
{
configFileListInSize = wcslen(configFileList);
wc_string_to_utf8(configFileList, &configFileListInSize, s_configFileList, &configFileListOutSize);
}
if (inbuf != 0)
{
inbufInSize = wcslen(inbuf);
wc_string_to_utf8(inbuf, &inbufInSize, s_inbuf, &inbufOutSize);
}
if (logFileName != 0)
{
logFileNameInSize = wcslen(logFileName);
wc_string_to_utf8(logFileName, &logFileNameInSize, s_logFileName, &logFileNameOutSize);
}
if (settingsString != 0)
{
settingsStringInSize = wcslen(settingsString);
wc_string_to_utf8(settingsString, &settingsStringInSize, s_settingsString, &settingsStringOutSize);
}

return lbu_translateString(
	((configFileList != 0) ? (const char *)s_configFileList : 0),
	((inbuf != 0) ? (const char *)s_inbuf : 0),
	inlen, outbuf, outlen,
	((logFileName != 0) ? (const char *)s_logFileName : 0),
	((settingsString != 0) ? (const char *)s_settingsString : 0),
	mode);
}

_DLL_EXPORT int EXPORT_CALL
lbu_translateString (const char *configFileList,
		     const char *inbuf, int inlen, widechar * outbuf,
		     int *outlen,
		     const char *logFileName, const char *settingsString,
		     unsigned int mode)
{
/* Translate the well-formed xml expression in inbuf into braille 
* according to the specifications in configFileList. If the expression 
* is not well-formed or there are oteer errors, return 0.*/
  int k;
  char *xmlInbuf;
  if (!read_configuration_file
      (configFileList, logFileName, settingsString, mode))
    return 0;
  if (inbuf == NULL || outbuf == NULL || outlen == NULL)
    return 0;
  ud->inbuf = inbuf;
  ud->inlen = inlen;
  ud->outbuf = outbuf;
  ud->outlen = *outlen;
  ud->inFile = ud->outFile = NULL;
  for (k = 0; k < inlen; k++)
    if (inbuf[k] > ' ')
      break;
  if (inbuf[k] != '<')
    {
      if (ud->format_for == utd)
	k = utd_transcribe_text_string ();
      else
	k = transcribe_text_string ();
      *outlen = ud->outlen_so_far;
      lou_logEnd ();
      return k;
    }
  if (inbuf[k + 1] == '?')
    xmlInbuf = (char *) inbuf;
  else
    {
      inlen += strlen (ud->xml_header);
      if (!(xmlInbuf = malloc (inlen + 4)))
	{
	  lou_logPrint ("Not enough memory");
	  return 0;
	}
      strcpy (xmlInbuf, ud->xml_header);
      strcat (xmlInbuf, "\n");
      strcat (xmlInbuf, inbuf);
    }
  k = processXmlDocument (xmlInbuf, inlen);
  *outlen = ud->outlen_so_far;
  if (xmlInbuf != inbuf)
    free (xmlInbuf);
  lou_logEnd ();
  return k;
}

_DLL_EXPORT int EXPORT_CALL lbu_translateFileW(
	widechar *configFileList,
	widechar *inFileName,
	widechar *outFileName,
	widechar *logFileName,
	widechar *settingsString,
	unsigned int mode)
{
int configFileListInSize = 0;
int configFileListOutSize = MAX_CONFIG_FILE_LIST_SIZE;
int inFileNameInSize = 0;
int inFileNameOutSize = MAX_FILE_NAME_SIZE;
int outFileNameInSize = 0;
int outFileNameOutSize = MAX_FILE_NAME_SIZE;
int logFileNameInSize = 0;
int logFileNameOutSize = MAX_FILE_NAME_SIZE;
int settingsStringInSize = 0;
int settingsStringOutSize = MAX_SETTINGS_STRING_SIZE;

if (configFileList != 0)
{
configFileListInSize = wcslen(configFileList);
wc_string_to_utf8(configFileList, &configFileListInSize, s_configFileList, &configFileListOutSize);
}
if (inFileName != 0)
{
inFileNameInSize = wcslen(inFileName);
wc_string_to_utf8(inFileName, &inFileNameInSize, s_inFileName, &inFileNameOutSize);
}
if (outFileName != 0)
{
outFileNameInSize = wcslen(outFileName);
wc_string_to_utf8(outFileName, &outFileNameInSize, s_outFileName, &outFileNameOutSize);
}
if (logFileName != 0)
{
logFileNameInSize = wcslen(logFileName);
wc_string_to_utf8(logFileName, &logFileNameInSize, s_logFileName, &logFileNameOutSize);
}
if (settingsString != 0)
{
settingsStringInSize = wcslen(settingsString);
wc_string_to_utf8(settingsString, &settingsStringInSize, s_settingsString, &settingsStringOutSize);
}

return lbu_translateFile(
	((configFileList != 0) ? (const char *)s_configFileList : 0),
	((inFileName != 0) ? (const char *)s_inFileName : 0),
	((outFileName != 0) ? (const char *)s_outFileName : 0),
	((logFileName != 0) ? (const char *)s_logFileName : 0),
	((settingsString != 0) ? (const char *)s_settingsString : 0),
	mode);
}

_DLL_EXPORT int
  EXPORT_CALL lbu_translateFile
  (const char *configFileList, const char *inFileName,
   const char *outFileName, const char *logFileName,
   const char *settingsString, unsigned int mode)
{
/* Translate the well-formed xml expression in inFileName into 
* braille according to the specifications in configFileList. If the 
* expression is not well-formed or there are other errors, 
* return 0. */
  int k;
  if (!read_configuration_file
      (configFileList, logFileName, settingsString, mode))
    return 0;
  if (inFileName == NULL || outFileName == NULL)
    return 0;
  if (strcmp (outFileName, "stdout"))
    {
      if (!(ud->outFile = fopen (outFileName, "wb")))
	{
	  lou_logPrint ("Can't open output file %s.", outFileName);
	  return 0;
	}
    }
  else
    ud->outFile = stdout;
  k = processXmlDocument (inFileName, 0);
  if (ud->outFile != stdout)
    fclose (ud->outFile);
  lou_logEnd ();
  return k;
}

_DLL_EXPORT int EXPORT_CALL lbu_translateTextFileW(
	widechar *configFileList,
	widechar *inFileName,
	widechar *outFileName,
	widechar *logFileName,
	widechar *settingsString,
	unsigned int mode)
{
int configFileListInSize = 0;
int configFileListOutSize = MAX_CONFIG_FILE_LIST_SIZE;
int inFileNameInSize = 0;
int inFileNameOutSize = MAX_FILE_NAME_SIZE;
int outFileNameInSize = 0;
int outFileNameOutSize = MAX_FILE_NAME_SIZE;
int logFileNameInSize = 0;
int logFileNameOutSize = MAX_FILE_NAME_SIZE;
int settingsStringInSize = 0;
int settingsStringOutSize = MAX_SETTINGS_STRING_SIZE;

if (configFileList != 0)
{
configFileListInSize = wcslen(configFileList);
wc_string_to_utf8(configFileList, &configFileListInSize, s_configFileList, &configFileListOutSize);
}
if (inFileName != 0)
{
inFileNameInSize = wcslen(inFileName);
wc_string_to_utf8(inFileName, &inFileNameInSize, s_inFileName, &inFileNameOutSize);
}
if (outFileName != 0)
{
outFileNameInSize = wcslen(outFileName);
wc_string_to_utf8(outFileName, &outFileNameInSize, s_outFileName, &outFileNameOutSize);
}
if (logFileName != 0)
{
logFileNameInSize = wcslen(logFileName);
wc_string_to_utf8(logFileName, &logFileNameInSize, s_logFileName, &logFileNameOutSize);
}
if (settingsString != 0)
{
settingsStringInSize = wcslen(settingsString);
wc_string_to_utf8(settingsString, &settingsStringInSize, s_settingsString, &settingsStringOutSize);
}

return lbu_translateTextFile(
	((configFileList != 0) ? (const char *)s_configFileList : 0),
	((inFileName != 0) ? (const char *)s_inFileName : 0),
	((outFileName != 0) ? (const char *)s_outFileName : 0),
	((logFileName != 0) ? (const char *)s_logFileName : 0),
	((settingsString != 0) ? (const char *)s_settingsString : 0),
	mode);
}

_DLL_EXPORT int
  EXPORT_CALL lbu_translateTextFile
  (const char *configFileList, const char *inFileName,
   const char *outFileName, const char *logFileName,
   const char *settingsString, unsigned int mode)
{
/* Translate the text file in inFileName into braille according to
* the specifications in configFileList. If there are errors, print 
* an error message and return 0.*/
  int k;
  widechar outbuf[2 * BUFSIZE];
  if (!read_configuration_file
      (configFileList, logFileName, settingsString, mode))
    return 0;
  if (inFileName == NULL || outFileName == NULL)
    return 0;
  ud->outbuf = outbuf;
  ud->outlen = (sizeof (outbuf) / CHARSIZE) - 4;
  if (strcmp (inFileName, "stdin"))
    {
      if (!(ud->inFile = fopen (inFileName, "rb")))
	{
	  lou_logPrint ("Can't open input file %s.", inFileName);
	  return 0;
	}
    }
  else
    ud->inFile = stdin;
  if (strcmp (outFileName, "stdout"))
    {
      if (!(ud->outFile = fopen (outFileName, "wb")))
	{
	  lou_logPrint ("Can't open output file %s.", outFileName);
	  return 0;
	}
    }
  else
    ud->outFile = stdout;
  if (ud->format_for == utd)
    k = utd_transcribe_text_file ();
  else
    k = transcribe_text_file ();
  if (!k)
    {
      freeEverything ();
      return 0;
    }
  if (ud->inFile != stdin)
    fclose (ud->inFile);
  if (ud->outFile != stdout)
    fclose (ud->outFile);
  lou_logEnd ();
  return k;
}

_DLL_EXPORT int EXPORT_CALL lbu_backTranslateStringW(
	widechar *configFileList,
	widechar *inbuf,
	int inlen,
	widechar *outbuf,
	int *outlen,
	widechar *logFileName,
	widechar *settingsString,
	unsigned int mode)
{
int configFileListInSize = 0;
int configFileListOutSize = MAX_CONFIG_FILE_LIST_SIZE;
int inbufInSize = 0;
int inbufOutSize = MAX_INBUF_SIZE;
int logFileNameInSize = 0;
int logFileNameOutSize = MAX_FILE_NAME_SIZE;
int settingsStringInSize = 0;
int settingsStringOutSize = MAX_SETTINGS_STRING_SIZE;

if (configFileList != 0)
{
configFileListInSize = wcslen(configFileList);
wc_string_to_utf8(configFileList, &configFileListInSize, s_configFileList, &configFileListOutSize);
}
if (inbuf != 0)
{
inbufInSize = wcslen(inbuf);
wc_string_to_utf8(inbuf, &inbufInSize, s_inbuf, &inbufOutSize);
}
if (logFileName != 0)
{
logFileNameInSize = wcslen(logFileName);
wc_string_to_utf8(logFileName, &logFileNameInSize, s_logFileName, &logFileNameOutSize);
}
if (settingsString != 0)
{
settingsStringInSize = wcslen(settingsString);
wc_string_to_utf8(settingsString, &settingsStringInSize, s_settingsString, &settingsStringOutSize);
}

return lbu_backTranslateString(
	((configFileList != 0) ? (const char *)s_configFileList : 0),
	((inbuf != 0) ? (const char *)s_inbuf : 0),
	inlen, outbuf, outlen,
	((logFileName != 0) ? (const char *)s_logFileName : 0),
	((settingsString != 0) ? (const char *)s_settingsString : 0),
	mode);
}

_DLL_EXPORT int EXPORT_CALL
lbu_backTranslateString (const char *configFileList,
			 const char *inbuf, int inlen, widechar
			 * outbuf,
			 int *outlen,
			 const char *logFileName, const char
			 *settingsString, unsigned int mode)
{
  int k;
  if (!read_configuration_file
      (configFileList, logFileName, settingsString, mode))
    return 0;
  if (inbuf == NULL || outbuf == NULL || outlen == NULL)
    return 0;
  ud->inbuf = inbuf;
  ud->inlen = inlen;
  ud->outbuf = outbuf;
  ud->outlen = *outlen;
  ud->inFile = ud->outFile = NULL;
  if (ud->format_for == utd)
    k = utd_back_translate_braille_string ();
  else
    k = back_translate_braille_string ();
  if (!k)
    {
      freeEverything ();
      return 0;
    }
  *outlen = ud->outlen_so_far;
  lou_logEnd ();
  return 1;
}

_DLL_EXPORT int EXPORT_CALL lbu_backTranslateFileW(
widechar *configFileList,
widechar *inFileName,
widechar *outFileName,
widechar *logFileName,
widechar *settingsString,
unsigned int mode)
{
int configFileListInSize = 0;
int configFileListOutSize = MAX_CONFIG_FILE_LIST_SIZE;
int inFileNameInSize = 0;
int inFileNameOutSize = MAX_FILE_NAME_SIZE;
int outFileNameInSize = 0;
int outFileNameOutSize = MAX_FILE_NAME_SIZE;
int logFileNameInSize = 0;
int logFileNameOutSize = MAX_FILE_NAME_SIZE;
int settingsStringInSize = 0;
int settingsStringOutSize = MAX_SETTINGS_STRING_SIZE;

if (configFileList != 0)
{
configFileListInSize = wcslen(configFileList);
wc_string_to_utf8(configFileList, &configFileListInSize, s_configFileList, &configFileListOutSize);
}
if (inFileName != 0)
{
inFileNameInSize = wcslen(inFileName);
wc_string_to_utf8(inFileName, &inFileNameInSize, s_inFileName, &inFileNameOutSize);
}
if (outFileName != 0)
{
outFileNameInSize = wcslen(outFileName);
wc_string_to_utf8(outFileName, &outFileNameInSize, s_outFileName, &outFileNameOutSize);
}
if (logFileName != 0)
{
logFileNameInSize = wcslen(logFileName);
wc_string_to_utf8(logFileName, &logFileNameInSize, s_logFileName, &logFileNameOutSize);
}
if (settingsString != 0)
{
settingsStringInSize = wcslen(settingsString);
wc_string_to_utf8(settingsString, &settingsStringInSize, s_settingsString, &settingsStringOutSize);
}

return lbu_backTranslateFile(
	((configFileList != 0) ? (const char *)s_configFileList : 0),
	((inFileName != 0) ? (const char *)s_inFileName : 0),
	((outFileName != 0) ? (const char *)s_outFileName : 0),
	((logFileName != 0) ? (const char *)s_logFileName : 0),
	((settingsString != 0) ? (const char *)s_settingsString : 0),
	mode);
}

_DLL_EXPORT int
  EXPORT_CALL lbu_backTranslateFile
  (const char *configFileList, const char *inFileName,
   const char *outFileName, const char *logFileName,
   const char *settingsString, unsigned int mode)
{
/* Back translate the braille file in inFileName into either an 
* xml file or a text file according to
* the specifications in configFileList. If there are errors, print an 
* error message and return 0.*/
  int k;
  widechar outbuf[2 * BUFSIZE];
  if (!read_configuration_file
      (configFileList, logFileName, settingsString, mode))
    return 0;
  if (inFileName == NULL || outFileName == NULL)
    return 0;
  ud->outbuf = outbuf;
  ud->outlen = (sizeof (outbuf) / CHARSIZE) - 4;
  if (strcmp (inFileName, "stdin"))
    {
      if (!(ud->inFile = fopen (inFileName, "rb")))
	{
	  lou_logPrint ("Can't open input file %s.", inFileName);
	  return 0;
	}
    }
  else
    ud->inFile = stdin;
  if (strcmp (outFileName, "stdout"))
    {
      if (!(ud->outFile = fopen (outFileName, "wb")))
	{
	  lou_logPrint ("Can't open output file %s.", outFileName);
	  return 0;
	}
    }
  else
    ud->outFile = stdout;
  if (ud->format_for == utd)
    k = utd_back_translate_file ();
  else
    k = back_translate_file ();
  if (!k)
    {
      freeEverything ();
      return 0;
    }
  if (ud->inFile != stdin)
    fclose (ud->inFile);
  if (ud->outFile != stdout)
    fclose (ud->outFile);
  lou_logEnd ();
  return 1;
}

static char writeablePath[MAXNAMELEN];
static char *writeablePathPtr = NULL;

_DLL_EXPORT widechar *EXPORT_CALL lbu_setWriteablePathW (widechar *path)
{
return 0;
}

_DLL_EXPORT char *EXPORT_CALL
lbu_setWriteablePath (const char *path)
{
  writeablePathPtr = NULL;
  if (path == NULL)
    return NULL;
  strcpy (writeablePath, path);
  writeablePathPtr = writeablePath;
  return writeablePathPtr;
}

_DLL_EXPORT widechar *EXPORT_CALL lbu_getWriteablePathW ()
{
return 0;
}

_DLL_EXPORT char *EXPORT_CALL
lbu_getWriteablePath ()
{
  return writeablePathPtr;
}

_DLL_EXPORT int EXPORT_CALL
lbu_charToDots (const char *tableList, const unsigned char *inbuf,
		unsigned char *outbuf, int length, const char *logFile,
		unsigned int mode)
{
  widechar *interBuf;
  int wcLength;
  int utf8Length;
  int result = 0;
  if (tableList == NULL || inbuf == NULL || outbuf == NULL)
    return 0;
  lou_logFile (logFile);
  interBuf = malloc (length * CHARSIZE);
  utf8Length = length;
  wcLength = length;
  utf8_string_to_wc (inbuf, &utf8Length, interBuf, &wcLength);
  result = lou_charToDots (tableList, interBuf, interBuf, wcLength, mode
			   | ucBrl);
  if (result)
    {
      utf8Length = length;
      wc_string_to_utf8 (interBuf, &wcLength, outbuf, &utf8Length);
    }
  lou_logEnd ();
  free (interBuf);
  return result;
}

_DLL_EXPORT int EXPORT_CALL
lbu_dotsToChar (const char *tableList, const unsigned char *inbuf,
		unsigned char *outbuf, int length, const char *logFile,
		unsigned int mode)
{
  widechar *interBuf;
  int wcLength;
  int utf8Length;
  int result = 0;
  if (tableList == NULL || inbuf == NULL || outbuf == NULL)
    return 0;
  lou_logFile (logFile);
  interBuf = malloc (length * CHARSIZE);
  utf8Length = length;
  wcLength = length;
  utf8_string_to_wc (inbuf, &utf8Length, interBuf, &wcLength);
  result = lou_dotsToChar (tableList, interBuf, interBuf, wcLength, mode);
  if (result)
    {
      utf8Length = length;
      wc_string_to_utf8 (interBuf, &wcLength, outbuf, &utf8Length);
    }
  lou_logEnd ();
  free (interBuf);
  return result;
}

_DLL_EXPORT int EXPORT_CALL
lbu_checkTable (const char *tableList, const char *logFile, unsigned int mode)
{
  int result = 1;
  lou_logFile (logFile);
  if (!lou_getTable (tableList))
    result = 0;
  lou_logEnd ();
  return result;
}

_DLL_EXPORT void EXPORT_CALL
lbu_free ()
{
/* Free all memory used by liblouisutdml. You MUST call this function at 
* the END of your application.*/
  lou_logEnd ();
  lou_free ();
  destroy_semantic_table ();
  if (ud != NULL)
    free (ud);
  ud = NULL;
}
