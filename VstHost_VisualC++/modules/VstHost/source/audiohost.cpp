#include <cstdio>
#include <iostream>

#include "audiohost.h"
#include "stringconvert.h"
#include "JsonUtils.h"
#include "file.h"
#include "arg_parser.h"
#include "Utils.h"

INITIALIZE_EASYLOGGINGPP

AUDIOHOSTLIB_EXPORT AudioProcessingVstHost::~AudioProcessingVstHost()
{
    this->Terminate();
}

int AUDIOHOSTLIB_EXPORT AudioProcessingVstHost::SetMutliplePluginParameters(const config_type processing_config)
{
    int status = VST_ERROR_STATUS::VST_HOST_ERROR;
    for (auto& [key, value] : processing_config)
    {
        auto single_plugin_params = value.find(CONFIG_STRING);
        if (single_plugin_params == value.end())
        {
            LOG(ERROR) << "Missing id '" << CONFIG_STRING << "' in plugin config.";
            return VST_ERROR_STATUS::MISSING_ID;
        }

        if (!std::filesystem::exists(single_plugin_params->second))
        {
            LOG(WARNING) << "Missing config for plugin '" << key << "'";
        }
        else
        {
            status = this->SetPluginParameters(key, single_plugin_params->second);
            RETURN_ERROR_IF_NOT_SUCCESS(status);
        }
    }

    return status;
}

AUDIOHOSTLIB_EXPORT int AudioProcessingVstHost::SetPluginParameters(std::string plugin_id, const std::string& plugin_config)
{
    if (!vst_plugins_[plugin_id].plug_provider_)
    {
        LOG(ERROR) << "Plugin instance was not created.";
        return VST_ERROR_STATUS::NULL_POINTER;
    }

    Steinberg::OPtr<Steinberg::Vst::IEditController> editController = vst_plugins_[plugin_id].plug_provider_->getController();

    if (!editController)
    {
        LOG(ERROR) << "No EditController found (needed for allowing editor to edit params)";
        return VST_ERROR_STATUS::CREATE_EDIT_CONTROLER_ERROR;
    }
    
    nlohmann::json plugin_config_json;
    VST_ERROR_STATUS status = VST_ERROR_STATUS::SUCCESS;
    
    status = JsonUtils::LoadJson(plugin_config, &plugin_config_json);
    if (status != VST_ERROR_STATUS::SUCCESS)
    {
        LOG(ERROR) << "Loding json file with plugin params failed." << std::endl;
        return status;
    }
    
    Steinberg::Vst::ParameterInfo info;
    Steinberg::Vst::ParamValue value;
    Steinberg::int32 index = 0;

    for (int32_t i = 0; i < editController->getParameterCount(); i++)
    {
        editController->getParameterInfo(i, info);
        std::string param_name = VST3::StringConvert::convert(info.title);
        auto single_param_map = plugin_config_json.find(param_name);
        if (single_param_map != plugin_config_json.end())
        {
            editController->setParamNormalized(info.id, *single_param_map);
            value = editController->getParamNormalized(info.id);
            LOG_IF(verbose_ > LogLevelType::LOG_LEVEL::ERROR_LOG, INFO) << "Value for param " << 
                   param_name << " has been set to " << value;
            Steinberg::Vst::IParamValueQueue* queue = inputParameterChanges.addParameterData(info.id, index);
            if (queue)
            {
                queue->addPoint(0, value, index);
            }
        }
        else
        {
            LOG(WARNING) << "Parameter " << param_name << 
                            " has not been found in json and it value will not be changed. (" << value << ")";
        }
    }
    return status;
}

int AUDIOHOSTLIB_EXPORT AudioProcessingVstHost::GetMutliplePluginParameters(const config_type processing_config)
{
    int status = VST_ERROR_STATUS::VST_HOST_ERROR;;
    for (auto& [key, value] : processing_config)
    {
        auto single_plugin_params = value.find(CONFIG_STRING);
        if (single_plugin_params == value.end())
        {
            LOG(ERROR) << "Missing id '" << CONFIG_STRING << "' in plugin config.";
            return VST_ERROR_STATUS::MISSING_ID;
        }
        status = this->GetPluginParameters(key, single_plugin_params->second);
        RETURN_ERROR_IF_NOT_SUCCESS(status);
        vst_plugins_[key].plugin_path_   = processing_config.at(key).at(PLUGINS_STRING);
        vst_plugins_[key].plugin_config_ = processing_config.at(key).at(CONFIG_STRING);
    }

    return status;
}

