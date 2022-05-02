#include "JsonUtils.h"

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
    std::ifstream file(plugin_config_path);
    if (!file.is_open())
    {
        return VST_ERROR_STATUS::OPEN_FILE_ERROR;
    }

    file >> *json_config;
    file.close();
    return VST_ERROR_STATUS::SUCCESS;
}
