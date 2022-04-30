import ctypes

if __name__ == "__main__":
    vst_host_instance = ctypes.CDLL("C:\\Users\\RinoReyns\\Desktop\\Projekty\\VstHost\\VstHost_VisualC++\\build\\bin\\Debug\\AudioHostLib.dll")

    vst_host_instance.CApiInitialize.restype = ctypes.c_void_p

    vst_instance = vst_host_instance.CApiInitialize()
    vst_host_instance.CApiSetVerbosity.argtypes = [ctypes.c_void_p,  ctypes.c_int]
    vst_host_instance.CApiSetVerbosity(vst_instance, 1)

    vst_host_instance.CApiCreatePluginInstance.argtypes = [ctypes.c_void_p, ctypes.c_char_p]
    status = vst_host_instance.CApiCreatePluginInstance(vst_instance, ctypes.c_char_p(b'adelay.vst3'))

    vst_host_instance.CApiSetPluginParameters.argtypes = [ctypes.c_void_p, ctypes.c_char_p]
    status = vst_host_instance.CApiSetPluginParameters(vst_instance,
                                                       ctypes.c_char_p(b'pretty_test.json'))

    vst_host_instance.CApiGetPluginParameters.argtypes = [ctypes.c_void_p, ctypes.c_char_p]
    status = vst_host_instance.CApiGetPluginParameters(vst_instance,
                                                       ctypes.c_char_p(b'plugin_params_py.json'))

    vst_host_instance.CApiProcessWaveFileWithSinglePlugin.argtypes = [ctypes.c_void_p,
                                                                      ctypes.c_char_p,
                                                                      ctypes.c_char_p]
    status = vst_host_instance.CApiProcessWaveFileWithSinglePlugin(vst_instance,
                                                                   ctypes.c_char_p(b'out_trim.wav'),
                                                                   ctypes.c_char_p(b'test.wav'))

    vst_host_instance.CApiDeleteInstance.argtypes = [ctypes.c_void_p]
    status = vst_host_instance.CApiDeleteInstance(vst_instance)
    print(status)
