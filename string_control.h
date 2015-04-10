#ifndef __STRING_CONTROL_H__
#define __STRING_CONTROL_H__

#include <string>
#include <unordered_map>

class StringControl
{
    private:
        typedef std::unordered_map<char, std::string> MappingType;
        std::string init_str;
        std::string str;
        int iteration_num;
        MappingType mapping;
        bool str_is_processed;

        bool SubstituteAll();
        void ProcessString(int iterations);
    public:
        StringControl(const std::string &filename);
        void LoadFromFile(const std::string &filename);
        const std::string &GetResult();
        std::string GetResult(int iterations);
};

#endif //__STRING_CONTROL_H__
