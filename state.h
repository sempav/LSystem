#ifndef __STATE_H__
#define __STATE_H__

#include <glm/glm.hpp>

struct State {
    glm::vec3 pos;
    glm::vec3 dir;
    glm::vec3 x_axis;
    glm::vec3 y_axis;
    glm::vec3 z_axis;
    int depth;
    State() :
        pos(0, 0, 0), dir(0, 1, 0),
        x_axis(1, 0, 0), y_axis(0, 1, 0), z_axis(0, 0, 1),
        depth(0)
    { }
};

#endif //__STATE_H__
