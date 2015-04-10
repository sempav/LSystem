#pragma once

#include <glm/glm.hpp>
#include <GL/glew.h>
#include <algorithm>

class Camera 
{
    float dist;
    float rot_y, rot_x;

    glm::vec3 pos;

public:
	Camera() : dist(1.0f), rot_y(0), rot_x(0), pos(0.0f)
    { }

    void SetPos(glm::vec3 pos) {
        this->pos = pos;
    }
    void Translate(float x) { dist *= (1 + x); }
    void RotateY(float d)   { rot_y += d; }
    void RotateX(float d)   { rot_x += d; }
    void LookFrom()
    {
        glTranslatef(0.0f, 0.0f, -dist);
        glRotatef(rot_x, 1.0f, 0.0f, 0.0f);
        glRotatef(rot_y, 0.0f, 1.0f, 0.0f);
        glTranslatef(-pos.x, -pos.y, -pos.z);
    }
};
