#ifndef SHAPEGENERATOR_H
#define SHAPEGENERATOR_H

#include "shapedata.h"

class ShapeGenerator
{
public:
    static ShapeData makeTriangle();
    static ShapeData makeCube();
    static ShapeData makePlane(uint dimensions = 10);
    static ShapeData makeSphere(uint tesselation = 10);
};

#endif // SHAPEGENERATOR_H
