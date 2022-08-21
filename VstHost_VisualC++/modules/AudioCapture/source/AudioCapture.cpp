#include "AudioCapture.h"
#include "VstHostMacro.h"

AudioCapture::AudioCapture(uint8_t verbose) :
    verbose_(verbose)
{
    endpoint_type_ = eCapture;
}

AudioCapture::~AudioCapture()
{
    this->Release();
}

VST_ERROR_STATUS AudioCapture::Release()
{
    if (pAudioClient)
    {
        auto status = pAudioClient->Stop();
        LOG(INFO) << "Stop audio capture client status: " << status;
        SAFE_RELEASE(pAudioClient);
    }

    if (device_format_)
    {
        CoTaskMemFree(device_format_);
        memset(&device_format_, 0, sizeof(device_format_));
    }

    if (wave_writer_)
    {
        wave_writer_->~CMFWaveWriter();
    }

    SAFE_RELEASE(pEnumerator);
    SAFE_RELEASE(pDevice);
    SAFE_RELEASE(pCaptureClient);

    return VST_ERROR_STATUS::SUCCESS;
}

VST_ERROR_STATUS AudioCapture::GetEndpointSamplingRate(uint32_t* sampling_rate)
{
    RETURN_ERROR_IF_NULL(device_format_);
    *sampling_rate = static_cast<uint32_t>(device_format_->nSamplesPerSec);
    return VST_ERROR_STATUS::SUCCESS;
}

VST_ERROR_STATUS AudioCapture::Init()
{
    HRESULT hr = S_OK;

    RETURN_IF_AUDIO_CAPTURE_FAILED(CoCreateInstance(CLSID_MMDeviceEnumerator,
        NULL,
        CLSCTX_ALL,
        IID_IMMDeviceEnumerator,
        (void**)&pEnumerator));

    UINT discovered_devices_count = 0;

    LOG(INFO) << "--------- Capture devices: ---------";
    RETURN_ERROR_IF_NOT_SUCCESS(ListAudioCaptureEndpoints(&discovered_devices_count));
    RETURN_ERROR_IF_NOT_SUCCESS(SetAudioEnpoint(&discovered_devices_count));

    RETURN_IF_AUDIO_CAPTURE_FAILED(pDevice->Activate(IID_IAudioClient,
        CLSCTX_ALL,
        NULL,
        (void**)&pAudioClient));

    RETURN_IF_AUDIO_CAPTURE_FAILED(pAudioClient->GetMixFormat(&device_format_));
    RETURN_IF_AUDIO_CAPTURE_FAILED(pAudioClient->Initialize(AUDCLNT_SHAREMODE_SHARED,
        0,
        static_cast<REFERENCE_TIME>(REFTIMES_PER_SEC),
        0,
        device_format_,
        NULL));

    RETURN_IF_AUDIO_CAPTURE_FAILED(pAudioClient->GetService(IID_IAudioCaptureClient,
        (void**)&pCaptureClient));

    UINT32 buffer_frame_count; // TODO: rename

    RETURN_IF_AUDIO_CAPTURE_FAILED(pAudioClient->GetBufferSize(&buffer_frame_count));
    recording_loop_sleep_time_ = (static_cast<REFERENCE_TIME>(REFTIMES_PER_SEC) * buffer_frame_count) /
        (device_format_->nSamplesPerSec * 2 * REFTIMES_PER_MILLISEC);

    wave_writer_.reset(new CMFWaveWriter(this->verbose_));
    RETURN_ERROR_IF_NOT_SUCCESS(wave_writer_->Initialize(AUDIO_CAPUTRE_RENDER_FILE, device_format_));

    return VST_ERROR_STATUS::SUCCESS;
}

VST_ERROR_STATUS AudioCapture::RecordAudioStream()
{
    HRESULT hr = S_OK;
    UINT32 uiFileLength = 0;

    try
    {
        RETURN_IF_AUDIO_CAPTURE_FAILED(pAudioClient->Start());
        UINT32 packetLength = 0;
        UINT32 numFramesAvailable;
        BYTE* pData;
        DWORD flags;

        while (run_recording_loop_ == FALSE)
        {
            Sleep(this->recording_loop_sleep_time_);

            RETURN_IF_AUDIO_CAPTURE_FAILED(pCaptureClient->GetNextPacketSize(&packetLength));

            while (packetLength != 0)
            {
                RETURN_IF_AUDIO_CAPTURE_FAILED(pCaptureClient->GetBuffer(&pData,
                    &numFramesAvailable,
                    &flags,
                    NULL,
                    NULL));

                if (flags & AUDCLNT_BUFFERFLAGS_SILENT)
                {
                    LOG_IF(verbose_ > LogLevelType::LOG_LEVEL::ERROR_LOG, INFO) << L"AUDCLNT_BUFFERFLAGS_SILENT";
                    break;
                }

                if (flags & AUDCLNT_BUFFERFLAGS_DATA_DISCONTINUITY)
                {
                    LOG_IF(verbose_ > LogLevelType::LOG_LEVEL::ERROR_LOG, INFO) << L"AUDCLNT_BUFFERFLAGS_DATA_DISCONTINUITY";
                }

                if (flags & AUDCLNT_BUFFERFLAGS_TIMESTAMP_ERROR)
                {
                    LOG_IF(verbose_ > LogLevelType::LOG_LEVEL::ERROR_LOG, INFO) << L"AUDCLNT_BUFFERFLAGS_TIMESTAMP_ERROR";
                }

                assert(packetLength == numFramesAvailable);

                RETURN_IF_AUDIO_CAPTURE_FAILED(wave_writer_->WriteWaveData(
                    pData,
                    numFramesAvailable * device_format_->nBlockAlign) ? S_OK : E_FAIL);
                uiFileLength += numFramesAvailable;
                RETURN_IF_AUDIO_CAPTURE_FAILED(pCaptureClient->ReleaseBuffer(numFramesAvailable));
                RETURN_IF_AUDIO_CAPTURE_FAILED(pCaptureClient->GetNextPacketSize(&packetLength));
            }
        }
    }
    catch (HRESULT) {}

    if (hr == S_OK && device_format_ != NULL)
    {
        wave_writer_->FinalizeHeader(device_format_, uiFileLength);
    }

    this->Release();

    return VST_ERROR_STATUS::SUCCESS;
}

BOOL AudioCapture::GetRunRecordingLoop()
{
    return this->run_recording_loop_;
}

void AudioCapture::SetRunRecordingLoop(BOOL status)
{
    this->run_recording_loop_ = status;
}
