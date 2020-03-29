#include "dag.h"
#include "nodedag.h"
#include <data_structures/trapezoidalMap.h>
#include <typeinfo>
#include <algorithms/algorithms.h>
#include <data_structures/trapezoid.h>

/**
 * @brief Constructor of dag
 */
Dag::Dag(){
    dag = nullptr;
    TrapezoidalMap trapezoidalMap;
}

/**
 * @brief Method to inzialize dag with the leaf for the bounding box
 * @param[in] boundingBox Trapezoid
 */
void Dag::inizializeDag(trapezoid boundingBox){
    Leaf* trap = new Leaf(boundingBox);
    dag = trap;
}

/**
 * @brief Method to update the dag with the new subtrees and the new trapezoids
 * @param[in] segment Segment
 */
void Dag::updateDag(const cg3::Segment2d segment){

    trapezoid firstTrapezoid = findTrapezoid(segment.p1(), false, this->getDag());
    trapezoid lastTrapezoid = findTrapezoid(segment.p2(), false, this->getDag());

    //split node
    nodeDag* splitNode= GasAlgorithms::findSplitNode(segment, this->getDag());

    if(firstTrapezoid.getTrapezoid() == lastTrapezoid.getTrapezoid()){
        insertSingleTrapezoid(segment);
        pointersMap.clear();
    }
    else{
        insertMultipleTrapezoids(segment, splitNode);
        pointersMap.clear();
    }
}

/**
 * @brief Method to process the point location query or to find the single trapezoid by point
 * @param[in] point Point, queryPoint bool, root nodeDag*
 * @param[out] trapezoid Trapezoid
 */
trapezoid  Dag::findTrapezoid(cg3::Point2d point, bool queryPoint, nodeDag* root){
    nodeDag** tmp = &root;

    while((*tmp)->getType() != "PK4Leaf"){
        if((*tmp)->getType() == "PK1X"){

            tmp = ((X*)*tmp)->pointToPoint(point);

        }
        else if((*tmp)->getType() == "PK1Y"){

            tmp = ((Y*)*tmp)->pointToSegment(point);
        }

    }
    if(!queryPoint){
        pointersMap.insert(std::make_pair(((Leaf*)*tmp)->getTrapezoid(), tmp));
    }

    return ((Leaf*)*tmp)->getTrapezoid();
}


/**
 * @brief Method to find multiple trapezoids in which our new segment passes
 * @param[in] segment Segment, node nodeDag*, temp nodeDag*
 */
void Dag::findTrapezoids(const cg3::Segment2d segment, nodeDag* node, nodeDag* temp){
    cg3::Point2d p1 = segment.p1();
    cg3::Point2d p2 = segment.p2();

    nodeDag** tmp= &node;

    if((*tmp)->getType() != "PK4Leaf"){
        temp=*tmp;
        if((*tmp)->getType() == "PK1X"){
            if(p1.x() < ((X*)(*tmp))->getPoint().x()){
                findTrapezoids(segment, (*tmp)->getLeftChild(), temp);
            }
            if(p2.x() > ((X*)(*tmp))->getPoint().x()){
                findTrapezoids(segment, (*tmp)->getRightChild(), temp);
            }
        }
        else if((*tmp)->getType() == "PK1Y"){
            if(((*tmp)->determinant(((Y*)(*tmp))->getSegment(), p1) > 0 && (*tmp)->determinant(((Y*)(*tmp))->getSegment(), p2) > 0) ||
                   ((*tmp)->determinant(segment, ((Y*)(*tmp))->getSegment().p1()) < 0 || (*tmp)->determinant(segment, ((Y*)(*tmp))->getSegment().p2()) < 0)){
                findTrapezoids(segment, (*tmp)->getLeftChild(), temp);
            }
            else if(((*tmp)->determinant(((Y*)(*tmp))->getSegment(), p1) < 0 && (*tmp)->determinant(((Y*)(*tmp))->getSegment(), p2) < 0) ||
                   ((*tmp)->determinant(segment, ((Y*)(*tmp))->getSegment().p1()) > 0 || (*tmp)->determinant(segment, ((Y*)(*tmp))->getSegment().p2()) > 0)){
                findTrapezoids(segment, (*tmp)->getRightChild(), temp);
            }
        }
    }
    else{      
        if((temp)->getType() == "PK1Y"){
            if(((temp)->determinant(((Y*)(temp))->getSegment(), p1) > 0 && (temp)->determinant(((Y*)(temp))->getSegment(), p2) > 0) ||
                    ((temp)->determinant(segment, ((Y*)(temp))->getSegment().p1()) < 0 || (temp)->determinant(segment, ((Y*)(temp))->getSegment().p2()) < 0)){
                pointersMap.insert(std::make_pair(((Leaf*)*tmp)->getTrapezoid(), temp->getLeftChildP()));
            }
            else{
                pointersMap.insert(std::make_pair(((Leaf*)*tmp)->getTrapezoid(), temp->getRightChildP()));
            }
        }
        else if((temp)->getType() == "PK1X"){
            if(temp->getLeftChild()->getType() == "PK4Leaf"){
                pointersMap.insert(std::make_pair(((Leaf*)*tmp)->getTrapezoid(), temp->getLeftChildP()));
            }
            else{
                pointersMap.insert(std::make_pair(((Leaf*)*tmp)->getTrapezoid(), temp->getRightChildP()));
            }
        }
    }
}

