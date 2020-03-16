#include "dag.h"
#include "nodedag.h"
#include <data_structures/trapezoidalMap.h>
#include <typeinfo>

Dag::Dag(){
    dag = nullptr;
    TrapezoidalMap trapezoidalMap;
}


void Dag::inizializeDag(std::array<cg3::Point2d, 4> boundingBox){
    class Leaf* trap = new class Leaf(boundingBox);
    dag = trap;
}

void Dag::updateDag(const cg3::Segment2d segment){

    //find trapezoid in the dag in which the segment intersect

    std::array<cg3::Point2d, 4> firstTrapezoid = findTrapezoid(segment.p1(), false, this->getDag());
    std::array<cg3::Point2d, 4> lastTrapezoid = findTrapezoid(segment.p2(), false, this->getDag());

    if(firstTrapezoid == lastTrapezoid){
        insertSingleTrapezoid(segment);
    }
    else{
        pointersMap.clear();
        //split node
        nodeDag* splitNode= findSplitNode(segment);
        insertMultipleTrapezoids(segment, splitNode);
    }
}

std::array<cg3::Point2d, 4>  Dag::findTrapezoid(cg3::Point2d point, bool queryPoint, nodeDag* root){
    nodeDag* tmp = root;
    nodeDag* parent = nullptr;

    while(tmp->getType() != "PK4Leaf"){
        parent = tmp;
        if(tmp->getType() == "PK1X"){ //tipo punto

            tmp = ((X*)tmp)->pointToPoint(point);

        } //tipo segmento
        else if(tmp->getType() == "PK1Y"){

            tmp = ((Y*)tmp)->pointToSegment(point);
        }

    }

    if(!queryPoint){
        if(parent == nullptr){
            pointersMap.insert(std::make_pair(((Leaf*)tmp)->getTrapezoid(), tmp));
        }
        else{
            pointersMap.insert(std::make_pair(((Leaf*)tmp)->getTrapezoid(), parent));
        }
    }

    return ((Leaf*)tmp)->getTrapezoid();
}

void Dag::insertSingleTrapezoid(const cg3::Segment2d segment){
    cg3::Point2d p1 = segment.p1();
    cg3::Point2d p2 = segment.p2();

    nodeDag* point1 = new X(p1);
    nodeDag* point2 = new X(p2);
    nodeDag* segment1 = new Y(segment);


    point1->setLeftChild(new Leaf(*(traps.begin()+((traps).size()-4))));
    point1->setRightChild(point2);
    point2->setLeftChild(segment1);
    point2->setRightChild(new Leaf(*(traps.begin()+((traps).size()-1))));
    segment1->setLeftChild(new Leaf(*(traps.begin()+((traps).size()-2))));
    segment1->setRightChild(new Leaf(*(traps.begin()+((traps).size()-3))));


    if(nTrapezoids <= 4){
        dag = point1;
    }
    else{
        if(pointersMap.begin()->second->getLeftChild()->getType() == "PK4Leaf" &&
           pointersMap.begin()->second->getRightChild()->getType() != "PK4Leaf"){
           pointersMap.begin()->second->setLeftChild(point1);
        }
        else if(pointersMap.begin()->second->getRightChild()->getType() == "PK4Leaf" &&
                pointersMap.begin()->second->getLeftChild()->getType() != "PK4Leaf"){
           pointersMap.begin()->second->setRightChild(point1);
        }
        else{
            if(pointersMap.begin()->second->getRightChild()->getType() == "PK4Leaf" &&
                   pointersMap.begin()->second->getLeftChild()->getType() == "PK4Leaf"){
                    if(dag->determinant(((Y*)(pointersMap.begin()->second))->getSegment(), segment.p1()) < 0){
                        (*((Y*)(pointersMap.begin()->second))).setLeftChild(point1);
                    }
                    else{
                        (*((Y*)(pointersMap.begin()->second))).setRightChild(point1);
                    }
            }
        }

    }
    pointersMap.erase(pointersMap.begin());
}

void Dag::findTrapezoids(const cg3::Segment2d segment, nodeDag* node){
    cg3::Point2d p1 = segment.p1();
    cg3::Point2d p2 = segment.p2();

    nodeDag* parent = nullptr;

    if(node->getType() != "PK4Leaf"){
        parent=node;
        if(node->getType() == "PK1X"){
            if(p1.x() < ((X*)node)->getPoint().x()){
                findTrapezoids(segment, node->getLeftChild());
            }
            if(p2.x() > ((X*)node)->getPoint().x()){
                findTrapezoids(segment, node->getRightChild());
            }
        }
        else if(node->getType() == "PK1Y"){
            if(node->determinant(((Y*)node)->getSegment(), p1) < 0 && node->determinant(((Y*)node)->getSegment(), p2)){
                findTrapezoids(segment, node->getLeftChild());
            }
            else{
                findTrapezoids(segment, node->getRightChild());
            }
        }
    }
    else{
        if(parent == nullptr){
            pointersMap.insert(std::make_pair(((Leaf*)node)->getTrapezoid(), node));
        }
        else{
            pointersMap.insert(std::make_pair(((Leaf*)node)->getTrapezoid(), parent));
        }
    }

}

nodeDag* Dag::findSplitNode(const cg3::Segment2d segment){
    nodeDag* leftPoint = this->getDag();
    nodeDag* rightPoint = this->getDag();
    nodeDag* parent = nullptr;

    while((leftPoint->getType() != "PK4Leaf" || rightPoint->getType() != "PK4Leaf") && leftPoint == rightPoint){
        parent = leftPoint;

        /*
         * left endpoint
         */
        if(leftPoint->getType() == "PK1X"){ //tipo punto

            leftPoint = ((X*)leftPoint)->pointToPoint(segment.p1());

        } //tipo segmento
        else if(leftPoint->getType() == "PK1Y"){

            leftPoint = ((Y*)leftPoint)->pointToSegment(segment.p1());
        }

        /*
         * right endpoint
         */
        if(rightPoint->getType() == "PK1X"){ //tipo punto

            rightPoint = ((X*)rightPoint)->pointToPoint(segment.p2());

        } //tipo segmento
        else if(rightPoint->getType() == "PK1Y"){

            rightPoint = ((Y*)rightPoint)->pointToSegment(segment.p2());
        }
    }

    return parent;
}

void Dag::insertMultipleTrapezoids(const cg3::Segment2d segment, nodeDag* splitNode){
    /* Aggiorna dag negli n trapezoidi trovati
     *
     * Ricorda ordine trapezoidi per ricerca
     *
     */
}


void Dag::setTrapezoidToInsert (std::array<cg3::Point2d, 4> trapezoid, int num){
    nTrapezoids = num;
    traps.push_back(trapezoid);
}

nodeDag* Dag::getDag(){
    return dag;
}

std::map<std::array<cg3::Point2d, 4>, nodeDag*> Dag::getPointerMap(){
    return pointersMap;
}

void Dag::clearTraps(){
    traps.clear();
}

void Dag::clearDag(){
    traps.clear();
    dag = nullptr;
    pointersMap.clear();
}

