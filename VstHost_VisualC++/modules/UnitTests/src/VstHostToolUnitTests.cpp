#include "gtest/gtest.h"

#include <filesystem>

#include "VstHostTool.h"
#include "file.h"
#include "enums.h"
#include "Common.h"
#include "json.hpp"

namespace VstHostToolUnitTest
{
    class VstHostToolTest : public testing::Test
    {
        protected:
            VstHostToolTest()          = default;
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
    };

    TEST_F(VstHostToolTest, RunToolWithoutSettingArgs)
    {
        int status = vst_host_tool_->Run();
        EXPECT_EQ(status, VST_ERROR_STATUS::PARS_ARGS_ERROR);
    }

    TEST_F(VstHostToolTest, ProcessSignalWithSinglePlugin)
    {
        std::vector<std::string> arg_params = {
            "OfflineToolsUnitTests.exe", 
            "-vst_plugin",
            VST_PLUGIN_PATH,
            "-input_wave",
            INPUT_WAVE_PATH,
            "-output_wave_path",
            OUTPUT_WAVE_PATH,
            "-plugin_config",
            LOAD_JSON_FILE_PATH
            };

        int status = vst_host_tool_->PrepareArgs(arg_params);
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

    TEST_F(VstHostToolTest, GetConfig)
    {
        std::vector<std::string> arg_params = {
            "OfflineToolsUnitTests.exe",
            "-vst_plugin",
            VST_PLUGIN_PATH,
            "-input_wave",
            INPUT_WAVE_PATH,
            "-output_wave_path",
            OUTPUT_WAVE_PATH,
            "-plugin_config",
            DUMP_JSON_FILE_PATH,
            "-dump_plugin_params"
        };

        int status = vst_host_tool_->PrepareArgs(arg_params);
        EXPECT_EQ(status, VST_ERROR_STATUS::SUCCESS);
        status = vst_host_tool_->Run();
        EXPECT_EQ(status, VST_ERROR_STATUS::SUCCESS);
        RemoveDumpedJsonConfig();
    }

    TEST_F(VstHostToolTest, RunToolWithoutPassingOutputWavePath)
    {
        std::vector<std::string> arg_params = {
            "OfflineToolsUnitTests.exe",
            "-vst_plugin",
            VST_PLUGIN_PATH,
            "-input_wave",
            INPUT_WAVE_PATH,
            "-plugin_config",
            LOAD_JSON_FILE_PATH
        };

        int status = vst_host_tool_->PrepareArgs(arg_params);
        EXPECT_EQ(status, VST_ERROR_STATUS::SUCCESS);
        status = vst_host_tool_->Run();
        EXPECT_EQ(status, VST_ERROR_STATUS::EMPTY_ARG);
    }
}
