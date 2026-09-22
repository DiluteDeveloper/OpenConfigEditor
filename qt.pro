QT += core gui widgets

INCLUDEPATH += $$[QT_INSTALL_HEADERS] include/
TARGET = OpenConfigEditor
TEMPLATE = app

CONFIG += c++20
HEADERS += $$files(include/*.h, true)
SOURCES += $$files(src/*.cpp, true)
