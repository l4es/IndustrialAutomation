 /**************************************************************************************************
*
*                          msr_base64.h
*
*           Base64kodierung
*           
*           Autor: Wilhelm Hagemeister
*
*           (C) Copyright IgH 2001
*           Ingenieurgemeinschaft IgH
*           Heinz-Bäcker Str. 34
*           D-45356 Essen
*           Tel.: +49 201/61 99 31
*           Fax.: +49 201/61 98 36
*           E-mail: hm@igh-essen.com
*
*
*           $RCSfile: msr_base64.h,v $
*           $Revision: 1.1 $
*           $Author: hm $
*           $Date: 2005/06/14 12:34:59 $
*           $State: Exp $
*
*
*  This file is modified from GNU libc - libc/resolv/base64.c 
*
*
*
*
**************************************************************************************************/

#ifndef _MSR_BASE64_H_
#define _MSR_BASE64_H_


int gsasl_base64_encode (char const *src,int srclength, char *target, int targsize);

#endif

