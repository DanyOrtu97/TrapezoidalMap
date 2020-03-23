#include "nodedag.h"

#include <cg3/geometry/point2.h>
#include <cg3/geometry/segment2.h>

/**
 * @brief This method return the pointer at the left child
 */
nodeDag* nodeDag::getLeftChild() const{
    return leftChildNode;
}

void nodeDag::setLeftChild(nodeDag *leftChild){
   leftChildNode = leftChild;
}

/**
 * @brief This method return the pointer at the right child
 */
nodeDag* nodeDag::getRightChild() const{
    return rightChildNode;
}

void nodeDag::setRightChild(nodeDag *rightChild){
    rightChildNode = rightChild;
}


/**
 * @brief This method return the double pointer at the left child
 */
nodeDag** nodeDag::getLeftChildP(){
    return &leftChildNode;
}

/**
 * @brief This method return the double pointer at the right child
 */
nodeDag** nodeDag::getRightChildP(){
    return &rightChildNode;
}

nodeDag::~nodeDag(){
    rightChildNode=nullptr;
    leftChildNode=nullptr;
}

/**
 * @brief This method return the value of the determinant between one segment and one point
 */
long double nodeDag::determinant(const cg3::Segment2d segment, const cg3::Point2d p){
    return roundl((segment.p1().x() * ((segment.p2().y() * 1) - (1 * p.y())))-
           (segment.p1().y() * ((segment.p2().x() * 1) - (1 * p.x())))+
           (1 * ((segment.p2().x() * p.y()) - (segment.p2().y() * p.x()))));
}


/**
 * @brief Constructor of node of type X(point)
 */
X::X(cg3::Point2d p){
    this->point = p;
    this->setLeftChild(new Leaf());
    this->setRightChild(new Leaf());
}


std::string X::getType() const{
    std::string s = typeid(this).name();
    return s;
}


cg3::Point2d X::getPoint() const{
    return point;
}

void X::setPoint(cg3::Point2d point){
    this->point = point;
}


/**
 * @brief Method to select the correct direction by two point
 */
nodeDag** X::pointToPoint(const cg3::Point2d point){
    if (point.x() < this->getPoint().x()){
        return this->getLeftChildP();
    }
    else{
        return this->getRightChildP();
    }
}


/**
 * @brief Constructor of node of type Y(segment)
 */
Y::Y(cg3::Segment2d s){
    this->segment = s;
    this->setLeftChild(new Leaf());
    this->setRightChild(new Leaf());
}


std::string  Y::getType() const{
    std::string s = typeid(this).name();
    return s;
}

cg3::Segment2d Y::getSegment() const{
   return segment;
}

void Y::setSegment(cg3::Segment2d segment){
    this->segment = segment;
}


/**
 * @brief Method to select the correct direction by one segment and one point using the determinant
 */
nodeDag** Y::pointToSegment(const cg3::Point2d point){
    if(determinant(this->getSegment(), point) > 0){
        return this->getLeftChildP();
    }
    else{
        return this->getRightChildP();
    }
}


/**
 * @brief Constructor of node of type Leaf(trapezoid)
 */
Leaf::Leaf(Trapezoid t){
    setTrapezoid(t);
    this->setLeftChild(nullptr);
    this->setRightChild(nullptr);
}


std::string  Leaf::getType() const{
    std::string s = typeid(this).name();
    return s;
}

std::array<cg3::Point2d, 4> Leaf::getTrapezoid() const{
    return trapezoid;
}

void Leaf::setTrapezoid(Trapezoid trapezoid){
    this->trapezoid = trapezoid;
}