AUDIOHOSTLIB_EXPORT int AudioProcessingVstHost::GetPluginParameters(std::string plugin_id, const std::string& plugin_config)
{
    if (!vst_plugins_[plugin_id].plug_provider_)
    {
        LOG(ERROR) << "Plugin instance was not created.";
        return VST_ERROR_STATUS::NULL_POINTER;
    }
    Steinberg::OPtr<Steinberg::Vst::IEditController> editController = vst_plugins_[plugin_id].plug_provider_->getController();

    if (!editController)
    {
        LOG(ERROR) << "No EditController found (needed for allowing editor) in file";
        return VST_ERROR_STATUS::CREATE_EDIT_CONTROLER_ERROR;
    }

    Steinberg::Vst::ParameterInfo info;
    nlohmann::json plugin_config_json;

    for (int32_t i = 0; i < editController->getParameterCount(); i++)
    {
        editController->getParameterInfo(i, info);
        std::string param_name = VST3::StringConvert::convert(info.title);
        plugin_config_json[param_name] = editController->getParamNormalized(info.id);
   }

    return JsonUtils::DumpJson(plugin_config_json, plugin_config);
}

static void assignBusBuffers(const AudioProcessingVstHost::Buffers& buffers, 
                             Steinberg::Vst::HostProcessData& processData,
                             float* input, 
                             float* output, 
                             bool unassign = false)
{
    // Set outputs
    auto bufferIndex   = 0;
    for (auto busIndex = 0; busIndex < processData.numOutputs; busIndex++)
    {
        auto channelCount = processData.outputs[busIndex].numChannels;
        for (auto chanIndex = 0; chanIndex < channelCount; chanIndex++)
        {
            if (bufferIndex < buffers.numOutputs)
            {
                processData.setChannelBuffer(Steinberg::Vst::BusDirections::kOutput, 
                                             busIndex, 
                                             chanIndex,
                                             unassign ? nullptr : &output[bufferIndex]);
                bufferIndex ++;
            }
        }
    }

    // Set inputs
    bufferIndex = 0;
    for (auto busIndex = 0; busIndex < processData.numInputs; busIndex++)
    {
        auto channelCount = processData.inputs[busIndex].numChannels;
        for (auto chanIndex = 0; chanIndex < channelCount; chanIndex++)
        {
            if (bufferIndex < buffers.numInputs)
            {
                processData.setChannelBuffer(Steinberg::Vst::BusDirections::kInput, 
                                             busIndex, 
                                             chanIndex,
                                             unassign ? nullptr : &input[bufferIndex]);
                bufferIndex ++;
            }
        }
    }
}

int AUDIOHOSTLIB_EXPORT AudioProcessingVstHost::CreateMutliplePluginInstance(const config_type processing_config)
{
    int status = VST_ERROR_STATUS::VST_HOST_ERROR;
    for (auto& [key, value] : processing_config)
    { 
        auto single_plugin_params = value.find(PLUGINS_STRING);
        if (single_plugin_params == value.end())
        {
            LOG(ERROR) << "Missing id '" << PLUGINS_STRING << "' in plugin config.";
            return VST_ERROR_STATUS::MISSING_ID;
        }
        status = this->CreatePluginInstance(single_plugin_params->second, key);
        RETURN_ERROR_IF_NOT_SUCCESS(status);
        vst_plugins_[key].plugin_config_ = processing_config.at(key).at(CONFIG_STRING);
    }
    return status;
}

int AUDIOHOSTLIB_EXPORT AudioProcessingVstHost::CreatePluginInstance(const std::string& plugin_path,
                                                                     const std::string& plugin_id)
{
    VST3::Optional<VST3::UID> uid;
    return CreatePluginInstance(plugin_path, plugin_id, std::move(uid));
}

