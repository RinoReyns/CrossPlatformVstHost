#ifndef ENUMS_H
#define ENUMS_H

#define RETURN_ERROR_IF_NULL(value) {if(!value) return VST_ERROR_STATUS::NULL_POINTER;}
#define RETURN_ERROR_IF_NOT_SUCCESS(value) {if(value != VST_ERROR_STATUS::SUCCESS) return value;}

enum VST_ERROR_STATUS
{
    SUCCESS                             = 0,
    VST_HOST_ERROR                      = 1,
    PATH_NOT_EXISTS                     = 2,
    OPEN_FILE_ERROR                     = 3,
    CREATE_HOSTING_MODULE_ERROR         = 4,
    CREATE_PLUGIN_PROVIDER_ERROR        = 5,
    CREATE_EDIT_CONTROLER_ERROR         = 6,
    READ_WRITE_ERROR                    = 7,
    CREATE_PROCESSING_INSTANCE_ERROR    = 8,
    PLUGIN_PROCESSING_FAILED            = 9,
    JSON_CONFIG_ERROR                   = 10,
    NULL_POINTER                        = 11,
    ARG_PARSER_ERROR                    = 12,
    EMPTY_ARG                           = 13,
    UNSUPPORTED_CONFIGURATION           = 14,
    MISSING_ID                          = 15,

    MAX_STATUS_VALUE                    = MISSING_ID
};

namespace LogLevelType
{
    enum LOG_LEVEL
    {
        ERROR_LOG           = 0,
        INFO                = 1,
        DEBUG               = 2,

        MAX_LOG_LEVEL_VALUE = DEBUG
    };
}
#endif // ENUMS_H