/**
 * @brief Method to update the dag in the case of insertion in a single trapezoid
 * @param[in] segment Segment
 */
void Dag::insertSingleTrapezoid(const cg3::Segment2d segment){
    cg3::Point2d p1 = segment.p1();
    cg3::Point2d p2 = segment.p2();

    nodeDag* point1 = new X(p1, false);
    nodeDag* point2 = new X(p2, true);
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
        trapezoid trap = pointersMap.begin()->first;
        ((*(pointersMap.begin()->second)))=point1;

        findMultipleTrapezoids(trap, this->getDag(), nullptr);
        /*
        if(trap.getTrapezoid() == (findTrapezoid(segment.p2(), false, this->getDag())).getTrapezoid()){
            ((*(pointersMap.begin()->second)))=point1;
        }*/
        for(std::map<trapezoid, nodeDag**>::iterator it = multipleAdresses.begin(); it!=multipleAdresses.end(); it++){
            *(it->second)=point1;
        }

    }
    pointersMap.erase(pointersMap.begin());
    multipleAdresses.clear();
}


/**
 * @brief Method to update the dag with the new subtrees and the new trapezoids in the case of multiple trapezoids
 * @param[in] segment Segment, splitNode nodeDag*
 */
void Dag::insertMultipleTrapezoids(const cg3::Segment2d segment, nodeDag* splitNode){
    cg3::Point2d p1 = segment.p1();
    cg3::Point2d p2 = segment.p2();
    nodeDag* point1 = new X(p1, false);
    nodeDag* point2 = new X(p2, true);
    nodeDag* segment1a = new Y(segment);
    bool up, invert=false, upInitial=false, downInitial=false;

    nodeDag* segmentInner;
    nodeDag* segmentInner1;

    int contaStep=0, contaInvert=0;


    //sort traps (n log n) I need it
    if(pointersMap.size() > 2){
        std::sort(traps.begin(), traps.end(), [](trapezoid a, trapezoid b){
            if(a.trapezoid::getTrapezoid()[0].x() < b.trapezoid::getTrapezoid()[0].x()){
                return true;
            }
            else if(a.trapezoid::getTrapezoid()[0].x() == b.trapezoid::getTrapezoid()[0].x()){
                if(a.trapezoid::getTrapezoid()[0].y() > b.trapezoid::getTrapezoid()[0].y()){
                    return true;
                }
                else{
                    return false;
                }
            }
            else{
                return false;
            }
        });
    }


    for(std::map<trapezoid, nodeDag**>::iterator it = pointersMap.begin(); it!=pointersMap.end(); it++){
        trapezoid trap=it->first;


        findMultipleTrapezoids(trap, this->getDag(), nullptr);


        if(it == pointersMap.begin()){
            if(splitNode->getType() == "PK1X"){
                if(dag->determinant(segment, (((X*)splitNode))->getPoint()) < 0){
                    up=true;
                }
                else{
                    up=false;
                }
            }

            if(trap.getTrapezoid()[0].y() == (1e+6)){
                upInitial=true;
            }
            if(trap.getTrapezoid()[3].y() == -(1e+6)){
                downInitial=true;
            }
            point1->setRightChild(segment1a);
            point1->setLeftChild(new Leaf(*(traps.begin()+contaStep)));
            contaStep++;
            segment1a->setLeftChild(new Leaf(*(traps.begin()+contaStep)));
            contaStep++;
            segment1a->setRightChild(new Leaf(*(traps.begin()+contaStep)));
            contaStep++;
            (*it->second)=point1;

            for(std::map<trapezoid, nodeDag**>::iterator it2 = multipleAdresses.begin(); it2!=multipleAdresses.end(); it2++){
                *(it2->second)=point1;
            }
        }
        else if(it == --(pointersMap.end())){
            nodeDag* segment1b = new Y(segment);
            point2->setLeftChild(segment1b);
            if(pointersMap.size() == 2){
                point2->setRightChild(new Leaf(*(traps.begin()+contaStep)));
                if(up){
                    segment1b->setLeftChild(segment1a->getLeftChild());
                    segment1b->setRightChild(new Leaf(*(traps.begin()+((traps).size()-1))));
                }
                else{
                    segment1b->setLeftChild(new Leaf(*(traps.begin()+((traps).size()-1))));
                    segment1b->setRightChild(segment1a->getRightChild());
                }
                (*it->second)=point2;

                for(std::map<trapezoid, nodeDag**>::iterator it2 = multipleAdresses.begin(); it2!=multipleAdresses.end(); it2++){
                    *(it2->second)=point2;
                }
            }
            else{
                if(up){
                    if(trap.getTrapezoid()[0].y() != (1e+6)){
                        invert=!invert;
                    }
                    if(contaInvert > 1){
                        if(trap.getTrapezoid()[3].y() == -(1e+6)){
                            invert=!invert;
                        }
                    }
                    if(invert){
                        point2->setRightChild(new Leaf(*(traps.begin()+((traps).size()-1))));
                        segment1b->setLeftChild(new Leaf(*(traps.begin()+contaStep)));
                        segment1b->setRightChild(segmentInner->getRightChild());
                    }
                    else{
                        point2->setRightChild(new Leaf(*(traps.begin()+((traps).size()-1))));
                        segment1b->setLeftChild(segmentInner->getLeftChild());
                        segment1b->setRightChild(new Leaf(*(traps.begin()+contaStep)));
                    }
                }
                else{
                    if(trap.getTrapezoid()[3].y() != -(1e+6)){
                        invert=!invert;
                    }
                    if(contaInvert > 1){
                        if(trap.getTrapezoid()[0].y() == (1e+6)){
                            invert=!invert;
                        }
                    }
                    if(invert){
                        point2->setRightChild(new Leaf(*(traps.begin()+((traps).size()-1))));
                        segment1b->setLeftChild(segmentInner->getLeftChild());
                        segment1b->setRightChild(new Leaf(*(traps.begin()+contaStep)));
                    }
                    else{
                        point2->setRightChild(new Leaf(*(traps.begin()+((traps).size()-1))));
                        segment1b->setLeftChild(new Leaf(*(traps.begin()+contaStep)));
                        segment1b->setRightChild(segmentInner->getRightChild());
                    }
                }
                (*it->second)=point2;

                for(std::map<trapezoid, nodeDag**>::iterator it2 = multipleAdresses.begin(); it2!=multipleAdresses.end(); it2++){
                    *(it2->second)=point2;
                }
            }
        }
        else{ //more than two traps
            if(pointersMap.size() > 2 && it == ++pointersMap.begin()){
                segmentInner = new Y(segment);
                if(up){
                    if(downInitial && trap.getTrapezoid()[3].y() != -(1e+6)){
                        segmentInner->setLeftChild(segment1a->getLeftChild());
                        segmentInner->setRightChild(new Leaf(*(traps.begin()+contaStep)));
                        contaStep++;
                    }
                    else{
                        segmentInner->setLeftChild(new Leaf(*(traps.begin()+contaStep)));
                        contaStep++;
                        segmentInner->setRightChild(segment1a->getRightChild());
                    }
                    if(trap.getTrapezoid()[0].y() != (1e+6)){
                        invert=!invert;
                        contaInvert++;
                    }
                    else if(trap.getTrapezoid()[3].y() == -(1e+6)){
                        invert=!invert;
                        contaInvert++;
                    }
                }
                else{
                    if(upInitial && trap.getTrapezoid()[0].y() != (1e+6)){
                        segmentInner->setLeftChild(new Leaf(*(traps.begin()+contaStep)));
                        contaStep++;
                        segmentInner->setRightChild(segment1a->getRightChild());
                    }
                    else{
                        segmentInner->setLeftChild(segment1a->getLeftChild());
                        segmentInner->setRightChild(new Leaf(*(traps.begin()+contaStep)));
                        contaStep++;

                    }
                    if(trap.getTrapezoid()[3].y() != -(1e+6)){
                        invert=!invert;
                        contaInvert++;
                    }
                    else if(trap.getTrapezoid()[0].y() == (1e+6)){
                        invert=!invert;
                        contaInvert++;
                    }
                }
                (*it->second)=segmentInner;

                for(std::map<trapezoid, nodeDag**>::iterator it2 = multipleAdresses.begin(); it2!=multipleAdresses.end(); it2++){
                    *(it2->second)=segmentInner;
                }
            }
            else{
                segmentInner1 = new Y(segment);
                if(up){
                    if(trap.getTrapezoid()[0].y() != (1e+6)){
                        invert=!invert;
                        contaInvert++;
                    }
                    else if(trap.getTrapezoid()[3].y() == -(1e+6) && contaInvert>1){
                        invert=!invert;
                        contaInvert++;
                    }
                    if(invert){
                        segmentInner1->setLeftChild(new Leaf(*(traps.begin()+contaStep)));
                        contaStep++;
                        segmentInner1->setRightChild(segmentInner->getRightChild());
                    }
                    else{
                        segmentInner1->setLeftChild(segmentInner->getLeftChild());
                        segmentInner1->setRightChild(new Leaf(*(traps.begin()+contaStep)));
                        contaStep++;
                    }
                }
                else{
                    if(trap.getTrapezoid()[3].y() != -(1e+6)){
                        invert=!invert;
                        contaInvert++;
                    }
                    else if(trap.getTrapezoid()[0].y() == (1e+6) && contaInvert>1){
                        invert=!invert;
                        contaInvert++;
                    }
                    if(invert){
                        segmentInner1->setLeftChild(segmentInner->getLeftChild());
                        segmentInner1->setRightChild(new Leaf(*(traps.begin()+contaStep)));
                        contaStep++;
                    }
                    else{
                        segmentInner1->setLeftChild(new Leaf(*(traps.begin()+contaStep)));
                        contaStep++;
                        segmentInner1->setRightChild(segmentInner->getRightChild());
                    }
                }
                segmentInner = segmentInner1;
                (*it->second)=segmentInner;

                for(std::map<trapezoid, nodeDag**>::iterator it2 = multipleAdresses.begin(); it2!=multipleAdresses.end(); it2++){
                    *(it2->second)=segmentInner;
                }
            }
        }
        multipleAdresses.clear();
    }
}


