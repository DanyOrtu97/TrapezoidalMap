#include "drawableTrapezoidalMap.h"

#include <cg3/viewer/opengl_objects/opengl_objects2.h>

#include <cg3/viewer/drawable_objects/drawable_polygon2.h>

#include <cstdlib>

int rand();


DrawableTrapezoidalMap::DrawableTrapezoidalMap():
    trapezoidsColor(),
    trapezoidColor(0,0,40),
    verticalLineColor(255,0,0),
    verticalLineSize(6)
{
}



//methods to draw trapezoidal map
void DrawableTrapezoidalMap::draw() const
{
    int i =0;
    //draw trapezoids

    for (const std::array<cg3::Point2d, 4> trap : getTrapezoids()){
        //cerca come fare l'highlight e migliora colori
        if (getFoundTrapezoid().size()>1 && trap == getFoundTrapezoid()){
            cg3::opengl::drawQuad2(trap, (cg3::Color(0,20,255)).hue(), 1, true);
        }
        else{
            cg3::opengl::drawQuad2(trap, trapezoidColor /*this->trapezoidsColor[i]*/, 2, false /*true*/);
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

void DrawableTrapezoidalMap::setTrapezoidsColor(){
    for(int i=0;i<20;i++){
        trapezoidsColor.push_back(cg3::Color(rand()%210, rand()%200, rand()%180));
    }
}

std::vector<cg3::Color> DrawableTrapezoidalMap::getTrapezoidsColor(){
    return trapezoidsColor;
}




