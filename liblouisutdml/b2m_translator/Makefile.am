INCLUDES =		\
	-I$(top_srcdir) \
	-DLBU_PATH=\"$(datadir)/liblouisutdml/lbu_files/\" \
	-DLIBLOUIS_TABLES_PATH=\"$(datadir)/liblouis/tables/\"
liblouisutdmlincludedir = $(includedir)/liblouisutdml

liblouisutdmlinclude_HEADERS = \
	liblouisutdml.h \
	louisutdml.h

lib_LTLIBRARIES = liblouisutdml.la

liblouisutdml_la_CFLAGS =	\
	$(BASE_DEPENDENCIES_CFLAGS)

liblouisutdml_la_LDFLAGS =	\
	-version-info \
	$(LIBLOUISUTDML_CURRENT):$(LIBLOUISUTDML_REVISION):$(LIBLOUISUTDML_AGE) \
	-no-undefined \
	$(BASE_DEPENDENCIES_LIBS)

liblouisutdml_la_SOURCES =			\
	math_parser.cpp \
	String_utils.cpp \
DOMAttr.cpp \
DOMCDATASection.cpp \
DOMCharacterData.cpp \
DOMComment.cpp \
DOMDocument.cpp \
DOMDocumentFragment.cpp \
DOMElement.cpp \
DOMEntity.cpp \
DOMEntityReference.cpp \
DOMNamedNodeList.cpp \
DOMNode.cpp \
DOMNodeList.cpp \
DOMNodeListItem.cpp \
DOMProcessingInstruction.cpp \
DOMText.cpp
