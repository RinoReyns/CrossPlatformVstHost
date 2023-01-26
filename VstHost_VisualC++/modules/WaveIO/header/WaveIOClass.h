#ifndef WAVE_IO_CLASS
#define WAVE_IO_CLASS

#include <memory>
#include <iostream>
#include "enums.h"
#include <vector>

struct WaveDataContainer{
    std::vector<float> data;
    uint64_t frame_number;
    int sample_rate;
    int channel_number;
    int bits_per_sample;
    std::string file_path;
};      

class WaveIOClass
{
public:
    WaveIOClass() = default;
    ~WaveIOClass() = default;

    int LoadWave(WaveDataContainer* data);

};

#endif // WAVE_IO_CLASS
