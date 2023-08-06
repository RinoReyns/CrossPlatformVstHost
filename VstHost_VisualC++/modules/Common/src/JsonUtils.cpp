#include <fstream>
#include <iostream>

#include "JsonUtils.h"
#include "VstHostMacro.h"

VST_ERROR_STATUS JsonUtils::DumpJson(nlohmann::json json_config,
                                     std::string path_to_save)
{
    std::ofstream file(path_to_save);
    if (!file.is_open())
    {
        return VST_ERROR_STATUS::OPEN_FILE_ERROR;
    }

    file << std::setw(4) << json_config << std::endl;
    file.close();

    return VST_ERROR_STATUS::SUCCESS;
}

VST_ERROR_STATUS JsonUtils::LoadJson(std::string plugin_config_path,
                                     nlohmann::json* json_config)
{
    RETURN_ERROR_IF_NULL(json_config);
    std::ifstream file(plugin_config_path);
    if (!file.is_open())
    {
        return VST_ERROR_STATUS::OPEN_FILE_ERROR;
    }

    file >> *json_config;
    file.close();
    return VST_ERROR_STATUS::SUCCESS;
}

VST_ERROR_STATUS JsonUtils::JsonFileToMap(
    std::string plugin_config_path,
    config_type *params_map,
    std::vector<std::string> expected_ids)
{
    nlohmann::json plugin_config_json;
    VST_ERROR_STATUS status = JsonUtils::LoadJson(plugin_config_path, &plugin_config_json);
    RETURN_ERROR_IF_NOT_SUCCESS(status);
    for (auto& [plugin_id, plugin_values] : plugin_config_json.items()) 
    {
        std::map<std::string, std::string> single_plugin_params;
        // map params
        for (auto& single_param : plugin_values.items()) 
        {
            single_plugin_params.insert(std::make_pair(single_param.key(), single_param.value()));
        }
        
        // check if map has expected values
        for (auto& expected_id : expected_ids)
        {
            if (single_plugin_params.find(expected_id) == single_plugin_params.end())
            {
                return VST_ERROR_STATUS::MISSING_ID;
            }
        }
        params_map->insert(std::make_pair(plugin_id, single_plugin_params));
    }
    return VST_ERROR_STATUS::SUCCESS;
}

bool JsonUtils::CheckIfParamInDict(nlohmann::json dict, std::string parameter)
{
    return dict.find(parameter) == dict.end();
}
