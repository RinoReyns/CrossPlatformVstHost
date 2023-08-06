#ifndef VST_HOST_CONFIG_GENERATOR_H
#define VST_HOST_CONFIG_GENERATOR_H

#define PROCESSING_CONFIG_PARAM_STR "processing_config"
#define VST_HOST_CONFIG_PARAM_STR "vst_host"

#include "easylogging++.h"
#include "enums.h"
#include "JsonUtils.h"

class VstHostConfigGenerator
{
public:

    explicit VstHostConfigGenerator() = default;
    ~VstHostConfigGenerator() = default;

    VST_ERROR_STATUS DumpEmptyVstHostConfig(std::string config_path);
    VST_ERROR_STATUS DumpEmptyVstHostConfig(nlohmann::json plugin_config_json, std::string config_path);
    VST_ERROR_STATUS ReadAndDumpVstHostConfig(std::string config_path);
    nlohmann::json ReadVstHostConfig(std::string config_path);
    nlohmann::json GetConfigDict();
    // TODO:
    // validate config
private:
    VST_ERROR_STATUS AddParametersFromList(nlohmann::json &plugin_config_json, 
        std::vector<std::string> params_list);

    nlohmann::json plugin_config_json_;
    std::vector<std::string> single_params_list 
    { 
        "input_wave",
        "output_wave",
    };

    std::vector<std::string> dict_params_list
    {
        "preprocessing",
        "vst_host",
        "postprocessing"
    };

};

#endif //VST_HOST_CONFIG_GENERATOR_H
