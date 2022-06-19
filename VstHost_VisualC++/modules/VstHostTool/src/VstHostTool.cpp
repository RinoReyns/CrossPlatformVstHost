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

int VstHostTool::RunAudioCapture()
{
    int status = VST_ERROR_STATUS::AUDIO_CAPTURE_ERROR;

#ifdef _WIN32
    // TODO:
    // 1) Create queue 
    // 2) Write to queue in RecordAudioStream
    // 3) Read from queue and write to file on other thread
    // 4) Render data from queue
    // 5) Put data from queue throught the plugin and render processed results.

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

        int input_value;
        while (!audio_capture->GetRunRecordingLoop())
        {
            std::cout << "If you want to stop Audio Capture type '1' and press 'enter'" << std::endl;
            std::cin >> input_value;
            if (input_value == 1)
            {
                audio_capture->SetRunRecordingLoop(TRUE);
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
            std::cout << "Still sleeping..." << std::endl;
        }

        status = audio_capture_thread.get();

        CoUninitialize();
        return status;
    }
#endif //_WIN32

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
    std::unique_ptr<AudioProcessingVstHost> vst_host = std::make_unique<AudioProcessingVstHost>();

    if (arg_parser_->GetEnableAudioEndpoint())
    {
        return this->RunAudioCapture();
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
