//----------------------------------------------------------------------------------------------
// StdAfx.h
//----------------------------------------------------------------------------------------------
#ifndef STDAFX_H
#define STDAFX_H

#define WIN32_LEAN_AND_MEAN

#include <new>
#include <tchar.h>
#include <assert.h>
#include <initguid.h>
#include <mfidl.h>
#include <mfreadwrite.h>
#include <mferror.h>
#include <evr.h>
#include <uuids.h>
#include <d3d9.h>
#include <Evr9.h>
#include <dxva.h>
#pragma warning(push)
#pragma warning(disable:4201)
#include <dxvahd.h>
#pragma warning(pop)
#include <audioclient.h>
#include <Mmdeviceapi.h>

//----------------------------------------------------------------------------------------------
// Common Project Files

#include "MFMacro.h"
#define TRACE(x)

inline HRESULT _LOG_HRESULT(HRESULT hr, const char* sFileName, long lLineNo) 
{
	if (FAILED(hr)) 
	{
		TRACE((L"\n%S - Line: %d hr = %s\n", sFileName, lLineNo, MFErrorString(hr)));
	}
	else
	{
		_CRT_UNUSED(lLineNo);
		_CRT_UNUSED(sFileName);
	}
	return hr;
}

#define LOG_HRESULT(hr)      _LOG_HRESULT(hr, __FILE__, __LINE__)
#define LOG_LAST_ERROR()     _LOG_HRESULT(HRESULT_FROM_WIN32(GetLastError()), __FILE__, __LINE__)

#endif
