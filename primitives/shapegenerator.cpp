#include "shapegenerator.h"

#include <glm/ext/scalar_constants.hpp>

using glm::vec2;
using glm::vec3;

namespace {

glm::vec3 randomColor()
{
    glm::vec3 ret;
    ret.x = rand() / (float)RAND_MAX;
    ret.y = rand() / (float)RAND_MAX;
    ret.z = rand() / (float)RAND_MAX;
    return ret;
}

ShapeData makePlaneVerts(uint dimensions)
{
    ShapeData ret;
    ret.vertices.resize(dimensions * dimensions);
    const uint half = dimensions / 2;
    for (uint i = 0; i < dimensions; i++)
    {
        for (uint j = 0; j < dimensions; j++)
        {
            Vertex& thisVert = ret.vertices[i * dimensions + j];
            thisVert.position.x = j - half;
            thisVert.position.z = i - half;
            thisVert.position.y = 0;
            thisVert.normal = glm::vec3(0.0f, 1.0f, 0.0f);
            thisVert.color = randomColor();
            thisVert.textureCoordinate = vec2((float)i / dimensions, (float)j / dimensions);
        }
    }
    return ret;
}

ShapeData makePlaneIndices(uint dimensions)
{
    ShapeData ret;
    const size_t numIndices = (dimensions - 1) * (dimensions - 1) * 2 * 3; // 2 triangles per square, 3 indices per triangle
    ret.indices.resize(numIndices);
    size_t runner = 0;
    for (uint row = 0; row < dimensions - 1; row++)
    {
        for (uint col = 0; col < dimensions - 1; col++)
        {
            ret.indices[runner++] = dimensions * row + col;
            ret.indices[runner++] = dimensions * row + col + dimensions;
            ret.indices[runner++] = dimensions * row + col + dimensions + 1;

            ret.indices[runner++] = dimensions * row + col;
            ret.indices[runner++] = dimensions * row + col + dimensions + 1;
            ret.indices[runner++] = dimensions * row + col + 1;
        }
    }
    assert(runner == numIndices);
    return ret;
}

} // anonymous namespace

ShapeData ShapeGenerator::makeTriangle()
{
    ShapeData triangle;
    triangle.vertices = {
        { // Top center
            vec3(+0.0f, +1.0f, +0.0f),
            vec3(+1.0f, +0.0f, +0.0f),
            vec3(+0.0f, +0.0f, +1.0f),
            vec2(+0.5f, +1.0f),
        },
        { // Bottom left
            vec3(-1.0f, -1.0f, +0.0f),
            vec3(+0.0f, +1.0f, +0.0f),
            vec3(+0.0f, +0.0f, +1.0f),
            vec2(+0.0f, +0.0f),
        },
        { // Bottom right
            vec3(+1.0f, -1.0f, +0.0f),
            vec3(+0.0f, +0.0f, +1.0f),
            vec3(+0.0f, +0.0f, +1.0f),
            vec2(+1.0, +0.0f),
        }
    };
    triangle.indices = {0, 1, 2};
    return triangle;
}

