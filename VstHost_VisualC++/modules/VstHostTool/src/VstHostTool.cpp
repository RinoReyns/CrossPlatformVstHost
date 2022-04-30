#include "VstHostTool.h"
#include "audiohost.h"
#include "easylogging++.h"
#include "AudioCapture.h"

INITIALIZE_EASYLOGGINGPP

// TODO:
// 1) allow to use multiple plugins and remember about configs for plugins
// 2) Clean up code for vst host
// 3) Read audio from audio endpoint and use it in code
// 4) Add queue to read from capture and process with vst
// 5) Add support for linux (starting point for andorid), mac os, arm, android
// 6) Clean up UT for python
// 7) Add readMe.md - add info about every component that was incorporated
// 8) Integrate better wave reader https://github.com/adamstark/AudioFile

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
        return VST_ERROR_STATUS::PARS_ARGS_ERROR;
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
        return VST_ERROR_STATUS::PARS_ARGS_ERROR;
    }
    
    int status = arg_parser_->ParsParameters(parser_arguments_);

    if (status != VST_ERROR_STATUS::SUCCESS)
    {
        LOG(ERROR) << "Arg Parser failed with status: " << status;
        return status;
    }

    LOG(INFO) << "------------------------------ Audio Host Started ------------------------------";
    std::unique_ptr<AudioProcessingVstHost> vst_host = std::make_unique<AudioProcessingVstHost>();
    VST3::Optional<VST3::UID> uid;

    // TODO:
    // clean up
   /* HRESULT stat = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
    if (SUCCEEDED(stat))
    {
        std::unique_ptr<AudioCapture> audio_capture(new AudioCapture(arg_parser->GetPluginVerbosity()));
        audio_capture->RecordAudioStream();
        CoUninitialize();
        return 0;
    }*/

    vst_host->SetVerbosity(arg_parser_->GetPluginVerbosity());
    status = vst_host->CreatePluginInstance(arg_parser_->GetPluginPath(), std::move(uid));

    if (status == VST_ERROR_STATUS::SUCCESS)
    {
        if (arg_parser_->GetDumpPluginParams())
        {
            status = vst_host->GetPluginParameters(arg_parser_->GetPluginConfig());
        }
        else
        {
            status = vst_host->SetPluginParameters(arg_parser_->GetPluginConfig());
            if (status == VST_ERROR_STATUS::SUCCESS)
            {
                status = vst_host->ProcessWaveFileWithSinglePlugin(arg_parser_->GetInputWavePath(),
                                                                   arg_parser_->GetOutputWavePath());
            }
        }
    }

    vst_host->Terminate();
    LOG(INFO) << "Application finished processing with status: " << status;
    return status;
}
