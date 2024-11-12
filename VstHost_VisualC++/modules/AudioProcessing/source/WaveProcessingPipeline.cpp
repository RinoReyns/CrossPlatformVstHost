#include "WaveProcessingPipeline.h"
#include "VstHostMacro.h"


WaveProcessingPipeline::WaveProcessingPipeline(uint8_t verbosity) :
    verbosity_(verbosity)
{}

int WaveProcessingPipeline::Init(nlohmann::json pipeline_config)
{
    pipeline_config_ = pipeline_config;
    // TODO:
    // move vst_host_config_ to vst_host
    vst_host_config_ = pipeline_config[VST_HOST_CONFIG_PARAM_STR][PROCESSING_CONFIG_PARAM_STR];
    int status = this->CreateVstHost();
    RETURN_ERROR_IF_NOT_SUCCESS_OR_BYPASS(status);

    status = this->CreatePreprocessingModule();
    RETURN_ERROR_IF_NOT_SUCCESS_OR_BYPASS(status);

    status = this->CreatePostprocessingModule();
    RETURN_ERROR_IF_NOT_SUCCESS_OR_BYPASS(status);

    return status;
}

int WaveProcessingPipeline::CreateVstHost()
{
    vst_host_.reset(new AudioProcessingVstHost());
    int status = vst_host_->SetEnableProcessing(pipeline_config_[VST_HOST_CONFIG_PARAM_STR][ENABLE_STRING]);
    RETURN_IF_BYPASS(status);

    vst_host_->SetVerbosity(this->verbosity_);
    return vst_host_->CreateMutliplePluginInstance(vst_host_config_);
}

int WaveProcessingPipeline::ProcessingVstHost()
{
    // VST HOST Processing
    int status = vst_host_->SetMutliplePluginParameters(vst_host_config_);
    RETURN_ERROR_IF_NOT_SUCCESS_OR_BYPASS(status);
    status = vst_host_->BufferProcessing(input_wave_.get(), output_wave_.get());
    RETURN_ERROR_IF_NOT_SUCCESS_OR_BYPASS(status);
    
    return status;
}

int WaveProcessingPipeline::CreatePreprocessingModule()
{
    return VST_ERROR_STATUS::SUCCESS;
}

int WaveProcessingPipeline::CreatePostprocessingModule()
{
    return VST_ERROR_STATUS::SUCCESS;
}

int WaveProcessingPipeline::GetConfig()
{
    return vst_host_->GetMutliplePluginParameters(vst_host_config_);
}

int WaveProcessingPipeline::SwapInOutBuffers()
{

    return VST_ERROR_STATUS::SUCCESS;
}

int WaveProcessingPipeline::Run(std::string input_path, std::string output_path)
{
    if (input_path == "" || output_path == "")
    {
        return VST_ERROR_STATUS::PATH_NOT_EXISTS;
    }

    std::unique_ptr<WaveIOClass> wave_io(new WaveIOClass());
    input_wave_.reset(new WaveDataContainer(input_path));
    output_wave_.reset(new WaveDataContainer(output_path));

    //Load wave file
    int status = wave_io->LoadWave(input_wave_.get());
    RETURN_ERROR_IF_NOT_SUCCESS(status);

    // TODO:
    // add preprocessing
    
    // VST Host Processing
    RETURN_ERROR_IF_NOT_SUCCESS_OR_BYPASS(ProcessingVstHost());

    // TODO:
    // Add option to run only model with OV
    // Add option to enable/disable filtration and OV processing

    // TODO:
    // add postprocessing

    // Save wave file
    status = wave_io->SaveWave(output_wave_.get());
    RETURN_ERROR_IF_NOT_SUCCESS(status);

    vst_host_->Terminate();
    return status;
}
