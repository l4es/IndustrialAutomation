/*  PyDAX - A Python extension module for OpenDAX 
 *  OpenDAX - An open source data acquisition and control system 
 *  Copyright (c) 2011 Phil Birkelbach
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 * 
 *  This is the main source code file for the Python extension module
 */

#include <pydax.h>

/* TODO: This should eventually be a Python object that we
 * create and handle.  Right now we'll keep it a global for
 * simplicity but it will only allow one connection per
 * Python script. */
dax_state *ds;

static PyObject *_create_py_object(void *buff, tag_type type, u_int32_t count);


/* This function is called as pydax.init("ModuleName") where the
 * ModuleName is the name that you want to use when registering
 * this module */
static PyObject *
pydax_init(PyObject *pSelf, PyObject *pArgs)
{
    char *modname = NULL;
    int result;
    /* TODO: Get command line arguments and process them too */
    int argc = 1;
    char *argv[] = {modname};
    
    if(!PyArg_ParseTuple(pArgs, "s", &modname)) return NULL;
    ds = dax_init(modname);

    if(ds == NULL) {
        PyErr_SetString(PyExc_IOError, "Unable to Allocate Dax State Object");
        return NULL;
    }
    /* Initialize and run the configuration */
    result = dax_init_config(ds, modname);
    if(result != 0) {
        PyErr_SetString(PyExc_IOError, "Unable to Allocate Configuration");
        return NULL;
    }
    dax_configure(ds, argc, argv, CFG_CMDLINE | CFG_DAXCONF);

    /* Free the configuration data */
    dax_free_config(ds);

    /* Check for OpenDAX and register the module */
    if( dax_connect(ds) ) {
        PyErr_SetString(PyExc_IOError, "Unable to Connect to OpenDAX Server");
        return NULL;
    }

    Py_RETURN_NONE;
}

static PyObject *
pydax_free(PyObject *pSelf, PyObject *pArgs)
{
    if(ds == NULL) {
        PyErr_SetString(PyExc_Exception, "OpenDAX is not initialized");
        return NULL;
    }

    if( dax_disconnect(ds) ) {
        PyErr_SetString(PyExc_IOError, "Unable to disconnect from OpenDAX Server");
        return NULL;
    }
    dax_free(ds);
    Py_RETURN_NONE;
}

/* This creates a datatype in the server.  It is called as
 * pydax.cdt_create(name, type) where name is a string with the
 * name of the new datatype, and type is a tuple of tuples that
 * represent the members new type.  For example ...
 * (("First", "INT", 10),("Second", "BOOL", 8)) */
static PyObject *
pydax_cdt_create(PyObject *pSelf, PyObject *pArgs)
{
    PyObject *t, *i;
    char *cdt_name, *m_name, *m_typename;
    dax_cdt *cdt;
    tag_type type;
    int m_count, result, n;
    Py_ssize_t count;
    
    if(ds == NULL) {
        PyErr_SetString(PyExc_Exception, "OpenDAX is not initialized");
        return NULL;
    }
    if(!PyArg_ParseTuple(pArgs, "sO", &cdt_name, &t)) return NULL;

    cdt = dax_cdt_new(cdt_name, NULL);
    if(cdt == NULL) {
        PyErr_Format(PyExc_Exception, "Unable to get new CDT object for '%s'", cdt_name);
        return NULL;
    }
    if(! PyTuple_Check(t)) {
        dax_cdt_free(cdt);
        PyErr_SetString(PyExc_Exception, "Second argument must be a tuple");
        return NULL;
    }
    count = PyTuple_Size(t);
    for(n = 0; n < count; n++) {
        i = PyTuple_GetItem(t, n);
        if(PyTuple_Size(i) != 3) {
            PyErr_SetString(PyExc_Exception, "Type definition is not right");
            dax_cdt_free(cdt);
            return NULL;
        }
        if(!PyArg_ParseTuple(i, "ssi", &m_name, &m_typename, &m_count)) {
            dax_cdt_free(cdt);
            return NULL;
        }
        result = dax_cdt_member(ds, cdt, m_name, dax_string_to_type(ds, m_typename), m_count);
        if(result) {
            PyErr_Format(PyExc_Exception, "Problem with member '%s'", m_name);
            dax_cdt_free (cdt);
            return NULL;
        }
    }
    result = dax_cdt_create(ds, cdt, &type);
    if(result) {
        PyErr_Format(PyExc_Exception, "Unable to create type '%s'", cdt_name);
        dax_cdt_free(cdt);
        return NULL;
    }
    return PyLong_FromUnsignedLong(type);
}

