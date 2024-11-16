#ifndef WAVE_IO_CLASS
#define WAVE_IO_CLASS

#include <memory>
#include <iostream>
#include "enums.h"
#include <vector>


class WaveDataContainer
{
public:
    WaveDataContainer(std::string file_path, size_t processing_sampling_rate) { file_path_ = file_path; processing_sampling_rate_ = processing_sampling_rate; };
    size_t GetProcessingSamplingRate() { return processing_sampling_rate_;}
    int SetParams(WaveDataContainer* data_container) 
    { 
        data.resize(data_container->data.size(), 0); 
        channel_number = data_container->channel_number;
        bits_per_sample = data_container->bits_per_sample;
        frame_number = data_container->frame_number;
        return VST_ERROR_STATUS::SUCCESS; 
    };

    size_t GetDataSize() { return data.size(); };
    ~WaveDataContainer() = default;

public:
    std::vector<float> data;
    uint64_t frame_number;
    size_t signal_sampling_rate_;
    uint16_t channel_number;
    uint16_t bits_per_sample;
    std::string file_path_;

private: 
    size_t processing_sampling_rate_ = 0;
};

class WaveIOClass
{
public:
    WaveIOClass() = default;
    ~WaveIOClass() = default;

    int LoadWave(WaveDataContainer* data);
    int SaveWave(WaveDataContainer* data);
};

#endif // WAVE_IO_CLASS
