#ifndef ARG_PARSER_H
#define ARG_PARSER_H

#include "argparser.hpp"
#include "easylogging++.h"

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

private:
    int CheckIfPathExists(std::string);
    int CheckPluginParams();
    int CheckOutputWave();
        
    std::unique_ptr<argparse::ArgumentParser> arg_parser_;
    std::string plugin_path_        = "";
    std::string input_wave_path_    = "";
    std::string output_wave_path_   = "";
    std::string plugin_config_      = "";
    std::string processing_config_  = "";
    uint8_t verbosity_              = 0;
    bool dump_plugin_params_        =  false;
};

#endif //ARG_PARSER_H