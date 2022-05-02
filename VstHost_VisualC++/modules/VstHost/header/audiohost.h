#ifndef AUDIO_PROCESSING_VST_HOST
#define AUDIO_PROCESSING_VST_HOST

#include "ivstevents.h"
#include "parameterchanges.h"
#include "processdata.h"
#include "plugprovider.h"
#include "AudioHostLib_Export.h"

#include "enums.h"
#include "easylogging++.h"

class AudioProcessingVstHost 
{
    public:
        AUDIOHOSTLIB_EXPORT AudioProcessingVstHost() = default;
        AUDIOHOSTLIB_EXPORT ~AudioProcessingVstHost();
        int AUDIOHOSTLIB_EXPORT ProcessWaveFileWithSinglePlugin(const std::string& input_wave_path,
                                                                const std::string& output_wave_path);
        int AUDIOHOSTLIB_EXPORT CreatePluginInstance(const std::string& plugin_path,
                                                     VST3::Optional<VST3::UID> effectID);
        int AUDIOHOSTLIB_EXPORT CreatePluginInstance(const std::string& plugin_path);
        int AUDIOHOSTLIB_EXPORT SetPluginParameters(const std::string& plugin_config);
        int AUDIOHOSTLIB_EXPORT GetPluginParameters(const std::string& plugin_config);
        void AUDIOHOSTLIB_EXPORT Terminate();
        void AUDIOHOSTLIB_EXPORT SetVerbosity(uint8_t value);
        struct Buffers
        {
            int32_t numInputs;
            int32_t numOutputs;
            int32_t numSamples;
        };

    private:
        VST3::Hosting::Module::Ptr module {nullptr};
        std::unique_ptr<Steinberg::Vst::PlugProvider> plugProvider {nullptr};
        Steinberg::Vst::ProcessContext processContext;
        Steinberg::Vst::ParameterChanges inputParameterChanges;
        Steinberg::Vst::HostProcessData processData;
        uint8_t verbose_         = 0;
        std::string plugin_path_ = "";
};


extern "C" {

    __declspec(dllexport) AudioProcessingVstHost* CApiInitialize()
    {
        AudioProcessingVstHost* dc = new AudioProcessingVstHost();
        return dc;
    }

    __declspec(dllexport) int CApiSetVerbosity(AudioProcessingVstHost* vst_host,
                                               uint8_t value)
    {
        RETURN_ERROR_IF_NULL(vst_host);
        vst_host->SetVerbosity(value);
        return VST_ERROR_STATUS::SUCCESS;
    }

    __declspec(dllexport) int CApiCreatePluginInstance(AudioProcessingVstHost* vst_host,
                                                       const char* plugin_path)
    {
        RETURN_ERROR_IF_NULL(vst_host);
        RETURN_ERROR_IF_NULL(plugin_path);
        std::string plugin_path_srt(plugin_path);
        return vst_host->CreatePluginInstance(plugin_path_srt);
    }

    __declspec(dllexport) int CApiGetPluginParameters(AudioProcessingVstHost* vst_host,
                                                      const char* plugin_config)
    {
        RETURN_ERROR_IF_NULL(vst_host);
        RETURN_ERROR_IF_NULL(plugin_config);
        std::string plugin_config_srt(plugin_config);
        return vst_host->GetPluginParameters(plugin_config_srt);
    }

    __declspec(dllexport) int CApiSetPluginParameters(AudioProcessingVstHost* vst_host,
                                                      const char* plugin_config)
    {
        RETURN_ERROR_IF_NULL(vst_host);
        RETURN_ERROR_IF_NULL(plugin_config);
        std::string plugin_config_srt(plugin_config);
        return vst_host->SetPluginParameters(plugin_config_srt);
    }

    __declspec(dllexport) int CApiProcessWaveFileWithSinglePlugin(AudioProcessingVstHost* vst_host,
                                                                  const char* input_wave_path,
                                                                  const char* output_wave_path)
    {
        RETURN_ERROR_IF_NULL(vst_host);
        RETURN_ERROR_IF_NULL(input_wave_path);
        RETURN_ERROR_IF_NULL(output_wave_path);
        std::string input_wave_path_srt(input_wave_path);
        std::string output_wave_path_srt(output_wave_path);
        return vst_host->ProcessWaveFileWithSinglePlugin(input_wave_path_srt, output_wave_path_srt);
    }

    __declspec(dllexport) int CApiDeleteInstance(AudioProcessingVstHost* vst_host)
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
