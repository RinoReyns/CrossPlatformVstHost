#ifndef ENUMS_H
#define ENUMS_H

#include <map>
#include <string>
#include "VstHostMacro.h"

#define PROCESSING_CONFIG_PARAM_STR "processing_config"
#define PREPROCESSING "preprocessing"
#define POSTPROCESSING "postprocessing"
#define VST_HOST_CONFIG_PARAM_STR "vst_host"
#define ENABLE_STRING "enable"
#define SAMPLING_RATE_PARAM_STR "sampling_rate"

constexpr auto PLUGINS_STRING = "plugin";
constexpr auto CONFIG_STRING = "config";

typedef std::map<std::string, std::map<std::string, std::string>> config_type;

enum VST_ERROR_STATUS
{
    SUCCESS = 0,
    VST_HOST_ERROR = 1,
    PATH_NOT_EXISTS = 2,
    OPEN_FILE_ERROR = 3,
    CREATE_HOSTING_MODULE_ERROR = 4,
    CREATE_PLUGIN_PROVIDER_ERROR = 5,
    CREATE_EDIT_CONTROLER_ERROR = 6,
    READ_WRITE_ERROR = 7,
    CREATE_PROCESSING_INSTANCE_ERROR = 8,
    PLUGIN_PROCESSING_FAILED = 9,
    JSON_CONFIG_ERROR = 10,
    NULL_POINTER = 11,
    ARG_PARSER_ERROR = 12,
    EMPTY_ARG = 13,
    UNSUPPORTED_CONFIGURATION = 14,
    MISSING_ID = 15,
    INSTANCE_ALREADY_EXISTS = 16,
    NO_PLUGIN_INITIALIZED = 17,
    NO_ELEMENT_FOUND = 18,
    WRONG_PARAMETER_FORMAT = 19,
    MISSING_CONFIG_FOR_PLUGIN = 20,
    AUDIO_CAPTURE_ERROR = 21,
    END_OF_FILE = 22,
    AUDIO_ENDPOINT_MANAGER_ERROR = 23,
    AUDIO_RENDER_ERROR = 24,
    ENPOINTS_SAMPLING_RATE_MISS_MATCH = 25,
    EMPTY_LIST_OF_ENPOINTS = 26,
    MISSING_PARAMETER_VALUE = 27,
    NOT_IMPLEMENTED = 28,
    NO_AUDIO_DEVICES_ID_FOUND = 29,
    BYPASS = 30,
    UNSUPPORTED_SAMPLING_RATE = 31,
    SIZE_MISSMATCH = 32,

    MAX_STATUS_VALUE = SIZE_MISSMATCH
};

namespace LogLevelType
{
    enum LOG_LEVEL
    {
        ERROR_LOG = 0,
        INFO = 1,
        DEBUG = 2,

        MAX_LOG_LEVEL_VALUE = DEBUG
    };
}

enum VST_VERSION_INDEXES
{
    MAJOR = 0,
    MINOR = 1,
    SUB = 2
};

#endif // ENUMS_H
