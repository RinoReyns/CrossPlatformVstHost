#ifndef AUDIO_PROCESSING_WRAPPER_H
#define AUDIO_PROCESSING_WRAPPER_H

#include "easylogging++.h"
#include "enums.h"
#include "filter.h"

class FilterWrapper
{
public:
    explicit FilterWrapper();
    int Init(float sampling_rate);
    int ApplyBwLowPassFilter(std::vector<float> input, std::vector<float>& output);
    ~FilterWrapper();

private:
    BWLowPass* bw_low_pass_filter_;
};

#endif //AUDIO_PROCESSING_WRAPPER_H
