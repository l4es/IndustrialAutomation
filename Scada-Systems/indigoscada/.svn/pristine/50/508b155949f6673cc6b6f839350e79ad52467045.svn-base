//-< EXCEPTION.H >---------------------------------------------------*--------*
// GigaBASE                  Version 1.0         (c) 1999  GARRET    *     ?  *
// (Post Relational Database Management System)                      *   /\|  *
//                                                                   *  /  \  *
//                          Created:     3-Oct-99 Sebastiano Suraci  * / [] \ *
//                          Last update: 5-Oct-99 K.A. Knizhnik      * GARRET *
//-------------------------------------------------------------------*--------*
// Database exception
//-------------------------------------------------------------------*--------*

#ifndef __EXCEPTION_H__
#define __EXCEPTION_H__

BEGIN_GIGABASE_NAMESPACE

/**
 * Exception class thrown by GigaBASE when compiled with THROW_EXCEPTION_ON_ERROR 
 */
class GIGABASE_DLL_ENTRY dbException
{
   protected:
     int   err_code;
     char* msg;
     int   arg;

   public:
     /**
      * Exception constructor
      * @param p_err_code erro code as defined in <code>dbErrorClass</code> enum in database.h
      * @param p_msg erro message
      * @param p_arg message optional argument
      */
     dbException(int p_err_code, const char* p_msg = NULL, int p_arg = 0);

     /**
      * Copy constructor
      */
     dbException(dbException const& ex);
     
     /**
      * Destructor
      */
     ~dbException();

     /**
      * Get error code
      * @return error code as defined in <code>dbErrorClass</code> enum in database.h
      */
     int   getErrCode() const { return err_code; }
     
     /**
      * Get message text
      */
     char* getMsg()     const { return msg; }

     /**
      * Get optional argument
      */
     long  getArg()     const { return arg; }
};

END_GIGABASE_NAMESPACE

#endif
