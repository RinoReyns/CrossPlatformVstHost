import unittest
import os
from sys import platform

from Modules.VstHostLibWrapper import VstHostLibWrapper
from Modules.Enums import Status


class TestCApi(unittest.TestCase):

    def setUp(self):
        self.dir_path = os.path.dirname(__file__)
        self.ut_data = os.path.join(self.dir_path, "..", "..", "VstHost_VisualC++", "modules", "UnitTests", "data")

        self.vst_lib_instance = None
        self._plugin_name = "plugin_1".encode("ascii")
        self.output_wave_path = "output.wav"
        self._input_wave_path = os.path.join(self.ut_data, "sine_440.wav")
        self.dump_params_config = "plugin_params.json"
        if platform == "linux" or platform == "linux2":
            self.vst_host_lib = "libAudioHostLib.so"
            self.vst_plugin_path = "adelay.vst3"
        elif platform == "darwin":
            self.vst_host_lib = "libAudioHostLib.dylib"
            self.vst_plugin_path = "adelay.vst3"
        elif platform == "win32":
            self.vst_host_lib = "AudioHostLib.dll"
            self.vst_plugin_path = "adelay.vst3"
        self._vst_host = VstHostLibWrapper()

    def tearDown(self):
        if os.path.exists(self.output_wave_path):
            os.remove(self.output_wave_path)
        if os.path.exists(self.dump_params_config):
            os.remove(self.dump_params_config)

    # TODO:
    # create multiple plugins and check performance

    def test_RunBasicFlow(self):
        # TODO:
        # print UT header etc
        # load wave and compare with reference
        # Add flag "local" to don't copy dll's and vst plugin and use from "debug" build of c++ code instead
        vst_host_ = VstHostLibWrapper()
        status = vst_host_.LoadLibrary(os.path.join(self.dir_path, self.vst_host_lib))
        self.assertEqual(status, Status.SUCCESS.value)

        status = vst_host_.CApiInitialize()
        self.assertEqual(status, Status.SUCCESS.value)

        status = vst_host_.CApiSetVerbosity(0)
        self.assertEqual(status, Status.SUCCESS.value)

        status = vst_host_.CApiCreatePluginInstance(
            os.path.join(self.dir_path, self.vst_plugin_path).encode("ascii"),
            self._plugin_name)
        self.assertEqual(status, Status.SUCCESS.value)

        status = vst_host_.CApiSetPluginParameters(os.path.join(self.ut_data, "adelay_config.json").encode('ascii'),
                                                   self._plugin_name)
        self.assertEqual(status, Status.SUCCESS.value)

        status = vst_host_.CApiGetPluginParameters(self.dump_params_config.encode("ascii"),
                                                   self._plugin_name)
        self.assertEqual(status, Status.SUCCESS.value)

        status = vst_host_.CApiProcessWaveFile(self._input_wave_path.encode('ascii'),
                                               self.output_wave_path.encode("ascii"))
        self.assertEqual(status, Status.SUCCESS.value)

        status = vst_host_.CApiDeleteInstance()
        self.assertEqual(status, Status.SUCCESS.value)
