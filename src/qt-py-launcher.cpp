// -*- mode: cpp; coding: utf-8 -*-
//
// Copyright 2014 Andrej Antonov <polymorphm@gmail.com>
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

#include <clocale>
#include <cstdlib>
#include <cwchar>

#include <Python.h>

#include <QString>
#include <QStringList>
#include <QCoreApplication>
#include <QDir>

namespace {

const QString python_home_name = "python";
const QString launcher_module_name = "qt_py_launcher";
const QString launcher_main_name = "main";

wchar_t *new_c_str (const QString &str) throw () {
    int size = str.size();
    wchar_t *c_str = new wchar_t [size + 1];
    
    str.toWCharArray(c_str);
    c_str[size] = 0;
    
    return c_str;
}

}

int main (int argc, char *argv[]) throw () {
    int exit_code = 0;
    wchar_t **c_args = 0;
    wchar_t *c_python_home = 0;
    wchar_t *c_launcher_module_name = 0;
    wchar_t *c_launcher_main_name = 0;
    PyObject *result = 0;
    PyObject *result_exit_code = 0;
    
    setlocale(LC_ALL, "");
    
    QCoreApplication qt_app(argc, argv);
    QStringList args = qt_app.arguments();
    int args_size = args.size();
    c_args = new wchar_t *[args_size];
    for (int i = 0; i < args_size; ++i) {
        c_args[i] = new_c_str(args.at(i));
    }
    QString python_home =
            qt_app.applicationDirPath() +
            QDir::separator() +
            python_home_name;
    if (QDir(python_home).exists()) {
        c_python_home = new_c_str(python_home);
        Py_SetPythonHome(c_python_home);
    }
    c_launcher_module_name = new_c_str(launcher_module_name);
    c_launcher_main_name = new_c_str(launcher_main_name);
    
    Py_Initialize();
    PySys_SetArgv(args_size, c_args);
    
    PyObject *globals = Py_BuildValue(
            "{s:O,s:u,s:u}",
            "__builtins__",
            PyEval_GetBuiltins(),
            "launcher_module_name",
            c_launcher_module_name,
            "launcher_main_name",
            c_launcher_main_name
            );
    PyObject *locals = Py_BuildValue("{}");
    
    result = PyRun_String(
            "try:\n"
            "  import importlib\n"
            "  launcher_module = importlib.import_module(launcher_module_name)\n"
            "  launcher_main = getattr(launcher_module, launcher_main_name)\n"
            "  launcher_main()\n"
            "except SystemExit as e:\n"
            "  exit_code = e.args[0]\n"
            "except:\n"
            "  import traceback\n"
            "  traceback.print_exc()\n"
            "  exit_code = 1\n",
            Py_file_input,
            globals,
            locals
            );
    if (!result) {
        fwprintf(stderr, L"error: PyRun_String() returned an error\n");
        exit_code = 1;
        goto finally;
    }
    
    result_exit_code = PyDict_GetItemString(locals, "exit_code");
    if (result_exit_code) {
        Py_INCREF(result_exit_code);
        exit_code = (int)PyLong_AsLong(result_exit_code);
    }
    
    finally:
    
    Py_CLEAR(result_exit_code);
    Py_CLEAR(result);
    Py_CLEAR(locals);
    Py_CLEAR(globals);
    
    Py_Finalize();
    
    if (c_args) {
        for (int i = 0; i < args_size; ++i) {
            delete [] c_args[i];
        }
        
        delete [] c_args;
        c_args = 0;
    }
    if (c_python_home) {
        delete [] c_python_home;
        c_python_home = 0;
    }
    if (c_launcher_module_name) {
        delete [] c_launcher_module_name;
        c_launcher_module_name = 0;
    }
    if (c_launcher_main_name) {
        delete [] c_launcher_main_name;
        c_launcher_main_name = 0;
    }
    
    return exit_code;
}
