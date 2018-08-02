/*
 *                         IndigoSCADA
 *
 *   This software and documentation are Copyright 2002 to 2009 Enscada 
 *   Limited and its licensees. All rights reserved. See file:
 *
 *                     $HOME/LICENSE 
 *
 *   for full copyright notice and license terms. 
 *
 */
#ifndef __OPCDA_2_0_CLASSES_H
#define __OPCDA_2_0_CLASSES_H

//////////OPC DA 2.0//////////////////////////////////
//COPCCallback class derived from IOPCDataCallback
//used with async updates OPC 2.05a version
//////////////////////////////////////////////////////

class ATL_NO_VTABLE COPCCallback : 
	public CComObjectRoot,
	public IOPCDataCallback
{
public:

BEGIN_COM_MAP(COPCCallback)
	COM_INTERFACE_ENTRY(IOPCDataCallback)
END_COM_MAP()

	STDMETHODIMP OnDataChange( 
    /* [in] */ DWORD dwTransid,
    /* [in] */ OPCHANDLE hGroup,
    /* [in] */ HRESULT hrMasterquality,
    /* [in] */ HRESULT hrMastererror,
    /* [in] */ DWORD dwCount,
    /* [size_is][in] */ OPCHANDLE __RPC_FAR *phClientItems,
    /* [size_is][in] */ VARIANT __RPC_FAR *pvValues,
    /* [size_is][in] */ WORD __RPC_FAR *pwQualities,
    /* [size_is][in] */ FILETIME __RPC_FAR *pftTimeStamps,
    /* [size_is][in] */ HRESULT __RPC_FAR *pErrors)
	{
		IT_IT("COPCCallback::OnDataChange");
		
		if(FAILED(hrMastererror))
		{
			Opc_client_da_imp::LogMessage(hrMastererror,"Update");

			//Exiting process to recover
			Opc_client_da_imp::fExit = 1;
		}

		for(DWORD dw = 0; dw < dwCount; dw++)
		{
			IT_COMMENT2("phClientItems[%d] = %d", dw, phClientItems[dw]);
			IT_COMMENT2("pwQualities[%d] = %d", dw, pwQualities[dw]);

			//fprintf(stderr,"phClientItems[%d] = %d\t", dw, phClientItems[dw]);
			//fflush(stderr);
			//fprintf(stderr,"pwQualities[%d] = %d\t", dw, pwQualities[dw]);
			//fflush(stderr);

			if((pwQualities[dw] == OPC_QUALITY_GOOD) && SUCCEEDED(pErrors[dw]))
			{
				VARIANT *pValue = &pvValues[dw];

				const FILETIME* ft = reinterpret_cast<const FILETIME *>(&pftTimeStamps[dw]);
																										
				Opc_client_da_imp::SendEvent2(pValue, ft, pwQualities[dw], phClientItems[dw], 0x03); //spontaneous cause
			}
			else if((pwQualities[dw] != OPC_QUALITY_GOOD) && SUCCEEDED(pErrors[dw]))
			{
				VARIANT *pValue = &pvValues[dw];

				const FILETIME* ft = reinterpret_cast<const FILETIME *>(&pftTimeStamps[dw]);
																										
				Opc_client_da_imp::SendEvent2(pValue, ft, pwQualities[dw], phClientItems[dw], 0x03); //spontaneous cause
			}
			else
			{
				Opc_client_da_imp::SendEvent2(0, 0, pwQualities[dw], phClientItems[dw], 0x03);
				
				char name[100];
				strcpy(name, Opc_client_da_imp::Item[phClientItems[dw] - 1].spname);

				switch(pwQualities[dw])
				{
					case OPC_QUALITY_GOOD:
						Opc_client_da_imp::LogMessage(S_OK, "Quality Good",(const char*)name);
					break;
					case OPC_QUALITY_BAD:
					default:
						Opc_client_da_imp::LogMessage(S_OK, "Quality Bad",(const char*)name);
					break;
					case OPC_QUALITY_UNCERTAIN:
						Opc_client_da_imp::LogMessage(S_OK, "Quality UNCERTAIN",(const char*)name);
					break;
					case OPC_QUALITY_CONFIG_ERROR:
						Opc_client_da_imp::LogMessage(S_OK, "CONFIG ERROR",(const char*)name);
					break;
					case OPC_QUALITY_NOT_CONNECTED:
						Opc_client_da_imp::LogMessage(S_OK, "NOT CONNECTED",(const char*)name);
					break;
					case OPC_QUALITY_DEVICE_FAILURE:
						Opc_client_da_imp::LogMessage(S_OK, "DEVICE FAILURE",(const char*)name);
					break;
					case OPC_QUALITY_OUT_OF_SERVICE:
						Opc_client_da_imp::LogMessage(S_OK, "OUT OF SERVICE",(const char*)name);
					break;
				}
			}
		}

		IT_EXIT;
		return S_OK;
	}

	STDMETHODIMP OnReadComplete( 
    /* [in] */ DWORD dwTransid,
    /* [in] */ OPCHANDLE hGroup,
    /* [in] */ HRESULT hrMasterquality,
    /* [in] */ HRESULT hrMastererror,
    /* [in] */ DWORD dwCount,
    /* [size_is][in] */ OPCHANDLE __RPC_FAR *phClientItems,
    /* [size_is][in] */ VARIANT __RPC_FAR *pvValues,
    /* [size_is][in] */ WORD __RPC_FAR *pwQualities,
    /* [size_is][in] */ FILETIME __RPC_FAR *pftTimeStamps,
    /* [size_is][in] */ HRESULT __RPC_FAR *pErrors)
	{
		IT_IT("COPCCallback::OnReadComplete");
		
		if(FAILED(hrMastererror))
		{
			Opc_client_da_imp::LogMessage(hrMastererror,"General Async2 Read");
			//Exiting process to recover
			Opc_client_da_imp::fExit = 1;
			IT_EXIT;
			return S_FALSE;
		}

		for(DWORD dw = 0; dw < dwCount; dw++)
		{
			IT_COMMENT2("phClientItems[%d] = %d", dw, phClientItems[dw]);
			IT_COMMENT2("pwQualities[%d] = %d", dw, pwQualities[dw]);

			//fprintf(stderr,"phClientItems[%d] = %d\t", dw, phClientItems[dw]);
			//fflush(stderr);
			//fprintf(stderr,"pwQualities[%d] = %d\t", dw, pwQualities[dw]);
			//fflush(stderr);

			if((pwQualities[dw] == OPC_QUALITY_GOOD) && SUCCEEDED(pErrors[dw]))
			{
				VARIANT *pValue = &pvValues[dw];

				const FILETIME* ft = reinterpret_cast<const FILETIME *>(&pftTimeStamps[dw]);
																										
				Opc_client_da_imp::SendEvent2(pValue, ft, pwQualities[dw], phClientItems[dw], 0x14);//GI cause
			}
			else if((pwQualities[dw] != OPC_QUALITY_GOOD) && SUCCEEDED(pErrors[dw]))
			{
				VARIANT *pValue = &pvValues[dw];

				const FILETIME* ft = reinterpret_cast<const FILETIME *>(&pftTimeStamps[dw]);
																										
				Opc_client_da_imp::SendEvent2(pValue, ft, pwQualities[dw], phClientItems[dw], 0x14);//GI cause
			}
			else
			{
				Opc_client_da_imp::SendEvent2(0, 0, pwQualities[dw], phClientItems[dw], 0x14);//GI cause

				char name[100];
				strcpy(name, Opc_client_da_imp::Item[phClientItems[dw] - 1].spname);

				switch(pwQualities[dw])
				{
					case OPC_QUALITY_GOOD:
						Opc_client_da_imp::LogMessage(S_OK, "Quality Good",(const char*)name);
					break;
					case OPC_QUALITY_BAD:
					default:
						Opc_client_da_imp::LogMessage(S_OK, "Quality Bad",(const char*)name);
					break;
					case OPC_QUALITY_UNCERTAIN:
						Opc_client_da_imp::LogMessage(S_OK, "Quality UNCERTAIN",(const char*)name);
					break;
					case OPC_QUALITY_CONFIG_ERROR:
						Opc_client_da_imp::LogMessage(S_OK, "CONFIG ERROR",(const char*)name);
					break;
					case OPC_QUALITY_NOT_CONNECTED:
						Opc_client_da_imp::LogMessage(S_OK, "NOT CONNECTED",(const char*)name);
					break;
					case OPC_QUALITY_DEVICE_FAILURE:
						Opc_client_da_imp::LogMessage(S_OK, "DEVICE FAILURE",(const char*)name);
					break;
					case OPC_QUALITY_OUT_OF_SERVICE:
						Opc_client_da_imp::LogMessage(S_OK, "OUT OF SERVICE",(const char*)name);
					break;
				}
			}
		}

		IT_COMMENT("General interrogation is complete!");
		fprintf(stderr,"General interrogation is complete!\n");
		fflush(stderr);
		IT_EXIT;
		return S_OK;
	}

	STDMETHODIMP OnWriteComplete( 
    /* [in] */ DWORD dwTransid,
    /* [in] */ OPCHANDLE hGroup,
    /* [in] */ HRESULT hrMastererr,
    /* [in] */ DWORD dwCount,
    /* [size_is][in] */ OPCHANDLE __RPC_FAR *pClienthandles,
    /* [size_is][in] */ HRESULT __RPC_FAR *pErrors)
	{
		struct iec_item item_to_send;
		USES_CONVERSION;
		IT_IT("COPCCallback::OnWriteComplete");

		//assert(dwCount == 1);

		memset(&item_to_send,0x00, sizeof(struct iec_item));
		
		if(FAILED(hrMastererr))
		{
			Opc_client_da_imp::LogMessage(hrMastererr,"General Async2 Write");
			//Exiting process to recover
			Opc_client_da_imp::fExit = 1;
		}

		if(Opc_client_da_imp::g_dwWriteTransID != dwTransid)
		{
			Opc_client_da_imp::LogMessage(S_OK,"Async2 Write callback, TransactionID's do not match");
			//Exiting process to recover
			Opc_client_da_imp::fExit = 1;
		}

		for(DWORD dw = 0; dw < dwCount; dw++)
		{
			item_to_send.msg_id = 0;
			item_to_send.iec_obj.ioa = Opc_client_da_imp::Item[pClienthandles[dw] - 1].ioa_control_center;
			//strcpy(item_to_send.opc_server_item_id, (const char*)W2T(Opc_client_da_imp::Item[pClienthandles[dw] - 1].wszName));

			item_to_send.iec_type = Opc_client_da_imp::Item[pClienthandles[dw] - 1].io_list_iec_type;

			fprintf(stderr,"Sending ack command for hClient %d, ioa %d, ItemID %s\n", 
				pClienthandles[dw], 
				item_to_send.iec_obj.ioa, 
				(const char*)W2T(Opc_client_da_imp::Item[pClienthandles[dw] - 1].wszName));
			fflush(stderr);
			
			//Item[pClienthandles[dw] - 1].iec_type may be 45 46 48 49 50 61 62 63
									
			if(FAILED(pErrors[dw]))
			{
				Opc_client_da_imp::LogMessage(pErrors[dw], "Async2 Write request");
				//Exiting process to recover
				Opc_client_da_imp::fExit = 1;
			}

			switch(pErrors[dw])
			{
				case S_OK:
				{
					//The data item was written!
					//Send message to monitor.exe telling the command WAS executed!

					switch(Opc_client_da_imp::Item[pClienthandles[dw] - 1].io_list_iec_type)
					{
						case C_SC_NA_1:
							item_to_send.iec_obj.o.type45.qu = 0x11; // In our implementation, this reserved value means that command is executed with success
						break;
						case C_SC_TA_1:
							item_to_send.iec_obj.o.type58.qu = 0x11; // In our implementation, this reserved value means that command is executed with success
						break;
						case C_DC_NA_1:
							item_to_send.iec_obj.o.type46.qu = 0x11; // In our implementation, this reserved value means that command is executed with success
						break;
						case C_DC_TA_1:
							item_to_send.iec_obj.o.type59.qu = 0x11; // In our implementation, this reserved value means that command is executed with success
						break;
						case C_BO_NA_1:
							item_to_send.iec_obj.o.type51.stcd.cd[0] = 0x11; // In our implementation, this value means that command is executed with success
						break;
						case C_BO_TA_1:
							item_to_send.iec_obj.o.type64.stcd.cd[0] = 0x11; // In our implementation, this reserved value means that command is executed with success
						break;
						case C_SE_NA_1:
							item_to_send.iec_obj.o.type48.ql = 0x11; // In our implementation, this reserved value means that command is executed with success
						break;
						case C_SE_TA_1:
							item_to_send.iec_obj.o.type61.ql = 0x11; // In our implementation, this reserved value means that command is executed with success
						break;
						case C_SE_NB_1:
							item_to_send.iec_obj.o.type49.ql = 0x11; // In our implementation, this reserved value means that command is executed with success
						break;
						case C_SE_TB_1:
							item_to_send.iec_obj.o.type62.ql = 0x11; // In our implementation, this reserved value means that command is executed with success
						break;
						case C_SE_NC_1:
							item_to_send.iec_obj.o.type50.ql = 0x11; // In our implementation, this reserved value means that command is executed with success
						break;
						case C_SE_TC_1:
							item_to_send.iec_obj.o.type63.ql = 0x11; // In our implementation, this reserved value means that command is executed with success
						break;
						default:
						break;
					}
				}
				break;
				case OPC_E_BADRIGHTS:
					//NOT executed, no permission of write
					fprintf(stderr,"NOT executed, no permission of write\n");
					fflush(stderr);
				break;
				case OPC_E_INVALIDHANDLE:
					//NOT executed, the passed item handle was invalid
					fprintf(stderr,"NOT executed, the passed item handle was invalid\n");
					fflush(stderr);
				break;
				case OPC_E_UNKNOWNITEMID:
					//NOT executed, unknown item id
					fprintf(stderr,"NOT executed, unknown item id\n");
					fflush(stderr);
				break;
				default:
					//S_XXX
					//The data item WAS written but there is a vendor 
					//specific warning (for example the value was clamped)
					//Send message to parent that command WAS executed!

					//E_XXX
					//The data item was NOT written and there is a vendor 
					//specific error which provide more information 
					//(for example the device is offline). 
					fprintf(stderr,"NOT executed, no permission of write\n");
					fflush(stderr);
				break;
			}
		}

		//Opc_client_da_imp::g_bWriteComplete = true;

		//Send message to monitor.exe///////////////////////////////////////////////////////////////////
		item_to_send.checksum = clearCrc((unsigned char *)&item_to_send, sizeof(struct iec_item));
		////////Middleware/////////////
		//publishing data
		//Opc_client_da_imp::queue_monitor_dir->put(&item_to_send, sizeof(struct iec_item));
		////////Middleware/////////////

		//Sleep(10);

		//Send in monitor direction
		//prepare published data
		/*
		memset(&(Opc_client_da_imp::instanceSend),0x00, sizeof(iec_item_type));

		Opc_client_da_imp::instanceSend.iec_type = item_to_send.iec_type;
		memcpy(&(Opc_client_da_imp::instanceSend.iec_obj), &(item_to_send.iec_obj), sizeof(struct iec_object));
		Opc_client_da_imp::instanceSend.cause = item_to_send.cause;
		Opc_client_da_imp::instanceSend.msg_id = item_to_send.msg_id;
		Opc_client_da_imp::instanceSend.ioa_control_center = item_to_send.ioa_control_center;
		Opc_client_da_imp::instanceSend.casdu = item_to_send.casdu;
		Opc_client_da_imp::instanceSend.is_neg = item_to_send.is_neg;
		Opc_client_da_imp::instanceSend.checksum = item_to_send.checksum;

		ORTEPublicationSend(Opc_client_da_imp::publisher);
		*/

		//Client must always return S_OK in this function
		IT_EXIT;
		return S_OK;
	}

	STDMETHODIMP OnCancelComplete( 
    /* [in] */ DWORD dwTransid,
    /* [in] */ OPCHANDLE hGroup)
	{
		IT_IT("COPCCallback::OnCancelComplete");
		IT_EXIT;
		return S_OK;
	}
};

typedef CComObject<COPCCallback> CComCOPCCallback;

#endif //__OPCDA_2_0_CLASSES_H