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

        insertSingleTrapezoid(segment);

        pointersMap.clear();
    }
    else{
        insertMultipleTrapezoids(segment, splitNode);
        pointersMap.clear();
    }
}

std::array<cg3::Point2d, 4>  Dag::findTrapezoid(cg3::Point2d point, bool queryPoint, nodeDag* root){
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
        addMapElement(std::make_pair(((Leaf*)*tmp)->getTrapezoid(), tmp));
    }

    return ((Leaf*)*tmp)->getTrapezoid();
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
    segment1->setLeftChild(new Leaf(*(traps.begin()+((traps).size()-3))));
    segment1->setRightChild(new Leaf(*(traps.begin()+((traps).size()-2))));


    if(nTrapezoids <= 4){
        dag = point1;
    }
    else{
        /*
         * Elimina multisplit e gestisci bene puntatori doppi -> devono aggiornare tutti i valori a cui puntano
         */
         ((*(pointersMap.begin()->second)))=point1;
    }
    pointersMap.erase(pointersMap.begin());
}

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
            if((*tmp)->determinant(((Y*)(*tmp))->getSegment(), p1) > 0 && (*tmp)->determinant(((Y*)(*tmp))->getSegment(), p2) > 0){
                findTrapezoids(segment, (*tmp)->getLeftChild(), temp);
            }
            else if((*tmp)->determinant(((Y*)(*tmp))->getSegment(), p1) < 0 && (*tmp)->determinant(((Y*)(*tmp))->getSegment(), p2) < 0){
                findTrapezoids(segment, (*tmp)->getRightChild(), temp);
            }
        }
    }
    else{
        if(temp->getLeftChild()->getType() == "PK4Leaf"){
            addMapElement(std::make_pair(((Leaf*)*tmp)->getTrapezoid(), temp->getLeftChildP()));
        }
        else{
            addMapElement(std::make_pair(((Leaf*)*tmp)->getTrapezoid(), temp->getRightChildP()));
        }


    }
}

void Dag::addMapElement(const std::pair<std::array<cg3::Point2d, 4>, nodeDag**> element){
    pointersMap.insert(element);
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

            leftPoint = *((X*)leftPoint)->pointToPoint(segment.p1());

        }
        else if(leftPoint->getType() == "PK1Y"){

            leftPoint = *((Y*)leftPoint)->pointToSegment(segment.p1());
        }

        /*
         * right endpoint
         */
        if(rightPoint->getType() == "PK1X"){

            rightPoint = *((X*)rightPoint)->pointToPoint(segment.p2());

        }
        else if(rightPoint->getType() == "PK1Y"){

            rightPoint = *((Y*)rightPoint)->pointToSegment(segment.p2());
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
    bool up, invert=false;

    nodeDag* segmentInner;
    nodeDag* segmentInner1;

    int contaStep=0;

    for(std::map<std::array<cg3::Point2d, 4>, nodeDag**>::iterator it = pointersMap.begin(); it!=pointersMap.end(); it++){
        if(it == pointersMap.begin()){
            if(splitNode->getType() == "PK1X"){
                if(dag->determinant(segment, (((X*)splitNode))->getPoint()) < 0){
                    up=true;
                }
                else{
                    up=false;
                }
            }
            point1->setRightChild(segment1a);
            point1->setLeftChild(new Leaf(*(traps.begin()+((traps).size()-(traps.size()-contaStep)))));
            contaStep++;
            if(pointersMap.size() == 2 || pointersMap.size() > 3){ //two traps
                segment1a->setLeftChild(new Leaf(*(traps.begin()+((traps).size()-(traps.size()-contaStep)))));
                contaStep++;
                segment1a->setRightChild(new Leaf(*(traps.begin()+((traps).size()-(traps.size()-contaStep)))));
                contaStep++;
                (*it->second)=point1;
            }
            else{

            }
        }
        else if(it == --(pointersMap.end())){
            nodeDag* segment1b = new Y(segment);
            point2->setLeftChild(segment1b);            
            if(pointersMap.size() == 2){
                point2->setRightChild(new Leaf(*(traps.begin()+((traps).size()-(traps.size()-3)))));
                if(up){
                    segment1b->setLeftChild(segment1a->getLeftChild());
                    segment1b->setRightChild(new Leaf(*(traps.begin()+((traps).size()-1))));
                }
                else{
                    segment1b->setLeftChild(new Leaf(*(traps.begin()+((traps).size()-1))));
                    segment1b->setRightChild(segment1a->getRightChild());
                }
                (*it->second)=point2;
            }
            else{
                point2->setRightChild(new Leaf(*(traps.begin()+((traps).size()-2))));
                segment1b->setLeftChild(segmentInner->getLeftChild());
                segment1b->setRightChild(new Leaf(*(traps.begin()+((traps).size()-1))));
                (*it->second)=point2;
            }
        }
        else{ //more than two traps
            if(pointersMap.size() > 2 && it == ++pointersMap.begin()){
                segmentInner = new Y(segment);
                if(up){
                    segmentInner->setLeftChild(segment1a->getLeftChild());
                    segmentInner->setRightChild(new Leaf(*(traps.begin()+((traps).size()-contaStep))));
                    contaStep++;
                }
                else{
                    segmentInner->setLeftChild(new Leaf(*(traps.begin()+((traps).size()-contaStep))));
                    contaStep++;
                    segmentInner->setRightChild(segment1a->getRightChild());
                }
                invert=!invert;
                (*it->second)=segmentInner;
            }
            else{
                segmentInner1 = new Y(segment);
                if(invert){
                    if(up){
                        segmentInner1->setLeftChild(new Leaf(*(traps.begin()+((traps).size()-contaStep))));
                        contaStep++;
                        segmentInner1->setRightChild(segmentInner->getRightChild());
                    }
                    else{
                        segmentInner1->setLeftChild(segmentInner->getLeftChild());
                        segmentInner1->setRightChild(new Leaf(*(traps.begin()+((traps).size()-contaStep))));
                        contaStep++;
                    }
                }
                else{
                    if(up){
                        segmentInner1->setLeftChild(segmentInner->getLeftChild());
                        segmentInner1->setRightChild(new Leaf(*(traps.begin()+((traps).size()-contaStep))));
                    }
                    else{
                        segmentInner1->setLeftChild(new Leaf(*(traps.begin()+((traps).size()-contaStep))));
                        segmentInner1->setRightChild(segmentInner->getRightChild());
                    }
                }
                segmentInner = segmentInner1;
                (*it->second)=segmentInner;
            }
        }
    }
}


void Dag::setTrapezoidToInsert (std::array<cg3::Point2d, 4> trapezoid, int num){
    nTrapezoids = num;
    traps.push_back(trapezoid);
}

nodeDag* Dag::getDag(){
    return dag;
}

std::map<std::array<cg3::Point2d, 4>, nodeDag**> Dag::getPointerMap(){
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

