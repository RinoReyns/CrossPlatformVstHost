#ifndef VST_HOST_MACRO_H
#define VST_HOST_MACRO_H

#define RETURN_ERROR_IF_NULL(value) {if(!value) return VST_ERROR_STATUS::NULL_POINTER;}
#define RETURN_ERROR_IF_NOT_SUCCESS(value) {if(value != VST_ERROR_STATUS::SUCCESS) return value;}
#define RETURN_IF_AUDIO_CAPTURE_FAILED(X) if(FAILED(hr = (X))){ return VST_ERROR_STATUS::AUDIO_CAPTURE_ERROR; }
#define EXIT_ON_ERROR(hres) if (FAILED(hres)) { goto Exit; }

#endif // VST_HOST_MACRO_H
