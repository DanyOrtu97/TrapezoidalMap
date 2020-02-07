#include "drawableTrapezoidalMap.h"


DrawableTrapezoidalMap::DrawableTrapezoidalMap():
    trapezoidColor(rand()%256, rand()%256, rand()%256)
{

}


const cg3::Color& DrawableTrapezoidalMap::getTrapezoidColor() const
{
    return trapezoidColor;
}


void DrawableTrapezoidalMap::fillTrapezoid(const cg3::Color &value)
{
    trapezoidColor = value;
}
