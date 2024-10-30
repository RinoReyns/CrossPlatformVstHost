#include <chrono>
#include <thread>
#include <future>
#include <iostream>
#include <cstdlib>
#include <cstring>

#include "VstHostMacro.h"
#include "AudioEndpointManager.h"

typedef double MY_TYPE;
#define FORMAT RTAUDIO_FLOAT64
#undef max

AudioEndpointManager::AudioEndpointManager(uint8_t verbose) :
    verbose_(verbose)
{
}

int AudioEndpointManager::RunWasapiAudioCapture()
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

int AudioEndpointManager::RunWasapiAudioRender()
{
    return audio_render_->RenderAudioStream();
}

size_t AudioEndpointManager::GetDeviceIndex(bool isInput = false)
{
    unsigned int device_id;
    std::string keyHit;
    std::cout << '\n';
    std::vector<std::string> device_names = adac_->getDeviceNames();
   
    for (device_id = 0; device_id < device_names.size(); device_id++)
    {
        std::cout << "  Device #" << device_id << ": " << device_names[device_id] << '\n';
    }
        
    do {
        if (isInput)
        {
            LOG(INFO) << "\nChoose an input device #: ";
        }   
        else
        {
            LOG(INFO) << "\nChoose an output device #: ";
        }   
        std::cin >> device_id;
    } while (device_id >= device_names.size());
    std::getline(std::cin, keyHit);  // used to clear out stdin
    return device_ids_[device_id];
}


int AudioEndpointManager::AudioCallback(void* outputBuffer, void* inputBuffer, unsigned int /*nBufferFrames*/,
    double /*streamTime*/, RtAudioStreamStatus status, void* data)
{
    AudioEndpointManager* _this = static_cast<AudioEndpointManager*>(data);

    if (status)
    {
        LOG(WARNING) << "Stream over/underflow detected." << std::endl;
    }

    // here is the place for processing
    memcpy(outputBuffer, inputBuffer, _this->buffer_bytes_);
    return VST_ERROR_STATUS::SUCCESS;
}

int AudioEndpointManager::CloseRtAudioStream()
{
    if (adac_->isStreamOpen())
    {
        adac_->closeStream();
    }  
    return VST_ERROR_STATUS::SUCCESS;
}

int AudioEndpointManager::GetDeviceIds()
{
    device_ids_ = adac_->getDeviceIds();
    if (device_ids_.size() < 1)
    {
        return VST_ERROR_STATUS::NO_AUDIO_DEVICES_ID_FOUND;
    }
    return VST_ERROR_STATUS::SUCCESS;
}

/// @brief Rt Audio based endpoint handler. 
/// @return VST_ERROR_STATUS status
int AudioEndpointManager::RunRtAudioEndpointHandler()
{
    adac_.reset(new RtAudio());
    RETURN_ERROR_IF_NOT_SUCCESS(this->GetDeviceIds());

    // Set the same number of channels for both input and output.
    unsigned int bufferFrames = 1024;
    RtAudio::StreamParameters iParams, oParams;
    uint16_t channels = 2;
    iParams.nChannels = channels;
    oParams.nChannels = channels;
    unsigned int fs = 48000;
  
    iParams.deviceId = this->GetDeviceIndex(true);
    oParams.deviceId = this->GetDeviceIndex();
    
    RtAudio::StreamOptions options;
    //options.flags |= RTAUDIO_NONINTERLEAVED;

    buffer_bytes_ = bufferFrames * channels * sizeof(MY_TYPE);

    if (adac_->openStream(&oParams, &iParams, FORMAT, fs, &bufferFrames, &AudioCallback, this, &options))
    {
        this->CloseRtAudioStream();
    }

    LOG(INFO) << "Stream latency = " << adac_->getStreamLatency() << " frames";
    
    if (adac_->startStream())
    {
        this->CloseRtAudioStream();
    }

    LOG(INFO) << "Running ... press <enter> to quit (buffer frames = " << bufferFrames << ").";
    char input;
    std::cin.get(input);

    // Stop the stream.
    if (adac_->isStreamRunning())
    {
        return adac_->stopStream();
    }

    return VST_ERROR_STATUS::SUCCESS;
}
