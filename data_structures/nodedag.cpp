#include "nodedag.h"

#include <cg3/geometry/point2.h>
#include <cg3/geometry/segment2.h>

//methods for node of dag

nodeDag* nodeDag::getLeftChild() const{
    return leftChildNode;
}

void nodeDag::setLeftChild(nodeDag *leftChild){
   leftChildNode = leftChild;
}

nodeDag* nodeDag::getRightChild() const{
    return rightChildNode;
}

void nodeDag::setRightChild(nodeDag *rightChild){
    rightChildNode = rightChild;
}

nodeDag** nodeDag::getLeftChildPointer(){
    return &leftChildNode;
}

nodeDag** nodeDag::getRightChildPointer(){
    return &rightChildNode;
}


long double nodeDag::determinant(const cg3::Segment2d segment, const cg3::Point2d p){
    return roundl((segment.p1().x() * ((segment.p2().y() * 1) - (1 * p.y())))-
           (segment.p1().y() * ((segment.p2().x() * 1) - (1 * p.x())))+
           (1 * ((segment.p2().x() * p.y()) - (segment.p2().y() * p.x()))));
}

nodeDag::~nodeDag(){
    rightChildNode=nullptr;
    leftChildNode=nullptr;
}


//methods for node of type x (point)
X::X(cg3::Point2d p){
    this->point = p;
    this->setLeftChild(new class Leaf());
    this->setRightChild(new class Leaf());
}

Types X::getType() const{
    return Types::X;
}

cg3::Point2d X::getPoint() const{
    return point;
}

void X::setPoint(cg3::Point2d point){
    this->point = point;
}

nodeDag* X::pointToPoint(const cg3::Point2d point){
    if (point.x() < this->getPoint().x()){
        return this->getLeftChild();
    }
    else{
        return this->getRightChild();
    }
}


//methods for node of type y (segment)
Y::Y(cg3::Segment2d s){
    this->segment = s;
    this->setLeftChild(new class Leaf());
    this->setRightChild(new class Leaf());
}

Types Y::getType() const{
    return Types::Y;
}

cg3::Segment2d Y::getSegment() const{
   return segment;
}

void Y::setSegment(cg3::Segment2d segment){
    this->segment = segment;
}

nodeDag* Y::pointToSegment(const cg3::Point2d point){
    //std::cout<<determinant(node->getSegment(), point)<<std::endl;
    if(determinant(this->getSegment(), point) < 0){
        return this->getLeftChild();
    }
    else{
        return this->getRightChild();
    }
}

//methods for node of type leaf (trapezoid)
Leaf::Leaf(Trapezoid t){
    setTrapezoid(t);
    this->setLeftChild(nullptr);
    this->setRightChild(nullptr);
}


Types Leaf::getType() const{
   return Types::Leaf;
}

std::array<cg3::Point2d, 4> Leaf::getTrapezoid() const{
    return trapezoid;
}

void Leaf::setTrapezoid(Trapezoid trapezoid){
    this->trapezoid = trapezoid;
}
