#ifndef WAVE_PROCESSING_PIPELINE_H
#define WAVE_PROCESSING_PIPELINE_H

#include "easylogging++.h"
#include "enums.h"
#include "filter.h"
#include "audiohost.h"
#include "JsonUtils.h"

class WaveProcessingPipeline
{
public:
    explicit WaveProcessingPipeline(uint8_t verbosity);

    ~WaveProcessingPipeline() = default;
    int Init(nlohmann::json vst_host_config);
    int GetConfig();
    int Run(std::string input_path, std::string output_path);

private:
    BWLowPass* bw_low_pass_filter_ = nullptr;
    uint8_t verbosity_ = 0;
    config_type vst_host_config_{};
    nlohmann::json pipeline_config_{};
    std::unique_ptr<AudioProcessingVstHost> vst_host_;
};

#endif //WAVE_PROCESSING_PIPELINE_H
