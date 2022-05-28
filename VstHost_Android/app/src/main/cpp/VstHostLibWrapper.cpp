#include <jni.h>
#include <audiohost.h>
#include <memory>
#include <iostream>
#include <filesystem>
#include <sys/utsname.h>

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

    JNIEXPORT jint JNICALL
    SimpleHostRun(JNIEnv *env, jobject thi, jstring path)
    {
        std::unique_ptr<AudioProcessingVstHost> test(new AudioProcessingVstHost());
        jboolean isCopy;

        const char *convertedValue = (env)->GetStringUTFChars(path, &isCopy);
        std::string str = convertedValue;
        int status = 0;
        try {
            status = test->CreatePluginInstance(str + "/adelay.vst3", "plugin_1");
            if (status != 0)
            {
                return status;
            }
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

    JNIEXPORT jstring JNICALL
    Java_com_rinoreyns_vsthost_1android_ExampleInstrumentedTest_getPlatformId(JNIEnv *env,
                                                                              jobject thiz)
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
    Java_com_rinoreyns_vsthost_1android_ExampleInstrumentedTest_intFromJNI(JNIEnv *env,
                                                                           jobject thiz,
                                                                           jstring path)
    {
        return SimpleHostRun(env, thiz, path);
    }
}
