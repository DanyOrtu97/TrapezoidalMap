#include "trapezoid.h"


trapezoid::trapezoid(){

}

trapezoid::trapezoid(const cg3::Point2d p1, const cg3::Point2d p2, const cg3::Point2d p3, const cg3::Point2d p4,
                     const cg3::Point2d left, const cg3::Point2d right, const cg3::Segment2d top, const cg3::Segment2d bottom){
    Trapezoid trap = {p1, p2, p3, p4};
    this->setTrapezoid(trap);
    this->setLeftP(left);
    this->setRightP(right);
    this->setTop(top);
    this->setBottom(bottom);
}

trapezoid::trapezoid(const cg3::Point2d p1, const cg3::Point2d p2, const cg3::Point2d p3, const cg3::Point2d p4,
          const cg3::Point2d left, const cg3::Point2d right, const cg3::Segment2d top, const cg3::Segment2d bottom,
          const int oneR, const int twoR, const int oneL, const int twoL){
    Trapezoid trap = {p1, p2, p3, p4};
    this->setTrapezoid(trap);
    this->setLeftP(left);
    this->setRightP(right);
    this->setTop(top);
    this->setBottom(bottom);
    this->oneRight = oneR;
    this->twoRight = twoR;
    this->oneLeft = oneL;
    this->twoLeft = twoL;
}

bool trapezoid::operator<(const trapezoid &other) const{
    if(this->trapezoide[0].x() < other.trapezoide[0].x()){
        return true;
    }
    else if(this->trapezoide[0].x() == other.trapezoide[0].x()){
        if(this->trapezoide[0].y() > other.trapezoide[0].y()){
            return true;
        }
        else{
            return false;
        }
    }
    else{
        return false;
    }
}

bool trapezoid::operator==(const trapezoid &other) const{
    if(this->trapezoide == other.trapezoide && this->tops == other.tops && this->bottoms == other.bottoms && this->leftp == other.leftp && this->rightp == other.rightp){
        return true;
    }
    else{
        return false;
    }
}


void trapezoid::setTrapezoid(Trapezoid trapezoid){
    this->trapezoide=trapezoid;
}

std::array<cg3::Point2d, 4> trapezoid::getTrapezoid(){
    return this->trapezoide;
}

void trapezoid::setLeftP(const cg3::Point2d left){
    this->leftp=left;
}

cg3::Point2d trapezoid::getLeftP(){
    return this->leftp;
}

void trapezoid::setRightP(const cg3::Point2d right){
    this->rightp=right;
}

cg3::Point2d trapezoid::getRightP(){
    return this->rightp;
}

void trapezoid::setTop(const cg3::Segment2d top){
    this->tops = top;
}

cg3::Segment2d trapezoid::getTop(){
    return this->tops;
}

void trapezoid::setBottom(const cg3::Segment2d bottom){
    this->bottoms = bottom;
}

cg3::Segment2d trapezoid::getBottom(){
    return this->bottoms;
}

void trapezoid::setNeighboors(const int oneR, const int twoR, const int oneL, const int twoL){
    this->oneRight = oneR;
    this->twoRight = twoR;
    this->oneLeft = oneL;
    this->twoLeft = twoL;
}

int trapezoid::getOneRight(){
    return this->oneRight;
}

int trapezoid::getTwoRight(){
    return this->twoRight;
}

int trapezoid::getOneLeft(){
    return this->oneLeft;
}

int trapezoid::getTwoLeft(){
    return this->twoLeft;
}

