#include "VstHostTool.h"
#include "audiohost.h"
#include "easylogging++.h"
#include "WaveIOClass.h"

#ifdef _WIN32
#include "AudioEndpointManager.h"
#endif

INITIALIZE_EASYLOGGINGPP

// TODO:
// Integrate better wave reader - check https://github.com/adamstark/AudioFile

VstHostTool::VstHostTool()
{
    arg_parser_.reset(new ArgParser());
}

int VstHostTool::PrepareArgs(int argc, char* argv[])
{
    try
    {
        std::copy(argv, argv + argc, std::back_inserter(parser_arguments_));
        return VST_ERROR_STATUS::SUCCESS;
    }
    catch (const std::exception&)
    {
        return VST_ERROR_STATUS::ARG_PARSER_ERROR;
    }
}

int VstHostTool::PrepareArgs(std::vector<std::string> args)
{
    parser_arguments_.assign(args.begin(), args.end());
    return VST_ERROR_STATUS::SUCCESS;
}

int VstHostTool::EndpointProcessingPipeline()
{
    // 1. Get Data From Mic 
    // 2. Put In queue
    // 3. Processing
    // 4. Render
    return 0;
}

int VstHostTool::OfflineProcessingPipeline()
{
    // 1. Read WaveFile -> this changes will affect UT on android, but it is necessery to move on with project.
    //      in the worst case scenario anroid will also include WaveIo dll and it will have it's owne AuioLib
    // 2. Process It (Pre processig, VST Processing, Post processing)
    // 3. Save wave file
    std::unique_ptr<AudioProcessingVstHost> vst_host = std::make_unique<AudioProcessingVstHost>();
    vst_host->SetVerbosity(arg_parser_->GetPluginVerbosity());
    
    int status = vst_host->CreateMutliplePluginInstance(arg_parser_->GetProcessingConfig());
    
    if (status == VST_ERROR_STATUS::SUCCESS)
    {
        if (arg_parser_->GetDumpPluginParams())
        {
            status = vst_host->GetMutliplePluginParameters(arg_parser_->GetProcessingConfig());
        }
        else
        {
            status = vst_host->SetMutliplePluginParameters(arg_parser_->GetProcessingConfig());
            if (status != VST_ERROR_STATUS::VST_HOST_ERROR)
            {
                // TODO:
                // 1. add option to apply any audio pre or post processing with or without plugin
                // 2. Pass buffer with data between modules.
                std::unique_ptr<WaveIOClass> wave_io(new WaveIOClass());
                
                WaveDataContainer input_wave;
                input_wave.file_path = arg_parser_->GetInputWavePath();
                status = wave_io->LoadWave(&input_wave);
                RETURN_ERROR_IF_NOT_SUCCESS(status);

                WaveDataContainer output_wave;
                output_wave.file_path = arg_parser_->GetOutputWavePath();
               
                status = vst_host->BufferProcessing(&input_wave, &output_wave);
                RETURN_ERROR_IF_NOT_SUCCESS(status);
            }
        }
    }
    vst_host->Terminate();
    return status;
}

int VstHostTool::Run()
{
    if (parser_arguments_.size() == 0)
    {
        LOG(ERROR) << "Empty args.";
        return VST_ERROR_STATUS::ARG_PARSER_ERROR;
    }

    int status = arg_parser_->ParsParameters(parser_arguments_);

    if (status != VST_ERROR_STATUS::SUCCESS)
    {
        LOG(ERROR) << "Arg Parser failed with status: " << status;
        return status;
    }

    LOG(INFO) << "------------------------------ Audio Host Started ------------------------------";

#ifdef _WIN32
    if (arg_parser_->GetEnableAudioEndpoint())
    {
        std::unique_ptr<AudioEndpointManager> endpoint_manager(new AudioEndpointManager(arg_parser_->GetPluginVerbosity()));
        return endpoint_manager->RunAudioEndpointHandler();
    }
#endif

    status = this->OfflineProcessingPipeline();
    LOG(INFO) << "Application finished processing with status: " << status;
    return status;
}
