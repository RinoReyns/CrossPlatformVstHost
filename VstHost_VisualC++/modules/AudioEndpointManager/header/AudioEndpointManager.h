#ifndef AUDIO_ENDPOINT_MANAGER_H
#define AUDIO_ENDPOINT_MANAGER_H
#include <string>

#include "EndpointUtils.h"
#include "enums.h"
#include "easylogging++.h"
#include "AudioCapture.h"
#include "AudioRender.h"


class AudioEndpointManager
{

public:
    explicit AudioEndpointManager(uint8_t verbose);
    ~AudioEndpointManager() = default;
    int RunAudioEndpointHandler();

private:
    int RunAudioCapture();
    int RunAudioRender();


private:
    std::unique_ptr<AudioCapture> audio_capture_;
    std::unique_ptr<AudioRender> audio_render_;
    uint8_t verbose_ = 0;
};

#endif // AUDIO_ENDPOINT_MANAGER_H
