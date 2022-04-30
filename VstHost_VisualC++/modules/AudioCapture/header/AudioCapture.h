#ifndef AUDIO_CAPTURE_H
#define AUDIO_CAPTURE_H
#include <string>

#include "StdAfx.h"
#include "WaveWriter.h"
#include "enums.h"
#include "easylogging++.h"

class AudioCapture
{

public:
    explicit AudioCapture(uint8_t verbose);
    ~AudioCapture() = default;
    VST_ERROR_STATUS RecordAudioStream();

private:
    void ReadEndpointFormat(WAVEFORMATEX* pwfx, BOOL* extensible_format_flag);
    VST_ERROR_STATUS ListAudioCaptureEndpoints(IMMDeviceEnumerator* pEnumerator, IMMDevice** pCurrentDevice);
    HRESULT printDeviceInfo(IMMDevice* pDevice, int index, LPCWSTR outFormat, LPWSTR strDefaultDeviceID);
    std::wstring getDeviceProperty(IPropertyStore* pStore, const PROPERTYKEY key);
    
    uint8_t verbose_ = 0;

};

#endif // AUDIO_CAPTURE_H
