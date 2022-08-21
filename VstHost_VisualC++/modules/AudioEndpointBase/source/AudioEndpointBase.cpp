#include <chrono>
#include <thread>
#include <future>

#include "VstHostMacro.h"
#include "AudioEndpointBase.h"
#include "Functiondiscoverykeys_devpkey.h"

#undef max

/// @brief List all endpoint avaiable. 
/// @return VST_ERROR_STATUS status
VST_ERROR_STATUS AudioEndpointBase::ListAudioCaptureEndpoints(
    UINT* discovered_devices_count /// - number of found enpoints
)
{
    IMMDeviceCollection* pDevices;
    IMMDevice* device;
    auto status = pEnumerator->EnumAudioEndpoints(endpoint_type_, DEVICE_STATE_ACTIVE, &pDevices);
    if SUCCEEDED(status)
    {
        pDevices->GetCount(discovered_devices_count);
        LOG(INFO) << "Discovered endpoints count: " << *discovered_devices_count;
        IMMDevice* pDefaultDevice;
        status = pEnumerator->GetDefaultAudioEndpoint(endpoint_type_, eConsole, &pDefaultDevice);
        if (SUCCEEDED(status))
        {
            for (int device_index = 1; device_index <= static_cast<int>(*discovered_devices_count); device_index++)
            {
                status = pDevices->Item(device_index - 1, &device);
                if (SUCCEEDED(status))
                {
                    status = PrintDeviceInfo(device, device_index);
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

    int endpoint_id = -1;
    bool is_endpoint_selected = false;
    LOG(INFO) << "Insert endpoint id (int):";
    while (!is_endpoint_selected)
    {
        std::cin >> endpoint_id;
        if (endpoint_id == 0 || endpoint_id > static_cast<int>(*discovered_devices_count))
        {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            LOG(INFO) << "Invalid input. Try again:";
        }
        else
        {
            is_endpoint_selected = true;
            LOG(INFO) << "Selected endpoint: " << enpoints_names_map_.at(static_cast<uint8_t>(endpoint_id));
        }
    }

    status = pDevices->Item(endpoint_id - 1, &pDevice);
    pDevices->Release();
    return VST_ERROR_STATUS::SUCCESS;
}

HRESULT AudioEndpointBase::PrintDeviceInfo(IMMDevice* device, int index)
{
    // Device ID
    LPWSTR strID = NULL;
    HRESULT hr = S_OK;
    RETURN_IF_AUDIO_CAPTURE_FAILED(device->GetId(&strID));

    // Device state
    // https://docs.microsoft.com/en-us/windows/win32/coreaudio/device-state-xxx-constants
    DWORD dwState;
    RETURN_IF_AUDIO_CAPTURE_FAILED(device->GetState(&dwState));

    IPropertyStore* pStore;

    hr = device->OpenPropertyStore(STGM_READ, &pStore);
    if (SUCCEEDED(hr))
    {
        std::string friendly_name = getDeviceProperty(pStore, PKEY_Device_FriendlyName);
        std::string description = getDeviceProperty(pStore, PKEY_Device_DeviceDesc);
        std::string interfaceFriendlyName = getDeviceProperty(pStore, PKEY_DeviceInterface_FriendlyName);
        enpoints_names_map_.insert(std::make_pair(static_cast<uint8_t>(index), friendly_name));
        LOG(INFO) << "\nAudio Device " << index << ":\n\t" << friendly_name.c_str()
            << "\n\tDescription: " << description
            << "\n\tInterface Name: " << interfaceFriendlyName
            << "\n\tDevice ID: " << strID
            << "\n\tDevice state:" << dwState;
    }

    pStore->Release();
    return hr;
}

std::string AudioEndpointBase::getDeviceProperty(IPropertyStore* pStore, const PROPERTYKEY key)
{
    PROPVARIANT prop;
    PropVariantInit(&prop);
    HRESULT hr = pStore->GetValue(key, &prop);
    if (!SUCCEEDED(hr))
    {
        PropVariantClear(&prop);
        return std::string("");
    }

    std::wstring wstring_text(prop.pwszVal);
    // Convert wstring to std::string
    std::string result;
    std::transform(wstring_text.begin(), wstring_text.end(), std::back_inserter(result), [](wchar_t c) {
        return (char)c;
        });

    PropVariantClear(&prop);
    return result;
}