AUDIOHOSTLIB_EXPORT int AudioProcessingVstHost::CreatePluginInstance(const std::string& plugin_path,
                                                                     const std::string& plugin_id,
                                                                     VST3::Optional<VST3::UID> effectID)
{
    std::string error;
    LOG(INFO) << "Loading plugin " + plugin_path + " in progress...";

    if ((vst_plugins_.find(plugin_id) != vst_plugins_.end()) && (vst_plugins_[plugin_id].module_ != nullptr))
    {
        return VST_ERROR_STATUS::INSTANCE_ALREADY_EXISTS;
    }

    if (vst_plugins_.find(plugin_id) == vst_plugins_.end())
    {
        vst_plugins_[plugin_id].plugin_path_ = plugin_path;
    }

    vst_plugins_[plugin_id].module_ = VST3::Hosting::Module::create(plugin_path, error);
    if (vst_plugins_[plugin_id].module_ == nullptr)
    {
        LOG(ERROR) << "Couldn't create Module for file:" + plugin_path + "\nError: " + error;
        return VST_ERROR_STATUS::CREATE_HOSTING_MODULE_ERROR;
    }

    auto factory = vst_plugins_[plugin_id].module_->getFactory();
    for (auto& classInfo : factory.classInfos())
    {
        // TODO:
        // if delay use info about that and test for different delays
        int status = this->CheckVstSdkCompatibility(classInfo.sdkVersion());
        RETURN_ERROR_IF_NOT_SUCCESS(status);
       
        if (classInfo.category() == kVstAudioEffectClass)
        {
            if (effectID)
            {
                if (*effectID != classInfo.ID())
                {
                    continue;
                }
            }
            vst_plugins_[plugin_id].plug_provider_ = std::unique_ptr<Steinberg::Vst::PlugProvider>(new Steinberg::Vst::PlugProvider(factory, classInfo, true));
        }
    }

    LOG(INFO) << "Plugin " + plugin_path + " loadded sucessfully.";

    return VST_ERROR_STATUS::SUCCESS;
}

int AudioProcessingVstHost::CheckVstSdkCompatibility(std::string plugin_sdk_version)
{
    std::string vst_string(VST_STRING);
    size_t pos = plugin_sdk_version.find(vst_string);
    if (pos != std::string::npos)
    {
        plugin_sdk_version.erase(pos, vst_string.length());
    }
    std::vector<size_t> plugin_sdk_splited;
    auto status = Utils::SplitString(plugin_sdk_version, ".", plugin_sdk_splited);    
    RETURN_ERROR_IF_NOT_SUCCESS(status);
    
    // NOTE:
    // plugin_sdk_splited.at(0) - kVstVersionMajor
    // plugin_sdk_splited.at(1) - kVstVersionMinor
    // plugin_sdk_splited.at(2) - kVstVersionSub
    if (kVstVersionMajor != plugin_sdk_splited.at(0) && kVstVersionMinor != plugin_sdk_splited.at(1))
    {
        LOG(ERROR) << "VST Plugin has been build with unsupported SDK " << plugin_sdk_version;
    }

    return VST_ERROR_STATUS::SUCCESS;
}

