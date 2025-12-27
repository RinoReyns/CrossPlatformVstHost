#include "gtest/gtest.h"
#include "gmock/gmock-matchers.h"

#include "FilterWrapper.h"
#include "UnitTestsCommon.h"
#include "file.h"
#include <filesystem>
#include "enums.h"

#ifndef EASYLOGGING_INITIALIZED
#define EASYLOGGING_INITIALIZED
#include "easylogging++.h"
INITIALIZE_EASYLOGGINGPP
#endif

namespace AudioProcessingWrapperClassUnitTest
{
    class AudioProcessingWrapperTest : public testing::Test
    {
    protected:
        std::unique_ptr<FilterWrapper> filter_wrapper_;

        AudioProcessingWrapperTest() = default;
        virtual ~AudioProcessingWrapperTest() = default;
        virtual void SetUp()
        {
            filter_wrapper_.reset(new FilterWrapper());
            CleanUpUtProducts();
        }

        virtual void TearDown()
        {
            filter_wrapper_.reset();
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
    };
       
    // Positive Tests
    TEST_F(AudioProcessingWrapperTest, ApplyBwLowPassFilterTest)
    {
        wave::File input_wave_file;
        int status = VST_ERROR_STATUS::SUCCESS;
        if (input_wave_file.Open(INPUT_WAVE_PATH, wave::kIn))
        {
            status = VST_ERROR_STATUS::OPEN_FILE_ERROR;
        }
        EXPECT_EQ(status, VST_ERROR_STATUS::SUCCESS);

        std::vector<float> content;

        if (input_wave_file.Read(&content))
        {
            status = VST_ERROR_STATUS::OPEN_FILE_ERROR;
        }
        EXPECT_EQ(status, VST_ERROR_STATUS::SUCCESS);
        std::vector<float> output(content.size(), 1);

        status = filter_wrapper_->Init(static_cast<size_t>(input_wave_file.sample_rate()));
        EXPECT_EQ(status, VST_ERROR_STATUS::SUCCESS);

        status = filter_wrapper_->SetEnableProcessing(true);
        EXPECT_EQ(status, VST_ERROR_STATUS::SUCCESS);

        status = filter_wrapper_->Process(content, output);
        EXPECT_EQ(status, VST_ERROR_STATUS::SUCCESS);
#ifndef __APPLE__
        std::vector<float> ref;
        status = LoadWave(REF_FILTRATED, &ref);
        EXPECT_EQ(status, VST_ERROR_STATUS::SUCCESS);
        ASSERT_THAT(output, testing::Pointwise(testing::FloatNear(PRECISION_9_DECIMAL_PLACES), ref));
#endif //!__APPLE__
    }

}
