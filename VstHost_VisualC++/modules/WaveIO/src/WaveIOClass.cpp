#include "WaveIOClass.h"
#include "file.h"

int WaveIOClass::LoadWave(WaveDataContainer* data)
{
	if (data->file_path == "")
	{
		return VST_ERROR_STATUS::PATH_NOT_EXISTS;
	}
	wave::File input_wave_file;
	if (input_wave_file.Open(data->file_path, wave::kIn))
	{
		return VST_ERROR_STATUS::OPEN_FILE_ERROR;
	}

	std::vector<float> content;

	if (input_wave_file.Read(&content))
	{

		return VST_ERROR_STATUS::READ_WRITE_ERROR;
	}

	data->data = content;
	data->bits_per_sample = input_wave_file.bits_per_sample();
	data->channel_number = input_wave_file.channel_number();

	data->frame_number = input_wave_file.frame_number();
	data->sample_rate = input_wave_file.sample_rate();

	return VST_ERROR_STATUS::SUCCESS;
}
