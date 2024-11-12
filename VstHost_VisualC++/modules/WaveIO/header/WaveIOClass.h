#ifndef WAVE_IO_CLASS
#define WAVE_IO_CLASS

#include <memory>
#include <iostream>
#include "enums.h"
#include <vector>


class WaveDataContainer
{
public:
    WaveDataContainer(std::string file_path) { file_path_ = file_path; };
    ~WaveDataContainer() = default;

public:
    std::vector<float> data;
    uint64_t frame_number;
    int sample_rate;
    uint16_t channel_number;
    uint16_t bits_per_sample;
    std::string file_path_;
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
