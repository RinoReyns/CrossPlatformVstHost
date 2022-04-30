#ifndef COMMON_H
#define COMMON_H

#include <string>

const std::string VST_PLUGIN_PATH               = "adelay.vst3";
const std::string INPUT_WAVE_PATH               = "data\\sine_440.wav";
const std::string OUTPUT_WAVE_PATH              = "data\\output.wav";
const std::string DUMP_JSON_FILE_PATH           = "data\\config.json";
const std::string LOAD_JSON_FILE_PATH           = "data\\adelay_config.json";
const std::string REF_OUTPUT_DEFAULT_CONFIG     = "data\\sine_440_output_ref.wav";
const std::string REF_OUTPUT_WITH_SET_DELAY     = "data\\sine_440_output_ref_0.4_delay.wav";

#endif //COMMON_H
