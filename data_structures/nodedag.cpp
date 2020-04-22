#include "nodedag.h"

#include <cg3/geometry/point2.h>
#include <cg3/geometry/segment2.h>
#include <data_structures/trapezoid.h>

/**
 * @brief This method return the pointer at the left child
 * @param[out] left nodeDag*
 */
nodeDag* nodeDag::getLeftChild() const{
    return leftChildNode;
}

/**
 * @brief This method set the value of the left child
 * @param[in] left nodeDag*
 */
void nodeDag::setLeftChild(nodeDag *leftChild){
   leftChildNode = leftChild;
}

/**
 * @brief This method return the pointer at the right child
 * @param[out] right nodeDag*
 */
nodeDag* nodeDag::getRightChild() const{
    return rightChildNode;
}

/**
 * @brief This method set the value of the right child
 * @param[in] right nodeDag*
 */
void nodeDag::setRightChild(nodeDag *rightChild){
    rightChildNode = rightChild;
}


/**
 * @brief This method return the double pointer at the left child
 * @param[out] left nodeDag**
 */
nodeDag** nodeDag::getLeftChildP(){
    return &leftChildNode;
}

/**
 * @brief This method return the double pointer at the right child
 * @param[out] right nodeDag**
 */
nodeDag** nodeDag::getRightChildP(){
    return &rightChildNode;
}

/**
 * @brief Destructor of nodeDag
 */
nodeDag::~nodeDag(){
    rightChildNode=nullptr;
    leftChildNode=nullptr;
}

/**
 * @brief This method return the value of the determinant between one segment and one point
 * @param[in] segment Segment, p Point
 * @param[out] determinant of the matrix between the segment and the point
 */
long double nodeDag::determinant(const cg3::Segment2d segment, const cg3::Point2d p){
    return roundl((segment.p1().x() * ((segment.p2().y() * 1) - (1 * p.y())))-
           (segment.p1().y() * ((segment.p2().x() * 1) - (1 * p.x())))+
           (1 * ((segment.p2().x() * p.y()) - (segment.p2().y() * p.x()))));
}


/**
 * @brief Constructor of node of type X(point)
 * @param[in] p Point
 */
X::X(cg3::Point2d p, bool right){
    this->point = p;
    this->setValueEndpoint(right);
    this->setLeftChild(nullptr);
    this->setRightChild(nullptr);
}

/**
 * @brief Return the type of the node
 * @param[out] string of the type of the node
 */
std::string X::getType() const{
    std::string s = typeid(this).name();
    return s;
}

/**
 * @brief Return the point on an X node
 * @param[out] p Point
 */
cg3::Point2d X::getPoint() const{
    return point;
}

/**
 * @brief Set the point in an X node
 * @param[in] point Point
 */
void X::setPoint(cg3::Point2d point){
    this->point = point;
}

/**
 * @brief Method to select the correct direction by two point
 * @param[in] point Point
 * @param[out] pointer nodeDag**
 */
nodeDag** X::pointToPoint(const cg3::Point2d point, const cg3::Point2d point2){
    if (point.x() < this->getPoint().x()){
        return this->getLeftChildP();
    }
    else if (point.x() > this->getPoint().x()){
        return this->getRightChildP();
    }
    else{
        if(this->rightEndpoint){
            if(point2.x() > this->getPoint().x()){
                return this->getRightChildP();
            }
            else{
                return this->getLeftChildP();
            }
        }
        else{
            if(point2.x() < this->getPoint().x()){
                return this->getLeftChildP();
            }
            else{
                return this->getRightChildP();
            }
        }
    }
}

/**
 * @brief Getter for the bool rightendpoint
 * @param[out] true if is the right endpoint, else otherwise
 */
bool X::getValueEndpoint(){
    return rightEndpoint;
}

/**
 * @brief Setter for the bool right endpoint
 * @param[in] bool value
 */
void X::setValueEndpoint(bool value){
    this->rightEndpoint=value;
}

/**
 * @brief Constructor of node of type Y(segment)
 * @param[in] s Segment
 */
Y::Y(cg3::Segment2d s){
    this->segment = s;
    this->setLeftChild(nullptr);
    this->setRightChild(nullptr);
}

/**
 * @brief Return the type of the node
 * @param[out] string of the type of the node
 */
std::string  Y::getType() const{
    std::string s = typeid(this).name();
    return s;
}

/**
 * @brief Return the segment on an Y node
 * @param[out] s Segment
 */
cg3::Segment2d Y::getSegment() const{
   return segment;
}

/**
 * @brief Set the segment in an Y node
 * @param[in] segment Segment
 */
void Y::setSegment(cg3::Segment2d segment){
    this->segment = segment;
}


/**
 * @brief Method to select the correct direction by one segment and one point using the determinant
 * @param[in] point Point
 * @param[out] pointer nodeDag**
 */
nodeDag** Y::pointToSegment(const cg3::Point2d point, const cg3::Point2d point2){
    if(this->getSegment().p1() == point){
        if(determinant(this->getSegment(), point2) > 0){ //sopra
            return this->getLeftChildP();
        }
        else{ //sotto
            return this->getRightChildP();
        }
    }
    else if(this->getSegment().p2() == point){
        if(determinant(this->getSegment(), point2) > 0){ //sopra
            return this->getLeftChildP();
        }
        else{ //sotto
            return this->getRightChildP();
        }
    }
    else{
        if(determinant(this->getSegment(), point) > 0){
            return this->getLeftChildP();
        }
        else{
            return this->getRightChildP();
        }
    }
}


/**
 * @brief Constructor of node of type Leaf(trapezoid)
 * @param[in] t Trapezoid
 */
Leaf::Leaf(trapezoid t){
    setTrapezoid(t);
    this->setLeftChild(nullptr);
    this->setRightChild(nullptr);
}


/**
 * @brief Return the type of the node
 * @param[out] string of the type of the node
 */
std::string  Leaf::getType() const{
    std::string s = typeid(this).name();
    return s;
}

/**
 * @brief Return the trapezoid on Leaf node
 * @param[out] t Trapezoid
 */
trapezoid Leaf::getTrapezoid(){
    return trapezoide;
}

/**
 * @brief Set the trapezoid in a Leaf node
 * @param[in] trapezoid Trapezoid
 */
void Leaf::setTrapezoid(trapezoid trapezoid){
    this->trapezoide = trapezoid;
}
