#include "string_control.h"

#include <iostream>
#include <fstream>
#include <stdexcept>
#include <string>

StringControl::StringControl(const std::string &filename) : 
    init_str(), str(), iteration_num(0), mapping(), str_is_processed(false)
{
    LoadFromFile(filename);
}

void StringControl::LoadFromFile(const std::string &filename)
{
    std::ifstream ifs(filename);
    if (!ifs) {
        throw std::runtime_error("Failed to open file " + filename + " for reading");
    }
    std::string temp_str, s, t;
    int temp_iteration_num;
    MappingType temp_map;
    ifs >> temp_str;
    ifs >> temp_iteration_num;
    if (!ifs) {
        throw std::runtime_error("Failed to read data from file " + filename);
    }
    int line = 0;
    while (ifs >> s >> t) {
        if (s.empty() && t.empty()) {
            break;
        }
        ++line;
        if (s.size() != 1) {
            std::cerr << "Error on line " << std::to_string(line);
            std::cerr << " in file " << filename;
            std::cerr << ": pattern must be a single char";
            throw std::runtime_error("Parse error");
        }
        temp_map[s[0]] = t;
    }

    init_str = temp_str;
    str = temp_str;
    iteration_num = temp_iteration_num;
    mapping = temp_map;
    str_is_processed = false;
}

// returns false if no mapping exists for any substring of str
bool StringControl::SubstituteAll()
{
    bool changed = false;
    for (size_t i = 0; i < str.size(); i++) {
        try {
            std::string r = mapping.at(str[i]);
            str.replace(i, 1, r);
            i += r.size() - 1; // don't overwrite fresh substr
            changed = true;
            //break;
        } catch(const std::out_of_range &e) { // no mapping found for this substring, do nothing
        }
    }
    return changed;
}

void StringControl::ProcessString(int iterations)
{
    for (int cur_iter = 0; cur_iter < iterations; cur_iter++) {
        if (!SubstituteAll()) {
            break; // can't substitute anything
        }
    }
    str_is_processed = true;
}

const std::string &StringControl::GetResult()
{
    if (!str_is_processed) {
        ProcessString(iteration_num);
        //std::cout << "Control string is " << str << std::endl;
    }
    return str;
}

std::string StringControl::GetResult(int iterations)
{
    std::string temp = str;
    str = init_str;
    ProcessString(iterations);
    std::string res = str;
    str = temp;
    return res;
}
