#ifndef AUDIO_PROCESSING_VST_HOST
#define AUDIO_PROCESSING_VST_HOST

#include "ivstevents.h"
#include "parameterchanges.h"
#include "processdata.h"
#include "plugprovider.h"
#include "enums.h"
#include "easylogging++.h"
#include "WaveIOClass.h"

#ifdef _WIN32
    #include "audiohostlib_export.h"
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
        int AUDIOHOSTLIB_EXPORT BufferProcessing(WaveDataContainer* input_data,
                                                 WaveDataContainer* output_data);
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

    C_API_PREFIX AudioProcessingVstHost* CApiInitialize();

    C_API_PREFIX int CApiSetVerbosity(AudioProcessingVstHost* vst_host,
        uint8_t value);

    C_API_PREFIX int CApiCreatePluginInstance(
        AudioProcessingVstHost* vst_host,
        const char* plugin_path,
        const char* plugin_id);

    C_API_PREFIX int CApiGetPluginParameters(
        AudioProcessingVstHost* vst_host,
        const char* plugin_config,
        const char* plugin_id);

    C_API_PREFIX int CApiSetPluginParameters(
        AudioProcessingVstHost* vst_host,
        const char* plugin_config,
        const char* plugin_id);

    C_API_PREFIX int CApiProcessWaveFile(AudioProcessingVstHost* vst_host,
        const char* input_wave_path,
        const char* output_wave_path);

    C_API_PREFIX int CApiDeleteInstance(AudioProcessingVstHost* vst_host);
}

#endif // AUDIO_PROCESSING_VST_HOST
