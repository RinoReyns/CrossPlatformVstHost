#include "VstHostConfigGenerator.h"

VST_ERROR_STATUS VstHostConfigGenerator::DumpEmptyAppConfig(
    nlohmann::json plugin_config_json, 
    std::string config_path)
{
    AddParametersFromList(plugin_config_json, main_config_sections_list_);
    plugin_config_json_ = plugin_config_json;
    return JsonUtils::DumpJson(plugin_config_json, config_path);
}

VST_ERROR_STATUS VstHostConfigGenerator::DumpEmptyAppConfig(std::string config_path)
{
    nlohmann::json plugin_config_json;
    return DumpEmptyAppConfig(plugin_config_json, config_path);
}


VST_ERROR_STATUS VstHostConfigGenerator::ReadAndDumpAppConfig(std::string config_path)
{
    nlohmann::json plugin_config_json = ReadAppConfig(config_path);
    return DumpEmptyAppConfig(plugin_config_json, config_path);
}

nlohmann::json VstHostConfigGenerator::ReadAppConfig(std::string config_path)
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
        if (JsonUtils::CheckIfParamNotInDict(plugin_config_json, param_name))
        {
            plugin_config_json[param_name] = { };
            if (!JsonUtils::CheckIfParamNotInDict(sub_sections_params_, param_name))
            {
                plugin_config_json[param_name] = sub_sections_params_.at(param_name);
            }
        }
    }
    return VST_ERROR_STATUS::SUCCESS;
}
