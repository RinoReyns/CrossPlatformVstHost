#include "VstHostTool.h"
#include "audiohost.h"
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
    std::unique_ptr<AudioProcessingVstHost> vst_host = std::make_unique<AudioProcessingVstHost>();

#ifdef _WIN32
    if (arg_parser_->GetEnableAudioEndpoint())
    {
        std::unique_ptr<AudioEndpointManager> endpoint_manager(new AudioEndpointManager(arg_parser_->GetPluginVerbosity()));
        return endpoint_manager->RunAudioEndpointHandler();
    }
#endif

    vst_host->SetVerbosity(arg_parser_->GetPluginVerbosity());

    status = vst_host->CreateMutliplePluginInstance(arg_parser_->GetProcessingConfig());
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
                // 1. add option to apply fillter pre or post processing with plugin
                // 2. Pass buffert with data between modules.
                //status = vst_host->ApplyFilter(
                //    arg_parser_->GetInputWavePath(),
                //    arg_parser_->GetOutputWavePath());
                status = vst_host->ProcessWaveFile(
                    arg_parser_->GetInputWavePath(),
                    arg_parser_->GetOutputWavePath());
            }
        }
    }

    vst_host->Terminate();
    LOG(INFO) << "Application finished processing with status: " << status;
    return status;
}
