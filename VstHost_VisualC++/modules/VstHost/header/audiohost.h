#ifndef AUDIO_PROCESSING_VST_HOST
#define AUDIO_PROCESSING_VST_HOST

#include "ivstevents.h"
#include "parameterchanges.h"
#include "processdata.h"
#include "plugprovider.h"
#include "enums.h"
#include "easylogging++.h"

#ifdef _WIN32
    #include "AudioHostLib_Export.h"
    #define C_API_PREFIX __declspec(dllexport)
#else
    #include "audiohostlib_export.h"
    #define C_API_PREFIX AUDIOHOSTLIB_EXPORT
#endif

constexpr char VST_STRING[] = "VST ";

class AudioProcessingVstHost 
{
    public:
        AUDIOHOSTLIB_EXPORT AudioProcessingVstHost() = default;
        AUDIOHOSTLIB_EXPORT ~AudioProcessingVstHost();
        int AUDIOHOSTLIB_EXPORT ProcessWaveFile(const std::string& input_wave_path,
                                                                const std::string& output_wave_path);
        int AUDIOHOSTLIB_EXPORT CreatePluginInstance(const std::string& plugin_path,
                                                     const std::string& plugin_id,
                                                     VST3::Optional<VST3::UID> effectID);
        int AUDIOHOSTLIB_EXPORT CreatePluginInstance(const std::string& plugin_path,
                                                     const std::string& plugin_id);
        int AUDIOHOSTLIB_EXPORT CreateMutliplePluginInstance(const config_type processing_config);
        int AUDIOHOSTLIB_EXPORT SetPluginParameters(std::string plugin_id, const std::string& plugin_config);
        int AUDIOHOSTLIB_EXPORT SetMutliplePluginParameters(const config_type processing_config);
        int AUDIOHOSTLIB_EXPORT GetPluginParameters(std::string plugin_id, const std::string& plugin_config);
        int AUDIOHOSTLIB_EXPORT GetMutliplePluginParameters(const config_type processing_config);
        void AUDIOHOSTLIB_EXPORT Terminate();
        void AUDIOHOSTLIB_EXPORT SetVerbosity(uint8_t value);
        // TODO:
        // remove this buffer
        struct Buffers
        {
            int32_t numInputs;
            int32_t numOutputs;
            int32_t numSamples;
        };

        struct VstPluginParameters
        {
            VST3::Hosting::Module::Ptr module_;
            std::unique_ptr<Steinberg::Vst::PlugProvider> plug_provider_;
            std::string plugin_path_;
            std::string plugin_config_;
        };

    private:
        int CheckVstSdkCompatibility(std::string sdk_version);

        std::map <std::string, AudioProcessingVstHost::VstPluginParameters> vst_plugins_;
        Steinberg::Vst::ProcessContext processContext;
        Steinberg::Vst::ParameterChanges inputParameterChanges;
        Steinberg::Vst::HostProcessData processData;
        uint8_t verbose_         = 0;
};


extern "C" {

    C_API_PREFIX AudioProcessingVstHost* CApiInitialize()
    {
        AudioProcessingVstHost* dc = new AudioProcessingVstHost();
        return dc;
    }

    C_API_PREFIX int CApiSetVerbosity(AudioProcessingVstHost* vst_host,
                                      uint8_t value)
    {
        RETURN_ERROR_IF_NULL(vst_host);
        vst_host->SetVerbosity(value);
        return VST_ERROR_STATUS::SUCCESS;
    }

    C_API_PREFIX int CApiCreatePluginInstance(
        AudioProcessingVstHost* vst_host,
        const char* plugin_path,
        const char* plugin_id)
    {
        RETURN_ERROR_IF_NULL(vst_host);
        RETURN_ERROR_IF_NULL(plugin_path);
        RETURN_ERROR_IF_NULL(plugin_id);
        std::string plugin_path_(plugin_path);
        std::string plugin_id_(plugin_id);
        return vst_host->CreatePluginInstance(plugin_path_, plugin_id_);
    }

    C_API_PREFIX int CApiGetPluginParameters(
        AudioProcessingVstHost* vst_host,
        const char* plugin_config,
        const char* plugin_id)
    {
        RETURN_ERROR_IF_NULL(vst_host);
        RETURN_ERROR_IF_NULL(plugin_config);
        RETURN_ERROR_IF_NULL(plugin_id);
        std::string plugin_config_(plugin_config);
        std::string plugin_id_(plugin_id);
        return vst_host->GetPluginParameters(plugin_id_, plugin_config_);
    }

    C_API_PREFIX int CApiSetPluginParameters(
        AudioProcessingVstHost* vst_host,
        const char* plugin_config,
        const char* plugin_id)
    {
        RETURN_ERROR_IF_NULL(vst_host);
        RETURN_ERROR_IF_NULL(plugin_config);
        RETURN_ERROR_IF_NULL(plugin_id);
        std::string plugin_config_(plugin_config);
        std::string plugin_id_(plugin_id);
        return vst_host->SetPluginParameters(plugin_id_, plugin_config_);
    }

    C_API_PREFIX int CApiProcessWaveFile(AudioProcessingVstHost* vst_host,
                                         const char* input_wave_path,
                                         const char* output_wave_path)
    {
        RETURN_ERROR_IF_NULL(vst_host);
        RETURN_ERROR_IF_NULL(input_wave_path);
        RETURN_ERROR_IF_NULL(output_wave_path);
        std::string input_wave_path_srt(input_wave_path);
        std::string output_wave_path_srt(output_wave_path);
        return vst_host->ProcessWaveFile(input_wave_path_srt, output_wave_path_srt);
    }

    C_API_PREFIX int CApiDeleteInstance(AudioProcessingVstHost* vst_host)
    {
        RETURN_ERROR_IF_NULL(vst_host);
        vst_host->Terminate();
        try
        {
            delete vst_host;
            return 0;
        }
        catch (...)
        {
            return -1;
        }
    }

}

#endif // AUDIO_PROCESSING_VST_HOST
