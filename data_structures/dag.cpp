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


    if(foundTrapezoids.size() > 1 ){
        insertMultipleTrapezoids(foundTrapezoids, segment);
    }
    else{
        insertSingleTrapezoid(foundTrapezoids.front(), segment);
    }

}

std::array<cg3::Point2d, 4>  Dag::findTrapezoid(cg3::Point2d point){
    nodeDag* root = dag;

    while(root->getType() != Types::Leaf){
        std::cout<<"type root: \n"<<std::endl;
        std::cout<<root->getType()<<std::endl;

        if(root->getType() == Types::X){ //tipo punto
            root = pointToPoint((class X*)root, point);
        } //tipo segmento
        else if(root->getType() == Types::Y){
            root = pointToSegment((class Y*)root, point);
        } //tipo trapezoide
    }
    std::cout<<"type end while: \n"<<std::endl;
    std::cout<<root->getType()<<std::endl;


    std::cout<<((class Leaf*)root)->getTrapezoid().begin()[0]<<std::endl;

    return ((class Leaf*)root)->getTrapezoid();
}


std::vector<std::array<cg3::Point2d, 4>> Dag::findTrapezoids(const cg3::Segment2d segment){

}

void Dag::insertSingleTrapezoid(const std::array<cg3::Point2d, 4> foundTrapezoid, const cg3::Segment2d segment){

}

void Dag::insertMultipleTrapezoids(const std::vector<std::array<cg3::Point2d, 4>> foundTrapezoids, const cg3::Segment2d segment){

}

nodeDag* Dag::pointToPoint(class X *node ,const cg3::Point2d point){
    if (point.x() < node->getPoint().x()){
        return node->getLeftChild();
    }
    else{
        return node->getRightChild();
    }
}

nodeDag* Dag::pointToSegment(class Y *node, const cg3::Point2d point){
    if(determinant(node->getSegment(), point) < 0){
        return node->getLeftChild();
    }
    else{
        return node->getRightChild();
    }
}

double Dag::determinant(const cg3::Segment2d segment, const cg3::Point2d p){
    return ((segment.p1().x() * ((segment.p2().y() * 1) - (1 * p.y())))-
           (segment.p1().y() * ((segment.p2().x() * 1) - (1 * p.x())))+
           (1 * ((segment.p2().x() * p.y()) - (segment.p2().y() * p.x()))));
}
