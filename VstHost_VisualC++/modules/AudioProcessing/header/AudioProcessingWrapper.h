#ifndef AUDIO_PROCESSING_WRAPPER_H
#define AUDIO_PROCESSING_WRAPPER_H

#include "easylogging++.h"
#include "enums.h"
#include "filter.h"

class AudioProcessingWrapper
{
public:
    explicit AudioProcessingWrapper();
    int Init(float sampling_rate);
    int ApplyBwLowPassFilter(std::vector<float> input, std::vector<float>& output);
    ~AudioProcessingWrapper();

private:
    BWLowPass* bw_low_pass_filter_;
};

#endif //AUDIO_PROCESSING_WRAPPER_H