struct _cdt_iter_item {
    PyObject *tuple;
    Py_ssize_t pos;
};

/* This is the cdt iterator callback that we are using 
 * to build the tuple for pydax.cdt_get() */
static void
_cdt_get_callback_tuple(cdt_iter member, void *data)
{
    struct _cdt_iter_item *item;
    PyObject *i;
    item = (struct _cdt_iter_item *)data;

    i = Py_BuildValue("ssi", member.name, dax_type_to_string(ds, member.type), member.count);
    PyTuple_SetItem(item->tuple, item->pos++, i);
}

static void
_cdt_get_callback_size(cdt_iter member, void *data)
{
    Py_ssize_t *size;
    size = data;
    (*size)++;
}

/* Returns a tuple of tuples that represent the data type
 * the tuple is stuctured just like the one that would have
 * been used to create the data type in pydax.create().  The
 * function takes a single argument which is the name of
 * the data type or the index. */
static PyObject *
pydax_cdt_get(PyObject *pSelf, PyObject *pArgs)
{
    PyObject *ob, *t;
    char *typename;
    tag_type index;
    Py_ssize_t size;
    struct _cdt_iter_item item;

    if(ds == NULL) {
        PyErr_SetString(PyExc_Exception, "OpenDAX is not initialized");
        return NULL;
    }
    if(!PyArg_ParseTuple(pArgs, "O", &ob)) return NULL;
    /* If the argument is an integer then we set index */
    if(PyInt_Check(ob)) {
        index = (tag_index)PyInt_AS_LONG(ob);
        typename = (char *)dax_type_to_string(ds, index);
    /* Else we set tagname to the string */
    } else if(PyString_Check(ob)) {
        typename = PyString_AsString(ob);
        index = dax_string_to_type(ds, typename);
    /* If it's neither then we have a problem */
    } else {
        PyErr_SetString(PyExc_Exception, "Argument should be integer or string");
        return NULL;
    }
    size = 0;
    dax_cdt_iter(ds, index, &size, _cdt_get_callback_size);

    t = PyTuple_New(size);
    item.tuple = t;
    item.pos = 0;
    dax_cdt_iter(ds, index, &item, _cdt_get_callback_tuple);
    return t;
}
    

/* This function adds a tag to OpenDAX.  It is called as
 * pydax.add("Tagname", "Type", count) */
static PyObject *
pydax_add(PyObject *pSelf, PyObject *pArgs)
{
    char *tagname, *typename;
    int count, result;

    if(ds == NULL) {
        PyErr_SetString(PyExc_Exception, "OpenDAX is not initialized");
        return NULL;
    }

    if(!PyArg_ParseTuple(pArgs, "ssi", &tagname, &typename, &count)) return NULL;

    result = dax_tag_add(ds, NULL, tagname, dax_string_to_type(ds, typename), count);
    if(result) {
        PyErr_Format(PyExc_IOError, "Unable to add tag '%s'", tagname);
        return NULL;
    }
    Py_RETURN_NONE;
}


static PyObject *
pydax_get(PyObject *pSelf, PyObject *pArgs)
{
    PyObject *ob, *o, *v;
    int result;
    dax_tag tag;
    tag_index index = 0;
    char *tagname = NULL;
    
    if(ds == NULL) {
        PyErr_SetString(PyExc_Exception, "OpenDAX is not initialized");
        return NULL;
    }
    if(!PyArg_ParseTuple(pArgs, "O", &ob)) return NULL;
    /* If the argument is an integer then we set index */
    if(PyInt_Check(ob)) {
        index = (tag_index)PyInt_AS_LONG(ob);
    /* Else we set tagname to the string */
    } else if(PyString_Check(ob)) {
        tagname = PyString_AsString(ob);
    /* If it's neither then we have a problem */
    } else {
        PyErr_SetString(PyExc_Exception, "Argument should be integer or string");
        return NULL;
    }
    /* If the string is not set then we get by index */
    if(tagname == NULL) {
        result = dax_tag_byindex(ds, &tag, index);
        if(result != 0) {
            PyErr_Format(PyExc_IOError, "Unable to get tag at index %d", index);
            return NULL;
        }
    } else {
        result = dax_tag_byname(ds, &tag, tagname);
        if(result != 0) {
            PyErr_Format(PyExc_IOError, "Unable to get tag '%s'", tagname);
            return NULL;
        }
    }
    o = PyDict_New();
    if(o == NULL) return NULL;
    v = PyString_FromString(tag.name);
    PyDict_SetItemString(o, "Name", v);
    Py_DECREF(v);
    v = PyString_FromString(dax_type_to_string(ds, tag.type));
    PyDict_SetItemString(o, "Type", v);
    Py_DECREF(v);
    v = PyInt_FromLong(tag.count);
    PyDict_SetItemString(o, "Count", v);
    Py_DECREF(v);

    return o;
}

