#include "WaveReader.h"

WaveReader::~WaveReader()
{
    CLOSE_HANDLE_IF(wave_file_handler_);
}

VST_ERROR_STATUS WaveReader::Initialize(const WCHAR* wave_file_path)
{
    CLOSE_HANDLE_IF(wave_file_handler_);
    wave_file_handler_ = CreateFile(wave_file_path,                 // file to open
                                    GENERIC_READ,                   // open for reading
                                    FILE_SHARE_READ,                // share for reading
                                    NULL,                           // default security
                                    OPEN_EXISTING,                  // existing file only
                                    FILE_ATTRIBUTE_NORMAL ,         // normal file
                                    0);                             // no attr. template

    if (wave_file_handler_ == INVALID_HANDLE_VALUE)
    {
        LOG(ERROR) << "Handler in WaveReader not created.";
        return VST_ERROR_STATUS::OPEN_FILE_ERROR;
    }
    // TODO:
    // read header, check signal lenght and read only until the one of file

    return VST_ERROR_STATUS::SUCCESS;
}

VST_ERROR_STATUS WaveReader::LoadData(BYTE* pData, DWORD dwLength,  DWORD* flag)
{
    DWORD dwWritten;
 
    BOOL status = ReadFile(wave_file_handler_, (LPVOID)pData, dwLength, &dwWritten, NULL);
    if (!status)
    {
        *flag = AUDCLNT_BUFFERFLAGS_DATA_DISCONTINUITY;
        VST_ERROR_STATUS::READ_WRITE_ERROR;
    }

    if (status && dwWritten == 0)
    {
        *flag = AUDCLNT_BUFFERFLAGS_SILENT;
        return VST_ERROR_STATUS::END_OF_FILE;
    }  

    *flag = 0;
    return VST_ERROR_STATUS::SUCCESS;
}
