#pragma once

#include <iostream>
#include <map>
#include <vector>
#include <regex.h>


struct jsonParser {
    std::map<std::string, jsonParser> object;
    std::vector<std::string> array;
    std::string string;
    double num;
    bool boolean;
};