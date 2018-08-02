//-< EXCEPTION.CPP >--------------------------------------------------*--------*
// GigaBASE                  Version 1.0         (c) 1999  GARRET    *     ?  *
// (Post Relational Database Management System)                      *   /\|  *
//                                                                   *  /  \  *
//                          Created:     30-Nov-2001  K.A. Knizhnik  * / [] \ *
//                          Last update: 30-Nov-2001  K.A. Knizhnik  * GARRET *
//-------------------------------------------------------------------*--------*
// Exception class implementation
//-------------------------------------------------------------------*--------*

#define INSIDE_GIGABASE

#include "gigabase.h"     

#ifdef THROW_EXCEPTION_ON_ERROR


BEGIN_GIGABASE_NAMESPACE

dbException::dbException(int p_err_code, const char* p_msg, int p_arg)
: err_code (p_err_code),
    msg (NULL),
    arg (p_arg)
{
    if (p_msg != NULL) {
        msg = new char[strlen(p_msg)+1];
        strcpy(msg, p_msg);
    }
}

dbException::dbException(dbException const& ex)
{
    err_code = ex.err_code;
    arg = ex.arg;
    if (ex.msg != NULL) {
        msg = new char[strlen(ex.msg)+1];
        strcpy(msg, ex.msg);
    } else {
        msg = NULL;
    }
}

dbException::~dbException() 
{
    delete[] msg;
}

END_GIGABASE_NAMESPACE

#endif
