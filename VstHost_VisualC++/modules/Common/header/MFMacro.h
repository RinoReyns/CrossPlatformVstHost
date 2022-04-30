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

template <class T> void SafeRelease(T** ppT)
{
	if (*ppT)
	{
		(*ppT)->Release();
		*ppT = NULL;
	}
}

#endif

#ifndef MF_SAFE_DELETE
#define MF_SAFE_DELETE
template<class T> inline void SAFE_DELETE(T*& p){

	if(p){
		delete p;
		p = NULL;
	}
}
#endif

#ifndef MF_SAFE_DELETE_ARRAY
#define MF_SAFE_DELETE_ARRAY
template<class T> inline void SAFE_DELETE_ARRAY(T*& p){

	if(p){
		delete[] p;
		p = NULL;
	}
}
#endif

#ifndef IF_FAILED_RETURN
#define IF_FAILED_RETURN(X) if(FAILED(hr = (X))){ return hr; }
#endif

#ifndef IF_FAILED_THROW
#define IF_FAILED_THROW(X) if(FAILED(hr = (X))){ throw hr; }
#endif

#ifndef IF_ERROR_RETURN
#define IF_ERROR_RETURN(b) if(b == FALSE){ return b; }
#endif

#ifndef CLOSE_HANDLE_IF
#define CLOSE_HANDLE_IF(h) if(h != INVALID_HANDLE_VALUE){ CloseHandle(h); h = INVALID_HANDLE_VALUE; }
#endif

#ifndef RETURN_STRING
#define RETURN_STRING(x) case x: return L#x
#endif

#endif
