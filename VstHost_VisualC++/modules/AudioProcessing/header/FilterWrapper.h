#ifndef FILTER_WRAPPER_H
#define FILTER_WRAPPER_H

#include "easylogging++.h"
#include "enums.h"
#include "filter.h"

class FilterWrapper
{
public:
    explicit FilterWrapper();
    int Init(size_t sampling_rate);
    int Process(std::vector<float> input, std::vector<float>& output);
    int SetEnableProcessing(bool enable);
    ~FilterWrapper();

public:
    const std::string module_name_ = "filter";

private:
    BWLowPass* bw_low_pass_filter_ = nullptr;
    bool enable_processing_        = false;
};

#endif //FILTER_WRAPPER_H
