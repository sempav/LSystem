#include "tree_painter.h"

#include <iostream>
#include <fstream>
#include <stack>
#include <stdexcept>

#include <GL/glew.h>
#include <glm/glm.hpp>
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include <glm/gtx/rotate_vector.hpp>

#include "state.h"

std::ostream &operator <<(std::ostream &os, const glm::vec3 &obj)
{
    os << "(" << obj.x << ", " << obj.y << ", " << obj.z << ")";
    return os;
}

inline std::ostream &operator <<(std::ostream &os, const State &state)
{
    os << "pos = " << state.pos << "\n";
    os << "dir = " << state.dir << "\n";
    os << "x_axis = " << state.x_axis << "\n";
    os << "y_axis = " << state.y_axis << "\n";
    os << "z_axis = " << state.z_axis << "\n";
    return os;
}

void TreePainter::Paint(const std::string &s, glm::vec3 &size, glm::vec3 start_pos) const
{
    State cur_state;
    cur_state.pos = start_pos;
    glm::vec3 vmax = start_pos;
    glm::vec3 vmin = start_pos;
    glBegin(GL_LINES);
    glColor3f(1.f, 1.f, 1.f);
    for (char c: s) {
        codes.at(c)->Perform(cur_state);
        vmax = glm::max(vmax, cur_state.pos);
        vmin = glm::min(vmin, cur_state.pos);
    }
    glEnd();
    size = vmax - vmin;
}

