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
    if(pointersMap.size()==1){
        insertSingleTrapezoid(segment);
        pointersMap.clear();
    }
    else{
        insertMultipleTrapezoids(segment);
        pointersMap.clear();
    }
}

/**
 * @brief Method to process the point location query or to find the single trapezoid by point
 * @param[in] point Point, queryPoint bool, root nodeDag*
 * @param[out] trapezoid Trapezoid
 */
trapezoid Dag::findTrapezoid(cg3::Point2d point, cg3::Point2d auxiliaryPoint, bool queryPoint, nodeDag* root){
    nodeDag** tmp = &root;

    while((*tmp)->getType() != "PK4Leaf"){
        if((*tmp)->getType() == "PK1X"){
            tmp = ((X*)*tmp)->pointToPoint(point, auxiliaryPoint);
        }
        else if((*tmp)->getType() == "PK1Y"){
            tmp = ((Y*)*tmp)->pointToSegment(point, auxiliaryPoint);
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
            if(((*tmp)->determinant(((Y*)(*tmp))->getSegment(), p1) > 0 && (*tmp)->determinant(((Y*)(*tmp))->getSegment(), p2) > 0)){
                findTrapezoids(segment, (*tmp)->getLeftChild(), temp);
            }
            else if(((*tmp)->determinant(((Y*)(*tmp))->getSegment(), p1) < 0 && (*tmp)->determinant(((Y*)(*tmp))->getSegment(), p2) < 0)){
                findTrapezoids(segment, (*tmp)->getRightChild(), temp);
            }
            else if(((*tmp)->determinant(segment, ((Y*)(*tmp))->getSegment().p1()) < 0 || (*tmp)->determinant(segment, ((Y*)(*tmp))->getSegment().p2()) < 0)){
                findTrapezoids(segment, (*tmp)->getLeftChild(), temp);
            }
            else{
                findTrapezoids(segment, (*tmp)->getRightChild(), temp);
            }
        }
    }
    else{      
        if((temp)->getType() == "PK1Y"){
            if(((temp)->determinant(((Y*)(temp))->getSegment(), p1) > 0 && (temp)->determinant(((Y*)(temp))->getSegment(), p2) > 0)){
                pointersMap.insert(std::make_pair(((Leaf*)*tmp)->getTrapezoid(), temp->getLeftChildP()));
            }
            else if(((temp)->determinant(((Y*)(temp))->getSegment(), p1) < 0 && (temp)->determinant(((Y*)(temp))->getSegment(), p2) < 0)){
                pointersMap.insert(std::make_pair(((Leaf*)*tmp)->getTrapezoid(), temp->getRightChildP()));
            }
            else if(((temp)->determinant(segment, ((Y*)(temp))->getSegment().p1()) < 0 || (temp)->determinant(segment, ((Y*)(temp))->getSegment().p2()) < 0)){
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

    if(traps.size()==2){
        segment1->setLeftChild(new Leaf(*(traps.begin()+0)));
        segment1->setRightChild(new Leaf(*(traps.begin()+1)));
        if(nTrapezoids <= 2){
            dag = segment1;
        }
        else{
            trapezoid trap = pointersMap.begin()->first;
            ((*(pointersMap.begin()->second)))=segment1;

            findMultipleTrapezoids(trap, this->getDag(), nullptr, 0);
            for(std::map<long int, nodeDag**>::iterator it = multipleAdresses.begin(); it!=multipleAdresses.end(); it++){
                *(it->second)=segment1;
            }

        }
    }
    else if(traps.size()==3){
        if(p1 == traps[0].getLeftP()){
            point2->setLeftChild(segment1);
            point2->setRightChild(new Leaf(*(traps.begin()+2)));
            segment1->setLeftChild(new Leaf(*(traps.begin()+0)));
            segment1->setRightChild(new Leaf(*(traps.begin()+1)));
            if(nTrapezoids <= 3){
                dag = point2;
            }
            else{
                trapezoid trap = pointersMap.begin()->first;
                ((*(pointersMap.begin()->second)))=point2;

                findMultipleTrapezoids(trap, this->getDag(), nullptr, 0);
                for(std::map<long int, nodeDag**>::iterator it = multipleAdresses.begin(); it!=multipleAdresses.end(); it++){
                    *(it->second)=point2;
                }
            }
        }
        else{
            point1->setLeftChild(new Leaf(*(traps.begin()+0)));
            point1->setRightChild(segment1);
            segment1->setLeftChild(new Leaf(*(traps.begin()+1)));
            segment1->setRightChild(new Leaf(*(traps.begin()+2)));
            if(nTrapezoids <= 3){
                dag = point1;
            }
            else{
                trapezoid trap = pointersMap.begin()->first;
                ((*(pointersMap.begin()->second)))=point1;

                findMultipleTrapezoids(trap, this->getDag(), nullptr, 0);
                for(std::map<long int, nodeDag**>::iterator it = multipleAdresses.begin(); it!=multipleAdresses.end(); it++){
                    *(it->second)=point1;
                }
            }
        }


    }
    else{
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

            findMultipleTrapezoids(trap, this->getDag(), nullptr, 0);
            for(std::map<long int, nodeDag**>::iterator it = multipleAdresses.begin(); it!=multipleAdresses.end(); it++){
                *(it->second)=point1;
            }

        }
    }
    pointersMap.erase(pointersMap.begin());
    multipleAdresses.clear();
}


/**
 * @brief Method to update the dag with the new subtrees and the new trapezoids in the case of multiple trapezoids
 * @param[in] segment Segment, splitNode nodeDag*
 */
void Dag::insertMultipleTrapezoids(const cg3::Segment2d segment){
    cg3::Point2d p1 = segment.p1();
    cg3::Point2d p2 = segment.p2();
    cg3::Point2d topLeft, bottomLeft, topRight, bottomRight;

    nodeDag* point1 = new X(p1, false);
    nodeDag* point2 = new X(p2, true);
    nodeDag* segment1a = new Y(segment);
    nodeDag* segmentInner;
    nodeDag* segmentInner1;
    int contaStep=0, i=0;

    for(std::map<trapezoid, nodeDag**>::iterator it = pointersMap.begin(); it!=pointersMap.end(); it++){
        trapezoid trap=it->first;

        topLeft = trap.getTrapezoid()[0];
        bottomLeft = trap.getTrapezoid()[3];
        topRight = trap.getTrapezoid()[1];
        bottomRight = trap.getTrapezoid()[2];

        if(i==0){
            if(p1 == trap.getLeftP()){
                segment1a->setLeftChild(new Leaf(*(traps.begin()+contaStep)));
                contaStep++;
                segment1a->setRightChild(new Leaf(*(traps.begin()+contaStep)));
                contaStep++;
                (*it->second)=segment1a;

                findMultipleTrapezoids(trap, this->getDag(), nullptr, 0);
                for(std::map<long int, nodeDag**>::iterator it2 = multipleAdresses.begin(); it2!=multipleAdresses.end(); it2++){
                    *(it2->second)=segment1a;
                }
            }
            else{
                point1->setRightChild(segment1a);
                point1->setLeftChild(new Leaf(*(traps.begin()+contaStep)));
                contaStep++;
                segment1a->setLeftChild(new Leaf(*(traps.begin()+contaStep)));
                contaStep++;
                segment1a->setRightChild(new Leaf(*(traps.begin()+contaStep)));
                contaStep++;
                (*it->second)=point1;

                findMultipleTrapezoids(trap, this->getDag(), nullptr, 0);
                for(std::map<long int, nodeDag**>::iterator it2 = multipleAdresses.begin(); it2!=multipleAdresses.end(); it2++){
                    *(it2->second)=point1;
                }
            }
            segmentInner = new Y(segment);
            segmentInner->setLeftChild(segment1a->getLeftChild());
            segmentInner->setRightChild(segment1a->getRightChild());
        }
        else if(trap.getLeftP() == topLeft && topLeft != bottomLeft){ //C2L
            if(topRight == bottomRight){ //C1R
                nodeDag* segment1b = new Y(segment);
                point2->setLeftChild(segment1b);
                point2->setRightChild(new Leaf(*(traps.begin()+((traps).size()-1))));
                segment1b->setLeftChild(new Leaf(*(traps.begin()+contaStep)));
                segment1b->setRightChild(segmentInner->getRightChild());

                multipleReferences((*(it->second)), trap, segment1b, point2, p2);
            }
            else if(trap.getRightP() == topRight && topRight != cg3::Point2d((1e+6), (1e+6))){ //C2R
                if(i==(int)pointersMap.size()-1){ //last trap
                    nodeDag* segment1b = new Y(segment);
                    point2->setLeftChild(segment1b);
                    point2->setRightChild(new Leaf(*(traps.begin()+((traps).size()-1))));
                    segment1b->setLeftChild(new Leaf(*(traps.begin()+contaStep)));
                    segment1b->setRightChild(segmentInner->getRightChild());

                    multipleReferences((*(it->second)), trap, segment1b, point2, p2);
                }
                else{
                    segmentInner1 = new Y(segment);
                    segmentInner1->setLeftChild(segmentInner->getLeftChild());
                    segmentInner1->setRightChild(segmentInner->getRightChild());

                    segmentInner = new Y(segment);
                    segmentInner->setLeftChild(new Leaf(*(traps.begin()+contaStep)));
                    contaStep++;
                    segmentInner->setRightChild(segmentInner1->getRightChild());

                    (*it->second)=segmentInner;
                    findMultipleTrapezoids(trap, this->getDag(), nullptr, 0);
                    for(std::map<long int, nodeDag**>::iterator it2 = multipleAdresses.begin(); it2!=multipleAdresses.end(); it2++){
                         *(it2->second)=segmentInner;
                    }
                }
            }
            else if(trap.getRightP() == bottomRight){ //C3R
                if(i==(int)pointersMap.size()-1){ //last trap
                    nodeDag* segment1b = new Y(segment);
                    point2->setLeftChild(segment1b);
                    point2->setRightChild(new Leaf(*(traps.begin()+((traps).size()-1))));
                    segment1b->setLeftChild(new Leaf(*(traps.begin()+contaStep)));
                    segment1b->setRightChild(segmentInner->getRightChild());

                    multipleReferences((*(it->second)), trap, segment1b, point2, p2);
                }
                else{
                    segmentInner1 = new Y(segment);
                    segmentInner1->setLeftChild(segmentInner->getLeftChild());
                    segmentInner1->setRightChild(segmentInner->getRightChild());

                    segmentInner = new Y(segment);
                    segmentInner->setLeftChild(new Leaf(*(traps.begin()+contaStep)));
                    contaStep++;
                    segmentInner->setRightChild(segmentInner1->getRightChild());

                    (*it->second)=segmentInner;
                    findMultipleTrapezoids(trap, this->getDag(), nullptr, 0);
                    for(std::map<long int, nodeDag**>::iterator it2 = multipleAdresses.begin(); it2!=multipleAdresses.end(); it2++){
                         *(it2->second)=segmentInner;
                    }
                }
            }
            else if(trap.getRightP().x() == topRight.x() && trap.getRightP() != topRight && trap.getRightP() != bottomRight){ //C4R
                if(i==(int)pointersMap.size()-1){ //last trap
                    nodeDag* segment1b = new Y(segment);
                    point2->setLeftChild(segment1b);
                    point2->setRightChild(new Leaf(*(traps.begin()+((traps).size()-1))));
                    segment1b->setLeftChild(new Leaf(*(traps.begin()+contaStep)));
                    segment1b->setRightChild(segmentInner->getRightChild());

                    multipleReferences((*(it->second)), trap, segment1b, point2, p2);
                }
                else{
                    if(getDag()->determinant(segment, trap.getRightP()) < 0 ){ //sopra
                        segmentInner1 = new Y(segment);
                        segmentInner1->setLeftChild(segmentInner->getLeftChild());
                        segmentInner1->setRightChild(segmentInner->getRightChild());

                        segmentInner = new Y(segment);
                        segmentInner->setLeftChild(new Leaf(*(traps.begin()+contaStep)));
                        contaStep++;
                        segmentInner->setRightChild(segmentInner1->getRightChild());

                        (*it->second)=segmentInner;
                        findMultipleTrapezoids(trap, this->getDag(), nullptr, 0);
                        for(std::map<long int, nodeDag**>::iterator it2 = multipleAdresses.begin(); it2!=multipleAdresses.end(); it2++){
                             *(it2->second)=segmentInner;
                        }
                    }
                    else{    
                        segmentInner1 = new Y(segment);
                        segmentInner1->setLeftChild(segmentInner->getLeftChild());
                        segmentInner1->setRightChild(segmentInner->getRightChild());

                        segmentInner = new Y(segment);
                        segmentInner->setLeftChild(new Leaf(*(traps.begin()+contaStep)));
                        contaStep++;
                        segmentInner->setRightChild(segmentInner1->getRightChild());

                        (*it->second)=segmentInner;
                        findMultipleTrapezoids(trap, this->getDag(), nullptr, 0);
                        for(std::map<long int, nodeDag**>::iterator it2 = multipleAdresses.begin(); it2!=multipleAdresses.end(); it2++){
                             *(it2->second)=segmentInner;
                        }
                    }
                }
            }
            else if(topRight == cg3::Point2d((1e+6), (1e+6))){ //CR(Right)
                nodeDag* segment1b = new Y(segment);
                point2->setLeftChild(segment1b);
                point2->setRightChild(new Leaf(*(traps.begin()+((traps).size()-1))));
                segment1b->setLeftChild(new Leaf(*(traps.begin()+contaStep)));
                segment1b->setRightChild(segmentInner->getRightChild());

                multipleReferences((*(it->second)), trap, segment1b, point2, p2);
            }
        }
        else if(trap.getLeftP() == bottomLeft){ //C3L
            if(topRight == bottomRight){ //C1R
                nodeDag* segment1b = new Y(segment);
                point2->setLeftChild(segment1b);
                point2->setRightChild(new Leaf(*(traps.begin()+((traps).size()-1))));
                segment1b->setLeftChild(segmentInner->getLeftChild());
                segment1b->setRightChild(new Leaf(*(traps.begin()+contaStep)));

                multipleReferences((*(it->second)), trap, segment1b, point2, p2);
            }
            else if(trap.getRightP() == topRight && topRight != cg3::Point2d((1e+6), (1e+6))){ //C2R
                if(i==(int)pointersMap.size()-1){ //last trap
                    nodeDag* segment1b = new Y(segment);
                    point2->setLeftChild(segment1b);
                    point2->setRightChild(new Leaf(*(traps.begin()+((traps).size()-1))));
                    segment1b->setLeftChild(segmentInner->getLeftChild());
                    segment1b->setRightChild(new Leaf(*(traps.begin()+contaStep)));

                    multipleReferences((*(it->second)), trap, segment1b, point2, p2);
                }
                else{
                    segmentInner1 = new Y(segment);
                    segmentInner1->setLeftChild(segmentInner->getLeftChild());
                    segmentInner1->setRightChild(segmentInner->getRightChild());

                    segmentInner = new Y(segment);
                    segmentInner->setLeftChild(segmentInner1->getLeftChild());
                    segmentInner->setRightChild(new Leaf(*(traps.begin()+contaStep)));
                    contaStep++;

                    (*it->second)=segmentInner;
                    findMultipleTrapezoids(trap, this->getDag(), nullptr, 0);
                    for(std::map<long int, nodeDag**>::iterator it2 = multipleAdresses.begin(); it2!=multipleAdresses.end(); it2++){
                         *(it2->second)=segmentInner;
                    }
                }
            }
            else if(trap.getRightP() == bottomRight){ //C3R
                if(i==(int)pointersMap.size()-1){ //last trap
                    nodeDag* segment1b = new Y(segment);
                    point2->setLeftChild(segment1b);
                    point2->setRightChild(new Leaf(*(traps.begin()+((traps).size()-1))));
                    segment1b->setLeftChild(segmentInner->getLeftChild());
                    segment1b->setRightChild(new Leaf(*(traps.begin()+contaStep)));

                    multipleReferences((*(it->second)), trap, segment1b, point2, p2);
                }
                else{
                    segmentInner1 = new Y(segment);
                    segmentInner1->setLeftChild(segmentInner->getLeftChild());
                    segmentInner1->setRightChild(segmentInner->getRightChild());

                    segmentInner = new Y(segment);
                    segmentInner->setLeftChild(segmentInner1->getLeftChild());
                    segmentInner->setRightChild(new Leaf(*(traps.begin()+contaStep)));
                    contaStep++;

                    (*it->second)=segmentInner;
                    findMultipleTrapezoids(trap, this->getDag(), nullptr, 0);
                    for(std::map<long int, nodeDag**>::iterator it2 = multipleAdresses.begin(); it2!=multipleAdresses.end(); it2++){
                         *(it2->second)=segmentInner;
                    }
                }
            }
            else if(trap.getRightP().x() == topRight.x() && trap.getRightP() != topRight && trap.getRightP() != bottomRight){ //C4R
                if(i==(int)pointersMap.size()-1){ //last trap
                    nodeDag* segment1b = new Y(segment);
                    point2->setLeftChild(segment1b);
                    point2->setRightChild(new Leaf(*(traps.begin()+((traps).size()-1))));
                    segment1b->setLeftChild(segmentInner->getLeftChild());
                    segment1b->setRightChild(new Leaf(*(traps.begin()+contaStep)));

                    multipleReferences((*(it->second)), trap, segment1b, point2, p2);
                }
                else{
                    if(getDag()->determinant(segment, trap.getRightP()) < 0 ){ //sopra
                        segmentInner1 = new Y(segment);
                        segmentInner1->setLeftChild(segmentInner->getLeftChild());
                        segmentInner1->setRightChild(segmentInner->getRightChild());

                        segmentInner = new Y(segment);
                        segmentInner->setLeftChild(segmentInner1->getLeftChild());
                        segmentInner->setRightChild(new Leaf(*(traps.begin()+contaStep)));
                        contaStep++;

                        (*it->second)=segmentInner;
                        findMultipleTrapezoids(trap, this->getDag(), nullptr, 0);
                        for(std::map<long int, nodeDag**>::iterator it2 = multipleAdresses.begin(); it2!=multipleAdresses.end(); it2++){
                             *(it2->second)=segmentInner;
                        }
                    }
                    else{  
                        segmentInner1 = new Y(segment);
                        segmentInner1->setLeftChild(segmentInner->getLeftChild());
                        segmentInner1->setRightChild(segmentInner->getRightChild());

                        segmentInner = new Y(segment);
                        segmentInner->setLeftChild(segmentInner1->getLeftChild());
                        segmentInner->setRightChild(new Leaf(*(traps.begin()+contaStep)));
                        contaStep++;

                        (*it->second)=segmentInner;
                        findMultipleTrapezoids(trap, this->getDag(), nullptr, 0);
                        for(std::map<long int, nodeDag**>::iterator it2 = multipleAdresses.begin(); it2!=multipleAdresses.end(); it2++){
                             *(it2->second)=segmentInner;
                        }
                    }
                }
            }
            else if(topRight == cg3::Point2d((1e+6), (1e+6))){ //CR(Right)
                nodeDag* segment1b = new Y(segment);
                point2->setLeftChild(segment1b);
                point2->setRightChild(new Leaf(*(traps.begin()+((traps).size()-1))));
                segment1b->setLeftChild(segmentInner->getLeftChild());
                segment1b->setRightChild(new Leaf(*(traps.begin()+contaStep)));

                multipleReferences((*(it->second)), trap, segment1b, point2, p2);
            }
        }
        else if(trap.getLeftP().x() == topLeft.x() && trap.getLeftP() != topLeft && trap.getLeftP() != bottomLeft){ //C4L
            if(topRight == bottomRight){ //C1R
                if(getDag()->determinant(segment, trap.getLeftP()) < 0 ){ //sopra
                    nodeDag* segment1b = new Y(segment);
                    point2->setLeftChild(segment1b);
                    point2->setRightChild(new Leaf(*(traps.begin()+((traps).size()-1))));
                    segment1b->setLeftChild(segmentInner->getLeftChild());
                    segment1b->setRightChild(new Leaf(*(traps.begin()+contaStep)));

                    multipleReferences((*(it->second)), trap, segment1b, point2, p2);
                }
                else{
                    nodeDag* segment1b = new Y(segment);
                    point2->setLeftChild(segment1b);
                    point2->setRightChild(new Leaf(*(traps.begin()+((traps).size()-1))));
                    segment1b->setLeftChild(new Leaf(*(traps.begin()+contaStep)));
                    segment1b->setRightChild(segmentInner->getRightChild());

                    multipleReferences((*(it->second)), trap, segment1b, point2, p2);
                }

            }
            else if(trap.getRightP() == topRight && topRight != cg3::Point2d((1e+6), (1e+6))){ //C2R
                if(getDag()->determinant(segment, trap.getLeftP()) < 0 ){ //sopra
                    if(i==(int)pointersMap.size()-1){ //last trap
                        nodeDag* segment1b = new Y(segment);
                        point2->setLeftChild(segment1b);
                        point2->setRightChild(new Leaf(*(traps.begin()+((traps).size()-1))));
                        segment1b->setLeftChild(segmentInner->getLeftChild());
                        segment1b->setRightChild(new Leaf(*(traps.begin()+contaStep)));

                        multipleReferences((*(it->second)), trap, segment1b, point2, p2);
                    }
                    else{
                        segmentInner1 = new Y(segment);
                        segmentInner1->setLeftChild(segmentInner->getLeftChild());
                        segmentInner1->setRightChild(segmentInner->getRightChild());

                        segmentInner = new Y(segment);
                        segmentInner->setLeftChild(segmentInner1->getLeftChild());
                        segmentInner->setRightChild(new Leaf(*(traps.begin()+contaStep)));
                        contaStep++;

                        (*it->second)=segmentInner;
                        findMultipleTrapezoids(trap, this->getDag(), nullptr, 0);
                        for(std::map<long int, nodeDag**>::iterator it2 = multipleAdresses.begin(); it2!=multipleAdresses.end(); it2++){
                             *(it2->second)=segmentInner;
                        }
                    }
                }
                else{
                    if(i==(int)pointersMap.size()-1){ //last trap
                        nodeDag* segment1b = new Y(segment);
                        point2->setLeftChild(segment1b);
                        point2->setRightChild(new Leaf(*(traps.begin()+((traps).size()-1))));
                        segment1b->setLeftChild(new Leaf(*(traps.begin()+contaStep)));
                        segment1b->setRightChild(segmentInner->getRightChild());

                        multipleReferences((*(it->second)), trap, segment1b, point2, p2);
                    }
                    else{
                        segmentInner1 = new Y(segment);
                        segmentInner1->setLeftChild(segmentInner->getLeftChild());
                        segmentInner1->setRightChild(segmentInner->getRightChild());

                        segmentInner = new Y(segment);
                        segmentInner->setLeftChild(new Leaf(*(traps.begin()+contaStep)));
                        contaStep++;
                        segmentInner->setRightChild(segmentInner1->getRightChild());

                        (*it->second)=segmentInner;
                        findMultipleTrapezoids(trap, this->getDag(), nullptr, 0);
                        for(std::map<long int, nodeDag**>::iterator it2 = multipleAdresses.begin(); it2!=multipleAdresses.end(); it2++){
                             *(it2->second)=segmentInner;
                        }
                    }
                }
            }
            else if(trap.getRightP() == bottomRight){ //C3R
                if(getDag()->determinant(segment, trap.getLeftP()) < 0 ){ //sopra
                    if(i==(int)pointersMap.size()-1){ //last trap
                        nodeDag* segment1b = new Y(segment);
                        point2->setLeftChild(segment1b);
                        point2->setRightChild(new Leaf(*(traps.begin()+((traps).size()-1))));
                        segment1b->setLeftChild(segmentInner->getLeftChild());
                        segment1b->setRightChild(new Leaf(*(traps.begin()+contaStep)));

                        multipleReferences((*(it->second)), trap, segment1b, point2, p2);
                    }
                    else{
                        segmentInner1 = new Y(segment);
                        segmentInner1->setLeftChild(segmentInner->getLeftChild());
                        segmentInner1->setRightChild(segmentInner->getRightChild());

                        segmentInner = new Y(segment);
                        segmentInner->setLeftChild(segmentInner1->getLeftChild());
                        segmentInner->setRightChild(new Leaf(*(traps.begin()+contaStep)));
                        contaStep++;

                        (*it->second)=segmentInner;
                        findMultipleTrapezoids(trap, this->getDag(), nullptr, 0);
                        for(std::map<long int, nodeDag**>::iterator it2 = multipleAdresses.begin(); it2!=multipleAdresses.end(); it2++){
                             *(it2->second)=segmentInner;
                        }
                    }
                }
                else{
                    if(i==(int)pointersMap.size()-1){ //last trap
                        nodeDag* segment1b = new Y(segment);
                        point2->setLeftChild(segment1b);
                        point2->setRightChild(new Leaf(*(traps.begin()+((traps).size()-1))));
                        segment1b->setLeftChild(new Leaf(*(traps.begin()+contaStep)));
                        segment1b->setRightChild(segmentInner->getRightChild());

                        multipleReferences((*(it->second)), trap, segment1b, point2, p2);
                    }
                    else{
                        segmentInner1 = new Y(segment);
                        segmentInner1->setLeftChild(segmentInner->getLeftChild());
                        segmentInner1->setRightChild(segmentInner->getRightChild());

                        segmentInner = new Y(segment);
                        segmentInner->setLeftChild(new Leaf(*(traps.begin()+contaStep)));
                        contaStep++;
                        segmentInner->setRightChild(segmentInner1->getRightChild());

                        (*it->second)=segmentInner;
                        findMultipleTrapezoids(trap, this->getDag(), nullptr, 0);
                        for(std::map<long int, nodeDag**>::iterator it2 = multipleAdresses.begin(); it2!=multipleAdresses.end(); it2++){
                             *(it2->second)=segmentInner;
                        }
                    }
                }
            }
            else if(trap.getRightP().x() == topRight.x() && trap.getRightP() != topRight && trap.getRightP() != bottomRight){ //C4R
                if(getDag()->determinant(segment, trap.getLeftP()) < 0 ){ //sopra
                    if(i==(int)pointersMap.size()-1){ //last trap
                        nodeDag* segment1b = new Y(segment);
                        point2->setLeftChild(segment1b);
                        point2->setRightChild(new Leaf(*(traps.begin()+((traps).size()-1))));
                        segment1b->setLeftChild(segmentInner->getLeftChild());
                        segment1b->setRightChild(new Leaf(*(traps.begin()+contaStep)));

                        multipleReferences((*(it->second)), trap, segment1b, point2, p2);
                    }
                    else{
                        if(getDag()->determinant(segment, trap.getRightP()) < 0 ){ //sopra
                            segmentInner1 = new Y(segment);
                            segmentInner1->setLeftChild(segmentInner->getLeftChild());
                            segmentInner1->setRightChild(segmentInner->getRightChild());

                            segmentInner = new Y(segment);
                            segmentInner->setLeftChild(segmentInner1->getLeftChild());
                            segmentInner->setRightChild(new Leaf(*(traps.begin()+contaStep)));
                            contaStep++;

                            (*it->second)=segmentInner;
                            findMultipleTrapezoids(trap, this->getDag(), nullptr, 0);
                            for(std::map<long int, nodeDag**>::iterator it2 = multipleAdresses.begin(); it2!=multipleAdresses.end(); it2++){
                                 *(it2->second)=segmentInner;
                            }
                        }
                        else{
                            segmentInner1 = new Y(segment);
                            segmentInner1->setLeftChild(segmentInner->getLeftChild());
                            segmentInner1->setRightChild(segmentInner->getRightChild());

                            segmentInner = new Y(segment);
                            segmentInner->setLeftChild(segmentInner1->getLeftChild());
                            segmentInner->setRightChild(new Leaf(*(traps.begin()+contaStep)));
                            contaStep++;

                            (*it->second)=segmentInner;
                            findMultipleTrapezoids(trap, this->getDag(), nullptr, 0);
                            for(std::map<long int, nodeDag**>::iterator it2 = multipleAdresses.begin(); it2!=multipleAdresses.end(); it2++){
                                 *(it2->second)=segmentInner;
                            }
                        }
                    }
                }
                else{
                    if(i==(int)pointersMap.size()-1){ //last trap
                        nodeDag* segment1b = new Y(segment);
                        point2->setLeftChild(segment1b);
                        point2->setRightChild(new Leaf(*(traps.begin()+((traps).size()-1))));
                        segment1b->setLeftChild(new Leaf(*(traps.begin()+contaStep)));
                        segment1b->setRightChild(segmentInner->getRightChild());

                        multipleReferences((*(it->second)), trap, segment1b, point2, p2);
                    }
                    else{
                        if(getDag()->determinant(segment, trap.getRightP()) < 0 ){ //sopra
                            segmentInner1 = new Y(segment);
                            segmentInner1->setLeftChild(segmentInner->getLeftChild());
                            segmentInner1->setRightChild(segmentInner->getRightChild());

                            segmentInner = new Y(segment);
                            segmentInner->setLeftChild(new Leaf(*(traps.begin()+contaStep)));
                            contaStep++;
                            segmentInner->setRightChild(segmentInner1->getRightChild());

                            (*it->second)=segmentInner;
                            findMultipleTrapezoids(trap, this->getDag(), nullptr, 0);
                            for(std::map<long int, nodeDag**>::iterator it2 = multipleAdresses.begin(); it2!=multipleAdresses.end(); it2++){
                                 *(it2->second)=segmentInner;
                            }
                        }
                        else{
                            segmentInner1 = new Y(segment);
                            segmentInner1->setLeftChild(segmentInner->getLeftChild());
                            segmentInner1->setRightChild(segmentInner->getRightChild());

                            segmentInner = new Y(segment);
                            segmentInner->setLeftChild(new Leaf(*(traps.begin()+contaStep)));
                            contaStep++;
                            segmentInner->setRightChild(segmentInner1->getRightChild());

                            (*it->second)=segmentInner;
                            findMultipleTrapezoids(trap, this->getDag(), nullptr, 0);
                            for(std::map<long int, nodeDag**>::iterator it2 = multipleAdresses.begin(); it2!=multipleAdresses.end(); it2++){
                                 *(it2->second)=segmentInner;
                            }
                        }
                    }
                }
            }
            else if(topRight == cg3::Point2d((1e+6), (1e+6))){ //CR(Right)
                if(getDag()->determinant(segment, trap.getLeftP()) < 0 ){ //sopra
                    nodeDag* segment1b = new Y(segment);
                    point2->setLeftChild(segment1b);
                    point2->setRightChild(new Leaf(*(traps.begin()+((traps).size()-1))));
                    segment1b->setLeftChild(segmentInner->getLeftChild());
                    segment1b->setRightChild(new Leaf(*(traps.begin()+contaStep)));

                    multipleReferences((*(it->second)), trap, segment1b, point2, p2);
                }
                else{
                    nodeDag* segment1b = new Y(segment);
                    point2->setLeftChild(segment1b);
                    point2->setRightChild(new Leaf(*(traps.begin()+((traps).size()-1))));
                    segment1b->setLeftChild(new Leaf(*(traps.begin()+contaStep)));
                    segment1b->setRightChild(segmentInner->getRightChild());

                    multipleReferences((*(it->second)), trap, segment1b, point2, p2);
                }
            }
        }
        i++;
        multipleAdresses.clear();
    }
}

/**
 * @brief Method to find all the addresses of a given trapezoid
 * @param[in] trapezoid trap, nodeDag *node, nodeDag *temp, long int indexTrap (to avoid duplicates in the map)
 */
void Dag::findMultipleTrapezoids(trapezoid trap, nodeDag* node, nodeDag* temp, long int indexTrap){

    nodeDag** tmp= &node;;

    if((*tmp)->getType() != "PK4Leaf"){
        temp=*tmp;
        indexTrap++;
        findMultipleTrapezoids(trap, (*tmp)->getLeftChild(), temp, indexTrap+rand()%50);
        indexTrap++;
        findMultipleTrapezoids(trap, (*tmp)->getRightChild(), temp, indexTrap+rand()%50);
    }
    else{
        if(((Leaf*)*tmp)->getTrapezoid().getTrapezoid() == trap.getTrapezoid()){
            if((temp)->getType() == "PK1Y"){
                if((temp)->getLeftChild()->getType() == "PK4Leaf"){
                    if(((Leaf*)(temp)->getLeftChild())->getTrapezoid().getTrapezoid() == trap.getTrapezoid()){
                        multipleAdresses.insert(std::make_pair(indexTrap, temp->getLeftChildP()));
                    }
                }

                if((temp)->getRightChild()->getType() == "PK4Leaf"){
                    if(((Leaf*)(temp)->getRightChild())->getTrapezoid().getTrapezoid() == trap.getTrapezoid()){
                        multipleAdresses.insert(std::make_pair(indexTrap, temp->getRightChildP()));
                    }
                }
            }
            else if((temp)->getType() == "PK1X"){
                if((temp)->getLeftChild()->getType() == "PK4Leaf"){
                    if(((Leaf*)(temp)->getLeftChild())->getTrapezoid().getTrapezoid() == trap.getTrapezoid()){
                        multipleAdresses.insert(std::make_pair(indexTrap, temp->getLeftChildP()));
                    }
                }

                if((temp)->getRightChild()->getType() == "PK4Leaf"){
                    if(((Leaf*)(temp)->getRightChild())->getTrapezoid().getTrapezoid() == trap.getTrapezoid()){
                        multipleAdresses.insert(std::make_pair(indexTrap, temp->getRightChildP()));
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
void Dag::setTrapezoidToInsert (const trapezoid trapezoid, int num){
    nTrapezoids = num;
    traps.push_back(trapezoid);
}

/**
 * @brief Method to update the multi references in the dag
 * @param[in] nodeDag node to update, trap to find, segment1b to insert, point2 to insert and p2(second endopoint of the segment)
 */
void Dag::multipleReferences(nodeDag*& node, trapezoid& trap, nodeDag*& segment1b, nodeDag*& point2, cg3::Point2d& p2){
    if(p2 == trap.getRightP()){
        node=segment1b;
        findMultipleTrapezoids(trap, this->getDag(), nullptr, 0);
        for(std::map<long int, nodeDag**>::iterator it2 = multipleAdresses.begin(); it2!=multipleAdresses.end(); it2++){
             *(it2->second)=segment1b;
        }
    }
    else{
        node=point2;
        findMultipleTrapezoids(trap, this->getDag(), nullptr, 0);
        for(std::map<long int, nodeDag**>::iterator it2 = multipleAdresses.begin(); it2!=multipleAdresses.end(); it2++){
             *(it2->second)=point2;
        }
    }
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
 * @brief Method to get the multiple addresses map
 */
std::map<long int, nodeDag**> Dag::getMultipleAdressesMap(){
    return multipleAdresses;
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

