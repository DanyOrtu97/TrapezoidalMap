#include "dag.h"

Dag::Dag()
{


}

void Dag::insertNewSegment(cg3::Segment2d segment){
    cg3::Point2d p1 = segment.p1();
    cg3::Point2d p2 = segment.p2();

}



void Dag::clearDag(){
    segments.clear();
    endpointsLeft.clear();
    endpointsRight.clear();
    leaves.clear();
}
