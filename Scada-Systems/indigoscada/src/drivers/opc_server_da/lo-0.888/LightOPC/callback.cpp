/**************************************************************************
 *                                                                        *
 * Light OPC Server development library                                   *
 *                                                                        *
 *   Copyright (c) 2000 by Timofei Bondarenko                             *
                                                                          *
 General call for callbacks
 **************************************************************************/

#include "privopc.h"

#define loGET_IFACE(to,from,IFACE,model)  (((to) = (IFACE*)(from))->AddRef())

/*********************************************************************************/

int LightOPCServer::send_callback(LightOPCGroup *grp, 
                                  unsigned group_key, loUpdList *upl, int advmask)
{
 int tr = 0;
 IOPCDataCallback *c_databack = 0;
 IAdviseSink *c_datatime = 0,
             *c_dataonly = 0,
             *c_writecomp = 0;
 unsigned cform_dtime, cform_donly, cform_wrcompl;
 OPCHANDLE client_handle;

// UL_DEBUG((LOGID, "send_notify(){..."));
 if (!upl->used)
   {
    if (!upl->trqid) return 0;
    if (S_OK == upl->master_err) upl->master_err = S_FALSE;
   }

 if (FAILED(upl->master_err)) upl->master_qual = S_FALSE;

 if (!grp)
   {
    lock_read();
    if (0 == (grp = by_handle_g(group_key)))
      {
       unlock();
       UL_INFO((LOGID, "send_callback(%x) FAILED: No such group"));
       return 0;
      }
   }
#if LO_USE_BOTHMODEL && 0
 else group_key = grp->ServerHandle;
#endif
 client_handle = grp->ClientHandle;

/* There is only client_sched() thread allowed to change connection info under read_lock ! */
 if ((advmask & loRQ_CONN_DATABACK) && grp->conn_databack)
   loGET_IFACE(c_databack, grp->conn_databack, IOPCDataCallback, grp->initphase);

 if (advmask & (loRQ_CONN_DATA_1 | loRQ_CONN_WRITECOMPL))
   {
    if ((advmask & loRQ_CONN_DATATIME) && grp->conn_datatime)
      {
       cform_dtime = se->cform_datatime;
       loGET_IFACE(c_datatime, grp->conn_datatime, IAdviseSink, grp->initphase);
/* ?? advise_present &= ~loRQ_CONN_DATATIME */
      }

    if ((advmask & loRQ_CONN_DATAONLY) && grp->conn_dataonly)
      {
       cform_donly = se->cform_dataonly;
       loGET_IFACE(c_dataonly, grp->conn_dataonly, IAdviseSink, grp->initphase);
/* ?? advise_present &= ~loRQ_CONN_DATAONLY */
      }

    if ((advmask & loRQ_CONN_WRITECOMPL) && grp->conn_writecompl)
      {
       cform_wrcompl = se->cform_writecompl;
       loGET_IFACE(c_writecomp, grp->conn_writecompl, IAdviseSink, grp->initphase);
/* ?? advise_present &= ~loRQ_CONN_WRITECOMPL */
      }
   }

 unlock();

// UL_DEBUG((LOGID, "send_notify() #3"));
 if (c_databack)
   {
    HRESULT hr = E_INVALIDARG;

    switch(advmask & loRQ_OPER_MASK)
      {
    case loRQ_OP_REFRESH:
       hr = c_databack->OnDataChange(upl->trqid, client_handle,
            upl->master_err, upl->master_qual, upl->used, upl->opchandle,
            upl->variant, upl->quality, upl->timestamp, upl->errors);
       break;
    case loRQ_OP_READ:
       hr = c_databack->OnReadComplete(upl->transaction_id, client_handle,
            upl->master_err, upl->master_qual, upl->used, upl->opchandle,
            upl->variant, upl->quality, upl->timestamp, upl->errors);
       break;
    case loRQ_OP_WRITE:
       hr = c_databack->OnWriteComplete(upl->transaction_id, client_handle,
            upl->master_err, upl->used, upl->opchandle, upl->errors);
       break;
    case loRQ_OP_CANCEL:
       hr = c_databack->OnCancelComplete(upl->transaction_id, client_handle);
       break;
      }
    c_databack->Release();
    if (S_OK == hr) 
      {
       tr++;
       UL_DEBUG((LOGID, "OnDataChange2(0x%X) Ok", advmask));
      }
    else
      {
       UL_WARNING((LOGID, "%!l OnDataChange2(0x%X) FAILED", hr, advmask));
      }
//    UL_WARNING((LOGID, "OnDataChange() 2 Ok"));
   }

 if (advmask & loRQ_CONN_DATA_1)
   {
    HGLOBAL gmem = 0;
    unsigned datasize = 0;

    if (c_datatime)
      {
//       UL_DEBUG((LOGID, "OnDataChange1() with time"));
       if (0 <= lo_OnDataChange1(upl, client_handle, cform_dtime,
                                1, c_datatime, &gmem, &datasize)) tr++;
       c_datatime->Release();
      }
    if (c_dataonly)
      {
//       UL_DEBUG((LOGID, "OnDataChange1() without time"));
       if (0 <= lo_OnDataChange1(upl, client_handle, cform_donly,
                                0, c_dataonly, &gmem, &datasize)) tr++;
       c_dataonly->Release();
      }

    if (gmem && (// && (!GlobalSize(gmem) ||*/ GlobalFree(gmem)))
      //(GlobalFlags(gmem) & (GMEM_INVALID_HANDLE | GMEM_DISCARDED)) || 
        GlobalFree(gmem) ) )
      {
       UL_WARNING((LOGID, "%!L GlobalFree(%X) FAILED flags=0x%X", 
                           gmem, GlobalFlags(gmem)));       
      }
   }
 else if (c_writecomp)
   {
//    UL_DEBUG((LOGID, "OnWriteComplete1()"));
    if (0 <= lo_OnWriteComplete1(upl, client_handle, cform_wrcompl,
                                 c_writecomp, 0)) tr++;
    c_writecomp->Release();
   }

// UL_DEBUG((LOGID, "send_datachange() finished}"));
 return tr;
}

/**************************************************************************/
/* end of callback.cpp */
