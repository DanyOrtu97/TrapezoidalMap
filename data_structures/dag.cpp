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

    std::array<cg3::Point2d, 4> firstTrapezoid = findTrapezoid(segment.p1(), false, this->getDag());
    std::array<cg3::Point2d, 4> lastTrapezoid = findTrapezoid(segment.p2(), false, this->getDag());

    //split node
    nodeDag* splitNode= findSplitNode(segment);
    if(firstTrapezoid == lastTrapezoid){
        if(splitNode == pointersMap.begin()->second){
             insertSingleTrapezoid(segment, false);
        }
        else{
            insertSingleTrapezoid(segment, true);
        }
        pointersMap.clear();
    }
    else{
        insertMultipleTrapezoids(segment, splitNode);
        pointersMap.clear();
    }
}

std::array<cg3::Point2d, 4>  Dag::findTrapezoid(cg3::Point2d point, bool queryPoint, nodeDag* root){
    nodeDag* tmp = root;
    nodeDag* parent = nullptr;

    while(tmp->getType() != "PK4Leaf"){
        parent = tmp;
        if(tmp->getType() == "PK1X"){

            tmp = ((X*)tmp)->pointToPoint(point);

        }
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

void Dag::insertSingleTrapezoid(const cg3::Segment2d segment, bool multiSplit){
    cg3::Point2d p1 = segment.p1();
    cg3::Point2d p2 = segment.p2();

    nodeDag* point1 = new X(p1);
    nodeDag* point2 = new X(p2);
    nodeDag* segment1 = new Y(segment);


    point1->setLeftChild(new Leaf(*(traps.begin()+((traps).size()-4))));
    point1->setRightChild(point2);
    point2->setLeftChild(segment1);
    point2->setRightChild(new Leaf(*(traps.begin()+((traps).size()-1))));
    segment1->setLeftChild(new Leaf(*(traps.begin()+((traps).size()-3))));
    segment1->setRightChild(new Leaf(*(traps.begin()+((traps).size()-2))));


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
                    if(dag->determinant(((Y*)(pointersMap.begin()->second))->getSegment(), segment.p1()) > 0){
                        if(multiSplit){
                            (*((Y*)(pointersMap.begin()->second))).setLeftChild(point1);
                            pointersMap.clear();
                            std::array<cg3::Point2d, 4> lastTrapezoid = findTrapezoid(segment.p2(), false, this->getDag());
                            (*((Y*)(pointersMap.begin()->second))).setLeftChild(point1);
                        }
                        else{
                            (*((Y*)(pointersMap.begin()->second))).setLeftChild(point1);
                        }
<<<<<<< HEAD
=======

>>>>>>> 71e958c67d343affe32c788aa3ee8dc327e1d3a9
                    }
                    else{
                        if(multiSplit){
                            (*((Y*)(pointersMap.begin()->second))).setRightChild(point1);
                            pointersMap.clear();
                            std::array<cg3::Point2d, 4> lastTrapezoid = findTrapezoid(segment.p2(), false, this->getDag());
                            (*((Y*)(pointersMap.begin()->second))).setRightChild(point1);

                        }
                        else{
                            (*((Y*)(pointersMap.begin()->second))).setRightChild(point1);
                        }
                    }
            }
        }

    }
    //scrivere sul pointer e modificarli entrambi, non solo uno
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
            if(node->determinant(((Y*)node)->getSegment(), p1) > 0 && node->determinant(((Y*)node)->getSegment(), p2) > 0){
                findTrapezoids(segment, node->getLeftChild());
            }
            else if(node->determinant(((Y*)node)->getSegment(), p1) < 0 && node->determinant(((Y*)node)->getSegment(), p2) < 0){
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
        if(leftPoint->getType() == "PK1X"){

            leftPoint = ((X*)leftPoint)->pointToPoint(segment.p1());

        }
        else if(leftPoint->getType() == "PK1Y"){

            leftPoint = ((Y*)leftPoint)->pointToSegment(segment.p1());
        }

        /*
         * right endpoint
         */
        if(rightPoint->getType() == "PK1X"){

            rightPoint = ((X*)rightPoint)->pointToPoint(segment.p2());

        }
        else if(rightPoint->getType() == "PK1Y"){

            rightPoint = ((Y*)rightPoint)->pointToSegment(segment.p2());
        }
    }

    return parent;
}

