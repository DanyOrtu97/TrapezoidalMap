#include "dag.h"

#include "nodedag.h"




Dag::Dag(){
    dag = nullptr;
}


void Dag::inizializeDag(std::array<cg3::Point2d, 4> boundingBox){
    class Leaf* trap = new class Leaf(boundingBox);
    dag = trap;
}

void Dag::updateDag(const cg3::Segment2d segment){

    //find trapezoid in the dag in which the segment intersect
    std::vector<std::array<cg3::Point2d, 4>> foundTrapezoids = findTrapezoids(segment);

    if(foundTrapezoids.size() > 1){
        insertMultipleTrapezoids(segment);
    }
    else{
        insertSingleTrapezoid(segment);
    }

}

std::array<cg3::Point2d, 4>  Dag::findTrapezoid(cg3::Point2d point, bool queryPoint){
    nodeDag* root = dag;

    while(dag->getType() != Types::Leaf){
        if(dag->getType() == Types::X){ //tipo punto

            dag = ((class X*)dag)->pointToPoint(point);

        } //tipo segmento
        else if(dag->getType() == Types::Y){

            dag = ((class Y*)dag)->pointToSegment(point);
        }
    }

    if(!queryPoint){
        pointersMap = {};
        pointersMap.insert(std::make_pair(((class Leaf*)dag)->getTrapezoid(), &dag));
    }

    return ((class Leaf*)dag)->getTrapezoid();
}


std::vector<std::array<cg3::Point2d, 4>> Dag::findTrapezoids(const cg3::Segment2d segment){
    cg3::Point2d p1 = segment.p1();
    cg3::Point2d p2 = segment.p2();


    std::vector<std::array<cg3::Point2d, 4>> foundTrapezoids;

    std::array<cg3::Point2d, 4> firstTrapezoid = findTrapezoid(p1, false);
    std::array<cg3::Point2d, 4> lastTrapezoid = findTrapezoid(p2, false);


    if(firstTrapezoid == lastTrapezoid){
        foundTrapezoids.push_back(firstTrapezoid);
    }
    else{
        foundTrapezoids.push_back(firstTrapezoid);
        foundTrapezoids.push_back(lastTrapezoid);
    }

    return foundTrapezoids;
}


void Dag::insertSingleTrapezoid(const cg3::Segment2d segment){
    cg3::Point2d p1 = segment.p1();
    cg3::Point2d p2 = segment.p2();


    //improve by the correct trapezoids
    *pointersMap.begin()->second = new class X(p1);
    class X* point2 = new class X(p2);
    class Y* segment1 = new class Y(segment);

    (*pointersMap.begin()->second)->setLeftChild(new class Leaf(*(traps.begin()+((traps).size()-4))));
    (*pointersMap.begin()->second)->setRightChild(point2);
    point2->setLeftChild(segment1);
    point2->setRightChild(new class Leaf(*(traps.begin()+((traps).size()-3))));
    segment1->setLeftChild(new class Leaf(*(traps.begin()+((traps).size()-2))));
    segment1->setRightChild(new class Leaf(*(traps.begin()+((traps).size()-1))));

    //cancella il dag quando effettua la ricerca e sovvrascrive

    pointersMap.erase(pointersMap.begin());
}


void Dag::insertMultipleTrapezoids(const cg3::Segment2d segment){

}


void Dag::setTrapezoidToInsert (std::vector<std::array<cg3::Point2d, 4>> trapezoids){
    traps = trapezoids;
}

nodeDag* Dag::getDag(){
    return dag;
}


