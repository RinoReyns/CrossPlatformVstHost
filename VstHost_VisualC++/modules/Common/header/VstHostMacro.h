#ifndef VST_HOST_MACRO_H
#define VST_HOST_MACRO_H

#define RETURN_ERROR_IF_NULL(value) {if(!value) return VST_ERROR_STATUS::NULL_POINTER;}
#define RETURN_ERROR_IF_NOT_SUCCESS(value) {if(value != VST_ERROR_STATUS::SUCCESS) return value;}
#define RETURN_ERROR_IF_NOT_SUCCESS_OR_BYPASS(value) {if(value != VST_ERROR_STATUS::SUCCESS && value != VST_ERROR_STATUS::BYPASS) return value;}
#define RETURN_ERROR_IF_ENDPOINT_ERROR(value) {if(value != VST_ERROR_STATUS::SUCCESS) {CoUninitialize(); return value;}}
#define RETURN_IF_AUDIO_CAPTURE_FAILED(X) if(FAILED(hr = (X))){ return VST_ERROR_STATUS::AUDIO_CAPTURE_ERROR; }
#define RETURN_IF_AUDIO_RENDER_FAILED(X) if(FAILED(hr = (X))){ return VST_ERROR_STATUS::AUDIO_RENDER_ERROR; }
#define RETURN_IF_BYPASS(value) {if(value == VST_ERROR_STATUS::BYPASS) return value;}

#define CLOSE_HANDLE_IF(h) if(h != INVALID_HANDLE_VALUE){ CloseHandle(h); h = INVALID_HANDLE_VALUE; }
#define IF_ERROR_RETURN(b) if(b == FALSE){ return b; }

template <class T> inline void SAFE_RELEASE(T*& p)
{
    if (p != NULL)
    {
        p->Release();
        p = NULL;
    }
}

#endif // VST_HOST_MACRO_H