static int
_pyton_to_dax(tag_type type, dax_type_union *value, PyObject *po)
{
    
    switch (type) {
        case DAX_BYTE:
            value->dax_byte = (dax_byte)PyInt_AsLong(po);
            PySys_WriteStderr("%d\n", value->dax_byte);
            break;
        case DAX_SINT:
            value->dax_sint = (dax_sint)PyInt_AsLong(po);
            PySys_WriteStderr("%d\n", value->dax_sint);
            break;
        case DAX_WORD:
        case DAX_UINT:
            value->dax_uint = (dax_uint)PyInt_AsLong(po);
            PySys_WriteStderr("%ud\n", value->dax_uint);
            break;
        case DAX_INT:
            value->dax_int = (dax_int)PyInt_AsLong(po);
            PySys_WriteStderr("%d\n", value->dax_int);
            break;
        case DAX_DWORD:
        case DAX_UDINT:
        case DAX_TIME:
            value->dax_udint = (dax_udint)PyLong_AsLong(po);
            PySys_WriteStderr("%ud\n", value->dax_udint);
            break;
        case DAX_DINT:
            value->dax_dint = (dax_dint)PyLong_AsLong(po);
            PySys_WriteStderr("%d\n", value->dax_dint);
            break;
        case DAX_REAL:
            value->dax_real = (dax_real)PyFloat_AsDouble(po);
            PySys_WriteStderr("%f\n", value->dax_real);
            break;
        case DAX_LWORD:
        case DAX_ULINT:
            value->dax_ulint = (dax_ulint)PyLong_AsUnsignedLongLong(po);
            PySys_WriteStderr("%lld\n", value->dax_ulint);
            break;
        case DAX_LINT:
            value->dax_lint = (dax_lint)PyLong_AsLongLong(po);
            PySys_WriteStderr("%lld\n", value->dax_lint);
            break;
        case DAX_LREAL:
            value->dax_lreal = (dax_lreal)PyFloat_AsDouble(po);
            PySys_WriteStderr("%lf\n", value->dax_lreal);
            break;
    }

    return 0;
}

/* Converts a dax tag given by 'value' into a Python Object.
 * Does not handle BOOLs or CDT's, only base data types */
static PyObject *
_dax_to_python(tag_type type, dax_type_union value)
{
    PyObject *po = NULL;
    
    switch (type) {
        case DAX_BYTE:
            po = PyInt_FromLong((long)value.dax_byte);
            break;
        case DAX_SINT:
            po = PyInt_FromLong((long)value.dax_sint);
            break;
        case DAX_WORD:
        case DAX_UINT:
            po = PyInt_FromLong((long)value.dax_uint);
            break;
        case DAX_INT:
            po = PyInt_FromLong((long)value.dax_int);
            break;
        case DAX_DWORD:
        case DAX_UDINT:
        case DAX_TIME:
            po = PyLong_FromUnsignedLong((unsigned long)value.dax_udint);
            break;
        case DAX_DINT:
            po = PyLong_FromLong((long)value.dax_dint);
            break;
        case DAX_REAL:
            po = PyFloat_FromDouble((double)value.dax_real);
            break;
        case DAX_LWORD:
        case DAX_ULINT:
            po = PyFloat_FromDouble((double)value.dax_ulint);
            break;
        case DAX_LINT:
            po = PyFloat_FromDouble((double)value.dax_lint);
            break;
        case DAX_LREAL:
            po = PyFloat_FromDouble(value.dax_lreal);
            break;
        default:
            PyErr_SetString(PyExc_Exception, "Bad Data Type Passed");
            po = NULL;
    }
    return po;
}

