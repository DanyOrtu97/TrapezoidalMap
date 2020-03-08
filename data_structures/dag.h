#ifndef DAG_H
#define DAG_H


#include <cg3/geometry/point2.h>
#include <cg3/geometry/segment2.h>
#include <data_structures/Trapezoidalmap.h>

enum Types{X, Y, Leaf};

struct node{
    Types type;
    node *leftChild;
    node *rightChild;
    //node(cg3::Point2d p, cg3::Segment2d s, std::array<cg3::Point2d, 4> t){
    //this->type = Types::X;
    //}
};

class Dag{

public:
    Dag();



private:
    node *leftChild;
    node *rightChild;




private:

};
#endif // DAG_H
