#ifndef VST_HOST_CONFIG_GENERATOR_H
#define VST_HOST_CONFIG_GENERATOR_H

#include "easylogging++.h"
#include "enums.h"
#include "JsonUtils.h"

class VstHostConfigGenerator
{
public:

    explicit VstHostConfigGenerator() = default;
    ~VstHostConfigGenerator() = default;

    VST_ERROR_STATUS DumpEmptyAppConfig(std::string config_path);
    VST_ERROR_STATUS DumpEmptyAppConfig(nlohmann::json plugin_config_json, std::string config_path);
    VST_ERROR_STATUS ReadAndDumpAppConfig(std::string config_path);
    nlohmann::json ReadAppConfig(std::string config_path);
    nlohmann::json GetConfigDict();
    // TODO:
    // validate config
private:
    VST_ERROR_STATUS AddParametersFromList(nlohmann::json &plugin_config_json, 
        std::vector<std::string> params_list);

    nlohmann::json plugin_config_json_;

    const std::vector<std::string> main_config_sections_list_
    {
        "input_wave",
        "output_wave",
        "preprocessing",
        "vst_host",
        "postprocessing"
    };

    const nlohmann::json sub_sections_params_
    {
        {"preprocessing", 
            {
                {"filter", 
                    {
                        {"enable", false}
                    }
                }
            }
        },

        {"vst_host", 
            {
                {"enable", false},
                {"processing_config", 
                    {
                        {"plugin_1", 
                            { 
                                {"config", ""},
                                {"plugin", ""}
                            }
                        },
                    }
                }
            }
        },

        {"postprocessing", 
            {
                {"filter",
                    {
                        {"enable", false}
                    }
                }
            }
        },
    };
};

#endif //VST_HOST_CONFIG_GENERATOR_H
