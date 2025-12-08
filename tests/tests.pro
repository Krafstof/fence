QT -= gui
QT += widgets
CONFIG += console
CONFIG -= app_bundle
CONFIG += c++17

TEMPLATE = app
TARGET = fence_tests

# GoogleTest include paths
INCLUDEPATH += \
    ../3rdparty/googletest/googletest/include \
    ../3rdparty/googletest/googletest

# GoogleTest sources (без gtest_main!)
SOURCES += \
    ../3rdparty/googletest/googletest/src/gtest-all.cc \
    test_zabor_geometry.cpp \
    test_zabor_update.cpp

# your test main
SOURCES += \
    test_main.cpp \
    test_zabor.cpp \
    test_zabor_add.cpp

# ZaborWidget sources
SOURCES += \
    ../zaborwidget.cpp

HEADERS += \
    ../zaborwidget.h
