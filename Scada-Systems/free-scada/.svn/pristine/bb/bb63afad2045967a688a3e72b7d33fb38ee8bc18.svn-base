#pragma once

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers

//STL
#pragma warning(push,3) //Disable unreasonable warnings
#include <vector>
#include <functional>
#include <algorithm>
#include <list>
#pragma warning(pop)

using namespace std;

#include "Common.h"
#include "ObjectMenager.h"
#include "ObjRectangle.h"
#include "ObjGraph.h"
#include "ObjPolyline.h"
#include "ObjEllipse.h"
#include "ObjImage.h"
#include "ObjText.h"
#include "ObjButton.h"
#include "ObjInput.h"
#include "ObjSound.h"
#include "Selection.h"
#include "Track.h"
#include "ArchiveManager.h"
#include "MemFile.h"
#include "Equation.h"
#include "VariableMenager.h"
#include "ActionMenager.h"
#include "ActionMover.h"
#include "ActionSizer.h"
#include "ActionRotate.h"
#include "ActionVisual.h"
#include "ActionColor.h"
#include "ActionPlaySound.h"
#include "ActionChangeImage.h"
#include "ActionCurve.h"
#include "Image.h"
#include "Graphic.h"

#pragma comment(lib,"library.lib")
#pragma comment(lib,"winmm.lib")

//Library
#ifdef _UNICODE
        #ifdef _DEBUG
                #pragma comment(lib,"ZipArchive_STLud.lib")
                #pragma comment(lib,"FreeImageud.lib")
                #pragma comment(lib,"LibJPEGud.lib")
                #pragma comment(lib,"LibMNGud.lib")
                #pragma comment(lib,"LibPNGud.lib")
                #pragma comment(lib,"LibTIFFud.lib")
        #else
                #pragma comment(lib,"ZipArchive_STLu.lib")
                #pragma comment(lib,"FreeImageu.lib")
                #pragma comment(lib,"LibJPEGu.lib")
                #pragma comment(lib,"LibMNGu.lib")
                #pragma comment(lib,"LibPNGu.lib")
                #pragma comment(lib,"LibTIFFu.lib")
        #endif
#else
        #ifdef _DEBUG
                #pragma comment(lib,"ZipArchive_STLd.lib")
                #pragma comment(lib,"FreeImaged.lib")
                #pragma comment(lib,"LibGIFd.lib")
                #pragma comment(lib,"LibJPEGd.lib")
                #pragma comment(lib,"LibMNGd.lib")
                #pragma comment(lib,"LibPNGd.lib")
                #pragma comment(lib,"LibTIFFd.lib")
        #else
                #pragma comment(lib,"ZipArchive_STL.lib")
                #pragma comment(lib,"FreeImage.lib")
                #pragma comment(lib,"LibGIF.lib")
                #pragma comment(lib,"LibJPEG.lib")
                #pragma comment(lib,"LibMNG.lib")
                #pragma comment(lib,"LibPNG.lib")
                #pragma comment(lib,"LibTIFF.lib")
        #endif
#endif