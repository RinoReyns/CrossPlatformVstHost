#include <jni.h>
#include <audiohost.h>
#include <memory>
#include <iostream>
#include <filesystem>
#include <sys/utsname.h>
#include "file.h"

extern "C"
{
    JNIEXPORT jstring JNICALL
    GetPlatformId(JNIEnv *env, jobject thiz)
    {
        struct utsname unameData;
        int res = uname (&unameData);
        if (res != 0)
            return {};
        return (env)->NewStringUTF(unameData.machine);
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

    JNIEXPORT jint JNICALL
    SimpleHostRun(JNIEnv *env, jobject thi, jstring path)
    {
        std::unique_ptr<AudioProcessingVstHost> vst_host_(new AudioProcessingVstHost());
        jboolean isCopy;

        const char *convertedValue = (env)->GetStringUTFChars(path, &isCopy);
        std::string str = convertedValue;
        int status = 0;
        try {
            status = vst_host_->CreatePluginInstance(str + "/adelay.vst3", "plugin_1");
            if (status != 0)
            {
                return status;
            }
            std::string output_wave_path_ = str + "/out.wav";
            status = vst_host_->ProcessWaveFile(str + "/sine_440.wav", output_wave_path_);
            vst_host_->Terminate();
            // TODO:
            // add wave_io lib
            if (status == 0)
            {
                std::vector<float> processed_data;
                status = LoadWave(output_wave_path_, &processed_data);
                RETURN_ERROR_IF_NOT_SUCCESS(status);

                std::vector<float> ref_data;
                status = LoadWave(str + "/sine_440_output_ref.wav", &ref_data);
                RETURN_ERROR_IF_NOT_SUCCESS(status);
                if (processed_data != ref_data)
                {
                    status = -1;
                }
            }
        }
        catch(...)
        {
            std::exception_ptr p = std::current_exception();
            std::cout << "" << std::endl;
        }
        return static_cast<jint>(status);
    }

    JNIEXPORT jstring JNICALL
    Java_com_rinoreyns_vsthost_1android_ExampleInstrumentedTest_getPlatformId(JNIEnv *env, jobject thiz)
    {
        return GetPlatformId(env, thiz);
    }

    JNIEXPORT jstring JNICALL Java_com_rinoreyns_vsthost_1android_MainActivity_getPlatformId(JNIEnv *env, jobject thi)
    {
        return GetPlatformId(env, thi);
    }

    JNIEXPORT jint JNICALL
    Java_com_rinoreyns_vsthost_1android_MainActivity_intFromJNI(JNIEnv *env, jobject thi, jstring path)
    {
        return SimpleHostRun(env, thi, path);
    }

    JNIEXPORT jint JNICALL
    Java_com_rinoreyns_vsthost_1android_ExampleInstrumentedTest_intFromJNI(JNIEnv *env, jobject thiz, jstring path)
    {
        return SimpleHostRun(env, thiz, path);
    }
}
