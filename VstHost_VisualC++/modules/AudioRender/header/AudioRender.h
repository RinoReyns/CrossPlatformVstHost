#ifndef AUDIO_RENDER_H
#define AUDIO_RENDER_H
#include <string>

#include "EndpointUtils.h"
#include "enums.h"
#include "easylogging++.h"

class AudioRender
{

public:
    explicit AudioRender();
    ~AudioRender() = default;
    VST_ERROR_STATUS RenderAudioStream();
};

#endif // AUDIO_RENDER_H
