#include "gtest/gtest.h"
#include "gmock/gmock-matchers.h"

#include <filesystem>

#include "VstHostTool.h"
#include "file.h"
#include "enums.h"
#include "UnitTestsCommon.h"
#include "json.hpp"
#include "JsonUtils.h"

namespace VstHostToolUnitTest
{
    class VstHostToolTest : public testing::Test
    {
    protected:
        VstHostToolTest() = default;
        virtual ~VstHostToolTest() = default;

        std::unique_ptr<VstHostTool> vst_host_tool_;

        virtual void SetUp()
        {
            vst_host_tool_.reset(new VstHostTool());
        }

        virtual void TearDown()
        {
            vst_host_tool_.reset();
            CleanUpUtProducts();
        }

        void CleanUpUtProducts()
        {
            if (std::filesystem::exists(OUTPUT_WAVE_PATH))
            {
                std::remove(OUTPUT_WAVE_PATH.c_str());
            }

            if (std::filesystem::exists(DUMP_JSON_FILE_PATH))
            {
                std::remove(DUMP_JSON_FILE_PATH.c_str());
            }

            if (std::filesystem::exists(PROCESSING_CONFIG_PATH))
            {
                std::remove(PROCESSING_CONFIG_PATH.c_str());
            }
        }

        int LoadWave(std::string wave_path, std::vector<float>* data)
        {
            wave::File input_wave_file;
            if (input_wave_file.Open(wave_path, wave::kIn))
            {
                return VST_ERROR_STATUS::OPEN_FILE_ERROR;
            }

            if (input_wave_file.Read(data))
            {

                return VST_ERROR_STATUS::READ_WRITE_ERROR;
            }
            return VST_ERROR_STATUS::SUCCESS;
        }

        void RemoveDumpedJsonConfig()
        {
            int status = VST_ERROR_STATUS::PATH_NOT_EXISTS;
            if (std::filesystem::exists(DUMP_JSON_FILE_PATH))
            {
                std::remove(DUMP_JSON_FILE_PATH.c_str());
                status = VST_ERROR_STATUS::SUCCESS;
            }
            EXPECT_EQ(status, VST_ERROR_STATUS::SUCCESS);
        }

        int PrepareJsonConfig(nlohmann::json* json_config, bool enbale_vst_host)
        {
            RETURN_ERROR_IF_NULL(json_config);
            nlohmann::json json_config_ = *json_config;
            json_config_["sampling_rate"] = PROCESSING_SAMPLING_RATE;
            json_config_["vst_host"]["enable"] = enbale_vst_host;
            json_config_["vst_host"]["processing_config"]["plugin_1"] = {};
            json_config_["vst_host"]["processing_config"]["plugin_1"]["config"] = CONFIG_FOR_ADELAY_PLUGIN;
            json_config_["vst_host"]["processing_config"]["plugin_1"]["plugin"] = VST_PLUGIN_PATH;
            json_config_["input_wave"] = INPUT_WAVE_PATH;
            json_config_["output_wave"] = OUTPUT_WAVE_PATH;
            *json_config = json_config_;

            return VST_ERROR_STATUS::SUCCESS;
        }

        int DumpJsonAppConfig(std::string dump_path = PROCESSING_CONFIG_PATH)
        {
            std::vector<std::string> arg_params = {
                "OfflineToolsUnitTests.exe",
                DUMP_APP_CONFIG_PARAM,
                APP_CONFIG_PARAM,
                dump_path,
            };

            int status = vst_host_tool_->PrepareArgs(arg_params);
            EXPECT_EQ(status, VST_ERROR_STATUS::SUCCESS);

            status = vst_host_tool_->Run();
            EXPECT_EQ(status, VST_ERROR_STATUS::SUCCESS);
            return status;
        }

