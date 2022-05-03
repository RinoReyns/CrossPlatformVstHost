#ifndef UNIT_TESTS_COMMON_H
#define UNIT_TESTS_COMMON_H

#include <string>

const std::string VST_PLUGIN_PATH                   = "adelay.vst3";
#ifdef _WIN32
    const std::string INPUT_WAVE_PATH               = "data\\sine_440.wav";
    const std::string OUTPUT_WAVE_PATH              = "data\\output.wav";
    const std::string DUMP_JSON_FILE_PATH           = "data\\config.json";
    const std::string CONFIG_FOR_ADELAY_PLUGIN      = "data\\adelay_config.json";
    const std::string REF_OUTPUT_DEFAULT_CONFIG     = "data\\sine_440_output_ref.wav";
    const std::string REF_OUTPUT_WITH_SET_DELAY     = "data\\sine_440_output_ref_0.4_delay.wav";
    const std::string PROCESSING_CONFIG_PATH        = "data\\processing_config.json";
#else
    const std::string INPUT_WAVE_PATH               = "data/sine_440.wav";
    const std::string OUTPUT_WAVE_PATH              = "data/output.wav";
    const std::string DUMP_JSON_FILE_PATH           = "data/config.json";
    const std::string CONFIG_FOR_ADELAY_PLUGIN      = "data/adelay_config.json";
    const std::string REF_OUTPUT_DEFAULT_CONFIG     = "data/sine_440_output_ref.wav";
    const std::string REF_OUTPUT_WITH_SET_DELAY     = "data/sine_440_output_ref_0.4_delay.wav";
    const std::string PROCESSING_CONFIG_PATH        = "data/processing_config.json";
#endif
#endif //COMMON_H
