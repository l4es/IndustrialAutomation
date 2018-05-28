/*****************************************************************************
 *
 * Testmanager - Graphical Automation and Visualisation Tool
 *
 * Copyright (C) 2018  Florian Pose <fp@igh.de>
 *
 * This file is part of Testmanager.
 *
 * Testmanager is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation, either version 3 of the License, or (at your option)
 * any later version.
 *
 * Testmanager is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with Testmanager. If not, see <http://www.gnu.org/licenses/>.
 *
 ****************************************************************************/

#include <Python.h>
#include <Python-ast.h>
#include <errcode.h>

/****************************************************************************/

// see https://stackoverflow.com/a/25666878

/****************************************************************************/

static PyObject *run_mod(mod_ty mod, PyObject *filename, PyObject *globals,
        PyObject *locals, PyCompilerFlags *flags, PyArena *arena)
{
    PyCodeObject *co;
    PyObject *v;
    co = PyAST_CompileObject(mod, filename, flags, -1, arena);
    if (co == NULL)
        return NULL;
    v = PyEval_EvalCode((PyObject*)co, globals, locals);
    Py_DECREF(co);
    return v;
}

/****************************************************************************/

int PyRun_InteractiveOneObject(const char *command)
{
    PyObject *m, *d, *oenc = NULL, *mod_name;
    mod_ty mod;
    PyArena *arena;
    int errcode = 0;
    _Py_IDENTIFIER(__main__);

    PyObject *filename = PyUnicode_DecodeFSDefault("tm");
    if (filename == NULL) {
        PyErr_Print();
        return -1;
    }

    mod_name = _PyUnicode_FromId(&PyId___main__); /* borrowed */
    if (mod_name == NULL) {
        PyErr_Print();
        return -1;
    }

    arena = PyArena_New();
    if (arena == NULL) {
        Py_XDECREF(oenc);
        return -1;
    }
    mod = PyParser_ASTFromStringObject(command, filename,
                                     Py_single_input,
                                     NULL, arena);
    Py_XDECREF(oenc);
    if (mod == NULL) {
        PyArena_Free(arena);
        if (errcode == E_EOF) {
            PyErr_Clear();
            return E_EOF;
        }
        PyErr_Print();
        return -1;
    }
    m = PyImport_AddModuleObject(mod_name);
    if (m == NULL) {
        PyArena_Free(arena);
        return -1;
    }
    d = PyModule_GetDict(m);
    PyObject *v = run_mod(mod, filename, d, d, NULL, arena);
    PyArena_Free(arena);
    if (v == NULL) {
        PyErr_Print();
        //flush_io();
        return -1;
    }
    Py_DECREF(v);
    //flush_io();
    return 0;
}

/****************************************************************************/
