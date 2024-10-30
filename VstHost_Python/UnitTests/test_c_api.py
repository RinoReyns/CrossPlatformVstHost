import unittest
import os
from sys import platform
import soundfile as sf
import inspect
import re
import math

from Modules.VstHostLibWrapper import VstHostLibWrapper
from Modules.Enums import Status


class TestCApi(unittest.TestCase):
    MAX_TITLE_LENGTH = 150

    def setUp(self):
        self.vst_lib_instance = None
        self.dir_path = os.path.dirname(__file__)
        self.ut_data = os.path.join(self.dir_path, "..", "..", "VstHost_VisualC++", "modules", "UnitTests", "data")
        self.output_wave_path = "output.wav"
        self._ref_output = os.path.join(self.ut_data, "sine_440_output_ref_0.4_delay.wav")
        self._plugin_name = "plugin_1".encode("ascii")
        self._input_wave_path = os.path.join(self.ut_data, "sine_440.wav").encode('ascii')
        self._dump_params_config = "plugin_params.json"
        self._adelay_plugin_config = os.path.join(self.ut_data, "adelay_config.json")
        self._vst_host = VstHostLibWrapper()
        if platform == "linux" or platform == "linux2":
            self.vst_host_lib = "libAudioHostLib.so"
            self.vst_plugin_path = "adelay.vst3"
        elif platform == "darwin":
            self.vst_host_lib = "libAudioHostLib.dylib"
            self.vst_plugin_path = "adelay.vst3"
        elif platform == "win32":
            self.vst_host_lib = "AudioHostLib.dll"
            self.vst_plugin_path = "adelay.vst3"

        self._full_vst_host_lib_path = os.path.join(self.dir_path, self.vst_host_lib)
        self._vst_host = VstHostLibWrapper()

    def tearDown(self):
        if os.path.exists(self.output_wave_path):
            os.remove(self.output_wave_path)
        if os.path.exists(self._dump_params_config):
            os.remove(self._dump_params_config)

    def _CompareWaveFiles(self, output, ref):
        output_data, _ = sf.read(output)
        ref_data, _ = sf.read(ref)
        return (output_data == ref_data).all()

    def _PrintUnitTestName(self):
        test_name = ' '.join(re.findall('[A-Z][^A-Z]*', inspect.currentframe().f_back.f_code.co_name))
        test_name = f"Running Test: {test_name}"
        string_length = len(test_name)
        if string_length % 2:
            # odd
            left_separator = "=" * math.floor(((self.MAX_TITLE_LENGTH - string_length) / 2) - 1)
            right_separator = "=" * math.ceil(((self.MAX_TITLE_LENGTH - string_length) / 2) - 1)
            test_name = f"{left_separator} {test_name} {right_separator}"
        else:
            # even
            separator = "=" * int(((self.MAX_TITLE_LENGTH - string_length)/2) - 1)
            test_name = f"{separator} {test_name} {separator}"
        print(test_name)

    # TODO:
    # create multiple plugins and check performance

    def test_LoadWrongLibPath(self):
        self._PrintUnitTestName()
        status = self._vst_host.LoadLibrary(f"dummy.{self.vst_host_lib.split('.')[1]}")
        self.assertEqual(status, Status.LOAD_LIB_ERROR.value)

    def test_InitVstHostTwice(self):
        self._PrintUnitTestName()

        status = self._vst_host.LoadLibrary(self._full_vst_host_lib_path)
        self.assertEqual(status, Status.SUCCESS.value)

        status = self._vst_host.CApiInitialize()
        self.assertEqual(status, Status.SUCCESS.value)

        status = self._vst_host.CApiInitialize()
        self.assertEqual(status, Status.VST_HOST_INSTANCE_ALREADY_CREATED.value)

    def test_RunBasicFlow(self):
        self._PrintUnitTestName()

        status = self._vst_host.LoadLibrary(self._full_vst_host_lib_path)
        self.assertEqual(status, Status.SUCCESS.value)

        status = self._vst_host.CApiInitialize()
        self.assertEqual(status, Status.SUCCESS.value)

        status = self._vst_host.CApiSetVerbosity(0)
        self.assertEqual(status, Status.SUCCESS.value)

        print(os.listdir(self.dir_path))
        print(os.path.exit(os.path.join(self.dir_path, self.vst_plugin_path).encode("ascii")))
        status = self._vst_host.CApiCreatePluginInstance(
            os.path.join(self.dir_path, self.vst_plugin_path).encode("ascii"),
            self._plugin_name)
        self.assertEqual(status, Status.SUCCESS.value)

        status = self._vst_host.CApiSetPluginParameters(self._adelay_plugin_config.encode('ascii'),
                                                        self._plugin_name)
        self.assertEqual(status, Status.SUCCESS.value)

        status = self._vst_host.CApiGetPluginParameters(self._dump_params_config.encode("ascii"),
                                                        self._plugin_name)
        self.assertEqual(status, Status.SUCCESS.value)

        status = self._vst_host.CApiProcessWaveFile(self._input_wave_path,
                                                    self.output_wave_path.encode("ascii"))
        self.assertEqual(status, Status.SUCCESS.value)

        status = self._vst_host.CApiDeleteInstance()
        self.assertEqual(status, Status.SUCCESS.value)
        self.assertTrue(self._CompareWaveFiles(self.output_wave_path, self._ref_output))
