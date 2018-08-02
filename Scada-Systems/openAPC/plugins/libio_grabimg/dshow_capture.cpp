/********************************************************************************************

These sources are distributed in the hope that they will be useful, but WITHOUT ANY WARRANTY;
without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. All
the information given here, within the interface descriptions and within the specification
are subject to change without notice. Errors and omissions excepted.

These sources demonstrate the usage of the OpenAPC Plug-In programming interface. They can be
used freely according to the OpenAPC Dual License: As long as the sources and the resulting
applications/libraries/Plug-Ins are used together with the OpenAPC software, they are
licensed as freeware. When you use them outside the OpenAPC software they are licensed under
the terms of the GNU General Public License.

For more information please refer to http://www.openapc.com/oapc_license.php

*********************************************************************************************/


#ifdef ENV_WINDOWS

#pragma warning(disable: 4995)

#include "oapc_libio.h"
#include "libio_grabimg.h"
#include "dshow_capture.h"



EXTERN_C const CLSID CLSID_NullRenderer;
EXTERN_C const CLSID CLSID_SampleGrabber;



static void releaseDevContext(struct device_context *devContext)
{
   if (devContext->pMediaControl != NULL)        
   {
      if (devContext->graphRunning) devContext->pMediaControl->Stop();
      devContext->pMediaControl->Release();	
      devContext->pMediaControl=NULL;
   }
   if (devContext->pNullRenderer != NULL)        devContext->pNullRenderer->Release();
   devContext->pNullRenderer=NULL;

   if (devContext->pSampleGrabber != NULL)       devContext->pSampleGrabber->Release();
   devContext->pSampleGrabber=NULL;

   if (devContext->pSampleGrabberFilter != NULL) devContext->pSampleGrabberFilter->Release();
   devContext->pSampleGrabberFilter=NULL;

   if (devContext->pCap != NULL)                 devContext->pCap->Release();
   devContext->pCap=NULL;

//   if (devContext->pBuilder != NULL)             devContext->pBuilder->Release(); leads to crash when no devices could be found
   
   if (devContext->pGraph != NULL)               devContext->pGraph->Release();
   devContext->pGraph=NULL;

   if (devContext->pMoniker != NULL)             devContext->pMoniker->Release();
   devContext->pMoniker=NULL;

   if (devContext->pEnum != NULL)                devContext->pEnum->Release();
   devContext->pEnum=NULL;

//   if (devContext->pDevEnum != NULL)             devContext->pDevEnum->Release(); leads to crash when no devices could be found
   CoUninitialize();
}


static void setCaptureFormat(struct device_context *devContext)
{
   HRESULT hr = S_OK;
   int     width=0,height=0;

   if(devContext->pBuilder == NULL) return;

   IAMStreamConfig *pConfig = NULL;

   hr = devContext->pBuilder->FindInterface(&PIN_CATEGORY_CAPTURE,0,devContext->pCap,IID_IAMStreamConfig, (void**)&pConfig);
   if (hr!=S_OK) return;

   int iCount = 0, iSize = 0;
   hr = pConfig->GetNumberOfCapabilities(&iCount, &iSize);
   if (iSize == sizeof(VIDEO_STREAM_CONFIG_CAPS)) 
   {
      for (int iFormat = 0; iFormat < iCount; iFormat++)
      {
         VIDEO_STREAM_CONFIG_CAPS scc;
         AM_MEDIA_TYPE *pmt;
         hr = pConfig->GetStreamCaps(iFormat, &pmt, (BYTE*)&scc);
         if (hr==S_OK)
         {
            if (pmt->formattype == FORMAT_VideoInfo) 
            {
               if (pmt->cbFormat >= sizeof(VIDEOINFOHEADER))
               {
                  VIDEOINFOHEADER *pVih =  reinterpret_cast<VIDEOINFOHEADER*>(pmt->pbFormat);
                  BITMAPINFOHEADER *bmiHeader = &pVih->bmiHeader;

                  if ((bmiHeader->biWidth>=width) && (bmiHeader->biHeight>=height) && (bmiHeader->biBitCount==24))
                  {
                     printf("Res: %dx%d\r\n",bmiHeader->biWidth,bmiHeader->biHeight);
                     hr = pConfig->SetFormat(pmt);                     
                     width=bmiHeader->biWidth;
                     height=bmiHeader->biHeight;
                     if (hr!=S_OK) printf("SetFormat() failed: %d\r\n",hr);
                  }
               }
            }
            if (pmt->cbFormat)
            {
               CoTaskMemFree((PVOID)pmt->pbFormat);
               pmt->cbFormat = 0;
               pmt->pbFormat = NULL;
            }
            if (pmt->pUnk)
            {
               pmt->pUnk->Release();
               pmt->pUnk = NULL;
            }
         }
      }
   }
}


