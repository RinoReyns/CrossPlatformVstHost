#ifndef VST_HOST_TOOL
#define VST_HOST_TOOL

#include <memory>
#include "arg_parser.h"
#include "WaveProcessingPipeline.h"
#include "easylogging++.h"

#ifndef EASYLOGGINGPP_H
#include "easylogging++.h"
#endif

class VstHostTool
{
public:
    VstHostTool();
    ~VstHostTool() = default;
    int PrepareArgs(int argc, char* argv[]);
    int PrepareArgs(std::vector<std::string> args);
    int Run();

private:
    int OfflineProcessingPipeline();
    int EndpointProcessingPipeline();
    int ParsArgs();

private:
    std::unique_ptr<ArgParser> arg_parser_;
    std::vector<std::string> parser_arguments_;
};

#endif // VST_HOST_TOOL
