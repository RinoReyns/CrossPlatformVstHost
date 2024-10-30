#ifndef AUDIO_ENDPOINT_MANAGER_H
#define AUDIO_ENDPOINT_MANAGER_H
#include <string>

#include "EndpointUtils.h"
#include "enums.h"
#include "easylogging++.h"
#include "AudioCapture.h"
#include "AudioRender.h"
#include "RtAudio.h"

class AudioEndpointManager
{

public:
    explicit AudioEndpointManager(uint8_t verbose);
    ~AudioEndpointManager() = default;
    int RunRtAudioEndpointHandler();

private:
    int RunWasapiAudioCapture();
    int RunWasapiAudioRender();
    int CloseRtAudioStream();
    int GetDeviceIds();
    size_t GetDeviceIndex(bool isInput = false);
    static int AudioCallback(void* outputBuffer, void* inputBuffer, unsigned int nBufferFrames,
        double streamTime, RtAudioStreamStatus status, void* data);


private:
    std::unique_ptr<AudioCapture> audio_capture_;
    std::unique_ptr<AudioRender> audio_render_;
    std::unique_ptr<RtAudio> adac_;
    std::vector<unsigned int> device_ids_;
    size_t buffer_bytes_;
    uint8_t verbose_ = 0;
};

#endif // AUDIO_ENDPOINT_MANAGER_H
