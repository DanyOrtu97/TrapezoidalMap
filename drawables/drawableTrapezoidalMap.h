#ifndef DRAWABLETRAPEZOIDALMAP_H
#define DRAWABLETRAPEZOIDALMAP_H

#include "data_structures/trapezoidalMap.h"

#include <cg3/viewer/interfaces/drawable_object.h>

#include <cg3/utilities/color.h>


class DrawableTrapezoidalMap : public TrapezoidalMap, public cg3::DrawableObject
{
public:
    DrawableTrapezoidalMap();

    const cg3::Color& getTrapezoidColor() const;
    void fillTrapezoid(const cg3::Color& value);

private:
    cg3::Color trapezoidColor;

};

#endif // DRAWABLETRAPEZOIDALMAP_H