        int FirstConfigPreparations(nlohmann::json* json_config, bool enable_vst_host=true)
        {
            int status = DumpJsonAppConfig();
            EXPECT_EQ(status, VST_ERROR_STATUS::SUCCESS);

            status = JsonUtils::LoadJson(PROCESSING_CONFIG_PATH, json_config);
            EXPECT_EQ(status, VST_ERROR_STATUS::SUCCESS);
            EXPECT_EQ(json_config->size(), MODULES_COUNT);

            status = PrepareJsonConfig(json_config, enable_vst_host);
            EXPECT_EQ(status, VST_ERROR_STATUS::SUCCESS);    

            return status;
        }

        void AppProcessingWithOutputValidation(std::string ref_path, bool validate_quality=true, bool bit_exact=true)
        {
            std::vector<std::string> arg_params = {
                "OfflineToolsUnitTests.exe",
                APP_CONFIG_PARAM,
                PROCESSING_CONFIG_PATH,
            };

            vst_host_tool_.reset(new VstHostTool());

            int status = vst_host_tool_->PrepareArgs(arg_params);
            EXPECT_EQ(status, VST_ERROR_STATUS::SUCCESS);
            status = vst_host_tool_->Run();
            EXPECT_EQ(status, VST_ERROR_STATUS::SUCCESS);

            std::vector<float> output;
            status = LoadWave(OUTPUT_WAVE_PATH, &output);
            EXPECT_EQ(status, VST_ERROR_STATUS::SUCCESS);

            std::vector<float> ref;
            status = LoadWave(ref_path, &ref);
            EXPECT_EQ(status, VST_ERROR_STATUS::SUCCESS);
            if (validate_quality)
            {
                if (bit_exact)
                {
                    EXPECT_EQ(output, ref);
                }
                else
                {
                    ASSERT_THAT(output, testing::Pointwise(testing::FloatNear(PRECISION_7_DECIMAL_PLACES), ref));
                }
            }
        }

    };

    // TODO:
    // add more unit test for arg parser
    TEST_F(VstHostToolTest, RunToolWithoutSettingArgs)
    {
        int status = vst_host_tool_->Run();
        EXPECT_EQ(status, VST_ERROR_STATUS::ARG_PARSER_ERROR);
    }

    TEST_F(VstHostToolTest, ByPassProcessing)
    {
        nlohmann::json json_config;
        int status = FirstConfigPreparations(&json_config, false);
        EXPECT_EQ(status, VST_ERROR_STATUS::SUCCESS);

        status = JsonUtils::DumpJson(json_config, PROCESSING_CONFIG_PATH);

        std::vector<std::string> arg_params = {
            "OfflineToolsUnitTests.exe",
            APP_CONFIG_PARAM,
            PROCESSING_CONFIG_PATH,
        };

        vst_host_tool_.reset(new VstHostTool());
        status = vst_host_tool_->PrepareArgs(arg_params);
        EXPECT_EQ(status, VST_ERROR_STATUS::SUCCESS);
        status = vst_host_tool_->Run();
        EXPECT_EQ(status, VST_ERROR_STATUS::SUCCESS);

        std::vector<float> output;
        status = LoadWave(OUTPUT_WAVE_PATH, &output);
        EXPECT_EQ(status, VST_ERROR_STATUS::SUCCESS);

        std::vector<float> ref;
        status = LoadWave(INPUT_WAVE_PATH, &ref);
        EXPECT_EQ(status, VST_ERROR_STATUS::SUCCESS);
        EXPECT_EQ(output, ref);
    }

