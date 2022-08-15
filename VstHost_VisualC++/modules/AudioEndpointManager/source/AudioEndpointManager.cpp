#include <chrono>
#include <thread>
#include <future>

#include "VstHostMacro.h"
#include "AudioEndpointManager.h"

#undef max

AudioEndpointManager::AudioEndpointManager(uint8_t verbose) :
    verbose_(verbose)
{
}

int AudioEndpointManager::RunAudioCapture()
{
    auto audio_capture_thread = std::async(std::launch::async,
        &AudioCapture::RecordAudioStream,
        audio_capture_.get());

    int input_value;
    while (!audio_capture_->GetRunRecordingLoop())
    {
        std::cout << "If you want to stop Audio Capture type '1' and press 'enter'" << std::endl;
        std::cin >> input_value;
        if (input_value == 1)
        {
            audio_capture_->SetRunRecordingLoop(TRUE);
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        std::cout << "Still sleeping..." << std::endl;
    }

    return audio_capture_thread.get();
}

int AudioEndpointManager::RunAudioRender()
{
    return audio_render_->RenderAudioStream();
}

int AudioEndpointManager::RunAudioEndpointHandler()
{

    // TODO:
    // 1) Choose Render Endpoint
    // 2) Create queue 
    // 3) Write to queue in RecordAudioStream
    // 4) Read from queue and write to file on other thread
    // 5) Render data from queue
    // 6) Put data from queue throught the plugin and render processed results.
    // 7) First init audio render - it will render zeros or noise, but there will be minimal latency.
    //    The target for latency should be 1ms to 5 ms. Latency should be adjustable.

    HRESULT stat = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
    if (!SUCCEEDED(stat))
    {
        return VST_ERROR_STATUS::AUDIO_ENDPOINT_MANAGER_ERROR;
    }

    audio_capture_.reset(new AudioCapture(verbose_));
    RETURN_ERROR_IF_ENDPOINT_ERROR(audio_capture_->Init());

    uint32_t capture_sampling_rate;
    RETURN_ERROR_IF_ENDPOINT_ERROR(audio_capture_->GetEndpointSamplingRate(&capture_sampling_rate));

    audio_render_.reset(new AudioRender());
    RETURN_ERROR_IF_ENDPOINT_ERROR(audio_render_->Init());

    uint32_t render_sampling_rate;
    RETURN_ERROR_IF_ENDPOINT_ERROR(audio_render_->GetEndpointSamplingRate(&render_sampling_rate));

    if (render_sampling_rate != capture_sampling_rate)
    {
        CoUninitialize();
        LOG(ERROR) << "Sampling rate of render and capture miss match.";
        return VST_ERROR_STATUS::ENPOINTS_SAMPLING_RATE_MISS_MATCH;
    }

    RETURN_ERROR_IF_ENDPOINT_ERROR(this->RunAudioCapture());

    int status = this->RunAudioRender();
    CoUninitialize();

    return status;
}