void Dag::findMultipleTrapezoids(trapezoid trap, nodeDag* node, nodeDag* temp){

    nodeDag** tmp= &node;

    if((*tmp)->getType() != "PK4Leaf"){
        temp=*tmp;

        findMultipleTrapezoids(trap, (*tmp)->getLeftChild(), temp);

        findMultipleTrapezoids(trap, (*tmp)->getRightChild(), temp);

    }
    else{
        if(((Leaf*)*tmp)->getTrapezoid().getTrapezoid() == trap.getTrapezoid()){
            if((temp)->getType() == "PK1Y"){
                if((temp)->getLeftChild()->getType() == "PK4Leaf"){
                    if(((Leaf*)(temp)->getLeftChild())->getTrapezoid().getTrapezoid() == trap.getTrapezoid()){
                        multipleAdresses.insert(std::make_pair(((Leaf*)*tmp)->getTrapezoid(), temp->getLeftChildP()));
                    }
                }

                if((temp)->getRightChild()->getType() == "PK4Leaf"){
                    if(((Leaf*)(temp)->getRightChild())->getTrapezoid().getTrapezoid() == trap.getTrapezoid()){
                        multipleAdresses.insert(std::make_pair(((Leaf*)*tmp)->getTrapezoid(), temp->getRightChildP()));
                    }
                }
            }
            else if((temp)->getType() == "PK1X"){
                if((temp)->getLeftChild()->getType() == "PK4Leaf"){
                    if(((Leaf*)(temp)->getLeftChild())->getTrapezoid().getTrapezoid() == trap.getTrapezoid()){
                        multipleAdresses.insert(std::make_pair(((Leaf*)*tmp)->getTrapezoid(), temp->getLeftChildP()));
                    }
                }

                if((temp)->getRightChild()->getType() == "PK4Leaf"){
                    if(((Leaf*)(temp)->getRightChild())->getTrapezoid().getTrapezoid() == trap.getTrapezoid()){
                        multipleAdresses.insert(std::make_pair(((Leaf*)*tmp)->getTrapezoid(), temp->getRightChildP()));
                    }
                }
            }
        }
    }
}

/**
 * @brief Method to retrieve the trapezoids for insertion
 * @param[in] trapezoid Trapezoid, num Int
 */
void Dag::setTrapezoidToInsert (trapezoid trapezoid, int num){
    nTrapezoids = num;
    traps.push_back(trapezoid);
}

/**
 * @brief Method to get the dag
 */
nodeDag* Dag::getDag(){
    return dag;
}

/**
 * @brief Method to get the pointer map
 */
std::map<trapezoid, nodeDag**> Dag::getPointerMap(){
    return pointersMap;
}

/**
 * @brief Method to clear the vector with the new traps for the insertion
 */
void Dag::clearTraps(){
    traps.clear();
}

/**
 * @brief Method to clear the structures
 */
void Dag::clearDag(){
    traps.clear();
    dag = nullptr;
    pointersMap.clear();
    multipleAdresses.clear();
}