void Dag::insertMultipleTrapezoids(const cg3::Segment2d segment, nodeDag* splitNode){
    cg3::Point2d p1 = segment.p1();
    cg3::Point2d p2 = segment.p2();
    nodeDag* point1 = new X(p1);
    nodeDag* point2 = new X(p2);
    nodeDag* segment1a = new Y(segment);
    bool up;

    for(std::map<std::array<cg3::Point2d, 4>, nodeDag*>::iterator it = pointersMap.begin(); it!=pointersMap.end(); it++){
<<<<<<< HEAD

        if(it == pointersMap.begin()){ //first trapezoid found

            point1->setLeftChild(new Leaf(*(traps.begin()+((traps).size()-(traps.size())))));
            point1->setRightChild(segment1a);
            if(it->second->getLeftChild()->getType() == "PK4Leaf" &&
               it->second->getRightChild()->getType() != "PK4Leaf"){
                if(pointersMap.size() == 2){ //two traps
=======
        if(pointersMap.size() == 2){ //two traps

            if(it == pointersMap.begin()){ //first trapezoid found

                point1->setLeftChild(new Leaf(*(traps.begin()+((traps).size()-(traps.size())))));
                point1->setRightChild(segment1a);
                if(it->second->getLeftChild()->getType() == "PK4Leaf" &&
                   it->second->getRightChild()->getType() != "PK4Leaf"){
>>>>>>> 71e958c67d343affe32c788aa3ee8dc327e1d3a9
                    segment1a->setLeftChild(new Leaf(*(traps.begin()+((traps).size()-(traps.size()-1)))));
                    segment1a->setRightChild(new Leaf(*(traps.begin()+((traps).size()-(traps.size()-2)))));
                    it->second->setLeftChild(point1);
                }
<<<<<<< HEAD

                else{
                    if(pointersMap.size() > 3){
                        segment1a->setLeftChild(new Leaf(*(traps.begin()+((traps).size()-(traps.size()-1)))));
                        segment1a->setRightChild(new Leaf(*(traps.begin()+((traps).size()-(traps.size()-2)))));
                        it->second->setLeftChild(point1);
                    }
                    else{
                        if(splitNode->getType() == "PK1X" && dag->determinant(segment, ((X*)splitNode)->getPoint()) < 0){ //sopra
                            segment1a->setLeftChild(new Leaf(*(traps.begin()+((traps).size()-(traps.size()-5)))));
                            segment1a->setRightChild(new Leaf(*(traps.begin()+((traps).size()-(traps.size()-1)))));
                            it->second->setLeftChild(point1);
                        }
                        else{
                            segment1a->setLeftChild(new Leaf(*(traps.begin()+((traps).size()-(traps.size()-1)))));
                            segment1a->setRightChild(new Leaf(*(traps.begin()+((traps).size()-(traps.size()-5)))));
                            it->second->setLeftChild(point1);
                        }

                    }
                }
            }
            else if(it->second->getRightChild()->getType() == "PK4Leaf" &&
                    it->second->getLeftChild()->getType() != "PK4Leaf"){
                if(pointersMap.size() == 2){ //two traps
=======
                else if(it->second->getRightChild()->getType() == "PK4Leaf" &&
                        it->second->getLeftChild()->getType() != "PK4Leaf"){
>>>>>>> 71e958c67d343affe32c788aa3ee8dc327e1d3a9
                    segment1a->setLeftChild(new Leaf(*(traps.begin()+((traps).size()-(traps.size()-2)))));
                    segment1a->setRightChild(new Leaf(*(traps.begin()+((traps).size()-(traps.size()-1)))));
                    it->second->setRightChild(point1);
                }
<<<<<<< HEAD
                //some stuff
            }
            else{
                if(it->second->getRightChild()->getType() == "PK4Leaf" &&
                       it->second->getLeftChild()->getType() == "PK4Leaf"){
                        if(dag->determinant(((Y*)(it->second))->getSegment(), segment.p1()) > 0){
                            if(pointersMap.size() == 2){ //two traps
=======
                else{
                    if(it->second->getRightChild()->getType() == "PK4Leaf" &&
                           it->second->getLeftChild()->getType() == "PK4Leaf"){
                            if(dag->determinant(((Y*)(it->second))->getSegment(), segment.p1()) > 0){
>>>>>>> 71e958c67d343affe32c788aa3ee8dc327e1d3a9
                                segment1a->setLeftChild(new Leaf(*(traps.begin()+((traps).size()-(traps.size()-1)))));
                                segment1a->setRightChild(new Leaf(*(traps.begin()+((traps).size()-(traps.size()-2)))));
                                (*((Y*)(it->second))).setLeftChild(point1);
                                up=true;
                            }
<<<<<<< HEAD
                            //some stuff
                        }
                        else{
                            if(pointersMap.size() == 2){ //two traps
=======
                            else{
>>>>>>> 71e958c67d343affe32c788aa3ee8dc327e1d3a9
                                segment1a->setLeftChild(new Leaf(*(traps.begin()+((traps).size()-(traps.size()-1)))));
                                segment1a->setRightChild(new Leaf(*(traps.begin()+((traps).size()-(traps.size()-2)))));
                                (*((Y*)(it->second))).setRightChild(point1);
                                up=false;
                            }
<<<<<<< HEAD
                            //some stuff
                        }
                }
            }
        }
        else if(it == --(pointersMap.end())){
            nodeDag* segment1b = new Y(segment);
            point2->setLeftChild(segment1b);
            point2->setRightChild(new Leaf(*(traps.begin()+((traps).size()-(traps.size()-3)))));
            if(it->second->getLeftChild()->getType() == "PK4Leaf" &&
               it->second->getRightChild()->getType() != "PK4Leaf"){
                if(pointersMap.size() == 2){ //two traps
=======
                    }
                }
            }
            else{
                nodeDag* segment1b = new Y(segment);
                point2->setLeftChild(segment1b);
                point2->setRightChild(new Leaf(*(traps.begin()+((traps).size()-(traps.size()-3)))));
                if(it->second->getLeftChild()->getType() == "PK4Leaf" &&
                   it->second->getRightChild()->getType() != "PK4Leaf"){
>>>>>>> 71e958c67d343affe32c788aa3ee8dc327e1d3a9
                    segment1b->setLeftChild(segment1a->getLeftChild());
                    segment1b->setRightChild(new Leaf(*(traps.begin()+((traps).size()-1))));
                     it->second->setLeftChild(point2);
                }
<<<<<<< HEAD
                //some stuff

            }
            else if(it->second->getRightChild()->getType() == "PK4Leaf" &&
                    it->second->getLeftChild()->getType() != "PK4Leaf"){
                if(up){
                    if(pointersMap.size() == 2){ //two traps
=======
                else if(it->second->getRightChild()->getType() == "PK4Leaf" &&
                        it->second->getLeftChild()->getType() != "PK4Leaf"){
                    if(up){
>>>>>>> 71e958c67d343affe32c788aa3ee8dc327e1d3a9
                        segment1b->setLeftChild(segment1a->getLeftChild());
                        segment1b->setRightChild(new Leaf(*(traps.begin()+((traps).size()-1))));
                        it->second->setRightChild(point2);
                    }
<<<<<<< HEAD
                    //some stuff
                }
                else{
                    if(pointersMap.size() == 2){ //two traps
=======
                    else{
>>>>>>> 71e958c67d343affe32c788aa3ee8dc327e1d3a9
                        segment1b->setLeftChild(new Leaf(*(traps.begin()+((traps).size()-1))));
                        segment1b->setRightChild(segment1a->getRightChild());
                        it->second->setRightChild(point2);
                    }
<<<<<<< HEAD
                    //some stuff
                }
            }
            else{
                if(it->second->getRightChild()->getType() == "PK4Leaf" &&
                       it->second->getLeftChild()->getType() == "PK4Leaf"){
                        if(dag->determinant(((Y*)(it->second))->getSegment(), segment.p1()) > 0){
                            if(pointersMap.size() == 2){ //two traps
=======
                }
                else{
                    if(it->second->getRightChild()->getType() == "PK4Leaf" &&
                           it->second->getLeftChild()->getType() == "PK4Leaf"){
                            if(dag->determinant(((Y*)(it->second))->getSegment(), segment.p1()) > 0){
>>>>>>> 71e958c67d343affe32c788aa3ee8dc327e1d3a9
                                segment1b->setLeftChild(segment1a->getLeftChild());
                                segment1b->setRightChild(new Leaf(*(traps.begin()+((traps).size()-1))));
                                (*((Y*)(it->second))).setLeftChild(point2);
                            }
<<<<<<< HEAD
                            //some stuff
                        }
                        else{
                            if(pointersMap.size() == 2){ //two traps
=======
                            else{
>>>>>>> 71e958c67d343affe32c788aa3ee8dc327e1d3a9
                                segment1b->setLeftChild(new Leaf(*(traps.begin()+((traps).size()-1))));
                                segment1b->setRightChild(segment1a->getRightChild());
                                (*((Y*)(it->second))).setRightChild(point2);
                            }
<<<<<<< HEAD
                            //some stuff
                        }
                }
            }
        }
        else{ //more than two traps
            nodeDag* segment1ca = new Y(segment);
            nodeDag* segment1cb = new Y(segment);
            nodeDag* segment1cc = new Y(segment);
            nodeDag* segment1cd = new Y(segment);
            /*
            if(pointersMap.begin()->second->getLeftChild()->getType() == "PK4Leaf" &&
               pointersMap.begin()->second->getRightChild()->getType() != "PK4Leaf"){
               segment1ca->setLeftChild(new Leaf(*(traps.begin()+((traps).size()-(traps.size()-4)))));
               segment1ca->setRightChild(segment1a->getRightChild());
               pointersMap.begin()->second->setLeftChild(segment1ca);
            }
            else if(pointersMap.begin()->second->getRightChild()->getType() == "PK4Leaf" &&
                    pointersMap.begin()->second->getLeftChild()->getType() != "PK4Leaf"){
                segment1cb->setLeftChild();
                segment1cb->setRightChild();
                pointersMap.begin()->second->setRightChild(segment1cb);
            }
            else{
                if(pointersMap.begin()->second->getRightChild()->getType() == "PK4Leaf" &&
                       pointersMap.begin()->second->getLeftChild()->getType() == "PK4Leaf"){
                        if(dag->determinant(((Y*)(pointersMap.begin()->second))->getSegment(), segment.p1()) > 0){
                            segment1cc->setLeftChild(segment1ca->getLeftChild());
                            segment1cc->setRightChild(new Leaf(*(traps.begin()+((traps).size()-(traps.size()-6)))));
                            (*((Y*)(pointersMap.begin()->second))).setLeftChild(segment1cc);
                        }
                        else{
                            segment1cd->setLeftChild();
                            segment1cd->setRightChild();
                            (*((Y*)(pointersMap.begin()->second))).setRightChild(segment1cd);
                        }
                }
            }
            */
        }

=======
                    }
                }
            }

        }
        else{ //more than two traps

        }
>>>>>>> 71e958c67d343affe32c788aa3ee8dc327e1d3a9
    }
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