    TEST_F(VstHostToolTest, ProcessSignalWithSinglePlugin)
    {
        nlohmann::json json_config;
        int status = FirstConfigPreparations(&json_config);
        EXPECT_EQ(status, VST_ERROR_STATUS::SUCCESS);
        status = JsonUtils::DumpJson(json_config, PROCESSING_CONFIG_PATH);
        EXPECT_EQ(status, VST_ERROR_STATUS::SUCCESS);

        std::vector<std::string> arg_params = {
            "OfflineToolsUnitTests.exe",
            APP_CONFIG_PARAM,
            PROCESSING_CONFIG_PATH,
        };

        vst_host_tool_.reset(new VstHostTool());
        status = vst_host_tool_->PrepareArgs(arg_params);
        EXPECT_EQ(status, VST_ERROR_STATUS::SUCCESS);
        status = vst_host_tool_->Run();
        EXPECT_EQ(status, VST_ERROR_STATUS::SUCCESS);

        std::vector<float> output;
        status = LoadWave(OUTPUT_WAVE_PATH, &output);
        EXPECT_EQ(status, VST_ERROR_STATUS::SUCCESS);

        std::vector<float> ref;
        status = LoadWave(REF_OUTPUT_WITH_SET_DELAY, &ref);
        EXPECT_EQ(status, VST_ERROR_STATUS::SUCCESS);
        EXPECT_EQ(output, ref);
    }

    TEST_F(VstHostToolTest, DumpVstHostConfig)
    {
        int status = DumpJsonAppConfig(DUMP_JSON_FILE_PATH);
        EXPECT_EQ(status, VST_ERROR_STATUS::SUCCESS);

        nlohmann::json json_config;
        status = JsonUtils::LoadJson(DUMP_JSON_FILE_PATH, &json_config);
        EXPECT_EQ(status, VST_ERROR_STATUS::SUCCESS);
        EXPECT_EQ(json_config.size(), MODULES_COUNT);
        RemoveDumpedJsonConfig();
    }

    TEST_F(VstHostToolTest, DumpVstHostConfigTwice)
    {
        nlohmann::json json_config;
        int status = FirstConfigPreparations(&json_config);
        EXPECT_EQ(status, VST_ERROR_STATUS::SUCCESS);
        
        status = JsonUtils::DumpJson(json_config, DUMP_JSON_FILE_PATH);
        EXPECT_EQ(status, VST_ERROR_STATUS::SUCCESS);
        
        vst_host_tool_.reset(new VstHostTool());
        status = DumpJsonAppConfig(DUMP_JSON_FILE_PATH);
        EXPECT_EQ(status, VST_ERROR_STATUS::SUCCESS);

        json_config.clear();
        status = JsonUtils::LoadJson(DUMP_JSON_FILE_PATH, &json_config);
        EXPECT_EQ(status, VST_ERROR_STATUS::SUCCESS);
        EXPECT_EQ(json_config.size(), MODULES_COUNT);

        RemoveDumpedJsonConfig();
    }

    TEST_F(VstHostToolTest, GetVstPluginConfig)
    {
        nlohmann::json json_config;
        int status = FirstConfigPreparations(&json_config);
        EXPECT_EQ(status, VST_ERROR_STATUS::SUCCESS);
        
        json_config["vst_host"]["processing_config"]["plugin_1"]["config"] = DUMP_JSON_FILE_PATH;
        status = JsonUtils::DumpJson(json_config, PROCESSING_CONFIG_PATH);
        EXPECT_EQ(status, VST_ERROR_STATUS::SUCCESS);

        std::vector<std::string> arg_params = {
            "OfflineToolsUnitTests.exe",
            APP_CONFIG_PARAM,
            PROCESSING_CONFIG_PATH,
            "-dump_plugins_config"
        };

        vst_host_tool_.reset(new VstHostTool());
        status = vst_host_tool_->PrepareArgs(arg_params);
        EXPECT_EQ(status, VST_ERROR_STATUS::SUCCESS);

        status = vst_host_tool_->Run();
        EXPECT_EQ(status, VST_ERROR_STATUS::SUCCESS);

        json_config.clear();
        status = JsonUtils::LoadJson(DUMP_JSON_FILE_PATH, &json_config);
        EXPECT_EQ(status, VST_ERROR_STATUS::SUCCESS);
        EXPECT_EQ(json_config.size(), DELAY_PARAMS_COUNT);

        RemoveDumpedJsonConfig();
    }

