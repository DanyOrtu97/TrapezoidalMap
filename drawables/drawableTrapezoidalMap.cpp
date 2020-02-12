#include "drawableTrapezoidalMap.h"

#include <cg3/viewer/opengl_objects/opengl_objects2.h>

#include <cg3/viewer/drawable_objects/drawable_polygon2.h>


TrapezoidalMapDataset tmd1;


DrawableTrapezoidalMap::DrawableTrapezoidalMap():
    trapezoidColor(rand()%256, rand()%256, rand()%256),
    verticalLineColor(256,0,0),
    verticalLineSize(6)
{


}



//methods to draw vertical line and polygons
void DrawableTrapezoidalMap::draw() const
{

    for (const cg3::Segment2d& line : getVerticalLines()) {
        cg3::opengl::drawLine2(line.p1(), cg3::Point2d(line.p1().y(), tmd1.getBoundingBox().min().y()), verticalLineColor, static_cast<int>(verticalLineSize));
        cg3::opengl::drawLine2(line.p2(), cg3::Point2d(line.p2().y(), tmd1.getBoundingBox().min().y()), verticalLineColor, static_cast<int>(verticalLineSize));
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

