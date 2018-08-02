#pragma once
#include <WTypes.h>
#include "../Global.h"

#define TEST_HR(hr)(::TestHR((hr),__FILE__,__LINE__))
HELPERS_LIB_API BOOL TestHR(HRESULT, LPCSTR, ULONG);