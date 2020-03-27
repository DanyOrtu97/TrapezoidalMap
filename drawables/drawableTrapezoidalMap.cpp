#include "drawableTrapezoidalMap.h"

#include <cg3/viewer/opengl_objects/opengl_objects2.h>

#include <cg3/viewer/drawable_objects/drawable_polygon2.h>

#include <cstdlib>

#include <data_structures/trapezoidalMap.h>

#include <algorithms/algorithms.h>

DrawableTrapezoidalMap::DrawableTrapezoidalMap():
    trapezoidsColor(),
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
    for (const std::array<cg3::Point2d, 4> trap : getTrapezoids()){
        if (getFoundTrapezoid().size()>1 && trap == getFoundTrapezoid()){
            if(GasAlgorithms::degeneratedTrapezoid(trap)){
                cg3::opengl::drawTriangle2(GasAlgorithms::findTriangleByQuad(trap), (cg3::Color(0,20,255)).hue(), 1, true);
            }
            else{
                cg3::opengl::drawQuad2(trap, (cg3::Color(0,20,255)).hue(), 1, true);
            }
            //cg3::opengl::drawQuad2(trap, (cg3::Color(0,20,255)).hue(), 1, true);

        }
        else{
            if(GasAlgorithms::degeneratedTrapezoid(trap)){
                cg3::opengl::drawTriangle2(GasAlgorithms::findTriangleByQuad(trap), this->trapezoidsColor[i], 2, true);
            }
            else{
                cg3::opengl::drawQuad2(trap, this->trapezoidsColor[i%trapezoidsColor.size()], 2, true);
                cg3::opengl::drawLine2(trap[0], trap[3], verticalLineColor, static_cast<int>(verticalLineSize));
                cg3::opengl::drawLine2(trap[1], trap[2], verticalLineColor, static_cast<int>(verticalLineSize));
            }

            //cg3::opengl::drawQuad2(trap, this->trapezoidsColor[i], 2, true);
            //cg3::opengl::drawLine2(trap[0], trap[3], verticalLineColor, static_cast<int>(verticalLineSize));
            //cg3::opengl::drawLine2(trap[1], trap[2], verticalLineColor, static_cast<int>(verticalLineSize));
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

/**
 * @brief Method to generate some colors for the trapezoids
 */
void DrawableTrapezoidalMap::setTrapezoidsColor(){
    for(int i=0;i<30;i++){
        trapezoidsColor.push_back((cg3::Color(rand()%210, rand()%200, rand()%180)).fromHsv(rand()%359, rand()%128, 192 + rand()%63));
    }
}


/**
 * @brief Method to return the vector of colors for the trapezoids
 * @param[out] Vector of colors
 */
std::vector<cg3::Color> DrawableTrapezoidalMap::getTrapezoidsColor(){
    return trapezoidsColor;
}