int openVideoDevice(struct instData *data,int device_number)
{
   int     n;
   HRESULT hr;
	
   memset(&data->devContext,0,sizeof(struct device_context));
   hr= CoInitializeEx(NULL, COINIT_MULTITHREADED);
   if ((hr!=S_OK) && (hr!=RPC_E_CHANGED_MODE))
   {
      releaseDevContext(&data->devContext);
      return OAPC_ERROR_RESOURCE;
   }

	hr= CoCreateInstance(CLSID_FilterGraph, NULL,CLSCTX_INPROC_SERVER, IID_IGraphBuilder,(void**)&data->devContext.pGraph);
	if (hr!= S_OK)
    {
		releaseDevContext(&data->devContext);
        return OAPC_ERROR_RESOURCE;
    }
    
	hr= CoCreateInstance(CLSID_CaptureGraphBuilder2, NULL,CLSCTX_INPROC_SERVER, IID_ICaptureGraphBuilder2,(void **)&data->devContext.pBuilder);
	if (hr!= S_OK)
    {
		releaseDevContext(&data->devContext);
        return OAPC_ERROR_RESOURCE;
    }

	hr= data->devContext.pBuilder->SetFiltergraph(data->devContext.pGraph);
	if (hr!= S_OK)
    {
		releaseDevContext(&data->devContext);
        return OAPC_ERROR_RESOURCE;
    }

	hr= CoCreateInstance(CLSID_SystemDeviceEnum, NULL,CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&data->devContext.pDevEnum));
	if (hr!= S_OK)
    {
		releaseDevContext(&data->devContext);
        return OAPC_ERROR_RESOURCE;
    }

	hr= data->devContext.pDevEnum->CreateClassEnumerator(CLSID_VideoInputDeviceCategory, &data->devContext.pEnum, 0);
	if (hr!= S_OK)
    {
		releaseDevContext(&data->devContext);
        return OAPC_ERROR_DEVICE;
    }

	n = 0;
	for (;;)
	{
		hr= data->devContext.pEnum->Next(1, &data->devContext.pMoniker, NULL);
		if (hr== S_OK) n++;
		else
		{
			releaseDevContext(&data->devContext);
            return OAPC_ERROR_DEVICE;
		}		
		if (n >= device_number+1) break;
	}
	
	hr= data->devContext.pMoniker->BindToObject(0, 0,IID_IBaseFilter, (void**)&data->devContext.pCap);
	if (hr!= S_OK) 
    {
        releaseDevContext(&data->devContext);
        return OAPC_ERROR_RESOURCE;
    }
		
	hr= data->devContext.pGraph->AddFilter(data->devContext.pCap, L"Capture Filter");
	if (hr!= S_OK) 
    {
        releaseDevContext(&data->devContext);
        return OAPC_ERROR_RESOURCE;
    }

	hr= CoCreateInstance(CLSID_SampleGrabber, NULL,CLSCTX_INPROC_SERVER, IID_IBaseFilter,(void**)&data->devContext.pSampleGrabberFilter);
	if (hr!= S_OK)
    {
		releaseDevContext(&data->devContext);
        return OAPC_ERROR_RESOURCE;
    }

	hr= data->devContext.pSampleGrabberFilter->QueryInterface(DexterLib::IID_ISampleGrabber, (void**)&data->devContext.pSampleGrabber);
	if (hr!= S_OK)
    {
		releaseDevContext(&data->devContext);
        return OAPC_ERROR_RESOURCE;
    }

	hr= data->devContext.pSampleGrabber->SetBufferSamples(TRUE);
	if (hr!= S_OK)
    {
		releaseDevContext(&data->devContext);
        return OAPC_ERROR_RESOURCE;
    }

    data->devContext.mt.majortype = MEDIATYPE_Video;
    data->devContext.mt.subtype = MEDIASUBTYPE_RGB24;
    hr= data->devContext.pSampleGrabber->SetMediaType((DexterLib::_AMMediaType *)&data->devContext.mt);
    if (hr!= S_OK)
    {
        releaseDevContext(&data->devContext);
        return OAPC_ERROR_RESOURCE;
    }

	hr= data->devContext.pGraph->AddFilter(data->devContext.pSampleGrabberFilter, L"SampleGrab");
	if (hr!= S_OK)
    {
		releaseDevContext(&data->devContext);
        return OAPC_ERROR_RESOURCE;
    }

    //NullRenderer to avoid DirecTShow opens an own window
	hr= CoCreateInstance(CLSID_NullRenderer, NULL,CLSCTX_INPROC_SERVER, IID_IBaseFilter,(void**)&data->devContext.pNullRenderer);
	if (hr!= S_OK)
    {
		releaseDevContext(&data->devContext);
        return OAPC_ERROR_RESOURCE;
    }
	hr= data->devContext.pGraph->AddFilter(data->devContext.pNullRenderer, L"NullRender");
	if (hr!= S_OK)
    {
		releaseDevContext(&data->devContext);
        return OAPC_ERROR_RESOURCE;
    }

    setCaptureFormat(&data->devContext);

	hr= data->devContext.pBuilder->RenderStream(&PIN_CATEGORY_CAPTURE, &MEDIATYPE_Video,data->devContext.pCap,  data->devContext.pSampleGrabberFilter, data->devContext.pNullRenderer);
	if (hr!= S_OK)
    {
		releaseDevContext(&data->devContext);
        return OAPC_ERROR_RESOURCE;
    }
	
	hr=data->devContext.pGraph->QueryInterface(IID_IMediaControl,(void**)&data->devContext.pMediaControl);
	if (hr!= S_OK)
    {
        releaseDevContext(&data->devContext);
        return OAPC_ERROR_RESOURCE;
    }
    return OAPC_OK;
}


