#include "AudioCapture.h"
#include "Functiondiscoverykeys_devpkey.h"

#include "VstHostMacro.h"

#undef max

#define DEVICE_OUTPUT_FORMAT    "Audio Device %d: %ws"

AudioCapture::AudioCapture(uint8_t verbose): 
    verbose_(verbose)
{
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
        LOG(INFO) << "Stop audio client status: " << status;
        SAFE_RELEASE(pAudioClient);
    }
    
    if (pwfx)
    {
        CoTaskMemFree(pwfx);
        memset(&pwfx, 0, sizeof(pwfx));
    }

    SAFE_RELEASE(pEnumerator);
    SAFE_RELEASE(pDevice);
    SAFE_RELEASE(pCaptureClient);

    return VST_ERROR_STATUS::SUCCESS;
}

VST_ERROR_STATUS AudioCapture::ListAudioCaptureEndpoints()
{
    // TODO:
    // use this function to only enumerate.
    // set endpoint outside of this function.
    LPWSTR strDefaultDeviceID = '\0';
    IMMDeviceCollection* pDevices;
    IMMDevice* device; 
    UINT discovered_devices_count  = 0;
    auto status = pEnumerator->EnumAudioEndpoints(eCapture, DEVICE_STATE_ACTIVE, &pDevices);
    if SUCCEEDED(status)
    {
        pDevices->GetCount(&discovered_devices_count);
        LOG(INFO) << "Discovered endpoints count: " << discovered_devices_count;
        IMMDevice* pDefaultDevice;
        status = pEnumerator->GetDefaultAudioEndpoint(eCapture, eConsole, &pDefaultDevice);
        if (SUCCEEDED(status))
        {
            // TODO:
            // store enpoints names 
            status = pDefaultDevice->GetId(&strDefaultDeviceID);
            for (int i = 1; i <= static_cast<int>(discovered_devices_count); i++)
            {
                status = pDevices->Item(i - 1, &device);
                if (SUCCEEDED(status))
                {
                    status = PrintDeviceInfo(device, i, _T(DEVICE_OUTPUT_FORMAT), strDefaultDeviceID);
                    device->Release();
                }
            }
        }
    }

    int enpoint_id = -1;
    bool is_endpoint_chosen = false;
    std::cout << "Choose endpoint id (int):\n";
    while(!is_endpoint_chosen)
    {	
        std::cin >> enpoint_id;
        if (enpoint_id == 0 || enpoint_id > static_cast<int>(discovered_devices_count))
        {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "Invalid input. Try again:\n";
        }
        else
        {
            is_endpoint_chosen = true;
        }
    }

    status = pDevices->Item(enpoint_id - 1, &pDevice);
    pDevices->Release();
    return VST_ERROR_STATUS::SUCCESS;
}

HRESULT AudioCapture::PrintDeviceInfo(IMMDevice* device, int index, LPCWSTR outFormat, LPWSTR strDefaultDeviceID)
{
    // Device ID
    LPWSTR strID = NULL;
    HRESULT hr = S_OK;
    RETURN_IF_AUDIO_CAPTURE_FAILED(device->GetId(&strID));

    int deviceDefault = (strDefaultDeviceID != '\0' && (wcscmp(strDefaultDeviceID, strID) == 0));

    // Device state
    DWORD dwState;
    RETURN_IF_AUDIO_CAPTURE_FAILED(device->GetState(&dwState));

    IPropertyStore* pStore;

    hr = device->OpenPropertyStore(STGM_READ, &pStore);
    if (SUCCEEDED(hr))
    {
        std::wstring friendlyName = getDeviceProperty(pStore, PKEY_Device_FriendlyName);
        std::wstring Desc = getDeviceProperty(pStore, PKEY_Device_DeviceDesc);
        std::wstring interfaceFriendlyName = getDeviceProperty(pStore, PKEY_DeviceInterface_FriendlyName);

        if (SUCCEEDED(hr))
        {
            wprintf_s(outFormat,
                      index,
                      friendlyName.c_str(),
                      dwState,
                      deviceDefault,
                      Desc.c_str(),
                      interfaceFriendlyName.c_str(),
                      strID);
            wprintf_s(_T("\n"));
        }
    }

    pStore->Release();
    return hr;
}

std::wstring AudioCapture::getDeviceProperty(IPropertyStore* pStore, const PROPERTYKEY key)
{
    PROPVARIANT prop;
    PropVariantInit(&prop);
    HRESULT hr = pStore->GetValue(key, &prop);
    if (SUCCEEDED(hr))
    {
        std::wstring result(prop.pwszVal);
        PropVariantClear(&prop);
        return result;
    }
    else
    {
        return std::wstring(L"");
    }
}

VST_ERROR_STATUS AudioCapture::InitializeAudioStream()
{
    HRESULT hr = S_OK;

    RETURN_IF_AUDIO_CAPTURE_FAILED(CoCreateInstance(CLSID_MMDeviceEnumerator, 
                                                    NULL, 
                                                    CLSCTX_ALL, 
                                                    IID_IMMDeviceEnumerator, 
                                                    (void**)&pEnumerator));
    ListAudioCaptureEndpoints();
    //IF_FAILED_THROW(pEnumerator->GetDefaultAudioEndpoint(eCapture, eConsole, &pDevice));
    RETURN_IF_AUDIO_CAPTURE_FAILED(pDevice->Activate(IID_IAudioClient, 
                                                     CLSCTX_ALL, 
                                                     NULL, 
                                                     (void**)&pAudioClient));

    RETURN_IF_AUDIO_CAPTURE_FAILED(pAudioClient->GetMixFormat(&pwfx));
    RETURN_IF_AUDIO_CAPTURE_FAILED(pAudioClient->Initialize(AUDCLNT_SHAREMODE_SHARED,
                                                            0, 
                                                            static_cast<REFERENCE_TIME>(REFTIMES_PER_SEC), 
                                                            0, 
                                                            pwfx, 
                                                            NULL));

    RETURN_IF_AUDIO_CAPTURE_FAILED(pAudioClient->GetService(IID_IAudioCaptureClient, 
                                                            (void**)&pCaptureClient));

    UINT32 buffer_frame_count; // TODO: rename

    RETURN_IF_AUDIO_CAPTURE_FAILED(pAudioClient->GetBufferSize(&buffer_frame_count));
    recording_loop_sleep_time_ = (static_cast<REFERENCE_TIME>(REFTIMES_PER_SEC) * buffer_frame_count) /
                                 (pwfx->nSamplesPerSec * 2 * REFTIMES_PER_MILLISEC);

    wave_writer_.reset(new CMFWaveWriter(this->verbose_));
    RETURN_ERROR_IF_NOT_SUCCESS(wave_writer_->Initialize(AUDIO_CAPUTRE_RENDER_FILE, pwfx));

    return VST_ERROR_STATUS::SUCCESS;
}

VST_ERROR_STATUS AudioCapture::RecordAudioStream()
{
    HRESULT hr              = S_OK;
    UINT32 uiFileLength     = 0;
   
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
                    numFramesAvailable * pwfx->nBlockAlign) ? S_OK : E_FAIL);
                uiFileLength += numFramesAvailable;
                RETURN_IF_AUDIO_CAPTURE_FAILED(pCaptureClient->ReleaseBuffer(numFramesAvailable));
                RETURN_IF_AUDIO_CAPTURE_FAILED(pCaptureClient->GetNextPacketSize(&packetLength));
            }
        }
    }
    catch (HRESULT) {}

    if (hr == S_OK && pwfx != NULL)
    {
        wave_writer_->FinalizeHeader(pwfx, uiFileLength);
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
