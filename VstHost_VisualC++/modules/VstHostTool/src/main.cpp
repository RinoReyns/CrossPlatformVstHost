#include "VstHostTool.h"
#include <memory>

int main(int argc, char* argv[])
{   
    std::unique_ptr<VstHostTool> vst_host_tool(new VstHostTool());
    vst_host_tool->PrepareArgs(argc, argv);
    return vst_host_tool->Run();   
}
