#ifndef WAVE_PROCESSING_PIPELINE_H
#define WAVE_PROCESSING_PIPELINE_H

#include "easylogging++.h"
#include "enums.h"
#include "FilterWrapper.h"
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
    int CreateVstHost();
    int ProcessingVstHost();
    int CreatePreprocessingModules();
    int PreprocessingProcessing();
    int CreatePostprocessingModules();
    int PostprocessingProcessing();
    int SwapInOutBuffers();

private:
    uint8_t verbosity_ = 0;
    config_type vst_host_config_{};
    nlohmann::json pipeline_config_{};
    std::unique_ptr<AudioProcessingVstHost> vst_host_;
    std::unique_ptr<WaveDataContainer> input_wave_;
    std::unique_ptr<WaveDataContainer> output_wave_;
    std::unique_ptr<FilterWrapper> preprocessing_filter_wrapper_;
    std::unique_ptr<FilterWrapper> postprocessing_filter_wrapper_;
    size_t processing_sampling_rate_ = 0;
};

#endif //WAVE_PROCESSING_PIPELINE_H
