#ifndef __CONFIG_H__
#define __CONFIG_H__

#include <memory>
#include <string>
#include <unordered_map>

struct State;

class Instruction
{
public:
    virtual ~Instruction() { }
    virtual void Perform(State &state) = 0;
    virtual std::string Name() const { return "Unspecified instruction"; }
};

std::ostream &operator <<(std::ostream &os, const Instruction &obj);

namespace Config {
    std::unique_ptr<Instruction> MakeInstruction(std::string command);
    typedef std::unordered_map<char, std::unique_ptr<Instruction>> CodeMap;
    CodeMap ReadFile(std::string filename);
}

#endif //__CONFIG_H__