    TEST_F(VstHostToolTest, GetVstPluginConfigNegative)
    {
        int status = DumpJsonAppConfig();
        EXPECT_EQ(status, VST_ERROR_STATUS::SUCCESS);

        nlohmann::json json_config;
        status = JsonUtils::LoadJson(PROCESSING_CONFIG_PATH, &json_config);
        EXPECT_EQ(status, VST_ERROR_STATUS::SUCCESS);
        EXPECT_EQ(json_config.size(), MODULES_COUNT);

        json_config["sampling_rate"] = PROCESSING_SAMPLING_RATE;
        status = JsonUtils::DumpJson(json_config, PROCESSING_CONFIG_PATH);
        EXPECT_EQ(status, VST_ERROR_STATUS::SUCCESS);

        // Dump Second Time
        std::vector<std::string> arg_params = {
            "OfflineToolsUnitTests.exe",
            APP_CONFIG_PARAM,
            PROCESSING_CONFIG_PATH,
            "-dump_plugins_config"
        };

        vst_host_tool_.reset(new VstHostTool());
        status = vst_host_tool_->PrepareArgs(arg_params);
        EXPECT_EQ(status, VST_ERROR_STATUS::SUCCESS);
        status = vst_host_tool_->Run();
        EXPECT_EQ(status, VST_ERROR_STATUS::EMPTY_ARG);
    }

    TEST_F(VstHostToolTest, RunToolWithoutPassingOutputWavePath)
    {
        nlohmann::json json_config;
        int status = FirstConfigPreparations(&json_config);
        EXPECT_EQ(status, VST_ERROR_STATUS::SUCCESS);
        
        json_config["output_wave"] = { };
        status = JsonUtils::DumpJson(json_config, PROCESSING_CONFIG_PATH);
        EXPECT_EQ(status, VST_ERROR_STATUS::SUCCESS);

        std::vector<std::string> arg_params = {
            "OfflineToolsUnitTests.exe",
            APP_CONFIG_PARAM,
            PROCESSING_CONFIG_PATH,
        };

        vst_host_tool_.reset(new VstHostTool());
        status = vst_host_tool_->PrepareArgs(arg_params);
        EXPECT_EQ(status, VST_ERROR_STATUS::SUCCESS);
        status = vst_host_tool_->Run();
        EXPECT_EQ(status, VST_ERROR_STATUS::EMPTY_ARG);
    }

    TEST_F(VstHostToolTest, RunToolWithProcessingConfig)
    {
        nlohmann::json json_config;
        int status = FirstConfigPreparations(&json_config);
        EXPECT_EQ(status, VST_ERROR_STATUS::SUCCESS);

        json_config["vst_host"]["processing_config"]["plugin_2"] = {};
        json_config["vst_host"]["processing_config"]["plugin_2"]["config"] = CONFIG_FOR_ADELAY_PLUGIN_2;
        json_config["vst_host"]["processing_config"]["plugin_2"]["plugin"] = VST_PLUGIN_PATH;
        status = JsonUtils::DumpJson(json_config, PROCESSING_CONFIG_PATH);
        EXPECT_EQ(status, VST_ERROR_STATUS::SUCCESS);

        std::vector<std::string> arg_params = {
                  "OfflineToolsUnitTests.exe",
                  APP_CONFIG_PARAM,
                  PROCESSING_CONFIG_PATH,
        };

        vst_host_tool_.reset(new VstHostTool());
        status = vst_host_tool_->PrepareArgs(arg_params);
        EXPECT_EQ(status, VST_ERROR_STATUS::SUCCESS);
        status = vst_host_tool_->Run();
        EXPECT_EQ(status, VST_ERROR_STATUS::SUCCESS);

        std::vector<float> output;
        status = LoadWave(OUTPUT_WAVE_PATH, &output);
        EXPECT_EQ(status, VST_ERROR_STATUS::SUCCESS);

        // TOOD:
        // create new reference for mutliplugin processing
        std::vector<float> ref;
        status = LoadWave(REF_OUTPUT_WITH_TWO_PLUGINS, &ref);
        EXPECT_EQ(status, VST_ERROR_STATUS::SUCCESS);
        EXPECT_EQ(output, ref);
    }

