#include "AudioRender.h"
#include "Functiondiscoverykeys_devpkey.h"

#include "VstHostMacro.h"

const CLSID CLSID_MMDeviceEnumerator = __uuidof(MMDeviceEnumerator);
const IID IID_IMMDeviceEnumerator = __uuidof(IMMDeviceEnumerator);
const IID IID_IAudioClient = __uuidof(IAudioClient);
const IID IID_IAudioCaptureClient = __uuidof(IAudioCaptureClient);
const IID IID_IAudioRenderClient = __uuidof(IAudioRenderClient);

AudioRender::AudioRender()
{
    endpoint_type_ = eRender;
}

AudioRender::~AudioRender()
{
    this->Release();
}

VST_ERROR_STATUS AudioRender::Release()
{
    if (pAudioClient)
    {
        auto status = pAudioClient->Stop();
        LOG(INFO) << "Stop audio render client status: " << status;
        SAFE_RELEASE(pAudioClient);
    }

    if (device_format_)
    {
        CoTaskMemFree(device_format_);
        memset(&device_format_, 0, sizeof(device_format_));
    }

    if (wave_reader_)
    {
        wave_reader_->~WaveReader();
    }

    SAFE_RELEASE(pEnumerator);
    SAFE_RELEASE(pDevice);
    SAFE_RELEASE(pAudioClient);
    SAFE_RELEASE(pRenderClient);

    return VST_ERROR_STATUS::SUCCESS;
}

VST_ERROR_STATUS AudioRender::Init()
{
    HRESULT hr = S_OK;

    RETURN_IF_AUDIO_RENDER_FAILED(CoCreateInstance(
        CLSID_MMDeviceEnumerator,
        NULL,
        CLSCTX_ALL,
        IID_IMMDeviceEnumerator,
        (void**)&pEnumerator));

    UINT discovered_devices_count = 0;

    LOG(INFO) << "--------- Render devices: ---------";
    RETURN_ERROR_IF_NOT_SUCCESS(ListAudioCaptureEndpoints(&discovered_devices_count));
    RETURN_ERROR_IF_NOT_SUCCESS(SetAudioEnpoint(&discovered_devices_count));

    RETURN_IF_AUDIO_RENDER_FAILED(pDevice->Activate(
        IID_IAudioClient,
        CLSCTX_ALL,
        NULL,
        (void**)&pAudioClient));

    RETURN_IF_AUDIO_RENDER_FAILED(pAudioClient->GetMixFormat(&device_format_));
    RETURN_IF_AUDIO_RENDER_FAILED(pAudioClient->Initialize(AUDCLNT_SHAREMODE_SHARED,
        0,
        static_cast<REFERENCE_TIME>(REFTIMES_PER_SEC),
        0,
        device_format_,
        NULL));

    // Get the actual size of the allocated buffer.
    RETURN_IF_AUDIO_RENDER_FAILED(pAudioClient->GetBufferSize(&bufferFrameCount));
    RETURN_IF_AUDIO_RENDER_FAILED(pAudioClient->GetService(IID_IAudioRenderClient, (void**)&pRenderClient));

    return VST_ERROR_STATUS::SUCCESS;
}

VST_ERROR_STATUS AudioRender::GetEndpointSamplingRate(uint32_t* sampling_rate)
{
    RETURN_ERROR_IF_NULL(device_format_);
    *sampling_rate = static_cast<uint32_t>(device_format_->nSamplesPerSec);
    return VST_ERROR_STATUS::SUCCESS;
}

VST_ERROR_STATUS AudioRender::RenderAudioStream()
{
    HRESULT hr;

    UINT32 numFramesAvailable;
    UINT32 numFramesPadding;
    BYTE* pData;
    DWORD flags = 0;

    RETURN_ERROR_IF_NULL(pRenderClient);

    wave_reader_.reset(new WaveReader());
    RETURN_ERROR_IF_NOT_SUCCESS(wave_reader_->Initialize(AUDIO_CAPUTRE_RENDER_FILE));

    // Grab the entire buffer for the initial fill operation.
    RETURN_IF_AUDIO_RENDER_FAILED(pRenderClient->GetBuffer(bufferFrameCount, &pData));

    // Load the initial data into the shared buffer.
    RETURN_IF_AUDIO_RENDER_FAILED(wave_reader_->LoadData(
        pData,
        bufferFrameCount * device_format_->nBlockAlign,
        &flags));
    RETURN_IF_AUDIO_RENDER_FAILED(pRenderClient->ReleaseBuffer(bufferFrameCount, flags));

    // Calculate the actual duration of the allocated buffer.
    DWORD  sleep_time = (static_cast<REFERENCE_TIME>(REFTIMES_PER_SEC) * bufferFrameCount) /
        (device_format_->nSamplesPerSec * 2 * REFTIMES_PER_MILLISEC);
    RETURN_IF_AUDIO_RENDER_FAILED(pAudioClient->Start());  // Start playing

    // Each loop fills about half of the shared buffer.
    while (flags != AUDCLNT_BUFFERFLAGS_SILENT)
    {
        // Sleep for half the buffer duration.
        Sleep(sleep_time);

        // See how much buffer space is available.
        RETURN_IF_AUDIO_RENDER_FAILED(pAudioClient->GetCurrentPadding(&numFramesPadding));

        numFramesAvailable = bufferFrameCount - numFramesPadding;

        // Grab all the available space in the shared buffer.
        RETURN_IF_AUDIO_RENDER_FAILED(pRenderClient->GetBuffer(numFramesAvailable, &pData));

        // Get next 1/2-second of data from the audio source.
        RETURN_IF_AUDIO_RENDER_FAILED(wave_reader_->LoadData(
            pData,
            numFramesAvailable * device_format_->nBlockAlign,
            &flags));

        RETURN_IF_AUDIO_RENDER_FAILED(pRenderClient->ReleaseBuffer(numFramesAvailable, flags))
    }

    // Wait for last data in buffer to play before stopping.
    Sleep(sleep_time);

    RETURN_IF_AUDIO_RENDER_FAILED(pAudioClient->Stop()); // Stop playing.

    return VST_ERROR_STATUS::SUCCESS;
}
