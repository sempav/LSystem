#include "config.h"

#include <cstdlib>
#include <iostream>
#include <fstream>
#include <memory>
#include <stack>
#include <string>
#include <vector>

#include <GL/glew.h>

#include <glm/glm.hpp>
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include <glm/gtx/rotate_vector.hpp>

#include "state.h"
#include "tree_painter.h"

const float DISTANCE_COEFF = 3.0f/3.0f;

using std::string;
using std::unique_ptr;
using std::make_unique;
using std::vector;


std::ostream &operator <<(std::ostream &os, const Instruction &obj) {
    os << obj.Name() << std::endl;
    return os;
}

class Dummy: public Instruction
{
public:
    void Perform(State &state) override { }
    std::string Name() const override { return "Dummy"; }
};

class MoveDraw: public Instruction
{
    float distance;
public:
    MoveDraw(float distance) : distance(distance) { }
    void Perform(State &state) override {
        glVertex3f(state.pos.x, state.pos.y, state.pos.z);
        state.pos += distance * state.dir *
                     static_cast<float>(std::pow(DISTANCE_COEFF, state.depth));
        glVertex3f(state.pos.x, state.pos.y, state.pos.z);
        state.depth++;
    }
    std::string Name() const override { return "MoveDraw"; }
};

class MoveNoDraw: public Instruction
{
    float distance;
public:
    MoveNoDraw(float distance) : distance(distance) { }
    void Perform(State &state) override {
        state.pos += distance * state.dir *
                     static_cast<float>(std::pow(DISTANCE_COEFF, state.depth));
        state.depth++;
    }
    std::string Name() const override { return "MoveNoDraw"; }
};

class RotateX: public Instruction
{
    float angle;
public:
    RotateX(float angle) : angle(angle) { }
    void Perform(State &state) override {
        state.dir = glm::rotate(state.dir, angle, state.x_axis);
        state.y_axis = glm::rotate(state.y_axis, angle, state.x_axis);
        state.z_axis = glm::rotate(state.z_axis, angle, state.x_axis);
    }
    std::string Name() const override { return "RotateX"; }
};

class RotateY: public Instruction
{
    float angle;
public:
    RotateY(float angle) : angle(angle) { }
    void Perform(State &state) override {
        state.dir = glm::rotate(state.dir, angle, state.y_axis);
        state.x_axis = glm::rotate(state.x_axis, angle, state.y_axis);
        state.z_axis = glm::rotate(state.z_axis, angle, state.y_axis);
    }
    std::string Name() const override { return "RotateY"; }
};

class RotateZ: public Instruction
{
    float angle;
public:
    RotateZ(float angle) : angle(angle) { }
    void Perform(State &state) override {
        state.dir = glm::rotate(state.dir, angle, state.z_axis);
        state.x_axis = glm::rotate(state.x_axis, angle, state.z_axis);
        state.y_axis = glm::rotate(state.y_axis, angle, state.z_axis);
    }
    std::string Name() const override { return "RotateZ"; }
};

class StateStack {
    static StateStack *instance;

    std::stack<State> state_stack;

    StateStack(const StateStack &a) = delete;
    StateStack &operator= (const StateStack &a) = delete;
    StateStack() : state_stack() { }
    ~StateStack() { }
public:
    static StateStack &GetInstance() {
        if (!instance) {
            instance = new StateStack();
        }
        return *instance;
    }
    static void ResetInstance() {
        if (instance) {
            delete instance;
            instance = nullptr;
        }
    }

    bool empty() const            { return state_stack.empty(); }
    void push(const State &state) { state_stack.push(state);    }
    void pop()                    { state_stack.pop();          }
    const State& top() const      { return state_stack.top();   }
};

StateStack *StateStack::instance = nullptr;

class Push: public Instruction
{
public:
    void Perform(State &state) override {
        StateStack::GetInstance().push(state);
    }
    std::string Name() const override { return "Push"; }
};

class Pop: public Instruction
{
public:
    void Perform(State &state) override {
        assert(!StateStack::GetInstance().empty());
        state = StateStack::GetInstance().top();
        StateStack::GetInstance().pop();
    }
    std::string Name() const override { return "Pop"; }
};

