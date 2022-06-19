#ifndef UTILS_H
#define UTILS_H

#include "enums.h"
#include <vector>


class Utils
{
public:
    static VST_ERROR_STATUS SplitString(std::string, 
                                        std::string delimiter,
                                        std::vector<size_t>& splited_string);

};

#endif //UTILS_H
