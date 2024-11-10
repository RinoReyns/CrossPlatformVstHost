#include "WaveProcessingPipeline.h"
#include "VstHostMacro.h"


WaveProcessingPipeline::WaveProcessingPipeline(uint8_t verbosity) :
    verbosity_(verbosity)
{}

int WaveProcessingPipeline::Init(nlohmann::json pipeline_config)
{
    vst_host_.reset(new AudioProcessingVstHost());
    vst_host_->SetVerbosity(this->verbosity_);
    pipeline_config_ = pipeline_config;
    vst_host_config_ = pipeline_config[VST_HOST_CONFIG_PARAM_STR][PROCESSING_CONFIG_PARAM_STR];
    int status = vst_host_->CreateMutliplePluginInstance(vst_host_config_);
    RETURN_ERROR_IF_NOT_SUCCESS(status);
  
    return status;
}

int WaveProcessingPipeline::GetConfig()
{
    return vst_host_->GetMutliplePluginParameters(vst_host_config_);
}

int WaveProcessingPipeline::Run(std::string input_path, std::string output_path)
{
    if (input_path == "" || output_path == "")
    {
        return VST_ERROR_STATUS::PATH_NOT_EXISTS;
    }

    int status = vst_host_->SetMutliplePluginParameters(vst_host_config_);
    if (status != VST_ERROR_STATUS::VST_HOST_ERROR)
    {
        std::unique_ptr<WaveIOClass> wave_io(new WaveIOClass());

        // Load wave file
        WaveDataContainer input_wave;
        input_wave.file_path = input_path;
        status = wave_io->LoadWave(&input_wave);
        RETURN_ERROR_IF_NOT_SUCCESS(status);

        WaveDataContainer output_wave;
        output_wave.file_path = output_path;

        status = vst_host_->BufferProcessing(&input_wave, &output_wave);
        RETURN_ERROR_IF_NOT_SUCCESS(status);
        // TODO:
        // Add option to run only model with OV
        // Add option to enable/disable filtration and OV processing

        // Save wave file
        status = wave_io->SaveWave(&output_wave);
        RETURN_ERROR_IF_NOT_SUCCESS(status);
    }

    vst_host_->Terminate();
    return status;
}
