#ifndef DRAWABLETRAPEZOIDALMAP_H
#define DRAWABLETRAPEZOIDALMAP_H

#include "data_structures/trapezoidalMap.h"

#include "data_structures/trapezoidalmap_dataset.h"

#include <cg3/viewer/interfaces/drawable_object.h>

#include <cg3/utilities/color.h>


/**
 * @brief Class to draw the trapezoidal map.
 */
class DrawableTrapezoidalMap : public TrapezoidalMap, public TrapezoidalMapDataset , public cg3::DrawableObject
{
public:
    DrawableTrapezoidalMap();

    void draw() const;
    cg3::Point3d sceneCenter() const;
    double sceneRadius() const;

    const cg3::Color& getTrapezoidColor() const;
    void setTrapezoidColor(const cg3::Color& value);

    const cg3::Color& getVerticalLineColor() const;
    void setVerticalLineColor(const cg3::Color& value);

    unsigned int getVerticalLineSize() const;
    void setVerticalLineSize(unsigned int value);

    void insertColors();

    void setTrapezoidsColor();

    std::vector<cg3::Color> getTrapezoidsColor();


protected:
    std::vector<cg3::Color> trapezoidsColor;

private:
    cg3::Color trapezoidColor;
    cg3::Color verticalLineColor;
    unsigned int verticalLineSize;
;
};

#endif // DRAWABLETRAPEZOIDALMAP_H
