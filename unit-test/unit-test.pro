defineTest(copyToResourceDir) {
    files = $$1

    for(FILE, files) {
        DDIR = $$OUT_PWD/debug/resource/

        # Replace slashes in paths with backslashes for Windows
        win32:FILE ~= s,/,\\,g
        win32:DDIR ~= s,/,\\,g

        QMAKE_POST_LINK += $$QMAKE_COPY $$quote($$FILE) $$quote($$DDIR) $$escape_expand(\\n\\t)
    }

    export(QMAKE_POST_LINK)
}

QT  += core gui opengl
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = unit-test
TEMPLATE = app

# LIBS is from the main build directory (where debug/release folders are located)
win32 {
    # Windows build
    LIBS    += -L../IConsole/debug/ -lIConsole
}
unix {
    # Unix build - for some reason the -L/-l syntax doesn't work...
    LIBS    += ../IConsole/libIConsole.so
}

# Extra includes for libraries.
INCLUDEPATH += ../IConsole/inc

if(!debug_and_release|build_pass):CONFIG(debug, debug|release) {
   mac:LIBS = $$member(LIBS, 0) $$member(LIBS, 1)_debug
   win32:LIBS = $$member(LIBS, 0) $$member(LIBS, 1)d
}

SOURCES += \
    main.cpp

copyToResourceDir($$PWD/../resource/manifest.qss $$PWD/../resource/command_symbol.svg $$PWD/../resource/variable_symbol.svg)