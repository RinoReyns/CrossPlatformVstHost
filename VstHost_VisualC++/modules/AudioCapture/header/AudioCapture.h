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
    VST_ERROR_STATUS InitializeAudioStream();
    VST_ERROR_STATUS RecordAudioStream();
    BOOL GetRunRecordingLoop();
    void SetRunRecordingLoop(BOOL status);

private:
    void ReadEndpointFormat(BOOL* extensible_format_flag);
    VST_ERROR_STATUS ListAudioCaptureEndpoints();
    HRESULT printDeviceInfo(IMMDevice* pDevice, int index, LPCWSTR outFormat, LPWSTR strDefaultDeviceID);
    std::wstring getDeviceProperty(IPropertyStore* pStore, const PROPERTYKEY key);

private:
    std::atomic<BOOL> run_recording_loop_   = FALSE;
    uint8_t verbose_                        = 0;
    IMMDevice * pDevice                     = NULL;
    IMMDeviceEnumerator* pEnumerator        = NULL;
    IAudioClient* pAudioClient              = NULL;
    WAVEFORMATEX* pwfx                      = NULL;
    IAudioCaptureClient* pCaptureClient     = NULL;
};

#endif // AUDIO_CAPTURE_H
