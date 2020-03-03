#include "dag.h"


Dag::Dag():
    trapezoidIndex(0),
    structureDag(nullptr)

{

}



void Dag::insertNewSegment(cg3::Segment2d segment){


}


void Dag::insertSubTree(cg3::Segment2d segment, int indice, node *root){
    cg3::Point2d p1 = segment.p1();
    cg3::Point2d p2 = segment.p2();


}


int Dag::findTrapezoidByPoint(cg3::Point2d){
    int result=0;

    return result;
}


int Dag::getNumberOfTrapezoids(){
    return trapezoidIndex;
}


void Dag::printDag(){

}

void Dag::clearDag(){
    structureDag = nullptr;
}

