#include <filesystem>

#include "arg_parser.h"
#include "JsonUtils.h"


ArgParser::ArgParser()
{
    arg_parser_.reset(new argparse::ArgumentParser("audiohost"));
    arg_parser_->add_argument("-vst_plugin")
        .help("vst plugin that will be used for processing");
    
    arg_parser_->add_argument("-processing_config")
        .help("Json file that defines plugins' order and configs for them.");
    
    arg_parser_->add_argument("-input_wave")
        .help("Input wave that will be processed");

    arg_parser_->add_argument("-output_wave_path")
        .help("Output wave that will be processed");

    arg_parser_->add_argument("-verbosity")
        .help("Possible values:\n"
              "\t\t\tERROR = 0\n"
              "\t\t\tINFO = 1\n"
              "\t\t\tDEBUG = 2\n")
        .scan<'i', int>()
        .default_value(0);

    arg_parser_->add_argument("-plugin_config")
        .help("Path to the json file with VST Plugin parameters that needs to be set or in which params will be dumped.");
    
    arg_parser_->add_argument("-dump_plugin_params")
        .default_value(false)
        .implicit_value(true)
        .help("Allows to dump parameters for given plugin.");
}

int ArgParser::CheckInputArgsFormat(std::vector<std::string> args)
{
    for (auto arg : args)
    {
        if (arg.find('=') != std::string::npos)
        {
            LOG(INFO) << "VST Host Tool doesn't accept parameter definition with '='. "
                         "Please pass parameters in convetion without '=' e.g. '-vst_plugin plugin.vst3'.";
            return VST_ERROR_STATUS::WRONG_PARAMETER_FORMAT;
        }
        std::cout << arg << std::endl;
    }
    return VST_ERROR_STATUS::SUCCESS;
}

int ArgParser::ParsParameters(std::vector<std::string> args)
{
    if (args.size() == 1)
    {
        auto program_name =  arg_parser_->GetProgramName();
        program_name	  = program_name.substr(program_name.find_last_of("/\\") + 1);
        LOG(INFO) << "App execuction with empty params is not allowed.";
        LOG(INFO) << "Run: " << program_name << ".exe -h ";
        return VST_ERROR_STATUS::VST_HOST_ERROR;
    }

    int status = CheckInputArgsFormat(args);
    RETURN_ERROR_IF_NOT_SUCCESS(status);

    try 
    {
        arg_parser_->parse_args(args);
    }
    catch (const std::runtime_error& err) 
    {
        LOG(ERROR) << err.what();
        return VST_ERROR_STATUS::VST_HOST_ERROR;
    }
    
    // TODO:
    // create private set for each parameter
    status = CheckPluginParams();
    RETURN_ERROR_IF_NOT_SUCCESS(status);

    // dump_plugin_params
    dump_plugin_params_ = arg_parser_->get<bool>("-dump_plugin_params");
    
    if (!dump_plugin_params_)
    {
        // input_wave_path_
        status = CheckIfPathExists(arg_parser_->get<std::string>("-input_wave"));
        RETURN_ERROR_IF_NOT_SUCCESS(status);

        input_wave_path_ = arg_parser_->get<std::string>("-input_wave");
        
        status = CheckOutputWave();
        RETURN_ERROR_IF_NOT_SUCCESS(status);

        if (!arg_parser_->present("-processing_config"))
        {
            status = CheckIfPathExists(arg_parser_->get<std::string>("-plugin_config"));
            if (status != VST_ERROR_STATUS::SUCCESS && dump_plugin_params_ == false)
            {
                return status;
            }
            plugin_config_ = arg_parser_->get<std::string>("-plugin_config");          
            std::map<std::string, std::string> plugin_params = { {PLUGINS_STRING, plugin_path_}, 
                                                                 {CONFIG_STRING, plugin_config_}
                                                               };
            processing_config_.insert(std::make_pair("plugin_1", plugin_params));
        }
    }
    else
    {
        plugin_config_ = arg_parser_->get<std::string>("-plugin_config");
        if (plugin_config_ == "")
        {
            LOG(ERROR) << "-plugin_config can't be empty.";
            return VST_ERROR_STATUS::JSON_CONFIG_ERROR;
        }

        plugin_config_ = arg_parser_->get<std::string>("-plugin_config");
        std::map<std::string, std::string> plugin_params = { {PLUGINS_STRING, plugin_path_},
                                                             {CONFIG_STRING, plugin_config_}
        };
        processing_config_.insert(std::make_pair("plugin_1", plugin_params));
    }

    // verbosity_
    if (arg_parser_->get<int>("-verbosity") > LogLevelType::MAX_LOG_LEVEL_VALUE)
    {
        LOG(ERROR) << "Unsupported value of -verbosity.";
        return VST_ERROR_STATUS::VST_HOST_ERROR;
    }
    verbosity_ = static_cast<uint8_t>(arg_parser_->get<int>("-verbosity"));
    
    return status;
}

