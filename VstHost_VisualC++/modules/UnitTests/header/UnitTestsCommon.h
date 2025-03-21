#ifndef UNIT_TESTS_COMMON_H
#define UNIT_TESTS_COMMON_H

#include <string>

#ifdef _WIN32
    const std::string UT_DATA_FOLDER = "data\\";
#else
    const std::string UT_DATA_FOLDER = "data/";
#endif

const std::string VST_PLUGIN_PATH                   = "adelay.vst3";
const std::string INPUT_WAVE_PATH                   = UT_DATA_FOLDER + "sine_440.wav";
const std::string OUTPUT_WAVE_PATH                  = UT_DATA_FOLDER + "output.wav";
const std::string DUMP_JSON_FILE_PATH               = UT_DATA_FOLDER + "config.json";
const std::string CONFIG_FOR_ADELAY_PLUGIN          = UT_DATA_FOLDER + "adelay_config.json";
const std::string CONFIG_FOR_ADELAY_PLUGIN_2        = UT_DATA_FOLDER + "adelay_config_second_plugin.json";
const std::string REF_OUTPUT_DEFAULT_CONFIG         = UT_DATA_FOLDER + "sine_440_output_ref.wav";
const std::string REF_OUTPUT_WITH_SET_DELAY         = UT_DATA_FOLDER + "sine_440_output_ref_0.4_delay.wav";
const std::string REF_OUTPUT_WITH_TWO_PLUGINS       = UT_DATA_FOLDER + "sine_440_output_ref_two_plugins.wav";
const std::string REF_OUTPUT_WITH_TWO_PLUGINS_2     = UT_DATA_FOLDER + "sine_440_output_ref_two_plugins_one_config.wav";
const std::string PROCESSING_CONFIG_PATH            = UT_DATA_FOLDER + "processing_config.json";
const std::string PLUGIN_NAME                       = "plugin_1";
const std::string REF_FILTRATED                     = UT_DATA_FOLDER + "sine_440_filtrated.wav";
const std::string REF_PRE_POST_PROC_ONLY            = UT_DATA_FOLDER + "sine_440_filtrated_pre_and_post_proc_only.wav";
const std::string REF_PRE_POST_PROC_BOTH            = UT_DATA_FOLDER + "sine_440_filtrated_pre_and_post_proc_both.wav";
const std::string REF_PRE_POST_VST_HOST             = UT_DATA_FOLDER + "sine_440_filtrated_pre_post_vst_host.wav";
const std::string REF_PRE_VST_HOST                  = UT_DATA_FOLDER + "sine_440_filtrated_pre_vst_host.wav";

const float PRECISION_9_DECIMAL_PLACES              = 1e-9;
const float PRECISION_8_DECIMAL_PLACES              = 1e-8;
const int8_t MODULES_COUNT                          = 6;
const int8_t DELAY_PARAMS_COUNT                     = 2;
const std::string APP_CONFIG_PARAM                  = "-config";
const std::string DUMP_APP_CONFIG_PARAM             = "-dump_app_config";
const std::string ENABLE_AUDIO_CAPTURE              = "-enable_audio_capture";
const size_t PROCESSING_SAMPLING_RATE               = 44100;
const std::string PREPROCESSING_STRING              = "preprocessing";
const std::string POSTPROCESSING_STRING             = "postprocessing";

#endif //UNIT_TESTS_COMMON_H
