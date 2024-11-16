#ifndef ARG_PARSER_H
#define ARG_PARSER_H

#include "argparser.hpp"
#include "easylogging++.h"
#include "enums.h"
#include "JsonUtils.h"

class ArgParser 
{
    public:
        explicit ArgParser();
        ~ArgParser() = default;
        int ParsParameters(std::vector<std::string>);
        std::string GetInputWavePath();
        uint8_t GetPluginVerbosity();
        std::string GetOutputWavePath();
        bool GetDumpPluginParams();
        nlohmann::json GetProcessingConfig();
        bool GetEnableAudioEndpoint();
        bool GetDumpToolConfigParam();

    private:
        int CheckIfPathExists(std::string);
        int CheckOutputWave();
        int CheckInputArgsFormat(std::vector<std::string> args);
        int ValidateVstHostConfigParam();
        int DumpVstHostConfig();
        int ValidateSamplingRate(float sampling_rate);
        
        std::unique_ptr<argparse::ArgumentParser> arg_parser_;
        std::string input_wave_path_    = "";
        std::string output_wave_path_   = "";
        std::string vst_host_config_    = "";
        uint8_t verbosity_              = 0;
        bool dump_plugin_params_        = false;
        bool dump_tool_config_          = false;
        bool enable_audio_capture_      = false;

        nlohmann::json main_config_;

        std::vector<std::string> expected_keys_in_json_ = {PLUGINS_STRING, CONFIG_STRING };
        config_type processing_config_;
};

#endif //ARG_PARSER_H