ShapeData ShapeGenerator::makeCube()
{
    // TODO: texture coordinates
    ShapeData cube;
    cube.vertices = {
        {
            vec3(-1.0f, +1.0f, +1.0f), // 0
            vec3(+1.0f, +0.0f, +0.0f),
            vec3(+0.0f, +1.0f, +0.0f),
            vec2(+0.0, +0.0f),
        },
        {
            vec3(+1.0f, +1.0f, +1.0f), // 1
            vec3(+0.0f, +1.0f, +0.0f),
            vec3(+0.0f, +1.0f, +0.0f),
            vec2(+0.0, +0.0f),
        },
        {
            vec3(+1.0f, +1.0f, -1.0f), // 2
            vec3(+0.0f, +0.0f, +1.0f),
            vec3(+0.0f, +1.0f, +0.0f),
            vec2(+0.0, +0.0f),
        },
        {
            vec3(-1.0f, +1.0f, -1.0f), // 3
            vec3(+1.0f, +1.0f, +1.0f),
            vec3(+0.0f, +1.0f, +0.0f),
            vec2(+0.0, +0.0f),
        },
        {
            vec3(-1.0f, +1.0f, -1.0f), // 4
            vec3(+1.0f, +0.0f, +1.0f),
            vec3(+0.0f, +0.0f, -1.0f),
            vec2(+0.0, +0.0f),
        },
        {
            vec3(+1.0f, +1.0f, -1.0f), // 5
            vec3(+0.0f, +0.5f, +0.2f),
            vec3(+0.0f, +0.0f, -1.0f),
            vec2(+0.0, +0.0f),
        },
        {
            vec3(+1.0f, -1.0f, -1.0f), // 6
            vec3(+0.8f, +0.6f, +0.4f),
            vec3(+0.0f, +0.0f, -1.0f),
            vec2(+0.0, +0.0f),
        },
        {
            vec3(-1.0f, -1.0f, -1.0f), // 7
            vec3(+0.3f, +1.0f, +0.5f),
            vec3(+0.0f, +0.0f, -1.0f),
            vec2(+0.0, +0.0f),
        },
        {
            vec3(+1.0f, +1.0f, -1.0f), // 8
            vec3(+0.2f, +0.5f, +0.2f),
            vec3(+1.0f, +0.0f, +0.0f),
            vec2(+0.0, +0.0f),
        },
        {
            vec3(+1.0f, +1.0f, +1.0f), // 9
            vec3(+0.9f, +0.3f, +0.7f),
            vec3(+1.0f, +0.0f, +0.0f),
            vec2(+0.0, +0.0f),
        },
        {
            vec3(+1.0f, -1.0f, +1.0f), // 10
            vec3(+0.3f, +0.7f, +1.0f),
            vec3(+1.0f, +0.0f, +0.0f),
            vec2(+0.0, +0.0f),
        },
        {
            vec3(+1.0f, -1.0f, -1.0f), // 11
            vec3(+0.5f, +0.7f, +0.5f),
            vec3(+1.0f, +0.0f, +0.0f),
            vec2(+0.0, +0.0f),
        },
        {
            vec3(-1.0f, +1.0f, +1.0f), // 12
            vec3(+0.7f, +0.8f, +0.2f),
            vec3(-1.0f, +0.0f, +0.0f),
            vec2(+0.0, +0.0f),
        },
        {
            vec3(-1.0f, +1.0f, -1.0f), // 13
            vec3(+0.5f, +0.7f, +0.3f),
            vec3(-1.0f, +0.0f, +0.0f),
            vec2(+0.0, +0.0f),
        },
        {
            vec3(-1.0f, -1.0f, -1.0f), // 14
            vec3(+0.4f, +0.7f, +0.7f),
            vec3(-1.0f, +0.0f, +0.0f),
            vec2(+0.0, +0.0f),
        },
        {
            vec3(-1.0f, -1.0f, +1.0f), // 15
            vec3(+0.2f, +0.5f, +1.0f),
            vec3(-1.0f, +0.0f, +0.0f),
            vec2(+0.0, +0.0f),
        },
        {
            vec3(+1.0f, +1.0f, +1.0f), // 16
            vec3(+0.6f, +1.0f, +0.7f),
            vec3(+0.0f, +0.0f, +1.0f),
            vec2(+0.0, +0.0f),
        },
        {
            vec3(-1.0f, +1.0f, +1.0f), // 17
            vec3(+0.6f, +0.4f, +0.8f),
            vec3(+0.0f, +0.0f, +1.0f),
            vec2(+0.0, +0.0f),
        },
        {
            vec3(-1.0f, -1.0f, +1.0f), // 18
            vec3(+0.2f, +0.8f, +0.7f),
            vec3(+0.0f, +0.0f, +1.0f),
            vec2(+0.0, +0.0f),
        },
        {
            vec3(+1.0f, -1.0f, +1.0f), // 19
            vec3(+0.2f, +0.7f, +1.0f),
            vec3(+0.0f, +0.0f, +1.0f),
            vec2(+0.0, +0.0f),
        },
        {
            vec3(+1.0f, -1.0f, -1.0f), // 20
            vec3(+0.8f, +0.3f, +0.7f),
            vec3(+0.0f, -1.0f, +0.0f),
            vec2(+0.0, +0.0f),
        },
        {
            vec3(-1.0f, -1.0f, -1.0f), // 21
            vec3(+0.8f, +0.9f, +0.5f),
            vec3(+0.0f, -1.0f, +0.0f),
            vec2(+0.0, +0.0f),
        },
        {
            vec3(-1.0f, -1.0f, +1.0f), // 22
            vec3(+0.5f, +0.8f, +0.5f),
            vec3(+0.0f, -1.0f, +0.0f),
            vec2(+0.0, +0.0f),
        },
        {
            vec3(+1.0f, -1.0f, +1.0f), // 23
            vec3(+0.9f, +1.0f, +0.2f),
            vec3(+0.0f, -1.0f, +0.0f),
            vec2(+0.0, +0.0f),
        }
    };
    cube.indices = {
        0,   1,  2,  0,  2,  3, // Top
        4,   5,  6,  4,  6,  7, // Front
        8,   9, 10,  8, 10, 11, // Right
        12, 13, 14, 12, 14, 15, // Left
        16, 17, 18, 16, 18, 19, // Back
        20, 22, 21, 20, 23, 22, // Bottom
    };
    return cube;

}

ShapeData ShapeGenerator::makePlane(uint dimensions)
{
    ShapeData ret = makePlaneVerts(dimensions);
    ShapeData ret2 = makePlaneIndices(dimensions);
    ret.indices = ret2.indices;
    return ret;
}

ShapeData ShapeGenerator::makeSphere(uint tesselation)
{
    ShapeData ret = makePlaneVerts(tesselation);
    ShapeData ret2 = makePlaneIndices(tesselation);
    ret.indices = ret2.indices;

    uint dimensions = tesselation;
    const float RADIUS = 1.0f;
    const double CIRCLE = glm::pi<double>() * 2;
    const double SLICE_ANGLE = CIRCLE / (dimensions - 1);
    for (size_t col = 0; col < dimensions; col++)
    {
        double phi = -SLICE_ANGLE * col;
        for (size_t row = 0; row < dimensions; row++)
        {
            double theta = -(SLICE_ANGLE / 2.0) * row;
            size_t vertIndex = col * dimensions + row;
            Vertex& v = ret.vertices[vertIndex];
            v.position.x = RADIUS * cos(phi) * sin(theta);
            v.position.y = RADIUS * sin(phi) * sin(theta);
            v.position.z = RADIUS * cos(theta);
            v.normal = glm::normalize(v.position);
            v.color = vec3(1.0f, 1.0f, 1.0f);
        }
    }
    return ret;
}
