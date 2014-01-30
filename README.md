qt-py-launcher
==============

``qt-py-launcher`` is utility to run ``python``-code from execute-file.

This is mostly need for preparing ``python``-applications for Microsoft Windows.

For this project -- ``Qt`` used for avoiding some Windows-problems,
like unicode application arguments.


Current status
--------------

Beta version (0.1).


Microsoft Windows
-----------------

Example of compiling on Microsoft Windows:

    cd qt-py-launcher
    qmake CONFIG+=win_console win_python_dir=C:\devel\Python33
    nmake

On this step -- created file ``release\qt-py-launcher.exe``.
Copying other required files:

    copy EXAMPLE.qt_py_launcher.py release\qt_py_launcher.py
    xcopy /E C:\devel\Python33 release\python\
    copy C:\Windows\System32\python33.dll release\
    copy C:\devel\Qt\Qt5.1.1\5.1.1\msvc2012_64\bin\Qt5Core.dll release\
    copy C:\devel\Qt\Qt5.1.1\5.1.1\msvc2012_64\bin\icuin51.dll release\
    copy C:\devel\Qt\Qt5.1.1\5.1.1\msvc2012_64\bin\icuuc51.dll release\
    copy C:\devel\Qt\Qt5.1.1\5.1.1\msvc2012_64\bin\icudt51.dll release\
