#include "stdafx.h"

namespace Helpers
{
	CComVariant VariantSystemTimeToLocalTime(CComVariant SystemTime)
	{
		SystemTime.ChangeType(VT_DATE);
		SYSTEMTIME st;
		FILETIME ft;
		VariantTimeToSystemTime(SystemTime.date, &st);
		SystemTimeToFileTime(&st, &ft);
		FileTimeToLocalFileTime(&ft, &ft);
		FileTimeToSystemTime(&ft, &st);
		SystemTimeToVariantTime(&st, &SystemTime.date);
		return SystemTime;
	}
	CComVariant VariantLocalTimeToSystemTime(CComVariant LocalTime)
	{
		LocalTime.ChangeType(VT_DATE);
		SYSTEMTIME st;
		FILETIME ft;
		VariantTimeToSystemTime(LocalTime.date, &st);
		SystemTimeToFileTime(&st, &ft);
		LocalFileTimeToFileTime(&ft, &ft);
		FileTimeToSystemTime(&ft, &st);
		SystemTimeToVariantTime(&st, &LocalTime.date);
		return LocalTime;
	}
}