int captureImage(struct instData *data)
{
   HRESULT hr;
   long    bufLen=0;

   if (!data->devContext.graphRunning)
   {
      for (;;)
      {
         oapc_thread_sleep(0);
         hr=data->devContext.pMediaControl->Run();
         if (hr== S_OK) break;
         else if (hr== S_FALSE) continue;
         return OAPC_ERROR_RECV_DATA;
      }
      data->devContext.graphRunning=true;
   }

   for (;;)
   {
      hr= data->devContext.pSampleGrabber->GetCurrentBuffer(&bufLen, NULL);
      if (hr== S_OK && bufLen != 0) break;
      if (hr!= S_OK && hr!= VFW_E_WRONG_STATE)
      {
         printf("Buffer size error: %u\n",hr);
         return OAPC_ERROR_RECV_DATA;
      }
   }

   if (!data->rawBinBuf) data->rawBinBuf=oapc_util_alloc_bin_data(OAPC_BIN_TYPE_IMAGE,OAPC_BIN_SUBTYPE_IMAGE_RGB24,OAPC_COMPRESS_NONE,bufLen);
   if (!data->rawBinBuf) return OAPC_ERROR_NO_MEMORY;
	
   hr= data->devContext.pSampleGrabber->GetCurrentBuffer(&bufLen,(long*)&data->rawBinBuf->data);
   if (hr!= S_OK) return OAPC_ERROR_RECV_DATA;

   hr= data->devContext.pSampleGrabber->GetConnectedMediaType((DexterLib::_AMMediaType *)&data->devContext.mt);
   if (hr!= S_OK) return OAPC_ERROR_RECV_DATA;

   VIDEOINFOHEADER *pVih = NULL;
   if ((data->devContext.mt.formattype == FORMAT_VideoInfo) && 
       (data->devContext.mt.cbFormat >= sizeof(VIDEOINFOHEADER)) &&
       (data->devContext.mt.pbFormat != NULL)) 
   {
      pVih = (VIDEOINFOHEADER*)data->devContext.mt.pbFormat;

      data->width=pVih->bmiHeader.biWidth;
      data->height=pVih->bmiHeader.biHeight;
   }
   else return OAPC_ERROR_RECV_DATA;
   return OAPC_OK;
}


void closeVideoDevice(struct instData *data)
{
    if ((data->devContext.graphRunning) && (data->devContext.pMediaControl)) data->devContext.pMediaControl->Stop();
    data->devContext.graphRunning=false;
    if (data->devContext.mt.cbFormat != 0)
    {
        CoTaskMemFree((PVOID)data->devContext.mt.pbFormat);
        data->devContext.mt.cbFormat = 0;
        data->devContext.mt.pbFormat = NULL;
    }
    if (data->devContext.mt.pUnk != NULL)
    {
        data->devContext.mt.pUnk->Release();
        data->devContext.mt.pUnk = NULL;
    }
    releaseDevContext(&data->devContext);
}


#endif //ENV_WINDOWS
