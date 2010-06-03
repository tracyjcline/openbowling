# -------------------------------------------------
# Project created by QtCreator 2010-05-20T14:41:00
# -------------------------------------------------
QT += network \
    xml
QT -= gui
TARGET = pinsetter_test
CONFIG += console
CONFIG -= app_bundle
TEMPLATE = app
SOURCES += main.cpp \
    namedpipe.cpp \
    client.cpp
HEADERS += namedpipe.h \
    client.h
