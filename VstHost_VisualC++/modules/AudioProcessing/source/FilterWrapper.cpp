#include "FilterWrapper.h"

FilterWrapper::FilterWrapper()
{
}

FilterWrapper::~FilterWrapper()
{
	if (bw_low_pass_filter_ != nullptr)
	{
		free_bw_low_pass(bw_low_pass_filter_);
		LOG(INFO) << "bw_low_pass_filter released sucessfully." << std::endl;
	}
}

int FilterWrapper::Init(float sampling_rate)
{
	// TODO:
	// parameterization for each module is needed.
	 bw_low_pass_filter_ = create_bw_low_pass_filter(
		 8,
		 sampling_rate,
		 400);

	return VST_ERROR_STATUS::SUCCESS;
}

int FilterWrapper::ApplyBwLowPassFilter(std::vector<float> input,
	std::vector<float>& output)
{
	if (bw_low_pass_filter_ == nullptr)
	{
		return VST_ERROR_STATUS::NULL_POINTER;
	}

	for (int i = 0; i < input.size(); i++) {
		output[i] = bw_low_pass(bw_low_pass_filter_, input[i]);
	}

	return VST_ERROR_STATUS::SUCCESS;;
}
