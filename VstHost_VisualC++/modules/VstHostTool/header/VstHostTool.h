#ifndef VST_HOST_TOOL
#define VST_HOST_TOOL

#include <memory>
#include "arg_parser.h"

class VstHostTool
{
    public:
        VstHostTool();
        ~VstHostTool() = default;
        int PrepareArgs(int argc, char* argv[]);
        int PrepareArgs(std::vector<std::string> args);
        int Run();

    private:
        int RunAudioCapture();
        int RunAudioRender();

    private:
        std::unique_ptr<ArgParser> arg_parser_;
        std::vector<std::string> parser_arguments_;
};

#endif // VST_HOST_TOOL
