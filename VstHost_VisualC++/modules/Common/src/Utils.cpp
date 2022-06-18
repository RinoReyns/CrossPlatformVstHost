#include "Utils.h"
#include <iostream>

VST_ERROR_STATUS Utils::SplitString(std::string string_to_split, std::string delimiter, std::vector<size_t>& splited_string)
{
    size_t start = 0;
    size_t end   = 0;
 
    while (end != -1) 
    {
        if (end != 0)
        {
            start = end + delimiter.size();
        }
        end = string_to_split.find(delimiter, start);
        splited_string.push_back(stoi(string_to_split.substr(start, end - start)));
    }
  
    if (splited_string.size() == 0)
    {
        return VST_ERROR_STATUS::NO_ELEMENT_FOUND;
    }
    return VST_ERROR_STATUS::SUCCESS;
}