#include <filesystem>

#include "arg_parser.h"
#include "enums.h"

ArgParser::ArgParser()
{
    // MIT arg parser https://github.com/p-ranav/argparse
    arg_parser_.reset(new argparse::ArgumentParser("audiohost"));
    arg_parser_->add_argument("-vst_plugin")
        .required()
        .help("vst plugin that will be used for processing");

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
        .required()
        .help("Path to the json file with VST Plugin parameters that needs to be set or in which params will be dumped.");
    
    arg_parser_->add_argument("-dump_plugin_params")
        .default_value(false)
        .implicit_value(true)
        .help("Allows to dump parameters for given plugin.");
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

    // plugin_path_
    int status = CheckIfPathExists(arg_parser_->get<std::string>("-vst_plugin"));
    if (status != VST_ERROR_STATUS::SUCCESS)
    {
        return status;
    }
    plugin_path_ = arg_parser_->get<std::string>("-vst_plugin");

    // dump_pugin_params
    dump_plugin_params_ = arg_parser_->get<bool>("-dump_plugin_params");
    
    if (!dump_plugin_params_)
    {
        // input_wave_path_
        status = CheckIfPathExists(arg_parser_->get<std::string>("-input_wave"));
        if (status != VST_ERROR_STATUS::SUCCESS)
        {
            return status;
        }

        input_wave_path_ = arg_parser_->get<std::string>("-input_wave");
        
        try
        {
            output_wave_path_ = arg_parser_->get<std::string>("-output_wave_path");
            if (output_wave_path_.empty())
            {
                LOG(ERROR) << "-output_wave_path can't be empty.";
                return VST_ERROR_STATUS::EMPTY_ARG;
            }
        }
        catch (const std::exception&)
        {
            LOG(ERROR) << "-output_wave_path can't be empty.";
            return VST_ERROR_STATUS::EMPTY_ARG;
        }

        status = CheckIfPathExists(arg_parser_->get<std::string>("-plugin_config"));
        if (status != VST_ERROR_STATUS::SUCCESS && dump_plugin_params_ == false)
        {
            return status;
        }
        plugin_config_ = arg_parser_->get<std::string>("-plugin_config");
    }
    else
    {
        plugin_config_ = arg_parser_->get<std::string>("-plugin_config");
        if (plugin_config_ == "")
        {
            LOG(ERROR) << "-plugin_config can't be empty.";
            return VST_ERROR_STATUS::JSON_CONFIG_ERROR;
        }
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
