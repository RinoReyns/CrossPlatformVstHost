#include "AudioCapture.h"
#include "Functiondiscoverykeys_devpkey.h"

#undef max

#define AUDIO_CAPTURE_FILE		L"Capture.wav"
#define REFTIMES_PER_SEC		10000000
#define REFTIMES_PER_MILLISEC	10000
#define MAX_LOOP_BEFORE_STOP	20
#define DEVICE_OUTPUT_FORMAT    "Audio Device %d: %ws"

const CLSID CLSID_MMDeviceEnumerator = __uuidof(MMDeviceEnumerator);
const IID IID_IMMDeviceEnumerator	 = __uuidof(IMMDeviceEnumerator);
const IID IID_IAudioClient			 = __uuidof(IAudioClient);
const IID IID_IAudioCaptureClient	 = __uuidof(IAudioCaptureClient);

AudioCapture::AudioCapture(uint8_t verbose): 
    verbose_(verbose)
{
}

VST_ERROR_STATUS AudioCapture::ListAudioCaptureEndpoints(IMMDeviceEnumerator* pEnumerator, 
                                                         IMMDevice** pCurrentDevice)
{
    // TODO:
    // use this function to only enumerate.
    // set endpoint outside of this function.
    int deviceStateFilter     = DEVICE_STATE_ACTIVE;
    LPWSTR strDefaultDeviceID = '\0';
    LPCWSTR pDeviceFormatStr  = _T(DEVICE_OUTPUT_FORMAT);
    IMMDeviceCollection* pDevices;
    IMMDevice* device; 
    UINT count = 0;
    auto status = pEnumerator->EnumAudioEndpoints(eCapture, deviceStateFilter, &pDevices);
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
                    status = printDeviceInfo(device, i, pDeviceFormatStr, strDefaultDeviceID);
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

    status = pDevices->Item(enpoint_id - 1, pCurrentDevice);
    pDevices->Release();
    return VST_ERROR_STATUS::SUCCESS;
}

HRESULT AudioCapture::printDeviceInfo(IMMDevice* pDevice, int index, LPCWSTR outFormat, LPWSTR strDefaultDeviceID)
{
    // Device ID
    LPWSTR strID = NULL;
    HRESULT hr = pDevice->GetId(&strID);
    if (!SUCCEEDED(hr))
    {
        return hr;
    }

    int deviceDefault = (strDefaultDeviceID != '\0' && (wcscmp(strDefaultDeviceID, strID) == 0));

    // Device state
    DWORD dwState;
    hr = pDevice->GetState(&dwState);
    if (!SUCCEEDED(hr))
    {
        return hr;
    }

    IPropertyStore* pStore;
    hr = pDevice->OpenPropertyStore(STGM_READ, &pStore);
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

        pStore->Release();
    }
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

VST_ERROR_STATUS AudioCapture::RecordAudioStream()
{
    HRESULT hr							= S_OK;
    IMMDeviceEnumerator* pEnumerator	= NULL;
    IMMDevice* pDevice					= NULL;
    IAudioClient* pAudioClient			= NULL;
    IAudioCaptureClient* pCaptureClient = NULL;
    WAVEFORMATEX* pwfx					= NULL;
    REFERENCE_TIME hnsRequestedDuration = REFTIMES_PER_SEC;
    UINT32 uiFileLength					= 0;
    BOOL bExtensibleFormat				= FALSE;
    DWORD sleep_time;
    UINT32 bufferFrameCount;
    CMFWaveWriter WaveWriter;

    try
    {
        IF_FAILED_THROW(CoCreateInstance(CLSID_MMDeviceEnumerator, NULL, CLSCTX_ALL, IID_IMMDeviceEnumerator, (void**)&pEnumerator));
        ListAudioCaptureEndpoints(pEnumerator, &pDevice);
        //IF_FAILED_THROW(pEnumerator->GetDefaultAudioEndpoint(eCapture, eConsole, &pDevice));
        IF_FAILED_THROW(pDevice->Activate(IID_IAudioClient, CLSCTX_ALL, NULL, (void**)&pAudioClient));
        IF_FAILED_THROW(pAudioClient->GetMixFormat(&pwfx));
        ReadEndpointFormat(pwfx, &bExtensibleFormat);

        IF_FAILED_THROW(pAudioClient->Initialize(AUDCLNT_SHAREMODE_SHARED, 0, hnsRequestedDuration, 0, pwfx, NULL));
        IF_FAILED_THROW(pAudioClient->GetBufferSize(&bufferFrameCount));
        IF_FAILED_THROW(pAudioClient->GetService(IID_IAudioCaptureClient, (void**)&pCaptureClient));

        IF_FAILED_THROW(WaveWriter.Initialize(AUDIO_CAPTURE_FILE, bExtensibleFormat) ? S_OK : E_FAIL);
        sleep_time = (static_cast<REFERENCE_TIME>(REFTIMES_PER_SEC) * bufferFrameCount) / 
                     (pwfx->nSamplesPerSec * 2 * REFTIMES_PER_MILLISEC);

        IF_FAILED_THROW(pAudioClient->Start());

        BOOL bDone = FALSE;
        UINT32 packetLength = 0;
        UINT32 numFramesAvailable;
        BYTE* pData;
        DWORD flags;
        int iLoop = 0;

        while (bDone == FALSE)
        {
            Sleep(sleep_time);

            IF_FAILED_THROW(pCaptureClient->GetNextPacketSize(&packetLength));

            while (packetLength != 0)
            {
                IF_FAILED_THROW(pCaptureClient->GetBuffer(&pData, &numFramesAvailable, &flags, NULL, NULL));

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

                IF_FAILED_THROW(WaveWriter.WriteWaveData(pData, numFramesAvailable * pwfx->nBlockAlign) ? S_OK : E_FAIL);
                uiFileLength += numFramesAvailable;
                IF_FAILED_THROW(pCaptureClient->ReleaseBuffer(numFramesAvailable));
                IF_FAILED_THROW(pCaptureClient->GetNextPacketSize(&packetLength));
            }

            if (iLoop++ == MAX_LOOP_BEFORE_STOP)
            {
                bDone = TRUE;
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

void AudioCapture::ReadEndpointFormat(WAVEFORMATEX* pwfx, BOOL* extensible_format_flag)
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
