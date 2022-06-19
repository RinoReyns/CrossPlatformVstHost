#include "AudioCapture.h"
#include "Functiondiscoverykeys_devpkey.h"

#include "VstHostMacro.h"

#undef max

#define AUDIO_CAPTURE_FILE		L"Capture.wav"
#define REFTIMES_PER_SEC		10000000
#define REFTIMES_PER_MILLISEC	10000
#define DEVICE_OUTPUT_FORMAT    "Audio Device %d: %ws"

const CLSID CLSID_MMDeviceEnumerator = __uuidof(MMDeviceEnumerator);
const IID IID_IMMDeviceEnumerator	 = __uuidof(IMMDeviceEnumerator);
const IID IID_IAudioClient			 = __uuidof(IAudioClient);
const IID IID_IAudioCaptureClient	 = __uuidof(IAudioCaptureClient);

AudioCapture::AudioCapture(uint8_t verbose): 
    verbose_(verbose)
{
}

VST_ERROR_STATUS AudioCapture::ListAudioCaptureEndpoints()
{
    // TODO:
    // use this function to only enumerate.
    // set endpoint outside of this function.
    LPWSTR strDefaultDeviceID = '\0';
    IMMDeviceCollection* pDevices;
    IMMDevice* device; 
    UINT count = 0;
    auto status = pEnumerator->EnumAudioEndpoints(eCapture, DEVICE_STATE_ACTIVE, &pDevices);
    if SUCCEEDED(status)
    {
        pDevices->GetCount(&count);
        LOG(INFO) << "Discoverd endpoints count: " << count;
        IMMDevice* pDefaultDevice;
        status = pEnumerator->GetDefaultAudioEndpoint(eCapture, eConsole, &pDefaultDevice);
        if (SUCCEEDED(status))
        {
            // TODO:
            // store enpoints names 
            status = pDefaultDevice->GetId(&strDefaultDeviceID);
            for (int i = 1; i <= static_cast<int>(count); i++)
            {
                status = pDevices->Item(i - 1, &device);
                if (SUCCEEDED(status))
                {
                    status = printDeviceInfo(device, i, _T(DEVICE_OUTPUT_FORMAT), strDefaultDeviceID);
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
        if (enpoint_id == 0 || enpoint_id > static_cast<int>(count))
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

HRESULT AudioCapture::printDeviceInfo(IMMDevice* device, int index, LPCWSTR outFormat, LPWSTR strDefaultDeviceID)
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
                strID
            );
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

    return VST_ERROR_STATUS::SUCCESS;
}

VST_ERROR_STATUS AudioCapture::RecordAudioStream()
{
    HRESULT hr              = S_OK;
    UINT32 uiFileLength     = 0;
    BOOL bExtensibleFormat  = FALSE;
    DWORD sleep_time;
    UINT32 bufferFrameCount;
    CMFWaveWriter WaveWriter;

    try
    {
        // TODO:
        // clean up. Should be in init
        RETURN_IF_AUDIO_CAPTURE_FAILED(pAudioClient->GetBufferSize(&bufferFrameCount));
        sleep_time = (static_cast<REFERENCE_TIME>(REFTIMES_PER_SEC) * bufferFrameCount) / 
                     (pwfx->nSamplesPerSec * 2 * REFTIMES_PER_MILLISEC);
        
        ReadEndpointFormat(&bExtensibleFormat);
        RETURN_IF_AUDIO_CAPTURE_FAILED(WaveWriter.Initialize(AUDIO_CAPTURE_FILE, bExtensibleFormat) ? S_OK : E_FAIL);
        RETURN_IF_AUDIO_CAPTURE_FAILED(pAudioClient->Start());

        UINT32 packetLength = 0;
        UINT32 numFramesAvailable;
        BYTE* pData;
        DWORD flags;

        while (run_recording_loop_ == FALSE)
        {
            Sleep(sleep_time);

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

                RETURN_IF_AUDIO_CAPTURE_FAILED(WaveWriter.WriteWaveData(
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
        WaveWriter.FinalizeHeader(pwfx, uiFileLength, bExtensibleFormat);
    }

    if (pAudioClient)
    {
        auto status = pAudioClient->Stop();
        LOG(INFO) << "Stop audio client status: " << status;
        SAFE_RELEASE(pAudioClient);
    }

    CoTaskMemFree(pwfx);
    SAFE_RELEASE(pCaptureClient);
    SAFE_RELEASE(pEnumerator);
    SAFE_RELEASE(pDevice);
    
    return VST_ERROR_STATUS::SUCCESS;
}

void AudioCapture::ReadEndpointFormat(BOOL* extensible_format_flag)
{
    switch (pwfx->wFormatTag)
    {
        case WAVE_FORMAT_PCM:
            LOG_IF(verbose_ > LogLevelType::LOG_LEVEL::ERROR_LOG, INFO) << L"WAVE_FORMAT_PCM";
            break;

        case WAVE_FORMAT_IEEE_FLOAT:
            LOG_IF(verbose_ > LogLevelType::LOG_LEVEL::ERROR_LOG, INFO) << L"WAVE_FORMAT_IEEE_FLOAT";
            break;

        case WAVE_FORMAT_EXTENSIBLE:
            LOG_IF(verbose_ > LogLevelType::LOG_LEVEL::ERROR_LOG, INFO) << L"WAVE_FORMAT_EXTENSIBLE";
            *extensible_format_flag = TRUE;

            WAVEFORMATEXTENSIBLE* pWaveFormatExtensible = reinterpret_cast<WAVEFORMATEXTENSIBLE*>(pwfx);

            if (pWaveFormatExtensible->SubFormat == KSDATAFORMAT_SUBTYPE_PCM)
            {
                LOG_IF(verbose_ > LogLevelType::LOG_LEVEL::ERROR_LOG, INFO) << L"KSDATAFORMAT_SUBTYPE_PCM";
            }
            else if (pWaveFormatExtensible->SubFormat == KSDATAFORMAT_SUBTYPE_IEEE_FLOAT)
            {
                LOG_IF(verbose_ > LogLevelType::LOG_LEVEL::ERROR_LOG, INFO) << L"KSDATAFORMAT_SUBTYPE_IEEE_FLOAT";
            }
            break;
    }
}

BOOL AudioCapture::GetRunRecordingLoop()
{
    return this->run_recording_loop_;
}

void AudioCapture::SetRunRecordingLoop(BOOL status)
{
    this->run_recording_loop_ = status;
}

