#ifndef AUDIO_CAPTURE_H
#define AUDIO_CAPTURE_H
#include <string>

#include "EndpointUtils.h"
#include "WaveWriter.h"
#include "enums.h"
#include "easylogging++.h"
#include "AudioEndpointBase.h"

class AudioCapture : AudioEndpointBase
{

public:
    explicit AudioCapture(uint8_t verbose);
    ~AudioCapture();
    VST_ERROR_STATUS Init();
    VST_ERROR_STATUS RecordAudioStream();
    BOOL GetRunRecordingLoop();
    void SetRunRecordingLoop(BOOL status);
    VST_ERROR_STATUS Release();
    VST_ERROR_STATUS GetEndpointSamplingRate(uint32_t* sampling_rate);

private:
    std::atomic<BOOL> run_recording_loop_ = FALSE;
    uint8_t verbose_ = 0;
    IAudioClient* pAudioClient = NULL;
    WAVEFORMATEX* device_format_ = NULL;
    IAudioCaptureClient* pCaptureClient = NULL;
    DWORD recording_loop_sleep_time_ = 0;

    std::unique_ptr<CMFWaveWriter> wave_writer_;
};

#endif // AUDIO_CAPTURE_H
