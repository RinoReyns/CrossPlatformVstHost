import ctypes
import logging

from Modules.Enums import Status


class VstHostLibWrapper:
    def __init__(self):
        self._lib_instance = None
        self._vst_host_instance = None
        self._c_api_initialize = None
        self._c_api_delete_instance = None
        self._c_api_set_verbosity = None
        self._c_api_create_plugin_instance = None
        self._c_api_set_plugin_parameters = None
        self._c_api_get_plugin_parameters = None
        self._c_api_process_wave_file = None

    def __del__(self):
        if self._vst_host_instance is not None:
            self.CApiDeleteInstance()

    def _MapFucntions(self):
        if self._c_api_initialize is not None:
            return Status.FUNCTION_ALREADY_MAPED.value
        self._c_api_initialize = self._lib_instance.CApiInitialize
        self._c_api_initialize.restype = ctypes.c_void_p

        if self._c_api_delete_instance is not None:
            return Status.FUNCTION_ALREADY_MAPED.value

        self._c_api_delete_instance = self._lib_instance.CApiDeleteInstance
        self._c_api_delete_instance.argtypes = [ctypes.c_void_p]

        if self._c_api_set_verbosity is not None:
            return Status.FUNCTION_ALREADY_MAPED.value

        self._c_api_set_verbosity = self._lib_instance.CApiSetVerbosity
        self._c_api_set_verbosity.argtypes = [ctypes.c_void_p,  ctypes.c_int]

        if self._c_api_create_plugin_instance is not None:
            return Status.FUNCTION_ALREADY_MAPED.value

        self._c_api_create_plugin_instance = self._lib_instance.CApiCreatePluginInstance
        self._c_api_create_plugin_instance.argtypes = [ctypes.c_void_p, ctypes.c_char_p]

        if self._c_api_set_plugin_parameters is not None:
            return Status.FUNCTION_ALREADY_MAPED.value

        self._c_api_set_plugin_parameters = self._lib_instance.CApiSetPluginParameters
        self._c_api_set_plugin_parameters.argtypes = [ctypes.c_void_p, ctypes.c_char_p]

        if self._c_api_get_plugin_parameters is not None:
            return Status.FUNCTION_ALREADY_MAPED.value

        self._c_api_get_plugin_parameters = self._lib_instance.CApiGetPluginParameters
        self._c_api_get_plugin_parameters.argtypes = [ctypes.c_void_p, ctypes.c_char_p, ctypes.c_char_p]

        if self._c_api_process_wave_file is not None:
            return Status.FUNCTION_ALREADY_MAPED.value

        self._c_api_process_wave_file = self._lib_instance.CApiProcessWaveFile
        self._c_api_process_wave_file.argtypes = [ctypes.c_void_p,
                                                  ctypes.c_char_p,
                                                  ctypes.c_char_p]
        return Status.SUCCESS.value

    def LoadLibrary(self, lib_path):
        if self._lib_instance is not None:
            return Status.LIB_ALREADY_LOADED.value

        try:
            self._lib_instance = ctypes.CDLL(lib_path)
            logging.info(f"Lib {lib_path} has been loaded successfully.")
            status = self._MapFucntions()
            return status
        except Exception as e:
            logging.error(f'Lib loading failed with status:\n{e}.')
            return Status.LOAD_LIB_ERROR.value

    def CApiInitialize(self):
        if self._vst_host_instance is not None:
            return Status.VST_HOST_INSTANCE_ALREADY_CREATED.value

        self._vst_host_instance = self._c_api_initialize()

        if self._vst_host_instance < 0:
            return Status.VST_HOST_INIT_ERROR.value
        return Status.SUCCESS.value

    def CApiDeleteInstance(self):
        status = self._c_api_delete_instance(self._vst_host_instance)
        self._vst_host_instance = None
        return status

    def CApiSetVerbosity(self, value):
        return self._c_api_set_verbosity(self._vst_host_instance, value)

    def CApiCreatePluginInstance(self, plugin_path: bytes, plugin_name: bytes):
        if type(plugin_path) is not bytes or type(plugin_name) is not bytes:
            return Status.WRONG_DATA_TYPE.value
        return self._c_api_create_plugin_instance(self._vst_host_instance,
                                                  ctypes.c_char_p(plugin_path),
                                                  plugin_name)

    def CApiSetPluginParameters(self, config_path: bytes, plugin_name: bytes):
        if type(config_path) is not bytes or type(plugin_name) is not bytes:
            return Status.WRONG_DATA_TYPE.value
        return self._c_api_set_plugin_parameters(self._vst_host_instance,
                                                 ctypes.c_char_p(config_path),
                                                 plugin_name)

    def CApiGetPluginParameters(self, config_path: bytes, plugin_name: bytes):
        if type(config_path) is not bytes or type(plugin_name) is not bytes:
            return Status.WRONG_DATA_TYPE.value
        return self._c_api_get_plugin_parameters(self._vst_host_instance,
                                                 ctypes.c_char_p(config_path),
                                                 plugin_name)

    def CApiProcessWaveFile(self, input_wave_path: bytes, output_wave_path: bytes):
        if type(input_wave_path) is not bytes or type(output_wave_path) is not bytes:
            return Status.WRONG_DATA_TYPE.value
        return self._c_api_process_wave_file(self._vst_host_instance,
                                             ctypes.c_char_p(input_wave_path),
                                             ctypes.c_char_p(output_wave_path))
