#ifndef AUDIO_ENDPOINT_BASE_H
#define AUDIO_ENDPOINT_BASE_H
#include <string>

#include "EndpointUtils.h"
#include "enums.h"
#include "easylogging++.h"


class AudioEndpointBase
{

public:
    virtual ~AudioEndpointBase() = default;
    VST_ERROR_STATUS ListAudioCaptureEndpoints(UINT* discovered_devices_count);
    VST_ERROR_STATUS SetAudioEnpoint(UINT* discovered_devices_count);

private:
    HRESULT PrintDeviceInfo(IMMDevice* pDevice, int index);
    std::string getDeviceProperty(IPropertyStore* pStore, const PROPERTYKEY key);

protected:
    IMMDevice* pDevice = NULL;
    IMMDeviceEnumerator* pEnumerator = NULL;
    /// Type of enpoint e.g. capture or render. 
    EDataFlow endpoint_type_ = EDataFlow_enum_count;
    std::map<uint8_t, std::string> enpoints_names_map_;
};

#endif // AUDIO_ENDPOINT_BASE_H
