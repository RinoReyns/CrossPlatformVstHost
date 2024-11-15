#include "WaveProcessingPipeline.h"
#include "VstHostMacro.h"

WaveProcessingPipeline::WaveProcessingPipeline(uint8_t verbosity) :
    verbosity_(verbosity)
{}

int WaveProcessingPipeline::Init(nlohmann::json pipeline_config)
{
    pipeline_config_ = pipeline_config;
    processing_sampling_rate_ = static_cast<size_t>(pipeline_config_[SAMPLING_RATE_PARAM_STR]);

    int status = this->CreateVstHost();
    RETURN_ERROR_IF_NOT_SUCCESS_OR_BYPASS(status);

    status = this->CreatePreprocessingModules();
    RETURN_ERROR_IF_NOT_SUCCESS_OR_BYPASS(status);

    status = this->CreatePostprocessingModules();
    RETURN_ERROR_IF_NOT_SUCCESS_OR_BYPASS(status);

    return VST_ERROR_STATUS::SUCCESS;
}

int WaveProcessingPipeline::CreateVstHost()
{
    vst_host_.reset(new AudioProcessingVstHost());
    // TODO:
    // move vst_host_config_ to vst_host
    vst_host_config_ = pipeline_config_[vst_host_->module_name_][PROCESSING_CONFIG_PARAM_STR];
    int status = vst_host_->SetEnableProcessing(pipeline_config_[vst_host_->module_name_][ENABLE_STRING]);
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

int WaveProcessingPipeline::CreatePreprocessingModules()
{
    preprocessing_filter_wrapper_.reset(new FilterWrapper());
    int status = preprocessing_filter_wrapper_->SetEnableProcessing(pipeline_config_[PREPROCESSING][preprocessing_filter_wrapper_->module_name_][ENABLE_STRING]);
    RETURN_IF_BYPASS(status);
    return preprocessing_filter_wrapper_->Init(processing_sampling_rate_);
}

int WaveProcessingPipeline::PreprocessingProcessing()
{
    RETURN_ERROR_IF_NOT_SUCCESS_OR_BYPASS(preprocessing_filter_wrapper_->Process(input_wave_->data, output_wave_->data));
    return SwapInOutBuffers();
}

int WaveProcessingPipeline::CreatePostprocessingModules()
{
    postprocessing_filter_wrapper_.reset(new FilterWrapper());
    int status = postprocessing_filter_wrapper_->SetEnableProcessing(pipeline_config_[POSTPROCESSING][postprocessing_filter_wrapper_->module_name_][ENABLE_STRING]);
    RETURN_IF_BYPASS(status);
    return postprocessing_filter_wrapper_->Init(processing_sampling_rate_);
}

int WaveProcessingPipeline::PostprocessingProcessing()
{
    return postprocessing_filter_wrapper_->Process(input_wave_->data, output_wave_->data);
}

int WaveProcessingPipeline::GetConfig()
{
    return vst_host_->GetMutliplePluginParameters(vst_host_config_);
}

int WaveProcessingPipeline::SwapInOutBuffers()
{
    input_wave_->data = output_wave_->data;
    input_wave_->frame_number = output_wave_->frame_number;
    input_wave_->channel_number = output_wave_->channel_number;
    input_wave_->bits_per_sample = output_wave_->bits_per_sample;

    return VST_ERROR_STATUS::SUCCESS;
}

int WaveProcessingPipeline::Run(std::string input_path, std::string output_path)
{
    if (input_path == "" || output_path == "")
    {
        return VST_ERROR_STATUS::PATH_NOT_EXISTS;
    }

    std::unique_ptr<WaveIOClass> wave_io(new WaveIOClass());
    input_wave_.reset(new WaveDataContainer(input_path, processing_sampling_rate_));
    output_wave_.reset(new WaveDataContainer(output_path, processing_sampling_rate_));

    //Load wave file
    // TODO:
    // add resampling in wave loader for setting proper sampling rate for all modules
    int status = wave_io->LoadWave(input_wave_.get());
    RETURN_ERROR_IF_NOT_SUCCESS(status);

    output_wave_->SetParams(input_wave_.get());

    // Preprocessing
    RETURN_ERROR_IF_NOT_SUCCESS_OR_BYPASS(PreprocessingProcessing());
    
    // VST Host Processing
    RETURN_ERROR_IF_NOT_SUCCESS_OR_BYPASS(ProcessingVstHost());

    // Preprocessing
    RETURN_ERROR_IF_NOT_SUCCESS_OR_BYPASS(PostprocessingProcessing());

    // TODO:
    // Add option to run only model with OV

    // Save wave file
    status = wave_io->SaveWave(output_wave_.get());
    RETURN_ERROR_IF_NOT_SUCCESS(status);

    vst_host_->Terminate();
    return status;
}
