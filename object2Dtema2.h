#pragma once

#include <string>

#include "core/gpu/mesh.h"
#include "utils/glm_utils.h"


namespace object2Dtema2
{

    Mesh* CreateRectangle(const std::string& name, glm::vec3 center, float length, float width, glm::vec3 color);
}