AUDIOHOSTLIB_EXPORT int AudioProcessingVstHost::ProcessWaveFile(const std::string& input_wave_path,
                                                                const std::string& output_wave_path)
{
    std::string input_wave_path_  = input_wave_path;
    
    if (input_wave_path_ == "" || output_wave_path == "")
    {
        LOG(ERROR) << "Empty input or output path.";
        return VST_ERROR_STATUS::PATH_NOT_EXISTS;
    }

    if (vst_plugins_.size() == 0)
    {
        return VST_ERROR_STATUS::NO_PLUGIN_INITIALIZED;
    }
        
    for (auto& [plugin_id, _] : vst_plugins_)
    {

        if (!vst_plugins_[plugin_id].plug_provider_)
        {
            LOG(ERROR) << "Plugin " + vst_plugins_[plugin_id].plugin_path_ + " was not initialized.";
            return VST_ERROR_STATUS::CREATE_PLUGIN_PROVIDER_ERROR;
        }

        // audio processing data flow: 
        // https://developer.steinberg.help/display/VST/Audio+Processor+Call+Sequence
        Steinberg::OPtr<Steinberg::Vst::IComponent> component = vst_plugins_[plugin_id].plug_provider_->getComponent();
        Steinberg::FUnknownPtr<Steinberg::Vst::IAudioProcessor> processor(component);

        if (!processor)
        {
            LOG(ERROR) << "Processing instance not created.";
            return VST_ERROR_STATUS::CREATE_PROCESSING_INSTANCE_ERROR;
        }

        // TODO:
        // create class for management of read/write wave

        wave::File input_wave_file;

        // TODO:
        // collect status and print it
        if (input_wave_file.Open(input_wave_path_, wave::kIn))
        {
            LOG(ERROR) << "Open input wave file failed." << std::endl;
            return VST_ERROR_STATUS::OPEN_FILE_ERROR;
        }

        Buffers buffs = {};
        std::vector<float> content;

        if (input_wave_file.Read(&content))
        {
            LOG(ERROR) << "Read from input wave failed." << std::endl;
            return VST_ERROR_STATUS::READ_WRITE_ERROR;
        }

        std::vector<float> output(content.size(), 1);
        Steinberg::Vst::ProcessSetup setup
        {
            Steinberg::Vst::kOffline,
            Steinberg::Vst::kSample32,
            static_cast<Steinberg::int32>(input_wave_file.frame_number()),
            static_cast<Steinberg::Vst::SampleRate>(input_wave_file.sample_rate())
        };

        if (processor->setupProcessing(setup) != Steinberg::kResultOk)
        {
            LOG(ERROR) << "Processing set up failed.";
            return VST_ERROR_STATUS::VST_HOST_ERROR;
        }

        if (component->setActive(true) != Steinberg::kResultOk)
        {
            LOG(ERROR) << "Processing activated.";
            return VST_ERROR_STATUS::VST_HOST_ERROR;
        }

        auto status = processor->setProcessing(true);
        if (status != Steinberg::kResultOk)
        {
            if (status != Steinberg::kNotImplemented)
            {
                LOG(ERROR) << "Processing->setProcessing() error.";
                return VST_ERROR_STATUS::VST_HOST_ERROR;
            }

            if (verbose_ > LogLevelType::LOG_LEVEL::ERROR_LOG)
            {
                LOG(WARNING) << "Processing->setProcessing() not implemented.";
            }
        }

        processData.inputParameterChanges = &inputParameterChanges;
        processData.prepare(*component, input_wave_file.frame_number(), Steinberg::Vst::kSample32);

        processData.numSamples = input_wave_file.frame_number() * input_wave_file.channel_number();
        buffs.numInputs  = input_wave_file.channel_number();
        buffs.numOutputs = input_wave_file.channel_number();
        buffs.numSamples = input_wave_file.frame_number();

        processContext.continousTimeSamples = true;
        assignBusBuffers(buffs, processData, content.data(), output.data());

        if (processor->process(processData) != Steinberg::kResultOk)
        {
            LOG(ERROR) << "Processing failed.";
            return VST_ERROR_STATUS::PLUGIN_PROCESSING_FAILED;
        }

        wave::File write_file;
        if (write_file.Open(output_wave_path, wave::kOut))
        {
            LOG(ERROR) << "Opening of output wave file failed." << std::endl;
            return VST_ERROR_STATUS::OPEN_FILE_ERROR;
        }

        write_file.set_sample_rate(input_wave_file.sample_rate());
        write_file.set_bits_per_sample(input_wave_file.bits_per_sample());
        write_file.set_channel_number(input_wave_file.channel_number());
        auto output_buffer_ptr = static_cast<float*> (*processData.outputs[0].channelBuffers32);
        std::vector<float> out(output_buffer_ptr, output_buffer_ptr + content.size());
        if (write_file.Write(out))
        {
            LOG(ERROR) << "Write to output wave failed." << std::endl;
            return VST_ERROR_STATUS::READ_WRITE_ERROR;
        }
        assignBusBuffers(buffs, processData, content.data(), output.data(), true);
        input_wave_path_ = output_wave_path;
    }
    return VST_ERROR_STATUS::SUCCESS;
}

AUDIOHOSTLIB_EXPORT void AudioProcessingVstHost::SetVerbosity(uint8_t value)
{
    verbose_ = value;
    VLOG(value);
}

AUDIOHOSTLIB_EXPORT void AudioProcessingVstHost::Terminate()
{
    for (const auto& element : vst_plugins_)
    {
        vst_plugins_[element.first].plug_provider_.reset();
        vst_plugins_[element.first].module_.reset();
    }
    Steinberg::Vst::PluginContextFactory::instance().setPluginContext(nullptr);
}
