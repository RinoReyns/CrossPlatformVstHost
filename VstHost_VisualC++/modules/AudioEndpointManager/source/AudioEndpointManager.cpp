#include <chrono>
#include <thread>
#include <future>

#include "VstHostMacro.h"
#include "AudioEndpointManager.h"

#include "RtAudio.h"
#include <iostream>
#include <cstdlib>
#include <cstring>
typedef double MY_TYPE;
#define FORMAT RTAUDIO_FLOAT64
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


unsigned int getDeviceIndex(std::vector<std::string> deviceNames, bool isInput = false)
{
    unsigned int i;
    std::string keyHit;
    std::cout << '\n';
    for (i = 0; i < deviceNames.size(); i++)
        std::cout << "  Device #" << i << ": " << deviceNames[i] << '\n';
    do {
        if (isInput)
            std::cout << "\nChoose an input device #: ";
        else
            std::cout << "\nChoose an output device #: ";
        std::cin >> i;
    } while (i >= deviceNames.size());
    std::getline(std::cin, keyHit);  // used to clear out stdin
    return i;
}

double streamTimePrintIncrement = 1.0; // seconds
double streamTimePrintTime = 1.0; // seconds

int inout(void* outputBuffer, void* inputBuffer, unsigned int /*nBufferFrames*/,
    double streamTime, RtAudioStreamStatus status, void* data)
{
    // Since the number of input and output channels is equal, we can do
    // a simple buffer copy operation here.
    if (status) std::cout << "Stream over/underflow detected." << std::endl;

    if (streamTime >= streamTimePrintTime) {
        std::cout << "streamTime = " << streamTime << std::endl;
        streamTimePrintTime += streamTimePrintIncrement;
    }

    unsigned int* bytes = (unsigned int*)data;
    memcpy(outputBuffer, inputBuffer, *bytes);
    return 0;
}

int AudioEndpointManager::RunAudioEndpointHandler()
{

    unsigned int channels, fs, bufferBytes, oDevice = 0, iDevice = 0, iOffset = 0, oOffset = 0;

    // Minimal command-line checking
    // TODO:
    // print devices info to choose from
    fs = 48000;
    RtAudio adac;
    std::vector<unsigned int> deviceIds = adac.getDeviceIds();
    if (deviceIds.size() < 1) {
        std::cout << "\nNo audio devices found!\n";
        exit(1);
    }

    channels = (unsigned int) 2;
    iDevice = (unsigned int)3;
    oDevice = (unsigned int)0;
    iOffset = (unsigned int)0;
    oOffset = (unsigned int)0;

    // Let RtAudio print messages to stderr.
    adac.showWarnings(true);

    // Set the same number of channels for both input and output.
    unsigned int bufferFrames = 128;
    RtAudio::StreamParameters iParams, oParams;
    iParams.nChannels = channels;
    iParams.firstChannel = iOffset;
    oParams.nChannels = channels;
    oParams.firstChannel = oOffset;

    if (iDevice == 0)
        iParams.deviceId = adac.getDefaultInputDevice();
    else {
        if (iDevice >= deviceIds.size())
            iDevice = getDeviceIndex(adac.getDeviceNames(), true);
        iParams.deviceId = deviceIds[iDevice];
    }
    if (oDevice == 0)
        oParams.deviceId = adac.getDefaultOutputDevice();
    else {
        if (oDevice >= deviceIds.size())
            oDevice = getDeviceIndex(adac.getDeviceNames());
        oParams.deviceId = deviceIds[oDevice];
    }

    RtAudio::StreamOptions options;
    //options.flags |= RTAUDIO_NONINTERLEAVED;

    bufferBytes = bufferFrames * channels * sizeof(MY_TYPE);
    if (adac.openStream(&oParams, &iParams, FORMAT, fs, &bufferFrames, &inout, (void*)&bufferBytes, &options)) {
        goto cleanup;
    }

    if (adac.isStreamOpen() == false) goto cleanup;

    // Test RtAudio functionality for reporting latency.
    std::cout << "\nStream latency = " << adac.getStreamLatency() << " frames" << std::endl;

    if (adac.startStream()) goto cleanup;

    char input;
    std::cout << "\nRunning ... press <enter> to quit (buffer frames = " << bufferFrames << ").\n";
    std::cin.get(input);

    // Stop the stream.
    if (adac.isStreamRunning())
        adac.stopStream();

cleanup:
    if (adac.isStreamOpen()) adac.closeStream();
    
    
    return 0;
}
