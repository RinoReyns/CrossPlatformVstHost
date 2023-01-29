#include "WaveProcessingPipeline.h"
#include "VstHostMacro.h"


WaveProcessingPipeline::WaveProcessingPipeline(uint8_t verbosity):
    verbosity_(verbosity)
{

}

int WaveProcessingPipeline::Init(config_type vst_host_config)
{
    vst_host_.reset(new AudioProcessingVstHost());
    vst_host_->SetVerbosity(this->verbosity_);
    int status = vst_host_->CreateMutliplePluginInstance(vst_host_config);
    RETURN_ERROR_IF_NOT_SUCCESS(status);
    vst_host_config_ = vst_host_config;
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

        // Save wave file
        status = wave_io->SaveWave(&output_wave);
        RETURN_ERROR_IF_NOT_SUCCESS(status);
    }
    
    vst_host_->Terminate();
    return status;

}