    TEST_F(VstHostToolTest, RunToolWithProcessingConfigWithOneEmptyConfig)
    {
        nlohmann::json json_config;
        int status = FirstConfigPreparations(&json_config);
        EXPECT_EQ(status, VST_ERROR_STATUS::SUCCESS);

        json_config["vst_host"]["processing_config"]["plugin_2"] = {};
        json_config["vst_host"]["processing_config"]["plugin_2"]["config"] = "";
        json_config["vst_host"]["processing_config"]["plugin_2"]["plugin"] = VST_PLUGIN_PATH;
        status = JsonUtils::DumpJson(json_config, PROCESSING_CONFIG_PATH);
        EXPECT_EQ(status, VST_ERROR_STATUS::SUCCESS);

        AppProcessingWithOutputValidation(REF_OUTPUT_WITH_TWO_PLUGINS_2);
    }

    TEST_F(VstHostToolTest, RunToolWithPreprocessingOnly)
    {
        nlohmann::json json_config;
        std::string ref_file = REF_PRE_POST_PROC_ONLY;
        int status = FirstConfigPreparations(&json_config, false);
        EXPECT_EQ(status, VST_ERROR_STATUS::SUCCESS);
        json_config[PREPROCESSING_STRING]["filter"]["enable"] = true;
        json_config[POSTPROCESSING_STRING]["filter"]["enable"] = false;

        status = JsonUtils::DumpJson(json_config, PROCESSING_CONFIG_PATH);
        EXPECT_EQ(status, VST_ERROR_STATUS::SUCCESS);
#ifdef _WIN32 
        AppProcessingWithOutputValidation(ref_file, true, false);
#else
        AppProcessingWithOutputValidation(ref_file, false);
#endif //!__APPLE__
    }

    TEST_F(VstHostToolTest, RunToolWithPostprocessingOnly)
    {
        nlohmann::json json_config;
        std::string ref_file = REF_PRE_POST_PROC_ONLY;
        int status = FirstConfigPreparations(&json_config, false);
        EXPECT_EQ(status, VST_ERROR_STATUS::SUCCESS);
        json_config[PREPROCESSING_STRING]["filter"]["enable"] = false;
        json_config[POSTPROCESSING_STRING]["filter"]["enable"] = true;

        status = JsonUtils::DumpJson(json_config, PROCESSING_CONFIG_PATH);
        EXPECT_EQ(status, VST_ERROR_STATUS::SUCCESS);
#ifdef _WIN32 
        AppProcessingWithOutputValidation(ref_file, true, false);
#else
        AppProcessingWithOutputValidation(ref_file, false);
#endif //_WIN32 
    }

    TEST_F(VstHostToolTest, RunToolWithPreAndPostprocessing)
    {
        nlohmann::json json_config;
        int status = FirstConfigPreparations(&json_config, false);
        EXPECT_EQ(status, VST_ERROR_STATUS::SUCCESS);
        json_config[PREPROCESSING_STRING]["filter"]["enable"] = true;
        json_config[POSTPROCESSING_STRING]["filter"]["enable"] = true;

        status = JsonUtils::DumpJson(json_config, PROCESSING_CONFIG_PATH);
        EXPECT_EQ(status, VST_ERROR_STATUS::SUCCESS);
#ifdef _WIN32 
        AppProcessingWithOutputValidation(REF_PRE_POST_PROC_BOTH, true, false);
#else
        AppProcessingWithOutputValidation(REF_PRE_POST_PROC_BOTH, false);
#endif //_WIN32 
    }

