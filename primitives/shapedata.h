#ifndef SHAPEDATA_H
#define SHAPEDATA_H

#include <QtGui/qopengl.h>
#include <QVector>
#include "vertex.h"

struct ShapeData
{
    QVector<Vertex> vertices;
    QVector<GLushort> indices;
    GLsizeiptr vertexBufferSize() const
    {
        return sizeof(Vertex) * vertices.count();
    }
    GLsizeiptr indexBufferSize() const
    {
        return sizeof(GLushort) * indices.count();
    }
    GLsizeiptr vertexStride() const
    {
        return sizeof(Vertex);
    }
};

#endif // SHAPEDATA_H
