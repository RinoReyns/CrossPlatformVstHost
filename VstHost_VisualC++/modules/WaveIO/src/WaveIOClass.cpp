#include "WaveIOClass.h"
#include "file.h"
#include "easylogging++.h"

int WaveIOClass::LoadWave(WaveDataContainer* data)
{
    if (data->file_path_ == "")
    {
        return VST_ERROR_STATUS::PATH_NOT_EXISTS;
    }

    wave::File input_wave_file;
     if (input_wave_file.Open(data->file_path_, wave::kIn))
    {
        LOG(ERROR) << "Following file can't be opened: " << data->file_path_;
        return VST_ERROR_STATUS::OPEN_FILE_ERROR;
    }

    std::vector<float> content;
    if (input_wave_file.Read(&content))
    {
        return VST_ERROR_STATUS::READ_WRITE_ERROR;
    }

    if (input_wave_file.sample_rate() != data->GetProcessingSamplingRate())
    {
        // TODO:
        // implement resampling
        return VST_ERROR_STATUS::UNSUPPORTED_SAMPLING_RATE;
    }

    data->data = content;
    data->bits_per_sample = input_wave_file.bits_per_sample();
    data->channel_number = input_wave_file.channel_number();

    data->frame_number = input_wave_file.frame_number();
    data->signal_sampling_rate_ = input_wave_file.sample_rate();

    return VST_ERROR_STATUS::SUCCESS;
}

int WaveIOClass::SaveWave(WaveDataContainer* data)
{
    if (data->file_path_ == "")
    {
        return VST_ERROR_STATUS::PATH_NOT_EXISTS;
    }

    wave::File write_file;
    if (write_file.Open(data->file_path_, wave::kOut))
    {
        return VST_ERROR_STATUS::OPEN_FILE_ERROR;
    }

    write_file.set_sample_rate(static_cast<uint32_t>(data->signal_sampling_rate_));
    write_file.set_bits_per_sample(data->bits_per_sample);
    write_file.set_channel_number(data->channel_number);

    if (write_file.Write(data->data))
    {
        return VST_ERROR_STATUS::READ_WRITE_ERROR;
    }

    return VST_ERROR_STATUS::SUCCESS;
}

