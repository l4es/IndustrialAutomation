/*
 *  $Id: defines_api.h,v 0.0.0.1        2003/08/21 
 *
 *  -------------------------------------------------------------------  
 *                                ORTE                                 
 *                      Open Real-Time Ethernet                       
 *                                                                    
 *                      Copyright (C) 2001-2006                       
 *  Department of Control Engineering FEE CTU Prague, Czech Republic  
 *                      http://dce.felk.cvut.cz                       
 *                      http://www.ocera.org                          
 *                                                                    
 *  Author: 		 Petr Smolik	petr.smolik@wo.cz             
 *  Advisor: 		 Pavel Pisa                                   
 *  Project Responsible: Zdenek Hanzalek                              
 *  --------------------------------------------------------------------
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
 */

#ifndef _DEFINES_API_H
#define _DEFINES_API_H

#ifdef __cplusplus
extern "C" {
#endif

#define MAX_INTERFACES            5
#define ORTE_DEFAULT_DOMAIN       0
#define MAX_STRING_IPADDRESS_LENGTH 4*3+3+1
#define MAX_STRING_NTPTIME_LENGTH 30    //need fix

////////////////////////////////////////////////////////////////////////////////
//Return values
#define ORTE_OK			  0
#define ORTE_BAD_HANDLE		  -1
#define ORTE_TIMEOUT		  -2
#define ORTE_QUEUE_FULL		  -3

////////////////////////////////////////////////////////////////////////////////
//Logging - flags
#define LOG_FNONE	          0x00
#define LOG_FEXCEPTION        	  0x01
#define LOG_FWARN          	  0x02
#define LOG_FLOCAL_OBJECT  	  0x04  
#define LOG_FREMOTE_OBJECT        0x08
#define LOG_FPERIODIC             0x10
#define LOG_FCONTENT              0x20

/*****************************************************************/
/**
 * SeqNumberCmp - comparison of two sequence numbers
 * @sn1: source sequential number 1
 * @sn2: source sequential number 2
 * 
 * Return:  1 if sn1 > sn2
 *         -1 if sn1 < sn2
 *          0 if sn1 = sn2
 */
#define SeqNumberCmp(sn1, sn2) (                 \
          (sn1).high>(sn2).high ? 1 :            \
        ((sn1).high<(sn2).high ? -1 :            \
          ((sn1).low>(sn2).low ? 1 :             \
            ((sn1).low<(sn2).low ? -1 : 0)))     \
    )

/*****************************************************************/
/**
 * SeqNumberInc - incrementation of a sequence number
 * @res: result
 * @sn: sequential number to be incremented
 * 
 * res = sn + 1
 */
#define SeqNumberInc(res,sn) {                   \
      (res) = (sn);                              \
          if (++(res).low==0) (res).high++;      \
        }


/*****************************************************************/
/**
 * SeqNumberSub - addition of two sequential numbers
 * @res: result
 * @sn1: source sequential number 1
 * @sn2: source sequential number 2
 *
 * res = sn1 + sn2
 */
#define SeqNumberAdd(res,sn1,sn2) {              \
    (res).low = (sn1).low+(sn2).low;             \
    (res).high = (sn1).high+(sn2).high;          \
    if (((res).low < (sn1).low) ||               \
        ((res).low < (sn2).low)) {               \
      (res).high++;                              \
    }                                            \
}

/*****************************************************************/
/**
 * SeqNumberDec - decrementation of a sequence number
 * @res: result
 * @sn: sequential number to be decremented
 * 
 * res = sn - 1
 */
#define SeqNumberDec(res,sn) {                   \
      (res) = (sn);                              \
          if ((res).low--==0) (res).high--;      \
        }

/*****************************************************************/
/**
 * SeqNumberSub - substraction of two sequential numbers
 * @res: result
 * @sn1: source sequential number 1
 * @sn2: source sequential number 2
 *
 * res = sn1 - sn2
 */
#define SeqNumberSub(res,sn1,sn2) {                  \
    (res).low = (sn1).low-(sn2).low;                 \
    (res).high = (sn1).high-(sn2).high;              \
    if ((res).low > (sn1).low) {                     \
      (res).high--;                                  \
    }                                                \
}

/*****************************************************************/
/**
 * NtpTimeCmp - comparation of two NtpTimes
 * @time1: source time 1
 * @time2: source time 2
 *
 * Return value:
 * 1 if time 1 > time 2 
 * -1 if time 1 < time 2
 * 0 if time 1 = time 2
 */
#define NtpTimeCmp(time1, time2)                     \
((((time1).seconds) > ((time2).seconds)) ? 1 :       \
 ((((time1).seconds) < ((time2).seconds)) ? -1 :     \
  ((((time1).fraction) > ((time2).fraction)) ? 1 :   \
   ((((time1).fraction) < ((time2).fraction)) ? -1 : 0))))