static void
_read_callback(cdt_iter member, void *udata)
{
    unsigned char *data = ((struct iter_udata *)udata)->data;
    PyObject *po = ((struct iter_udata *)udata)->po;
    PyObject *item = NULL;
    int bit, byte, n;

/********TESTING*************
    PySys_WriteStdout("_read_callback() called for member %s\n", member.name);
    PySys_WriteStdout("member.type = %s\n", dax_type_to_string(ds, member.type));
    PySys_WriteStdout("member.byte = %d\n", member.byte);
    PySys_WriteStdout("member.bit = %d\n", member.bit);
    PySys_WriteStdout("member.count = %d\n", member.count);
    PySys_WriteStdout("_read_callback() called for member %s\n", member.name);
    PySys_WriteStdout("_read_callback() - udata = %p\n", udata);
    PySys_WriteStdout("_read_callback() - data = %p\n", data);
    PySys_WriteStdout("_read_callback() - po = %p\n", po);
    for(n = 0; n < 3; n++) {
        PySys_WriteStdout("_read_callback() - data[%d] = 0x%2X\n", n, data[n]);
    }
*/
    /* This handles the odd case where we have BOOLS that have a bit index
     * other than 0.  _create_py_object() can handle the others.  We put
     * this here because having a bit offset other than 0 only makes sense
     * in the context of a CDT */
    if(member.type == DAX_BOOL && member.bit != 0) {
        byte = member.byte;
        bit = member.bit;
        if(member.count > 1) {
            item = PyList_New(member.count);
            if(item == NULL) {
                ((struct iter_udata *)udata)->error = 1;
                return;
            }
            for(n = 0; n < member.count; n++) {
//                PySys_WriteStdout("_read_callback() - byte = %d, bit -= %d, 0x%X << 0x%X\n", byte, bit, ((u_int8_t *)data)[byte], (0x01 << bit));

                if(((u_int8_t *)data)[byte] & (0x01 << bit)) {
                    Py_INCREF(Py_True);
                    PyList_SetItem(item, n, Py_True);
                } else {
                    Py_INCREF(Py_False);
                    PyList_SetItem(item, n, Py_False);
                }
                bit++;
                if(bit == 8) {
                    bit = 0;
                    byte++;
                }
            }
        } else {
            if(((u_int8_t *)data)[byte] & (0x01 << bit)) {
                Py_INCREF(Py_True);
                item = Py_True;
            } else {
                Py_INCREF(Py_False);
                item = Py_False;
            }
        }
    } else {
//        PySys_WriteStdout("_read_callback() - About to call _create_py_object() for...\n");
        item = _create_py_object((void *)(data + member.byte), member.type, member.count);
        if(item == NULL) {
            ((struct iter_udata *)udata)->error = 1;
            return;
        }
    }
    PyDict_SetItemString(po, member.name, item);
    Py_DECREF(item);

}

/* This function takes a read buffer, tag type and tag count and converts
 * data in the buffer into a Python object that is best described by this
 * data.  The function is re-entrant and may be called recursively through
 * the cdt iterater callback mechanism */
static PyObject *
_create_py_object(void *buff, tag_type type, u_int32_t count)
{
    int n, offset;
    PyObject *po, *item;
    struct iter_udata udata;
    
    if(type == DAX_BOOL) {
        if(count > 1) {
            po = PyList_New(count);
            if(po == NULL) return NULL;
            for(n = 0; n < count; n++) {
                if(((u_int8_t *)buff)[n/8] & (0x01 << n%8)) {
                    Py_INCREF(Py_True);
                    PyList_SetItem(po, n, Py_True);
                } else {
                    Py_INCREF(Py_False);
                    PyList_SetItem(po, n, Py_False);
                }
            }
            return po;
        } else {
            if(((u_int8_t *)buff)[0]) {
                Py_RETURN_TRUE;
            } else {
                Py_RETURN_FALSE;
            }
        }
    } else if(IS_CUSTOM(type)) {
        if(count > 1) {
            for(n = 0; n < count; n++) {
                // Make dict
                // setup user data
                // call dax_cdt_iter()
                // assign dict object to list
            }
            // return list
        } else {
            po = PyDict_New();
            if(po == NULL) return NULL;
            udata.po = po;
            udata.data = buff;
            udata.error = 0;
//            PySys_WriteStdout("_create_py_object() - data = %p\n", udata.data);
//            PySys_WriteStdout("_create_py_object() - po = %p\n", udata.po);
//            PySys_WriteStdout("_create_py_object() - &udata = %p\n", &udata);
            dax_cdt_iter(ds, type, &udata, _read_callback);
            return po;
        }
        goto denied; // Not ready for this yet
    } else {  /* Base Datatype */
        if(count > 1) {
            po = PyList_New(count);
            if(po == NULL) return NULL;
            for(n = 0; n < count; n++) {
                offset = n * TYPESIZE(type)/8;
                item = _dax_to_python(type, *((dax_type_union *)(buff + offset)));
                if(item == NULL) return NULL;
                PyList_SetItem(po, n, item);
            }
            return po;
        } else {
            return _dax_to_python(type, *((dax_type_union *)buff));
        }
    }
denied:
    PyErr_SetString(PyExc_Exception, "Denied");
    return NULL; /* If we make it this far we have a problem */
}
        
