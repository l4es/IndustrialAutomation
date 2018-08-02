#ifndef DSHOW_CAPTURE_H
#define DSHOW_CAPTURE_H

#ifdef ENV_WINDOWS

#include <dshow.h>

#import "qedit.dll" raw_interfaces_only named_guids

struct device_context
{
   ICreateDevEnum            *pDevEnum;
   IEnumMoniker              *pEnum;
   IMoniker                  *pMoniker;
   IGraphBuilder             *pGraph;
   ICaptureGraphBuilder2     *pBuilder;
   IBaseFilter               *pCap,*pSampleGrabberFilter;
   DexterLib::ISampleGrabber *pSampleGrabber;
   IBaseFilter               *pNullRenderer;
   IMediaControl             *pMediaControl;
   AM_MEDIA_TYPE              mt;
   bool                       graphRunning;
};

extern int  openVideoDevice(struct instData *data,int device_number);
extern int  captureImage(struct instData *data);
extern void closeVideoDevice(struct instData *data);

#endif //ENV_WINDOWS

#endif //DSHOW_CAPTURE_H