    TEST_F(VstHostToolTest, RunToolWithPrePostVstHostProcessing)
    {
        nlohmann::json json_config;
        bool enable_vst_host = true;
        std::string ref_file = REF_PRE_POST_VST_HOST;
        int status = FirstConfigPreparations(&json_config, enable_vst_host);
        EXPECT_EQ(status, VST_ERROR_STATUS::SUCCESS);
        json_config[PREPROCESSING_STRING]["filter"]["enable"] = true;
        json_config[POSTPROCESSING_STRING]["filter"]["enable"] = true;

        status = JsonUtils::DumpJson(json_config, PROCESSING_CONFIG_PATH);
        EXPECT_EQ(status, VST_ERROR_STATUS::SUCCESS);
#ifdef _WIN32 
        AppProcessingWithOutputValidation(ref_file, true, false);
#else
        AppProcessingWithOutputValidation(ref_file, false);
#endif //_WIN32 
    }

    TEST_F(VstHostToolTest, RunToolWithPreVstHostProcessing)
    {
        nlohmann::json json_config;
        bool enable_vst_host = true;
        std::string ref_file = REF_PRE_VST_HOST;
        int status = FirstConfigPreparations(&json_config, enable_vst_host);
        EXPECT_EQ(status, VST_ERROR_STATUS::SUCCESS);
        json_config[PREPROCESSING_STRING]["filter"]["enable"] = true;
        json_config[POSTPROCESSING_STRING]["filter"]["enable"] = false;

        status = JsonUtils::DumpJson(json_config, PROCESSING_CONFIG_PATH);
        EXPECT_EQ(status, VST_ERROR_STATUS::SUCCESS);
#ifdef _WIN32 
        AppProcessingWithOutputValidation(ref_file, true, false);
#else
        AppProcessingWithOutputValidation(ref_file, false);
#endif //_WIN32 
    }

    TEST_F(VstHostToolTest, RunToolWithPostVstHostProcessing)
    {
        nlohmann::json json_config;
        bool enable_vst_host = true;
        std::string ref_file = REF_PRE_VST_HOST;
        int status = FirstConfigPreparations(&json_config, enable_vst_host);
        EXPECT_EQ(status, VST_ERROR_STATUS::SUCCESS);
        json_config[PREPROCESSING_STRING]["filter"]["enable"] = false;
        json_config[POSTPROCESSING_STRING]["filter"]["enable"] = true;

        status = JsonUtils::DumpJson(json_config, PROCESSING_CONFIG_PATH);
        EXPECT_EQ(status, VST_ERROR_STATUS::SUCCESS);
#ifdef _WIN32 
        AppProcessingWithOutputValidation(ref_file, true, false);
#else
        AppProcessingWithOutputValidation(ref_file, false);
#endif //_WIN32 
    }

#ifdef __linux__
    TEST_F(VstHostToolTest, RunToolWithEndpointProcessingPipeline)
    {
        nlohmann::json json_config;
        bool enable_vst_host = true;
        int status = FirstConfigPreparations(&json_config, enable_vst_host);
        EXPECT_EQ(status, VST_ERROR_STATUS::SUCCESS);

        status = JsonUtils::DumpJson(json_config, PROCESSING_CONFIG_PATH);
        EXPECT_EQ(status, VST_ERROR_STATUS::SUCCESS);

        std::vector<std::string> arg_params = {
            "OfflineToolsUnitTests.exe",
            APP_CONFIG_PARAM,
            PROCESSING_CONFIG_PATH,
            ENABLE_AUDIO_CAPTURE,
        };

        vst_host_tool_.reset(new VstHostTool());

        status = vst_host_tool_->PrepareArgs(arg_params);
        EXPECT_EQ(status, VST_ERROR_STATUS::SUCCESS);
        status = vst_host_tool_->Run();
        EXPECT_EQ(status, VST_ERROR_STATUS::NOT_IMPLEMENTED);
    }
#endif __linux__

}
