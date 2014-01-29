# -*- mode: text; coding: utf-8 -*-

win32 {
    isEmpty(win_python_dir) {
        error("variable ``win_python_dir`` must not be empty. usage example: qmake 'CONFIG += win_console' 'win_python_dir = C:/devel/Python33'")
    }
    
    win_console {
        CONFIG += console
    }
    
    INCLUDEPATH += $$quote($$win_python_dir/include)
    LIBS += $$quote($$win_python_dir/libs/python3.lib)
}

unix {
    CONFIG += link_pkgconfig
    PKGCONFIG += python3
}

QT -= gui
SOURCES += src/qt-py-launcher.cpp
