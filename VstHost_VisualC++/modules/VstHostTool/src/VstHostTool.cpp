#include "VstHostTool.h"
#include "audiohost.h"
#include "easylogging++.h"

#include <chrono>
#include <thread>
#include <future>

#ifdef _WIN32
#include "AudioCapture.h"
#include "AudioRender.h"
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
    std::unique_ptr<AudioCapture> audio_capture(new AudioCapture(arg_parser_->GetPluginVerbosity()));
    RETURN_ERROR_IF_NOT_SUCCESS(audio_capture->InitializeAudioStream())

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

    return audio_capture_thread.get();
}

int VstHostTool::RunAudioRender()
{
    std::unique_ptr<AudioRender> audio_render(new AudioRender());
    return audio_render->RenderAudioStream();
}

int VstHostTool::RunAudioEndpointHandler()
{
    int status = VST_ERROR_STATUS::AUDIO_CAPTURE_ERROR;

#ifdef _WIN32
    // TODO:
    // 1) Create queue 
    // 2) Write to queue in RecordAudioStream
    // 3) Read from queue and write to file on other thread
    // 4) Render data from queue
    // 5) Put data from queue throught the plugin and render processed results.
    // 6) First init audio render - it will render zeros or noise, but there will be minimal latency.
    //    The target for latency should be 1ms to 5 ms. Latency should be adjustable.

    HRESULT stat = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
    if (SUCCEEDED(stat))
    {
        status = this->RunAudioCapture();
        if (status == VST_ERROR_STATUS::SUCCESS)
        {
            status = this->RunAudioRender();
        }   
        CoUninitialize();
    }

#endif
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
        return RunAudioEndpointHandler();
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