int ArgParser::CheckIfPathExists(std::string path)
{
    int status = VST_ERROR_STATUS::SUCCESS;
    if (!std::filesystem::exists(path))
    {
        status = VST_ERROR_STATUS::PATH_NOT_EXISTS;
    }
    return status;
}

int ArgParser::CheckPluginParams()
{
    int status = VST_ERROR_STATUS::ARG_PARSER_ERROR;
    if (arg_parser_->present("-vst_plugin") && arg_parser_->present("-processing_config"))
    {
        LOG(ERROR) << "Unsupported configuration. You need to set either -processing_config or -vst_plugin parameter. You can't use both.";
        return VST_ERROR_STATUS::UNSUPPORTED_CONFIGURATION;
    }

    if (arg_parser_->present("-plugin_config") && arg_parser_->present("-processing_config"))
    {
        LOG(ERROR) << "Unsupported configuration. You need to set either -processing_config or -plugin_config parameter. You can't use both.";
        return VST_ERROR_STATUS::UNSUPPORTED_CONFIGURATION;
    }

    if (arg_parser_->present("-vst_plugin")) 
    {
        status = CheckIfPathExists(arg_parser_->get<std::string>("-vst_plugin"));
        if (status == VST_ERROR_STATUS::SUCCESS)
        {
            plugin_path_ = arg_parser_->get<std::string>("-vst_plugin");
        }
    }

    if (status != VST_ERROR_STATUS::SUCCESS && arg_parser_->present("-processing_config"))
    {
        status = CheckIfPathExists(arg_parser_->get<std::string>("-processing_config"));
        if (status == VST_ERROR_STATUS::SUCCESS)
        {
            status = JsonUtils::JsonFileToMap(arg_parser_->get<std::string>("-processing_config"),
                                              &processing_config_,
                                              expected_keys_in_json_);
            if (status == VST_ERROR_STATUS::MISSING_ID)
            {
                const char* const delim = ", ";
                std::ostringstream imploded;
                std::copy(expected_keys_in_json_.begin(), expected_keys_in_json_.end(),
                    std::ostream_iterator<std::string>(imploded, delim));
                LOG(ERROR) << "Missing ID in: '" << arg_parser_->get<std::string>("-processing_config") << 
                            "'. Please make sure that each param has following values: " << imploded.str();
            }
            return status;
        }
    }

    if (status == VST_ERROR_STATUS::SUCCESS && arg_parser_->present("-processing_config"))
    {
        status = VST_ERROR_STATUS::ARG_PARSER_ERROR;
        LOG(ERROR) << "Empty parameters. You need to set either -processing_config or -vst_plugin parameters.";
    }
    
    return status;
}

int ArgParser::CheckOutputWave()
{
    if (!arg_parser_->present("-output_wave_path"))
    {
        LOG(ERROR) << "-output_wave_path can't be empty.";
        return VST_ERROR_STATUS::EMPTY_ARG;
    }

    output_wave_path_ = arg_parser_->get<std::string>("-output_wave_path");
    if (output_wave_path_.empty())
    {
        LOG(ERROR) << "-output_wave_path can't be empty.";
        return VST_ERROR_STATUS::EMPTY_ARG;
    }
    return VST_ERROR_STATUS::SUCCESS;
}

std::string ArgParser::GetPluginPath()
{
    return plugin_path_;
}

std::string ArgParser::GetInputWavePath()
{
    return input_wave_path_;
}

std::string ArgParser::GetOutputWavePath()
{
    return output_wave_path_;
}

uint8_t ArgParser::GetPluginVerbosity()
{
    return verbosity_;
}

std::string ArgParser::GetPluginConfig()
{
    return plugin_config_;
}

bool ArgParser::GetDumpPluginParams()
{
    return dump_plugin_params_;
}

config_type ArgParser::GetProcessingConfig()
{
    return processing_config_;
}