class CompoundAction: public Instruction
{
    vector<unique_ptr<Instruction>> instructions;
public:
    CompoundAction(vector<unique_ptr<Instruction>> &&instr) : instructions()
    {
        instructions.resize(instr.size());
        for (size_t i = 0; i < instr.size(); ++i) {
            instructions[i] = std::move(instr[i]);
            assert(instr[i] == nullptr);
        }
    }
    CompoundAction(unique_ptr<Instruction> &&instr) : instructions()
    {
        instructions.push_back(std::move(instr));
        assert(instr == nullptr);
    }
    void AddInstruction(unique_ptr<Instruction> &&instr)
    {
        instructions.push_back(std::move(instr));
        assert(instr == nullptr);
    }
    void Perform(State &state) override {
        for (const unique_ptr<Instruction> &instr : instructions) {
            instr->Perform(state);
        }
    }
    std::string Name() const override {
        std::string res = "CompoundAction:";
        for (const auto &i: instructions) {
            res += " " + i->Name();
        }
        return res;
    }
};

string extract_word(string &line, char delim = ' ')
{
    size_t pos = line.find(delim);
    if (pos == string::npos) {
        pos = line.size();
    }
    string word = line.substr(0, pos);
    line.erase(0, pos + 1);
    return word;
}

string peek_word(const string &line, char delim = ' ')
{
    size_t pos = line.find(delim);
    if (pos == string::npos) {
        pos = line.size();
    }
    return line.substr(0, pos);
}

bool to_float(const string &s, float &res)
{
    if (s.empty()) {
        std::cerr << "Error: empty argument" << std::endl;
        return false;
    }
    size_t idx;
    res = std::stof(s, &idx);
    return idx == s.size();
}

unique_ptr<Instruction> Config::MakeInstruction(string command)
{
    if (command.size() == 0) {
        throw std::runtime_error("no command");
    }
    vector<unique_ptr<Instruction>> seq;
    while (command.size() > 0) {
        string word = extract_word(command);
        if (word == "move") {
            string arg = extract_word(command);
            bool nodraw = (arg == "nodraw");
            if (nodraw) {
                arg = extract_word(command);
            }
            float dist;
            if (!to_float(arg, dist)) {
                std::cerr << "Failed to convert '" << arg << "' to float" << std::endl;
                throw std::runtime_error("Failed to parse command");
            }
            if (nodraw) {
                seq.push_back(make_unique<MoveNoDraw>(dist));
            } else { // draw
                seq.push_back(make_unique<MoveDraw>(dist));
            }
        } else if (word == "rotx" ||
                   word == "roty" ||
                   word == "rotz") {
            string arg = extract_word(command);
            float rot;
            if (!to_float(arg, rot)) {
                std::cerr << "Failed to convert '" << arg << "' to float" << std::endl;
                throw std::runtime_error("Failed to parse command");
            }
            rot = glm::radians(rot);
            if (word == "rotx") {
                seq.push_back(make_unique<RotateX>(rot));
            } else if (word == "roty") {
                seq.push_back(make_unique<RotateY>(rot));
            } else { // rotz
                seq.push_back(make_unique<RotateZ>(rot));
            }
        } else if (word == "push") {
            seq.push_back(make_unique<Push>());
        } else if (word == "pop") {
            seq.push_back(make_unique<Pop>());
        } else if (word == "chill") {
            seq.push_back(make_unique<Dummy>());
        } else {
            std::cerr << "Failed to parse config: unknown instruction '";
            std::cerr << word << "'" << std::endl;
            throw std::runtime_error("Parse error");
        }
    }
    if (seq.size() > 1) {
        Instruction *res = new CompoundAction(std::move(seq));
        return std::unique_ptr<Instruction>(res);
    } else {
        auto res(std::move(seq[0]));
        assert(res.get());
        return res;
    }
}

namespace Config {

CodeMap ReadFile(std::string filename)
{
    CodeMap codes;
    std::ifstream fin(filename);
    if (!fin) {
        throw std::runtime_error("Failed to open config file");
    }
    std::string line;
    while (std::getline(fin, line)) {
        if (line.empty()) {
            continue;
        }
        try {
            codes[line[0]] = MakeInstruction(line.substr(2));
        } catch (const std::runtime_error &e) {
            std::cerr << e.what() << std::endl;
            std::cerr << "Failed to parse line '" << line << "'" << std::endl;
        }
    }
    return codes;
}

}
