#include <filesystem>

#include "arg_parser.h"
#include "JsonUtils.h"
#include "VstHostMacro.h"
#include "VstHostConfigGenerator.h"

#define DUMP_CMD_PARAM_STR "-dump_vst_host_config"
#define VST_HOST_CMD_PARAM_STR "-vst_host_config"
#define DUMP_PLUGINS_CONFIGS "-dump_plugins_config"

ArgParser::ArgParser()
{
    arg_parser_.reset(new argparse::ArgumentParser("audiohost"));

    arg_parser_->add_argument("-verbosity")
        .help("Possible values:\n"
            "\t\t\tERROR = 0\n"
            "\t\t\tINFO = 1\n"
            "\t\t\tDEBUG = 2\n")
        .scan<'i', int>()
        .default_value(0);

    arg_parser_->add_argument(VST_HOST_CMD_PARAM_STR)
        .help("path to the application config run.");

    arg_parser_->add_argument(DUMP_CMD_PARAM_STR)
        .help("allows to dump empty configuration file for VST Host Application that needs to be fill with parameters.\nThis parameter needes to be commbined with " + std::string(VST_HOST_CMD_PARAM_STR) + ".")
        .implicit_value(true)
        .default_value(false);

    arg_parser_->add_argument(DUMP_PLUGINS_CONFIGS)
        .help("allows to dump empty configuration for all plugins defined in VST Host Application config.\nThis parameter needes to be commbined with " + std::string(VST_HOST_CMD_PARAM_STR) + ".")
        .implicit_value(true)
        .default_value(false);

    arg_parser_->add_argument("-enable_audio_capture")
        .default_value(false)
        .implicit_value(true);
}

int ArgParser::CheckInputArgsFormat(std::vector<std::string> args)
{
    for (auto arg : args)
    {
        if (arg.find('=') != std::string::npos)
        {
            LOG(INFO) << "VST Host Tool doesn't accept parameter definition with '='. "
                "Please pass parameters in convetion without '=' e.g. '-vst_host_config config.json'.";
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
        auto program_name = arg_parser_->GetProgramName();
        program_name = program_name.substr(program_name.find_last_of("/\\") + 1);
        LOG(INFO) << "App execuction with empty params is not allowed.";
        LOG(INFO) << "Run: " << program_name << ".exe -h ";
        return VST_ERROR_STATUS::VST_HOST_ERROR;
    }

    RETURN_ERROR_IF_NOT_SUCCESS(CheckInputArgsFormat(args));

    try
    {
        arg_parser_->parse_args(args);
    }
    catch (const std::runtime_error& err)
    {
        LOG(ERROR) << err.what();
        return VST_ERROR_STATUS::VST_HOST_ERROR;
    }
    
    int status = VST_ERROR_STATUS::SUCCESS;
    dump_tool_config_ = arg_parser_->get<bool>(DUMP_CMD_PARAM_STR);
    dump_plugin_params_ = arg_parser_->get<bool>(DUMP_PLUGINS_CONFIGS);
    enable_audio_capture_ = arg_parser_->get<bool>("-enable_audio_capture");

    if (dump_tool_config_)
    {
        status = this->DumpVstHostConfig();
        if (status == VST_ERROR_STATUS::SUCCESS)
        {
            LOG(INFO) << "VST Host config has been dump successfully in location: " << std::filesystem::absolute(vst_host_config_);
        }
        return status;
    }
    else if (dump_plugin_params_)
    {
        status = this->DumpVstHostConfig();
        RETURN_ERROR_IF_NOT_SUCCESS(status);

        for(nlohmann::json params : main_config_["vst_host"]["processing_config"])
        {
            for(auto single_param : params.items())
            {
                if (single_param.value() == "")
                { 
                    LOG(INFO) << "Empty value for following parameter in VST Host Config: " << single_param.key();
                    return VST_ERROR_STATUS::EMPTY_ARG;
                }
            }
        }
        return status;
    }
    else if (!enable_audio_capture_)
    {
        status = this->ValidateVstHostConfigParam();
        
        std::unique_ptr<VstHostConfigGenerator> config_generator(new VstHostConfigGenerator());
        main_config_ = config_generator->ReadVstHostConfig(vst_host_config_);

        // input_wave_path_
        status = CheckIfPathExists(main_config_["input_wave"]);
        RETURN_ERROR_IF_NOT_SUCCESS(status);
        input_wave_path_ = main_config_["input_wave"];

        status = CheckOutputWave();
        RETURN_ERROR_IF_NOT_SUCCESS(status);
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

int ArgParser::ValidateVstHostConfigParam()
{
     if (!arg_parser_->present(VST_HOST_CMD_PARAM_STR))
     {
         LOG(ERROR) << VST_HOST_CMD_PARAM_STR + std::string(" can't be empty.");
         return VST_ERROR_STATUS::MISSING_PARAMETER_VALUE;
     }

     RETURN_ERROR_IF_NOT_SUCCESS(CheckIfPathExists(arg_parser_->get<std::string>(VST_HOST_CMD_PARAM_STR)));

     vst_host_config_ = arg_parser_->get<std::string>(VST_HOST_CMD_PARAM_STR);
     return VST_ERROR_STATUS::SUCCESS;
}

int ArgParser::DumpVstHostConfig()
{
    int status = this->ValidateVstHostConfigParam();
    if (status == VST_ERROR_STATUS::MISSING_PARAMETER_VALUE)
    {
        return status;
    }

    vst_host_config_ = arg_parser_->get<std::string>(VST_HOST_CMD_PARAM_STR);
    std::unique_ptr<VstHostConfigGenerator> config_generator(new VstHostConfigGenerator());
    if (status != VST_ERROR_STATUS::SUCCESS)
    {
        return config_generator->DumpEmptyVstHostConfig(vst_host_config_);
    }
 
    status = config_generator->ReadAndDumpVstHostConfig(vst_host_config_);
    RETURN_ERROR_IF_NOT_SUCCESS(status);
    
    main_config_ = config_generator->GetConfigDict();
    return status;
}

int ArgParser::CheckOutputWave()
{
    try
    {
        output_wave_path_ = main_config_["output_wave"];
    }
    catch (...)
    {
        LOG(ERROR) << "-output_wave_path can't be empty.";
        return VST_ERROR_STATUS::EMPTY_ARG;
    }   

    if (output_wave_path_.empty())
    {
        LOG(ERROR) << "-output_wave_path can't be empty.";
        return VST_ERROR_STATUS::EMPTY_ARG;
    }

    return VST_ERROR_STATUS::SUCCESS;
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

bool ArgParser::GetDumpPluginParams()
{
    return dump_plugin_params_;
}

config_type ArgParser::GetProcessingConfig()
{
    return main_config_[VST_HOST_CONFIG_PARAM_STR][PROCESSING_CONFIG_PARAM_STR];
}

bool ArgParser::GetEnableAudioEndpoint()
{
    return enable_audio_capture_;
}

bool ArgParser::GetDumpToolConfigParam()
{
    return dump_tool_config_;
}
