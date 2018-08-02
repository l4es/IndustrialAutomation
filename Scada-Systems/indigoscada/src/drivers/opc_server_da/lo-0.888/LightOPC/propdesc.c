/**************************************************************************
 *                                                                        *
 * Light OPC Server development library                                   *
 *                                                                        *
 *   Copyright (c) 2001  Timofei Bondarenko                               *
                                                                          *
 ItemProperties support: Property Descriptions
 **************************************************************************/

#include <stdio.h> /* sprintf() */
#include "util.h"


#define loNUM "\007"

static const char
    *pd001[8+1] = /* ...008 */
   {
    loNUM "OPC Specific Propery [ID Set 1] related to the OPC Server",
    "Item Canonical DataType",
    "Item Value",
    "Item Quality",
    "Item Timestamp",
    "Item Access Rights",
    "Server Scan Rate",
    "Item EU Type",
    "Item EU Info"
   },
   *pd100[9+1] =  /* ...108 */
   {
    loNUM "Recommended Property [ID Set 2] related to the Item Value"
#if LO_NONBASIC_PROPDESCR
      ,
    "EU Units",
    "Item Description",
    "High EU",
    "Low EU",
    "High Instrument Range",
    "Low Instrument Range",
    "Contact Close Label",
    "Contact Open Label",
    "Item Timezone"
#endif
   },
   *pd200[8+1] = /* ...207 */
   {
    loNUM "Recommended Property [ID Set 2] related operator displays"
#if LO_NONBASIC_PROPDESCR
      ,
    "Default Display",
    "Current Foreground Color",
    "Current Background Color",
    "Current Blink",
    "BMP File",
    "Sound File",
    "HTML File",
    "AVI File"
#endif
   },
   *pd300[13+1] = /* ...312 */
   {
    loNUM "Recommended Property [ID Set 2] related to Alarm and Condition Values"
#if LO_NONBASIC_PROPDESCR
      ,
    "Condition Status",
    "Alarm Quick Help",
    "Alarm Area List",
    "Primary Alarm Area",
    "Condition Logic",
    "Limit Exceeded",
    "Deadband",
    "HiHi Limit",
    "Hi Limit",
    "Lo Limit",
    "LoLo Limit",
    "Rate of Change Limit",
    "Deviation Limit"
#endif
   },
    pd400[] = loNUM "Recommended Property [ID Set 2] Reserved for OPC Batch",
    pd1000[] = loNUM "Recommended Property [ID Set 2]",
    pd5000[] = loNUM "Vendor specefic Property [ID Set 3]";

const char *lo_prop_descr(unsigned propid)
{
 if (propid < 100)
   return pd001[propid < SIZEOF_ARRAY(pd001)? propid: 0];
 if (propid < 200)
   return pd100[propid - 100 + 1 < SIZEOF_ARRAY(pd100)? propid - 100 + 1: 0];
 if (propid < 300)
   return pd200[propid - 200 + 1 < SIZEOF_ARRAY(pd200)? propid - 200 + 1: 0];
 if (propid < 400)
   return pd300[propid - 300 + 1 < SIZEOF_ARRAY(pd300)? propid - 300 + 1: 0];
 if (propid < 1000) return pd400;
 if (propid < 5000) return pd1000;
 return pd5000;
}

loWchar *lo_ComPropDescr(unsigned propid, const char *pd)
{
 char buf[128];

 if (!pd) pd = lo_prop_descr(propid)/*loNUM "Unknown Propery"*/;
 if (*pd == *loNUM)
   {
    sprintf(buf, "%d %s (0x%X)", propid, pd + 1, propid);
    pd = buf;
   }
 return loComMWstrdup(pd);
}

/* end of propdesc.c */
