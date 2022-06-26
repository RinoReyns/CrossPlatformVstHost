#include "WaveReader.h"
#define AUDIO_RENDER_FILE		L"Capture.wav"

WaveReader::~WaveReader()
{
    CLOSE_HANDLE_IF(wave_file_handler_);
}

VST_ERROR_STATUS WaveReader::Initialize()
{
    CLOSE_HANDLE_IF(wave_file_handler_);
    wave_file_handler_ = CreateFile(AUDIO_RENDER_FILE,     // file to open
                                    GENERIC_READ,          // open for reading
                                    FILE_SHARE_READ,       // share for reading
                                    NULL,                  // default security
                                    OPEN_EXISTING,         // existing file only
                                    FILE_ATTRIBUTE_NORMAL , // normal file
                                    0);                 // no attr. template

    if (wave_file_handler_ == INVALID_HANDLE_VALUE)
    {
        LOG(ERROR) << "Handler in WaveReader not created.";
        return VST_ERROR_STATUS::OPEN_FILE_ERROR;
    }
    // TODO:
    // read header, check signal lenght and read only until the one of file

    return VST_ERROR_STATUS::SUCCESS;
}

VST_ERROR_STATUS WaveReader::LoadData(BYTE* pData, DWORD dwLength,  DWORD* flags)
{
    // On the begining there will be a glich due to the header in wave file
    DWORD dwWritten;
 
    BOOL status = ReadFile(wave_file_handler_, (LPVOID)pData, dwLength, &dwWritten, NULL);
    if (!status)
    {
        VST_ERROR_STATUS::READ_WRITE_ERROR;
    }

    if (status && dwWritten == 0)
    {
        *flags = AUDCLNT_BUFFERFLAGS_SILENT;
        return VST_ERROR_STATUS::END_OF_FILE;
    }  

    *flags = 0;
    return VST_ERROR_STATUS::SUCCESS;
}