static PyObject *
pydax_read(PyObject *pSelf, PyObject *pArgs)
{
    char *tagname;
    int count, result;
    Handle h;
    void *buff;
    PyObject *po;

    if(ds == NULL) {
        PyErr_SetString(PyExc_Exception, "OpenDAX is not initialized");
        return NULL;
    }

    if(!PyArg_ParseTuple(pArgs, "si", &tagname, &count)) return NULL;

    result = dax_tag_handle(ds, &h, tagname, count);
    if(result) {
        PyErr_Format(PyExc_Exception, "Unable to retrieve handle for tag '%s'", tagname);
        return NULL;
    }
    buff = malloc(h.size);
    if(buff == NULL) {
        PyErr_Format(PyExc_MemoryError, "Unable to read tag '%s'", tagname);
        return NULL;
    }
    result = dax_read_tag(ds, h, buff);
    if(result) {
        PyErr_Format(PyExc_IOError, "Unable to read tag '%s'", tagname);
        free(buff);
        return NULL;
    }
    po = _create_py_object(buff, h.type, h.count);
    free(buff);
    return po;
}

static PyObject *
pydax_write(PyObject *pSelf, PyObject *pArgs)
{
    PyObject *data;
    char *tagname;
    int count, result;
    Handle h;
    dax_type_union value;
    
    if(ds == NULL) {
        PyErr_SetString(PyExc_Exception, "OpenDAX is not initialized");
        return NULL;
    }

    if(!PyArg_ParseTuple(pArgs, "sO", &tagname, &data)) return NULL;

    result = dax_tag_handle(ds, &h, tagname, 0);
    if(result) {
        PyErr_Format(PyExc_Exception, "Unable to retrieve handle for tag '%s'", tagname);
        return NULL;
    }

    if(h.count > 1) {
        PyErr_SetString(PyExc_Exception, "We're not doing arrays or CDT's yet");
        return NULL;
    }

    _pyton_to_dax(h.type, &value, data);
// Convert the data object to a buffer and write
    Py_RETURN_NONE;
}


static PyObject *
pydax_event_add(PyObject *pSelf, PyObject *pArgs)
{
    if(ds == NULL) {
        PyErr_SetString(PyExc_Exception, "OpenDAX is not initialized");
        return NULL;
    }

    Py_RETURN_NONE;
}

static PyObject *
pydax_event_del(PyObject *pSelf, PyObject *pArgs)
{
    if(ds == NULL) {
        PyErr_SetString(PyExc_Exception, "OpenDAX is not initialized");
        return NULL;
    }

    Py_RETURN_NONE;
}

static PyObject *
pydax_event_wait(PyObject *pSelf, PyObject *pArgs)
{
    if(ds == NULL) {
        PyErr_SetString(PyExc_Exception, "OpenDAX is not initialized");
        return NULL;
    }

    Py_RETURN_NONE;
}

static PyObject *
pydax_event_poll(PyObject *pSelf, PyObject *pArgs)
{
    if(ds == NULL) {
        PyErr_SetString(PyExc_Exception, "OpenDAX is not initialized");
        return NULL;
    }

    Py_RETURN_NONE;
}

static PyMethodDef pydax_methods[] = {
    {"init", pydax_init, METH_VARARGS, NULL},
    {"free", pydax_free, METH_VARARGS, NULL},
    {"add", pydax_add, METH_VARARGS, NULL},
    {"cdt_create", pydax_cdt_create, METH_VARARGS, NULL},
    {"cdt_get", pydax_cdt_get, METH_VARARGS, NULL},
    {"read", pydax_read, METH_VARARGS, NULL},
    {"get", pydax_get, METH_VARARGS, NULL},
    {"write", pydax_write, METH_VARARGS, NULL},
    {"event_add", pydax_event_add, METH_VARARGS, NULL},
    {"event_del", pydax_event_del, METH_VARARGS, NULL},
    {"event_wait", pydax_event_wait, METH_VARARGS, NULL},
    {"event_poll", pydax_event_poll, METH_VARARGS, NULL},
    {"write", pydax_write, METH_VARARGS, NULL},
    {NULL, NULL}};

PyMODINIT_FUNC
initpydax(void)
{
    Py_InitModule("pydax", pydax_methods);
}
