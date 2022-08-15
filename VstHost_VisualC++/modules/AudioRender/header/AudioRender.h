#ifndef AUDIO_RENDER_H
#define AUDIO_RENDER_H
#include <string>

#include "EndpointUtils.h"
#include "enums.h"
#include "easylogging++.h"
#include "WaveReader.h"

class AudioRender
{

public:
    explicit AudioRender();
    ~AudioRender();

    VST_ERROR_STATUS Init();
    VST_ERROR_STATUS Release();
    VST_ERROR_STATUS RenderAudioStream();
    VST_ERROR_STATUS GetEndpointSamplingRate(uint32_t* sampling_rate);

private:
    WAVEFORMATEX* device_format_ = NULL;
    IMMDeviceEnumerator* pEnumerator = NULL;
    IMMDevice* pDevice = NULL;
    IAudioClient* pAudioClient = NULL;
    IAudioRenderClient* pRenderClient = NULL;
    std::unique_ptr<WaveReader> wave_reader_;
    UINT32 bufferFrameCount;

};

#endif // AUDIO_RENDER_H
