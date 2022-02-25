#include "object2Dtema2.h"

#include <vector>

#include "core/engine.h"
#include "utils/gl_utils.h"


Mesh* object2Dtema2::CreateRectangle(
    const std::string& name,
    glm::vec3 corner,
    float length,
    float width,
    glm::vec3 color)
{
    std::vector<VertexFormat> vertices =
    {
        VertexFormat(corner, color),
        VertexFormat(corner + glm::vec3(length, 0, 0), color),
        VertexFormat(corner + glm::vec3(length, width, 0), color),
        VertexFormat(corner + glm::vec3(0, width, 0), color)
    };

    Mesh* rectangle = new Mesh(name);
    std::vector<unsigned int> indices = { 0, 1, 2, 3, 0, 2 };

    rectangle->InitFromData(vertices, indices);
    return rectangle;
}

