#include "AudioRender.h"
#include "Functiondiscoverykeys_devpkey.h"

#include "VstHostMacro.h"
#include "WaveReader.h"

#undef max

const CLSID CLSID_MMDeviceEnumerator = __uuidof(MMDeviceEnumerator);
const IID IID_IMMDeviceEnumerator    = __uuidof(IMMDeviceEnumerator);
const IID IID_IAudioClient           = __uuidof(IAudioClient);
const IID IID_IAudioCaptureClient    = __uuidof(IAudioCaptureClient);
const IID IID_IAudioRenderClient     = __uuidof(IAudioRenderClient);

AudioRender::AudioRender()
{
}

VST_ERROR_STATUS AudioRender::RenderAudioStream()
{
    HRESULT hr;
    IMMDeviceEnumerator* pEnumerator = NULL;
    IMMDevice* pDevice = NULL;
    IAudioClient* pAudioClient = NULL;
    IAudioRenderClient* pRenderClient = NULL;
    WAVEFORMATEX* pwfx = NULL;
    UINT32 bufferFrameCount;
    UINT32 numFramesAvailable;
    UINT32 numFramesPadding;
    BYTE* pData;
    DWORD flags = 0;
    std::unique_ptr<WaveReader> wave_reader(new WaveReader());

    RETURN_ERROR_IF_NOT_SUCCESS(wave_reader->Initialize(AUDIO_CAPUTRE_RENDER_FILE));

    hr = CoCreateInstance(
        CLSID_MMDeviceEnumerator, NULL,
        CLSCTX_ALL, IID_IMMDeviceEnumerator,
        (void**)&pEnumerator);
    EXIT_ON_ERROR(hr)

    hr = pEnumerator->GetDefaultAudioEndpoint(eRender, eConsole, &pDevice);
    EXIT_ON_ERROR(hr)

    hr = pDevice->Activate(
        IID_IAudioClient, CLSCTX_ALL,
        NULL, (void**)&pAudioClient);
    EXIT_ON_ERROR(hr)

    hr = pAudioClient->GetMixFormat(&pwfx);
    EXIT_ON_ERROR(hr)

    hr = pAudioClient->Initialize(AUDCLNT_SHAREMODE_SHARED,
                                  0,
                                  static_cast<REFERENCE_TIME>(REFTIMES_PER_SEC),
                                  0,
                                  pwfx,
                                  NULL);
   EXIT_ON_ERROR(hr)

    // Get the actual size of the allocated buffer.
    hr = pAudioClient->GetBufferSize(&bufferFrameCount);
    EXIT_ON_ERROR(hr)

    hr = pAudioClient->GetService(
        IID_IAudioRenderClient,
        (void**)&pRenderClient);
    EXIT_ON_ERROR(hr)

    // Grab the entire buffer for the initial fill operation.
    hr = pRenderClient->GetBuffer(bufferFrameCount, &pData);
    EXIT_ON_ERROR(hr)

    // Load the initial data into the shared buffer.
    hr = wave_reader->LoadData(pData, bufferFrameCount * pwfx->nBlockAlign, &flags);
    EXIT_ON_ERROR(hr)
    hr = pRenderClient->ReleaseBuffer(bufferFrameCount, flags);
    EXIT_ON_ERROR(hr)

    // Calculate the actual duration of the allocated buffer.
    DWORD  sleep_time = (static_cast<REFERENCE_TIME>(REFTIMES_PER_SEC) * bufferFrameCount) /
                        (pwfx->nSamplesPerSec * 2 * REFTIMES_PER_MILLISEC);
    hr = pAudioClient->Start();  // Start playing.
    EXIT_ON_ERROR(hr)

    // Each loop fills about half of the shared buffer.
    while (flags != AUDCLNT_BUFFERFLAGS_SILENT)
    {
        // Sleep for half the buffer duration.
        Sleep(sleep_time);

        // See how much buffer space is available.
        hr = pAudioClient->GetCurrentPadding(&numFramesPadding);
        EXIT_ON_ERROR(hr)

        numFramesAvailable = bufferFrameCount - numFramesPadding;

        // Grab all the available space in the shared buffer.
        hr = pRenderClient->GetBuffer(numFramesAvailable, &pData);
        EXIT_ON_ERROR(hr)

        // Get next 1/2-second of data from the audio source.
        hr = wave_reader->LoadData(pData, numFramesAvailable * pwfx->nBlockAlign, &flags);
        EXIT_ON_ERROR(hr)

        hr = pRenderClient->ReleaseBuffer(numFramesAvailable, flags);
        EXIT_ON_ERROR(hr)
    }

    // Wait for last data in buffer to play before stopping.
    Sleep(sleep_time);

    hr = pAudioClient->Stop();  // Stop playing.
    EXIT_ON_ERROR(hr)

    Exit:
        CoTaskMemFree(pwfx);
        SAFE_RELEASE(pEnumerator);
        SAFE_RELEASE(pDevice);
        SAFE_RELEASE(pAudioClient);
        SAFE_RELEASE(pRenderClient);

    return VST_ERROR_STATUS::SUCCESS;
}
