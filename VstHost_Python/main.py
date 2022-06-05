from Modules.VstHostLibWrapper import VstHostLibWrapper
from Modules.Common import CheckStatus


if __name__ == "__main__":
    # TODO:
    # add arg parser
    vst_host_ = VstHostLibWrapper()
    status = vst_host_.LoadLibrary("AudioHostLib.dll")
    CheckStatus(status)

    status = vst_host_.CApiInitialize()
    CheckStatus(status)

    status = vst_host_.CApiSetVerbosity(0)
    CheckStatus(status)

    status = vst_host_.CApiCreatePluginInstance(b'adelay.vst3', b'plugin_1')
    CheckStatus(status)

    status = vst_host_.CApiSetPluginParameters(b'adelay_config.json', b'plugin_1')
    CheckStatus(status)

    status = vst_host_.CApiGetPluginParameters(b'plugin_params_py.json', b'plugin_1')
    CheckStatus(status)

    status = vst_host_.CApiProcessWaveFile(b'sine_440.wav', b'test.wav')
    CheckStatus(status)

    status = vst_host_.CApiDeleteInstance()
    CheckStatus(status)
    print(status)
