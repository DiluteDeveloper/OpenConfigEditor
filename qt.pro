QT += core gui widgets

INCLUDEPATH += $$[QT_INSTALL_HEADERS] include/
TARGET = OpenConfigEditor
TEMPLATE = app
DEFINES += FULL_PATH_IN_STATUS_BAR

CONFIG += debug c++23
HEADERS += $$files(include/*.h, true)
SOURCES += $$files(src/*.cpp, true)
