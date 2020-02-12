#include "drawableTrapezoidalMap.h"

#include <cg3/viewer/opengl_objects/opengl_objects2.h>

#include <cg3/viewer/drawable_objects/drawable_polygon2.h>

DrawableTrapezoidalMap::DrawableTrapezoidalMap():
    trapezoidColor(rand()%256, rand()%256, rand()%256)
{

}

void DrawableTrapezoidalMap::draw() const
{


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
