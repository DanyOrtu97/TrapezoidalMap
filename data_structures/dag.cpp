#include "dag.h"

#include "nodedag.h"

#include <data_structures/Trapezoidalmap.h>

#include <typeinfo>

Dag::Dag(){
    dag = nullptr;
}


void Dag::inizializeDag(std::array<cg3::Point2d, 4> boundingBox){
    class Leaf* trap = new class Leaf(boundingBox);
    dag = trap;
}

void Dag::updateDag(const cg3::Segment2d segment){

    //find trapezoid in the dag in which the segment intersect
    std::vector<std::array<cg3::Point2d, 4>> foundTrapezoids;

    std::array<cg3::Point2d, 4> firstTrapezoid = findTrapezoid(segment.p1(), false, this->getDag());
    std::array<cg3::Point2d, 4> lastTrapezoid = findTrapezoid(segment.p2(), false, this->getDag());


    if(firstTrapezoid == lastTrapezoid){
        insertSingleTrapezoid(segment);
    }
    else{
        foundTrapezoids = findTrapezoids(segment);
        insertMultipleTrapezoids(segment, foundTrapezoids);

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

    //improve by the correct trapezoids
    nodeDag* point1 = new X(p1);
    nodeDag* point2 = new X(p2);
    nodeDag* segment1 = new Y(segment);

    /*
     * verifica ordine trapezoidi
     *
     * sono tutti incasinati....
     *
     * inserire puntatori inoltre
     */

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

std::vector<std::array<cg3::Point2d, 4>> Dag::findTrapezoids(const cg3::Segment2d segment){
    cg3::Point2d p1 = segment.p1();
    cg3::Point2d p2 = segment.p2();

    std::vector<std::array<cg3::Point2d, 4>> foundTrapezoids;

    std::array<cg3::Point2d, 4> firstTrapezoid = findTrapezoid(segment.p1(), false, this->getDag());
    std::array<cg3::Point2d, 4> lastTrapezoid = findTrapezoid(segment.p2(), false, this->getDag());



    /* Trova nel dag tutti i trapezoidi tra il primo e l'ultimo
     *
     *
     *
     */






    return foundTrapezoids;
}

void Dag::insertMultipleTrapezoids(const cg3::Segment2d segment, std::vector<std::array<cg3::Point2d, 4>> foundTrapezoids){
    /* Aggiorna dag negli n trapezoidi trovati
     *
     * Ricorda ordine trapezoidi per ricerca
     *
     */
}


void Dag::setTrapezoidToInsert (std::array<cg3::Point2d, 4> trapezoid, int num){
    //poi dovrà essere un puntatore a quei valori
    nTrapezoids = num;
    traps.push_back(trapezoid);
}

nodeDag* Dag::getDag(){
    return dag;
}

void Dag::clearTraps(){
    traps.clear();
}

void Dag::clearDag(){
    traps.clear();
    dag = nullptr;
    pointersMap.clear();
}

