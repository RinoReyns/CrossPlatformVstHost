import unittest
import ctypes
import os
from sys import platform
import sys


class TestCApi(unittest.TestCase):

    def setUp(self):
        self.dir_path = os.path.dirname(__file__)
        self.ut_data = os.path.join(self.dir_path, "..", "..", "VstHost_VisualC++", "modules", "UnitTests", "data")
        self.class_pointer = None
        self.vst_lib_instance = None
        self.output_wave_path = "output.wav"
        self.dump_params_config = "plugin_params.json"
        if platform == "linux" or platform == "linux2":
           self.vst_host_lib = "libAudioHostLib.so"
           self.vst_plugin_path = "adelay.vst3"
        elif platform == "darwin":
           sys.exit("MacOs is not supported.")
        elif platform == "win32":
           self.vst_host_lib = "AudioHostLib.dll"
           self.vst_plugin_path = "adelay.vst3"

    def tearDown(self):
        if self.class_pointer is not None:
            self.vst_lib_instance.CApiDeleteInstance(self.class_pointer)
        if os.path.exists(self.output_wave_path):
            os.remove(self.output_wave_path)
        if os.path.exists(self.dump_params_config):
            os.remove(self.dump_params_config)

    def test_run_basic_flow(self):
        # TODO:
        # clean up
        # print UT header etc
        # Add flag "local" to don't copy dll's and vst plugin and use from "debug" build of c++ code instead
        self.vst_lib_instance = ctypes.CDLL(os.path.join(self.dir_path, self.vst_host_lib))

        self.vst_lib_instance.CApiInitialize.restype = ctypes.c_void_p

        self.class_pointer = self.vst_lib_instance.CApiInitialize()
        self.vst_lib_instance.CApiSetVerbosity.argtypes = [ctypes.c_void_p, ctypes.c_int]
        self.vst_lib_instance.CApiSetVerbosity(self.class_pointer, 1)

        self.vst_lib_instance.CApiCreatePluginInstance.argtypes = [ctypes.c_void_p, ctypes.c_char_p]
        status = self.vst_lib_instance.CApiCreatePluginInstance(
            self.class_pointer,
            ctypes.c_char_p(os.path.join(self.dir_path, self.vst_plugin_path).encode("ascii")))
        self.assertEqual(status, 0)
        self.vst_lib_instance.CApiSetPluginParameters.argtypes = [ctypes.c_void_p, ctypes.c_char_p]
        print(os.path.join(self.ut_data, "adelay_config.json"))
        status = self.vst_lib_instance.CApiSetPluginParameters(
            self.class_pointer,
            ctypes.c_char_p(os.path.join(self.ut_data, "adelay_config.json").encode('ascii')))
        self.assertEqual(status, 0)
        self.vst_lib_instance.CApiGetPluginParameters.argtypes = [ctypes.c_void_p, ctypes.c_char_p]
        status = self.vst_lib_instance.CApiGetPluginParameters(
            self.class_pointer,
            ctypes.c_char_p(self.dump_params_config.encode("ascii")))
        self.assertEqual(status, 0)
        self.vst_lib_instance.CApiProcessWaveFileWithSinglePlugin.argtypes = [ctypes.c_void_p,
                                                                              ctypes.c_char_p,
                                                                              ctypes.c_char_p]
        status = self.vst_lib_instance.CApiProcessWaveFileWithSinglePlugin(
            self.class_pointer,
            ctypes.c_char_p(os.path.join(self.ut_data, "sine_440.wav").encode('ascii')),
            ctypes.c_char_p(self.output_wave_path.encode("ascii")))

        self.assertEqual(status, 0)
        self.vst_lib_instance.CApiDeleteInstance.argtypes = [ctypes.c_void_p]
        status = self.vst_lib_instance.CApiDeleteInstance(self.class_pointer)
        self.class_pointer = None
        self.assertEqual(status, 0)
