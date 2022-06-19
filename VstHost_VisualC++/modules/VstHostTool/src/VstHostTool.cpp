#include "VstHostTool.h"
#include "audiohost.h"
#include "easylogging++.h"

#include <chrono>
#include <thread>
#include <future>

#ifdef _WIN32
#include "AudioCapture.h"
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

    // TODO:
    // clean up

    HRESULT stat = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
    if (SUCCEEDED(stat))
    {
        std::unique_ptr<AudioCapture> audio_capture(new AudioCapture(arg_parser_->GetPluginVerbosity()));
        
        
        status = audio_capture->InitializeAudioStream();
        if (status != VST_ERROR_STATUS::SUCCESS)
        {
            CoUninitialize();
            return status;
        }
        
        auto audio_capture_thread = std::async(std::launch::async, 
                                               &AudioCapture::RecordAudioStream, 
                                               audio_capture.get());
        
        while (!audio_capture->run_recording_loop_)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
            std::cout << "Still sleeping..." << std::endl;
        }

        status = audio_capture_thread.get();

        CoUninitialize();
        return status;
    }    

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
                status = vst_host->ProcessWaveFile(arg_parser_->GetInputWavePath(),
                                                   arg_parser_->GetOutputWavePath());
            }
        }
    }

    vst_host->Terminate();
    LOG(INFO) << "Application finished processing with status: " << status;
    return status;
}
