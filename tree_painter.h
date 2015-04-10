#ifndef __TREE_PAINTER__
#define __TREE_PAINTER__

#include <glm/glm.hpp>
#include <string>

#include "config.h"

class TreePainter
{
    private:
        Config::CodeMap codes;

    public:
        TreePainter(const std::string &config_file) : 
            codes(Config::ReadFile(config_file))
        {}
        void Paint(const std::string &s, glm::vec3 &size,
                   glm::vec3 start_pos = glm::vec3(0.0f)) const;
        glm::vec3 GetDimensions() const;
};

#endif // __TREE_PAINTER__
