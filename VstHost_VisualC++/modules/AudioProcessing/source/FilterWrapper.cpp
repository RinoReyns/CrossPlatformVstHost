#include "FilterWrapper.h"

FilterWrapper::FilterWrapper()
{
}

FilterWrapper::~FilterWrapper()
{
	if (bw_low_pass_filter_ != nullptr)
	{
		free_bw_low_pass(bw_low_pass_filter_);
		LOG(DEBUG) << "bw_low_pass_filter released sucessfully." << std::endl;
	}
}

int FilterWrapper::Init(size_t sampling_rate)
{
	if (sampling_rate < 0)
	{
		LOG(ERROR) << "Unsupported sampling rate: " << sampling_rate << "Hz.";
		return VST_ERROR_STATUS::UNSUPPORTED_SAMPLING_RATE;
	}

    // TODO:
	// parameterization for each module is needed.
	bw_low_pass_filter_ = create_bw_low_pass_filter(
		8,
		static_cast<FTR_PRECISION>(sampling_rate),
		400);

	RETURN_ERROR_IF_NULL(bw_low_pass_filter_);

	return VST_ERROR_STATUS::SUCCESS;
}

int FilterWrapper::Process(
	std::vector<float> input,
	std::vector<float>& output)
{
	if (!enable_processing_)
	{
		return VST_ERROR_STATUS::BYPASS;
	}

	if (bw_low_pass_filter_ == nullptr)
	{
		return VST_ERROR_STATUS::NULL_POINTER;
	}

	if (output.size() < input.size())
	{
		return VST_ERROR_STATUS::SIZE_MISSMATCH;
	}

	for (int i = 0; i < input.size(); i++) 
	{
		output[i] = bw_low_pass(bw_low_pass_filter_, input[i]);
	}

	return VST_ERROR_STATUS::SUCCESS;
}

int FilterWrapper::SetEnableProcessing(bool enable)
{
	enable_processing_ = enable;
	if (!enable_processing_)
	{
		return VST_ERROR_STATUS::BYPASS;
	}
	return VST_ERROR_STATUS::SUCCESS;
}
