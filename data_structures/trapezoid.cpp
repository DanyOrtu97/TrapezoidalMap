#include "trapezoid.h"


trapezoid::trapezoid(){

}

/**
 * @brief Custom constructor of trapezoid
 * @param[in] 4 points for the trapezoid, leftp, rightp, top and bottom
 */
trapezoid::trapezoid(const cg3::Point2d p1, const cg3::Point2d p2, const cg3::Point2d p3, const cg3::Point2d p4,
                     const cg3::Point2d left, const cg3::Point2d right, const cg3::Segment2d top, const cg3::Segment2d bottom){
    Trapezoid trap = {p1, p2, p3, p4};
    this->setTrapezoid(trap);
    this->setLeftP(left);
    this->setRightP(right);
    this->setTop(top);
    this->setBottom(bottom);
}

/**
 * @brief Overload of the opertor<
 * @param[in] trapezoid other
 */
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

/**
 * @brief Overload of the opertor==
 * @param[in] trapezoid other
 */
bool trapezoid::operator==(const trapezoid &other) const{
    if(this->trapezoide == other.trapezoide && this->tops == other.tops && this->bottoms == other.bottoms && this->leftp == other.leftp && this->rightp == other.rightp){
        return true;
    }
    else{
        return false;
    }
}

/**
 * @brief setter for the trapezoid
 * @param[in] Trapezoid trapezoid
 */
void trapezoid::setTrapezoid(Trapezoid trapezoid){
    this->trapezoide=trapezoid;
}

/**
 * @brief getter for the Trapezoid trapezoide
 */
std::array<cg3::Point2d, 4> trapezoid::getTrapezoid(){
    return this->trapezoide;
}

/**
 * @brief setter for the point leftp
 * @param[in] point left
 */
void trapezoid::setLeftP(const cg3::Point2d left){
    this->leftp=left;
}

/**
 * @brief getter for the poit leftp
 */
cg3::Point2d trapezoid::getLeftP(){
    return this->leftp;
}

/**
 * @brief setter for the point rightp
 * @param[in] point right
 */
void trapezoid::setRightP(const cg3::Point2d right){
    this->rightp=right;
}

/**
 * @brief getter for the poit rightp
 */
cg3::Point2d trapezoid::getRightP(){
    return this->rightp;
}

/**
 * @brief setter for the segment top
 * @param[in] segment top
 */
void trapezoid::setTop(const cg3::Segment2d top){
    this->tops = top;
}

/**
 * @brief getter for the segment top
 */
cg3::Segment2d trapezoid::getTop(){
    return this->tops;
}

/**
 * @brief setter for the segment bottom
 * @param[in] segment bottom
 */
void trapezoid::setBottom(const cg3::Segment2d bottom){
    this->bottoms = bottom;
}


/**
 * @brief getter for the segment bottom
 */
cg3::Segment2d trapezoid::getBottom(){
    return this->bottoms;
}


