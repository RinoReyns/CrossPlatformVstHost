#include "VstHostTool.h"
#include "easylogging++.h"

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

int VstHostTool::ParsArgs()
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
    }

    return status;
}

int VstHostTool::EndpointProcessingPipeline()
{

#ifdef _WIN32
    std::unique_ptr<AudioEndpointManager> endpoint_manager(new AudioEndpointManager(arg_parser_->GetPluginVerbosity()));
    endpoint_manager->RunAudioEndpointHandler();
#endif
    // 1. Get Data From Mic 
    // 2. Put In queue
    // 3. Processing
    // 4. Render
    return VST_ERROR_STATUS::NOT_IMPLEMENTED;
}

int VstHostTool::OfflineProcessingPipeline()
{
    std::unique_ptr<WaveProcessingPipeline> pipeline;
    pipeline.reset(new WaveProcessingPipeline(arg_parser_->GetPluginVerbosity()));
    
    int status = pipeline->Init(arg_parser_->GetProcessingConfig());
    RETURN_ERROR_IF_NOT_SUCCESS(status);
    
    if (arg_parser_->GetDumpPluginParams())
    {
        return pipeline->GetConfig();
    }
    
    return pipeline->Run(arg_parser_->GetInputWavePath(), arg_parser_->GetOutputWavePath());
}

int VstHostTool::Run()
{
    int status = this->ParsArgs();
    RETURN_ERROR_IF_NOT_SUCCESS(status);

    LOG(INFO) << "------------------------------ Audio Host Started ------------------------------";

#ifdef _WIN32
    if (arg_parser_->GetEnableAudioEndpoint())
    {
        return this->EndpointProcessingPipeline();
    }
#endif
    if (!arg_parser_->GetDumpToolConfigParam() || arg_parser_->GetDumpPluginParams())
    {
        status = this->OfflineProcessingPipeline();
    }
    LOG(INFO) << "Application finished processing with status: " << status;
    return status;
}
