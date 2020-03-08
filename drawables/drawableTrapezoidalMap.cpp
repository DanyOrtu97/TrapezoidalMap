#include "drawableTrapezoidalMap.h"

#include <cg3/viewer/opengl_objects/opengl_objects2.h>

#include <cg3/viewer/drawable_objects/drawable_polygon2.h>


#include <cstdlib>
int rand();


DrawableTrapezoidalMap::DrawableTrapezoidalMap():
    trapezoidColor(0,0,40),
    verticalLineColor(255,0,0),
    verticalLineSize(3)
{
}


//methods to draw trapezoidal map
void DrawableTrapezoidalMap::draw() const
{
    //draw trapezoids
    for (const std::array<cg3::Point2d, 4> trap : getTrapezoids()){
        cg3::opengl::drawQuad2(trap, cg3::Color(rand()%180, rand()%180, rand()%180), 4, true);
        cg3::opengl::drawLine2(trap[0], trap[3], verticalLineColor, static_cast<int>(verticalLineSize));
        cg3::opengl::drawLine2(trap[1], trap[2], verticalLineColor, static_cast<int>(verticalLineSize));
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


const cg3::Color& DrawableTrapezoidalMap::getTrapezoidColor() const
{
    return trapezoidColor;
}


void DrawableTrapezoidalMap::setTrapezoidColor(const cg3::Color &value)
{
    trapezoidColor = value;
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


