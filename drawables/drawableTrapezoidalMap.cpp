#include "drawableTrapezoidalMap.h"

#include <cg3/viewer/opengl_objects/opengl_objects2.h>

#include <cg3/viewer/drawable_objects/drawable_polygon2.h>

#include <cstdlib>

#include <data_structures/trapezoidalMap.h>

#include <algorithms/algorithms.h>

#include <data_structures/trapezoid.h>



DrawableTrapezoidalMap::DrawableTrapezoidalMap():
    verticalLineColor(255,0,0),
    verticalLineSize(4)
{
}

/**
 * @brief This method draw vertical lines and trapezoids
 */
void DrawableTrapezoidalMap::draw() const
{
    int i =0;
    for (trapezoid trap : getTrapezoids()){
        if (getFoundTrapezoid().size()>1 && trap.getTrapezoid() == getFoundTrapezoid()){
            if(GasAlgorithms::degeneratedTrapezoid(trap)){
                cg3::opengl::drawTriangle2(GasAlgorithms::findTriangleByQuad(trap), (cg3::Color(0,20,255)).hue(), 1, true);
            }
            else{
                cg3::opengl::drawQuad2(trap.getTrapezoid(), (cg3::Color(0,20,255)).hue(), 1, true);
            }
        }
        else{
            if(GasAlgorithms::degeneratedTrapezoid(trap)){
                cg3::opengl::drawTriangle2(GasAlgorithms::findTriangleByQuad(trap), trap.getColor(), 2, true);
            }
            else{
                cg3::opengl::drawQuad2(trap.getTrapezoid(), trap.getColor(), 2, true);
            }
            if(GasAlgorithms::degeneratedTrapezoid(trap)){
                cg3::opengl::drawTriangle2(GasAlgorithms::findTriangleByQuad(trap), trap.getColor(), 2, true);
            }
            else{
                cg3::opengl::drawQuad2(trap.getTrapezoid(), trap.getColor(), 2, true);
            }
            cg3::opengl::drawLine2(trap.getTrapezoid()[0], trap.getTrapezoid()[3], verticalLineColor, static_cast<int>(verticalLineSize));
            cg3::opengl::drawLine2(trap.getTrapezoid()[1], trap.getTrapezoid()[2], verticalLineColor, static_cast<int>(verticalLineSize));
            i++;
        }
    }
}



cg3::Point3d DrawableTrapezoidalMap::sceneCenter() const
{
    const cg3::BoundingBox2& boundingBox = this->getBoundingBox();
    return cg3::Point3d(boundingBox.center().x(), boundingBox.center().y(), 0);
}


double DrawableTrapezoidalMap::sceneRadius() const
{
    const cg3::BoundingBox2& boundingBox = this->getBoundingBox();
    return boundingBox.diag();
}


const cg3::Color& DrawableTrapezoidalMap::getVerticalLineColor() const
{
    return verticalLineColor;
}

void DrawableTrapezoidalMap::setVerticalLineColor(const cg3::Color& value)
{
    verticalLineColor = value;
}

unsigned int DrawableTrapezoidalMap::getVerticalLineSize() const
{
    return verticalLineSize;
}

void DrawableTrapezoidalMap::setVerticalLineSize(unsigned int value)
{
    verticalLineSize = value;
}





