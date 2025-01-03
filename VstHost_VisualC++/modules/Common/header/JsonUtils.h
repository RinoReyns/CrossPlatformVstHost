#ifndef JSON_UTILS_H
#define JSON_UTILS_H

#include "enums.h"
#include "json.hpp"

class JsonUtils
{
    public:
        static VST_ERROR_STATUS DumpJson(nlohmann::json json_config, std::string path_to_save);
        static VST_ERROR_STATUS LoadJson(std::string plugin_config_path, nlohmann::json* json_config);
        static VST_ERROR_STATUS JsonFileToMap(std::string plugin_config_path, 
                                              config_type* prams_map,
                                              std::vector<std::string> expected_ids);
        static bool CheckIfParamNotInDict(nlohmann::json dict, std::string parameter);
};

#endif //JSON_UTILS_H
