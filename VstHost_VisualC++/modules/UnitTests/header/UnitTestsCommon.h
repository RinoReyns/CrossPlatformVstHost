#ifndef UNIT_TESTS_COMMON_H
#define UNIT_TESTS_COMMON_H

#include <string>

#ifdef _WIN32
    const std::string UT_DATA_FOLDER = "data\\";
#else
    const std::string UT_DATA_FOLDER = "data/";
#endif

const std::string VST_PLUGIN_PATH           = "adelay.vst3";
const std::string INPUT_WAVE_PATH           = UT_DATA_FOLDER + "sine_440.wav";
const std::string OUTPUT_WAVE_PATH          = UT_DATA_FOLDER + "output.wav";
const std::string DUMP_JSON_FILE_PATH       = UT_DATA_FOLDER + "config.json";
const std::string CONFIG_FOR_ADELAY_PLUGIN  = UT_DATA_FOLDER + "adelay_config.json";
const std::string REF_OUTPUT_DEFAULT_CONFIG = UT_DATA_FOLDER + "sine_440_output_ref.wav";
const std::string REF_OUTPUT_WITH_SET_DELAY = UT_DATA_FOLDER + "sine_440_output_ref_0.4_delay.wav";
const std::string PROCESSING_CONFIG_PATH    = UT_DATA_FOLDER + "processing_config.json";

#endif //COMMON_H
