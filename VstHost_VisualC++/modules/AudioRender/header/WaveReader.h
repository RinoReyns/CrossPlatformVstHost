#ifndef WAVER_READER_H
#define WAVER_READER_H

#include "enums.h"
#include "EndpointUtils.h"
#include "easylogging++.h"

class WaveReader
{
    public:
        explicit WaveReader() = default;
        ~WaveReader();
        VST_ERROR_STATUS Initialize();
        VST_ERROR_STATUS LoadData(BYTE* pData, DWORD dwLength, DWORD* flags);

    private:
        HANDLE wave_file_handler_ = INVALID_HANDLE_VALUE;
};
#endif // !WAVER_READER_H
