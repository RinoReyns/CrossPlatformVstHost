#include "audiohost.h"
#include "file.h"
#include <cstdio>
#include <iostream>
#include "arg_parser.h"
#include "easylogging++.h"
#include <iostream>
#include "AudioCapture.h"

INITIALIZE_EASYLOGGINGPP

AUDIOHOSTLIB_EXPORT AudioProcessingVstHost::~AudioProcessingVstHost()
{
    this->Terminate();
}

AUDIOHOSTLIB_EXPORT int AudioProcessingVstHost::SetPluginParameters(const std::string& plugin_config)
{
    // edit controler data flow: 
    // https://developer.steinberg.help/display/VST/Edit+Controller+Call+Sequence
    if (!plugProvider)
    {
        LOG(ERROR) << "Plugin instance was not created.";
        return VST_ERROR_STATUS::NULL_POINTER;
    }

    Steinberg::OPtr<Steinberg::Vst::IEditController> editController = plugProvider->getController();

    if (!editController)
    {
        LOG(ERROR) << "No EditController found (needed for allowing editor to edit params)";
        return VST_ERROR_STATUS::CREATE_EDIT_CONTROLER_ERROR;
    }
    
    nlohmann::json plugin_config_json;
    VST_ERROR_STATUS status = VST_ERROR_STATUS::SUCCESS;
    
    status = LoadJson(plugin_config, &plugin_config_json);
    if (status != VST_ERROR_STATUS::SUCCESS)
    {
        LOG(ERROR) << "Loding json file with plugin params failed." << std::endl;
        return status;
    }
    
    Steinberg::Vst::ParameterInfo info;
    Steinberg::Vst::ParamValue value;
    std::wstring param_name_wstring;
    Steinberg::int32 index = 0;
    
    for (int32_t i = 0; i < editController->getParameterCount(); i++)
    {
        editController->getParameterInfo(i, info);
        param_name_wstring = info.title;
        std::string param_name(param_name_wstring.begin(), param_name_wstring.end());
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

AUDIOHOSTLIB_EXPORT int AudioProcessingVstHost::GetPluginParameters(const std::string& plugin_config)
{
    // edit controler data flow: 
    // https://developer.steinberg.help/display/VST/Edit+Controller+Call+Sequence
    if (!plugProvider)
    {
        LOG(ERROR) << "Plugin instance was not created.";
        return VST_ERROR_STATUS::NULL_POINTER;
    }
    Steinberg::OPtr<Steinberg::Vst::IEditController> editController = plugProvider->getController();

    if (!editController)
    {
        LOG(ERROR) << "No EditController found (needed for allowing editor) in file";
        return VST_ERROR_STATUS::CREATE_EDIT_CONTROLER_ERROR;
    }

    Steinberg::Vst::ParameterInfo info;
    std::wstring param_name_wstring;
    nlohmann::json plugin_config_json;

    for (int32_t i = 0; i < editController->getParameterCount(); i++)
    {
        editController->getParameterInfo(i, info);
        param_name_wstring = info.title;
        std::string param_name(param_name_wstring.begin(), param_name_wstring.end());
        plugin_config_json[param_name] = editController->getParamNormalized(info.id);
    }

    return DumpJson(plugin_config_json, plugin_config);
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

VST_ERROR_STATUS AudioProcessingVstHost::DumpJson(nlohmann::json json_config, 
                                                  std::string path_to_save)
{
    std::ofstream file(path_to_save);
    if (!file.is_open())
    {
        LOG(ERROR) << "File for dumping json file couldn't be opened." << std::endl;
        return VST_ERROR_STATUS::OPEN_FILE_ERROR;
    }

    file << std::setw(4) << json_config << std::endl;
    file.close();

    return VST_ERROR_STATUS::SUCCESS;
}

VST_ERROR_STATUS AudioProcessingVstHost::LoadJson(std::string plugin_config_path, 
                                                  nlohmann::json* json_config)
{
    std::ifstream file(plugin_config_path);
    if (!file.is_open())
    {
        LOG(ERROR) << "Json file with plugin config couldn't be opened." << std::endl;
        return VST_ERROR_STATUS::OPEN_FILE_ERROR;   
    }

    file >> *json_config;
    file.close();
    return VST_ERROR_STATUS::SUCCESS;
}

int AUDIOHOSTLIB_EXPORT AudioProcessingVstHost::CreatePluginInstance(const std::string& plugin_path)
{
    VST3::Optional<VST3::UID> uid;
    return CreatePluginInstance(plugin_path, std::move(uid));
}

AUDIOHOSTLIB_EXPORT int AudioProcessingVstHost::CreatePluginInstance(const std::string& plugin_path,
                                                                     VST3::Optional<VST3::UID> effectID)
{
    // TODO: 
    // plugProvider should be a map: 1) id 2) path
    std::string error;
    module = VST3::Hosting::Module::create(plugin_path, error);
    if (!module)
    {
        LOG(ERROR) << "Couldn't create Module for file:" + plugin_path + "\nError: " + error;
        return VST_ERROR_STATUS::CREATE_HOSTING_MODULE_ERROR;
    }

    plugin_path_ = plugin_path;
    auto factory = module->getFactory();
    for (auto& classInfo : factory.classInfos())
    {
        // TODO:
        // if delay use info about that
        // and test for different delays
        if (classInfo.category() == kVstAudioEffectClass)
        {
            if (effectID)
            {
                if (*effectID != classInfo.ID())
                {
                    continue;
                }
            }
            plugProvider.reset(new Steinberg::Vst::PlugProvider(factory, classInfo, true));
            break;
        }
    }
    return VST_ERROR_STATUS::SUCCESS;
}

AUDIOHOSTLIB_EXPORT int AudioProcessingVstHost::ProcessWaveFileWithSinglePlugin(
    const std::string& input_wave_path,
    const std::string& output_wave_path)
{
    if (input_wave_path == "" || output_wave_path == "")
    {
        LOG(ERROR) << "Empty input or output path.";
        return VST_ERROR_STATUS::PATH_NOT_EXISTS;
    }
    
    if (!plugProvider)
    {
        LOG(ERROR) << "Plugin " + this->plugin_path_ +" was not initialized.";
        return VST_ERROR_STATUS::CREATE_PLUGIN_PROVIDER_ERROR;
    }

    // audio processing data flow: 
    // https://developer.steinberg.help/display/VST/Audio+Processor+Call+Sequence
    Steinberg::OPtr<Steinberg::Vst::IComponent> component = plugProvider->getComponent();
    Steinberg::FUnknownPtr<Steinberg::Vst::IAudioProcessor> processor = component;

    if (!processor)
    {
        LOG(ERROR) << "Processing instance not created.";
        return VST_ERROR_STATUS::CREATE_PROCESSING_INSTANCE_ERROR;
    }
    
    // TODO:
    // create class for management of read/write wave
    
    // read file's content
    // https://github.com/audionamix/wave
    wave::File input_wave_file;
    
    // TODO:
    // collect status and print it
    if (input_wave_file.Open(input_wave_path, wave::kIn))
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

    processData.numSamples	= input_wave_file.frame_number() * input_wave_file.channel_number();
    buffs.numInputs			= input_wave_file.channel_number();
    buffs.numOutputs		= input_wave_file.channel_number();
    buffs.numSamples		= input_wave_file.frame_number();

    processContext.continousTimeSamples = true;
    assignBusBuffers(buffs, processData, content.data(), output.data());

    if (processor->process(processData) != Steinberg::kResultOk)
    {
        LOG(ERROR) << "Processing failed.";
        return VST_ERROR_STATUS::PLUGIN_PROCESSING_FAILED;
    }

    wave::File write_file;
    // TODO:
    // investigte why UT fail when output_wave_path not found
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
    return VST_ERROR_STATUS::SUCCESS;
}

AUDIOHOSTLIB_EXPORT void AudioProcessingVstHost::SetVerbosity(uint8_t value)
{
    verbose_ = value;
    VLOG(value);
}

AUDIOHOSTLIB_EXPORT void AudioProcessingVstHost::Terminate()
{
    plugProvider.reset();
    module.reset();
    Steinberg::Vst::PluginContextFactory::instance().setPluginContext(nullptr);
}
