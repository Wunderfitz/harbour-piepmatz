#-------------------------------------------------
#
# Project created by QtCreator 2014-09-07T16:59:32
#
#-------------------------------------------------

#QT       -= gui

#TEMPLATE = lib
#CONFIG += staticlib c++11
QMAKE_CFLAGS += -std=c99

SOURCES += \
    $$PWD/qgumboattribute.cpp \
    $$PWD/qgumbodocument.cpp \
    $$PWD/qgumbonode.cpp \
    $$PWD/gumbo-parser/src/attribute.c \
    $$PWD/gumbo-parser/src/char_ref.c \
    $$PWD/gumbo-parser/src/error.c \
    $$PWD/gumbo-parser/src/parser.c \
    $$PWD/gumbo-parser/src/string_buffer.c \
    $$PWD/gumbo-parser/src/string_piece.c \
    $$PWD/gumbo-parser/src/tag.c \
    $$PWD/gumbo-parser/src/tokenizer.c \
    $$PWD/gumbo-parser/src/utf8.c \
    $$PWD/gumbo-parser/src/util.c \
    $$PWD/gumbo-parser/src/vector.c

HEADERS += \
    $$PWD/qgumboattribute.h \
    $$PWD/qgumbodocument.h \
    $$PWD/qgumbonode.h \
    $$PWD/gumbo-parser/src/attribute.h \
    $$PWD/gumbo-parser/src/char_ref.h \
    $$PWD/gumbo-parser/src/error.h \
    $$PWD/gumbo-parser/src/gumbo.h \
    $$PWD/gumbo-parser/src/insertion_mode.h \
    $$PWD/gumbo-parser/src/parser.h \
    $$PWD/gumbo-parser/src/string_buffer.h \
    $$PWD/gumbo-parser/src/string_piece.h \
    $$PWD/gumbo-parser/src/token_type.h \
    $$PWD/gumbo-parser/src/tokenizer_states.h \
    $$PWD/gumbo-parser/src/tokenizer.h \
    $$PWD/gumbo-parser/src/utf8.h \
    $$PWD/gumbo-parser/src/util.h \
    $$PWD/gumbo-parser/src/vector.h \
    $$PWD/HtmlTag.h

contains(QMAKE_CC, gcc): {
    QMAKE_CFLAGS_WARN_ON += -Wno-unused-parameter -Wno-sign-compare -Wno-old-style-declaration -Wno-type-limits
}
