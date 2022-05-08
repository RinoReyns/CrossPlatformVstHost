#ifndef ARG_PARSER_H
#define ARG_PARSER_H

#include "argparser.hpp"
#include "easylogging++.h"
#include "enums.h"

class ArgParser 
{
    public:
        explicit ArgParser();
        ~ArgParser() = default;
        int ParsParameters(std::vector<std::string>);
        std::string GetPluginPath();
        std::string GetInputWavePath();
        uint8_t GetPluginVerbosity();
        std::string GetOutputWavePath();
        std::string GetPluginConfig();
        bool GetDumpPluginParams();
        config_type GetProcessingConfig();

    private:
        int CheckIfPathExists(std::string);
        int CheckPluginParams();
        int CheckOutputWave();
        
        std::unique_ptr<argparse::ArgumentParser> arg_parser_;
        std::string plugin_path_        = "";
        std::string input_wave_path_    = "";
        std::string output_wave_path_   = "";
        std::string plugin_config_      = "";
        uint8_t verbosity_              = 0;
        bool dump_plugin_params_        =  false;

        std::vector<std::string> expected_keys_in_json_ = {PLUGINS_STRING, CONFIG_STRING };
        config_type processing_config_;
};

#endif //ARG_PARSER_H
