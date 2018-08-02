/**************************************************************************
 *                                                                        *
 * Light OPC Server development library                                   *
 *                                                                        *
 *   Copyright (c) 2000 by Timofei Bondarenko                             *
 **************************************************************************/

#ifndef PROPLIST_H
#define PROPLIST_H

/**************************************************************************
 Internal declarations of Properties and PropList
 **************************************************************************/
#ifndef OPTIONS_H
#include "options.h"
#endif

#ifdef __cplusplus
extern "C"
	{
#endif

struct loProperty
  {
   struct 
   loProperty    *prNext;
   unsigned       prPropID;
   const char    *prDescription;
   const loWchar *prDescriptionW;
   VARIANT        prValue;
   loWchar        prPath[1];
  };

typedef struct loProperty loProperty;


#define lo_proplist(se,undex) ((se)->proplist_count > (undex)? (se)->proplist + (undex): 0)

loProperty **lo_prop_locate(loProperty **prl, unsigned propid);
loProperty **lo_prop_find(loProperty **prl, unsigned propid);

void lo_proplist_clear(loService *se);
int lo_proplist_init(loService *se);

/**********************************************************************/

typedef struct loBaseName
  {
   const loWchar *base_name;
   int            base_level;
   int            base_len;
   loWchar        brsep;
  } loBaseName;

void loBaseName_init(loBaseName *bn, const loTagAttrib *ta, loWchar brsep);
const loWchar *lo_relative_name(loStringBuf *sb, loBaseName *bn, const loWchar *rel);

typedef struct loRelName
  {
   loStringBuf sb;
   loBaseName  bn;
   loService  *se;
   loCaller *cactx;
   int     locked; /* is se->lkMgmt already locked ? */
   HRESULT    err; /* result */
  } loRelName;

loTagId lo_relative_tag(loRelName *rn, const loWchar *rel, int ai_goal);

#ifdef __cplusplus
         }
#endif
#endif /*PROPLIST_H*/