/*****************************************************************/
/**
 * NtpTimeAdd - addition of two NtpTimes
 * @res: result
 * @time1: source time 1
 * @time2: source time 2
 *
 * res = time1 + time2
 */
#define NtpTimeAdd(res, time1, time2) {              \
    (res).seconds  = (time1).seconds + (time2).seconds;   \
    (res).fraction = (time1).fraction + (time2).fraction; \
    if (((res).fraction < (time1).fraction) ||       \
        ((res).fraction < (time2).fraction)) {       \
      (res).seconds++;                               \
    }                                                \
}

/*****************************************************************/
/**
 * NtpTimeSub - substraction of two NtpTimes
 * @res: result
 * @time1: source time 1
 * @time2: source time 2
 *
 * res = time1 - time2
 */
#define NtpTimeSub(res, time1, time2) {              \
   (res).seconds  = (time1).seconds - (time2).seconds;   \
   (res).fraction = (time1).fraction - (time2).fraction; \
   if ((res).fraction > (time1).fraction) {          \
     (res).seconds--;                                \
   }                                                 \
}

/*****************************************************************/
/**
 * NtpTimeAssembFromMs - converts seconds and miliseconds to NtpTime
 * @time: time given in NtpTime structure
 * @s: seconds portion of given time
 * @msec: miliseconds portion of given time
 */
#define NtpTimeAssembFromMs(time, s, msec) {         \
    register uint32_t ms = msec;                     \
    (time).seconds  = s;                             \
    (time).fraction = (ms<<22) + ((ms*393)<<8);      \
}

/**
 * NtpTimeDisAssembToMs - converts NtpTime to seconds and miliseconds
 * @s: seconds portion of given time
 * @msec: miliseconds portion of given time
 * @time: time given in NtpTime structure
 */
#define NtpTimeDisAssembToMs(s, msec, time) {        \
    s    = (time).seconds;                           \
    msec = ((time).fraction - ((time).fraction>>6) - \
           ((time).fraction>>7) + (1<<21))>>22;      \
    if ((msec) >= 1000 ) { (msec) -= 1000; (s)++; }  \
}

/**
 * NtpTimeAssembFromUs - converts seconds and useconds to NtpTime
 * @time: time given in NtpTime structure
 * @s: seconds portion of given time
 * @usec: microseconds portion of given time
 */
#define NtpTimeAssembFromUs(time, s, usec) {         \
    register uint32_t us = usec;                     \
    (time).seconds  = s;                             \
    (time).fraction = (us<<12)+ ((us*99)<<1)+ ((us*15 + ((us*61)>>7))>>4); \
}

/**
 * NtpTimeDisAssembToUs - converts NtpTime to seconds and useconds
 * @s: seconds portion of given time
 * @usec: microseconds portion of given time
 * @time: time given in NtpTime structure
 */
#define NtpTimeDisAssembToUs(s, usec, time) {        \
    register uint32_t NtpTemp = (time).fraction;     \
    s    = (time).seconds;                           \
    usec = ((time).fraction - (NtpTemp>>5)-(NtpTemp>>7)-(NtpTemp>>8)- \
            (NtpTemp>>9)-(NtpTemp>>10) - (NtpTemp>>12) - \
            (NtpTemp>>13)-(NtpTemp>>14) + (1<<11)) >> 12; \
    if ((usec) >= 1000000) { (usec) -= 1000000; (s)++; } \
}

/**
 * Domain2Port - converts Domain value to IP Port value
 * @d: domain
 * @p: port
 */
#define Domain2Port(d,p) {                   \
  p = RTPS_DEFAULT_PORT + d*10;              \
}

/**
 * Domain2PortMulticastUserdata - converts Domain value to userdata IP Port value
 * @d: domain
 * @p: port
 */
#define Domain2PortMulticastUserdata(d,p) {  \
    p = RTPS_DEFAULT_PORT + d*10+1;          \
}

/**
 * Domain2PortMulticastMetatraffic - converts Domain value to metatraffic IP Port value
 * @d: domain
 * @p: port
 */
#define Domain2PortMulticastMetatraffic(d,p) {        \
    p = RTPS_DEFAULT_PORT + d*10+2;          \
}


/* Align an address upward to a boundary, expressed as a number of bytes.
   E.g. align to an 8-byte boundary with argument of 8.  */

/*
 *   (this + boundary - 1)
 *          &
 *    ~(boundary - 1)
 */

#define ALIGN_ADDRESS(this, boundary) \
  ((void*)((( ((unsigned long)(this)) + (((unsigned long)(boundary)) -1)) & (~(((unsigned long)(boundary))-1)))))


#ifdef __cplusplus
} /* extern "C"*/
#endif

#endif /* _DEFINES_API_H */
