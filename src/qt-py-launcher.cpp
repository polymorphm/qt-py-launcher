// -*- mode: cpp; coding: utf-8 -*-
//
// Copyright 2014 Andrej Antonov <polymorphm@gmail.com>.
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

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
