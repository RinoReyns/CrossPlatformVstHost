#include <chrono>
#include <thread>
#include <future>

#include "VstHostMacro.h"
#include "AudioEndpointBase.h"
#include "Functiondiscoverykeys_devpkey.h"

#define DEVICE_OUTPUT_FORMAT  "Audio Device %d: %ws"
#undef max

/// @brief List all endpoint avaiable. 
/// @return VST_ERROR_STATUS status
VST_ERROR_STATUS AudioEndpointBase::ListAudioCaptureEndpoints(
    UINT* discovered_devices_count /// - number of found enpoints
)
{
    LPWSTR strDefaultDeviceID = '\0';
    IMMDeviceCollection* pDevices;
    IMMDevice* device;
    auto status = pEnumerator->EnumAudioEndpoints(endpoint_type_, DEVICE_STATE_ACTIVE, &pDevices);
    if SUCCEEDED(status)
    {
        pDevices->GetCount(discovered_devices_count);
        LOG(INFO) << "Discovered endpoints count: " << *discovered_devices_count;
        IMMDevice* pDefaultDevice;
        status = pEnumerator->GetDefaultAudioEndpoint(eCapture, eConsole, &pDefaultDevice);
        if (SUCCEEDED(status))
        {
            // TODO:
            // store enpoints names 
            status = pDefaultDevice->GetId(&strDefaultDeviceID);
            for (int i = 1; i <= static_cast<int>(*discovered_devices_count); i++)
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

    if (discovered_devices_count <= 0)
    {
        return VST_ERROR_STATUS::EMPTY_LIST_OF_ENPOINTS;
    }

    return VST_ERROR_STATUS::SUCCESS;
}

VST_ERROR_STATUS AudioEndpointBase::SetAudioEnpoint(UINT* discovered_devices_count)
{
    if (discovered_devices_count <= 0)
    {
        return VST_ERROR_STATUS::EMPTY_LIST_OF_ENPOINTS;
    }

    IMMDeviceCollection* pDevices;
    auto status = pEnumerator->EnumAudioEndpoints(endpoint_type_, DEVICE_STATE_ACTIVE, &pDevices);

    int enpoint_id = -1;
    bool is_endpoint_selected = false;
    LOG(INFO) << "Insert endpoint id (int):";
    while (!is_endpoint_selected)
    {
        std::cin >> enpoint_id;
        if (enpoint_id == 0 || enpoint_id > static_cast<int>(*discovered_devices_count))
        {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            LOG(INFO) << "Invalid input. Try again:";
        }
        else
        {
            is_endpoint_selected = true;
        }
    }

    status = pDevices->Item(enpoint_id - 1, &pDevice);
    pDevices->Release();
    return VST_ERROR_STATUS::SUCCESS;
}

HRESULT AudioEndpointBase::PrintDeviceInfo(
    IMMDevice* device,
    int index,
    LPCWSTR out_format,
    LPWSTR strDefaultDeviceID)
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
        std::wstring description = getDeviceProperty(pStore, PKEY_Device_DeviceDesc);
        std::wstring interfaceFriendlyName = getDeviceProperty(pStore, PKEY_DeviceInterface_FriendlyName);
        // TODO:
        // try to use logger
        wprintf_s(
            out_format,
            index,
            friendlyName.c_str(),
            dwState,
            deviceDefault,
            description.c_str(),
            interfaceFriendlyName.c_str(),
            strID);
        wprintf_s(_T("\n"));
    }

    pStore->Release();
    return hr;
}

std::wstring AudioEndpointBase::getDeviceProperty(IPropertyStore* pStore, const PROPERTYKEY key)
{
    PROPVARIANT prop;
    PropVariantInit(&prop);
    HRESULT hr = pStore->GetValue(key, &prop);
    if (!SUCCEEDED(hr))
    {
        PropVariantClear(&prop);
        return std::wstring(L"");
    }

    std::wstring result(prop.pwszVal);
    PropVariantClear(&prop);
    return result;
}
