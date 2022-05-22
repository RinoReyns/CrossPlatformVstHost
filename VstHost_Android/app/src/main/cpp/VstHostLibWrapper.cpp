#include <jni.h>
#include <audiohost.h>
#include <memory>
#include <iostream>
#include <filesystem>
#include <sys/utsname.h>

extern "C"
{
    JNIEXPORT jstring JNICALL Java_com_rinoreyns_vsthost_1android_MainActivity_getPlatformId(JNIEnv *env, jobject thi)
    {
        struct utsname unameData;
        int res = uname (&unameData);
        if (res != 0)
            return {};
        return (env)->NewStringUTF(unameData.machine);
    }

    JNIEXPORT jint JNICALL
    Java_com_rinoreyns_vsthost_1android_MainActivity_intFromJNI(JNIEnv *env, jobject thi, jstring path) {

        std::unique_ptr<AudioProcessingVstHost> test(new AudioProcessingVstHost());
        jboolean isCopy;

        const char *convertedValue = (env)->GetStringUTFChars(path, &isCopy);
        std::string str = convertedValue;
        int status = 0;
        try {
            status = test->CreatePluginInstance(str + "/adelay.vst3", "plugin_1");
            status = test->ProcessWaveFile(str + "/sine_440.wav", str + "out.wav");
            // TODO:
            // add wave_io lib
//            if (status == 0)
//            {
//                wave::File input_wave_file;
//                if (input_wave_file.Open(wave_path, wave::kIn))
//                {
//                    return VST_ERROR_STATUS::OPEN_FILE_ERROR;
//                }
//
//            }
        }
        catch(...)
        {
            std::exception_ptr p = std::current_exception();
            std::cout << "" << std::endl;
        }
        return static_cast<jint>(status);
    }

    JNIEXPORT jstring JNICALL
    Java_com_rinoreyns_vsthost_1android_ExampleInstrumentedTest_getPlatformId(JNIEnv *env, jobject thiz) {
        struct utsname unameData;
        int res = uname (&unameData);
        if (res != 0)
            return {};
        return (env)->NewStringUTF(unameData.machine);
    }

    JNIEXPORT jint JNICALL
    Java_com_rinoreyns_vsthost_1android_ExampleInstrumentedTest_intFromJNI(JNIEnv *env, jobject thiz, jstring path) {
        std::unique_ptr<AudioProcessingVstHost> test(new AudioProcessingVstHost());
        jboolean isCopy;

        const char *convertedValue = (env)->GetStringUTFChars(path, &isCopy);
        std::string str = convertedValue;
        int status = 0;
        try {
            status = test->CreatePluginInstance(str + "/adelay.vst3", "plugin_1");
            status = test->ProcessWaveFile(str + "/sine_440.wav", str + "out.wav");
            test->Terminate();
            // TODO:
            // add wave_io lib
    //            if (status == 0)
    //            {
    //                wave::File input_wave_file;
    //                if (input_wave_file.Open(wave_path, wave::kIn))
    //                {
    //                    return VST_ERROR_STATUS::OPEN_FILE_ERROR;
    //                }
    //
    //            }
        }
        catch(...)
        {
            std::exception_ptr p = std::current_exception();
            std::cout << "" << std::endl;
        }
        return static_cast<jint>(status);
    }


}

