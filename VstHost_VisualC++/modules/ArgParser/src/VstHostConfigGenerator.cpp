#include "VstHostConfigGenerator.h"

VST_ERROR_STATUS VstHostConfigGenerator::DumpEmptyVstHostConfig(
    nlohmann::json plugin_config_json, 
    std::string config_path)
{
    AddParametersFromList(plugin_config_json, single_params_list);
    AddParametersFromList(plugin_config_json, dict_params_list);
   
    if (JsonUtils::CheckIfParamInDict(plugin_config_json[VST_HOST_CONFIG_PARAM_STR], 
        PROCESSING_CONFIG_PARAM_STR))
    {
        plugin_config_json[VST_HOST_CONFIG_PARAM_STR][PROCESSING_CONFIG_PARAM_STR] = {};
        plugin_config_json[VST_HOST_CONFIG_PARAM_STR][PROCESSING_CONFIG_PARAM_STR]["plugin_1"] = {};
        plugin_config_json[VST_HOST_CONFIG_PARAM_STR][PROCESSING_CONFIG_PARAM_STR]["plugin_1"]["config"] = "";
        plugin_config_json[VST_HOST_CONFIG_PARAM_STR][PROCESSING_CONFIG_PARAM_STR]["plugin_1"]["plugin"] = "";
    }

    if (JsonUtils::CheckIfParamInDict(plugin_config_json["postprocessing"], "filter"))
    {
        plugin_config_json["postprocessing"]["filter"] = {};
        plugin_config_json["postprocessing"]["filter"]["enable"] = false;
    }

    if (JsonUtils::CheckIfParamInDict(plugin_config_json["preprocessing"], "filter"))
    {
        plugin_config_json["preprocessing"]["filter"] = {};
        plugin_config_json["preprocessing"]["filter"]["enable"] = false;
    }

    plugin_config_json_ = plugin_config_json;
    return JsonUtils::DumpJson(plugin_config_json, config_path);
}

VST_ERROR_STATUS VstHostConfigGenerator::DumpEmptyVstHostConfig(std::string config_path)
{
    nlohmann::json plugin_config_json;
    return DumpEmptyVstHostConfig(plugin_config_json, config_path);
}


VST_ERROR_STATUS VstHostConfigGenerator::ReadAndDumpVstHostConfig(std::string config_path)
{
    nlohmann::json plugin_config_json = ReadVstHostConfig(config_path);
    return DumpEmptyVstHostConfig(plugin_config_json, config_path);
}

nlohmann::json VstHostConfigGenerator::ReadVstHostConfig(std::string config_path)
{
    nlohmann::json plugin_config_json;
    RETURN_ERROR_IF_NOT_SUCCESS(JsonUtils::LoadJson(config_path, &plugin_config_json));
    return plugin_config_json;
}

nlohmann::json VstHostConfigGenerator::GetConfigDict()
{
    return plugin_config_json_;
}

VST_ERROR_STATUS VstHostConfigGenerator::AddParametersFromList(
    nlohmann::json &plugin_config_json,
    std::vector<std::string> params_list)
{
    for (auto param_name : params_list)
    {
        if (JsonUtils::CheckIfParamInDict(plugin_config_json, param_name))
        {
            plugin_config_json[param_name] = { };
        }
    }
    return VST_ERROR_STATUS::SUCCESS;
}
