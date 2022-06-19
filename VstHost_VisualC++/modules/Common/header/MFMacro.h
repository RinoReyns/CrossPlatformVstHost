//----------------------------------------------------------------------------------------------
// MFMacro.h
//----------------------------------------------------------------------------------------------
#ifndef MFMACRO_H
#define MFMACRO_H

#ifndef MF_SAFE_RELEASE
#define MF_SAFE_RELEASE
template <class T> inline void SAFE_RELEASE(T*& p){

	if(p){
		p->Release();
		p = NULL;
	}
}

#endif

#ifndef IF_ERROR_RETURN
#define IF_ERROR_RETURN(b) if(b == FALSE){ return b; }
#endif

#ifndef CLOSE_HANDLE_IF
#define CLOSE_HANDLE_IF(h) if(h != INVALID_HANDLE_VALUE){ CloseHandle(h); h = INVALID_HANDLE_VALUE; }
#endif

#endif
