#include "VstHostTool.h"
#include <memory>

int main(int argc, char* argv[])  // GCOVR_EXCL_START
{   
    std::unique_ptr<VstHostTool> vst_host_tool(new VstHostTool());
    int status = vst_host_tool->PrepareArgs(argc, argv);
    if (status != 0)
    { 
        return status;
    }
    return vst_host_tool->Run();   
}  // GCOVR_EXCL_STOP

