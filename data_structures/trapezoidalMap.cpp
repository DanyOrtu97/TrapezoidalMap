#include <drawables/drawableTrapezoidalMap.h>
#include <algorithms/algorithms.h>
#include "trapezoidalMap.h"
#include "dag.h"


cg3::Point2d topLeft;
cg3::Point2d bottomLeft;
cg3::Point2d topRight;
cg3::Point2d bottomRight;
Dag dag;


TrapezoidalMap::TrapezoidalMap() :
    boundingBox(cg3::Point2d(0,0),cg3::Point2d(0,0))
{
}

/**
 * @brief Method to inizialize the trapezoidal map
 */
void TrapezoidalMap::inizialize(){
    topLeft = GasAlgorithms::createPoint(topLeft, -(1e+6), (1e+6));
    bottomLeft = GasAlgorithms::createPoint(bottomLeft, -(1e+6), -(1e+6));
    topRight = GasAlgorithms::createPoint(topRight, (1e+6), (1e+6));
    bottomRight = GasAlgorithms::createPoint(bottomRight, (1e+6), -(1e+6));

    //inizialize the trapezoidal map with the bounding box
    std::array<cg3::Point2d, 4> trap = {topLeft, topRight, bottomRight, bottomLeft};
    trapezoid trapez = trapezoid(topLeft, topRight, bottomRight, bottomLeft, topLeft, topRight, cg3::Segment2d(trap[0], trap[1]), cg3::Segment2d(trap[3],trap[2]));
    trapezoidsList.push_back(trapez);

    //inizialize the dag with the pointer of the first polygon (the bounding box)
    dag.inizializeDag(trapez);

}

/**
 * @brief Method to update the trapezoidal map by the new trapezoids
 * @param[in] segment Segment
 */
void TrapezoidalMap::trapezoidalMapAlgorithm(cg3::Segment2d segment){
    trapezoid firstTrapezoid = dag.findTrapezoid(segment.p1(), segment.p2(), false, dag.getDag());
    trapezoid lastTrapezoid = dag.findTrapezoid(segment.p2(), segment.p1(), false, dag.getDag());
    nodeDag* splitNode;

    if(firstTrapezoid.getTrapezoid() == lastTrapezoid.getTrapezoid()){
        dag.clearTraps();
        updateTrapezoid(segment);
    }
    else{
        splitNode= GasAlgorithms::findSplitNode(segment, dag.getDag());
        dag.findTrapezoids(segment, splitNode, nullptr);
        dag.clearTraps();
        updateTrapezoid(segment);
    }
    dag.updateDag(segment, splitNode);
}

/**
 * @brief Method to update the trapezoids in the trapezoidal map
 * @param[in] segment Segment, vector of Trapezoid: foundTrapezoids
 */
void TrapezoidalMap::updateTrapezoid(const cg3::Segment2d& segment){
    cg3::Point2d p1 = segment.p1();
    cg3::Point2d p2 = segment.p2();

    cg3::Point2d upfirst, upsecond, downfirst, downsecond;

    cg3::Point2d rightPSeg, leftPSeg;

    std::map<trapezoid, nodeDag**> TrapMapDag = dag.getPointerMap();

    //this trapezoid is usefull for those trapezoids that are between 1 or more trapezoids
    trapezoid innerTrap;
    std::vector<trapezoid> insertAfterInner;
    bool insertionAfterInner=false;

    int i=0;
    for (std::map<trapezoid, nodeDag**>::iterator it =  TrapMapDag.begin(); it!= TrapMapDag.end(); it++){
        trapezoid trap=it->first;

        topLeft = trap.getTrapezoid()[0];
        bottomLeft = trap.getTrapezoid()[3];
        topRight = trap.getTrapezoid()[1];
        bottomRight = trap.getTrapezoid()[2];

        trapezoidsList.remove(trap);

        if(i==0){
            upfirst=GasAlgorithms::createPoint(upfirst, p1.x(), GasAlgorithms::handleSlopeSegment(trap.getTop(), p1));
            downfirst=GasAlgorithms::createPoint(downfirst, p1.x(), GasAlgorithms::handleSlopeSegment(trap.getBottom(), p1));
        }
        if(i == (int)TrapMapDag.size()-1){
            upsecond=GasAlgorithms::createPoint(upsecond, p2.x(), GasAlgorithms::handleSlopeSegment(trap.getTop(), p2));
            downsecond=GasAlgorithms::createPoint(downsecond, p2.x(), GasAlgorithms::handleSlopeSegment(trap.getBottom(), p2));
        }
        leftPSeg = GasAlgorithms::createPoint(leftPSeg, trap.getLeftP().x(), GasAlgorithms::handleSlopeSegment(segment, trap.getLeftP()));
        rightPSeg = GasAlgorithms::createPoint(rightPSeg, trap.getRightP().x(), GasAlgorithms::handleSlopeSegment(segment, trap.getRightP()));


        if(TrapMapDag.size()==1){
            singleTrapezoid(p1, p2, upfirst, upsecond, downfirst, downsecond, trap);
        }
        else{
            if(i==0 && p1 != trap.getLeftP()){
                addPolygon(topLeft, upfirst, downfirst, bottomLeft, trap.getLeftP(), p1, cg3::Segment2d(topLeft, upfirst), cg3::Segment2d(bottomLeft, downfirst));
            }
            if(topLeft == cg3::Point2d(-(1e+6), (1e+6))){ //CR(Left)
                if(topRight == bottomRight){ //C1R
                    addPolygon(upfirst, upsecond, p2, p1, p1, p2, cg3::Segment2d(upfirst, upsecond), cg3::Segment2d(p1, p2));
                    addPolygon(p1, p2, downsecond, downfirst, p1, p2, cg3::Segment2d(p1, p2), cg3::Segment2d(downfirst, downsecond));
                    addPolygon(upsecond, topRight, topRight, downsecond, p2, topRight, cg3::Segment2d(upsecond, topRight), cg3::Segment2d(downsecond, topRight));
                }
                else if(trap.getRightP() == topRight && topRight != cg3::Point2d((1e+6), (1e+6))){ //C2R
                    addPolygon(upfirst, topRight, rightPSeg, p1, p1, topRight, cg3::Segment2d(upfirst, topRight), cg3::Segment2d(p1, rightPSeg));
                    innerTrap = trapezoid(p1, rightPSeg, bottomRight, downfirst, p1, bottomRight, cg3::Segment2d(p1, rightPSeg), cg3::Segment2d(downfirst, bottomRight));
                }
                else if(trap.getRightP() == bottomRight){ //C3R
                    insertAfterInner.push_back(trapezoid(p1, rightPSeg, bottomRight, downfirst, p1, bottomRight, cg3::Segment2d(p1, rightPSeg), cg3::Segment2d(downfirst, bottomRight)));
                    innerTrap = trapezoid(upfirst, topRight, rightPSeg, p1, p1, topRight, cg3::Segment2d(upfirst, topRight), cg3::Segment2d(p1, rightPSeg));
                    insertionAfterInner=true;
                }
                else if(trap.getRightP().x() == topRight.x() && trap.getRightP() != topRight && trap.getRightP() != bottomRight){ //C4R
                    if(dag.getDag()->determinant(segment, trap.getRightP()) < 0 ){ //sopra
                        insertAfterInner.push_back(trapezoid(p1, rightPSeg, bottomRight, downfirst, p1, trap.getRightP(), cg3::Segment2d(p1, rightPSeg), cg3::Segment2d(downfirst, bottomRight)));
                        innerTrap = trapezoid(upfirst, topRight, rightPSeg, p1, p1, rightPSeg, cg3::Segment2d(upfirst, topRight), cg3::Segment2d(p1, rightPSeg));
                        insertionAfterInner=true;
                    }
                    else{
                        addPolygon(upfirst, topRight, rightPSeg, p1, p1, trap.getRightP(), cg3::Segment2d(upfirst, topRight), cg3::Segment2d(p1, rightPSeg));
                        innerTrap = trapezoid(p1, rightPSeg, bottomRight, downfirst, p1, bottomRight, cg3::Segment2d(p1, rightPSeg), cg3::Segment2d(downfirst, bottomRight));
                    }
                }
            }
            else if(topLeft == bottomLeft){ //C1L
                if(trap.getRightP() == topRight && topRight != cg3::Point2d((1e+6), (1e+6))){ //C2R
                    addPolygon(upfirst, topRight, rightPSeg, p1, p1, topRight, cg3::Segment2d(upfirst, topRight), cg3::Segment2d(p1, rightPSeg));
                    innerTrap = trapezoid(p1, rightPSeg, bottomRight, downfirst, p1, bottomRight, cg3::Segment2d(p1, rightPSeg), cg3::Segment2d(downfirst, bottomRight));
                }
                else if(trap.getRightP() == bottomRight){ //C3R
                    insertAfterInner.push_back(trapezoid(p1, rightPSeg, bottomRight, downfirst, p1, bottomRight, cg3::Segment2d(p1, rightPSeg), cg3::Segment2d(downfirst, bottomRight)));
                    innerTrap = trapezoid(upfirst, topRight, rightPSeg, p1, p1, topRight, cg3::Segment2d(upfirst, topRight), cg3::Segment2d(p1, rightPSeg));
                    insertionAfterInner=true;
                }
                else if(trap.getRightP().x() == topRight.x() && trap.getRightP() != topRight && trap.getRightP() != bottomRight){ //C4R
                    if(dag.getDag()->determinant(segment, trap.getRightP()) < 0 ){ //sopra
                        insertAfterInner.push_back(trapezoid(p1, rightPSeg, bottomRight, downfirst, p1, trap.getRightP(), cg3::Segment2d(p1, rightPSeg), cg3::Segment2d(downfirst, bottomRight)));
                        innerTrap = trapezoid(upfirst, topRight, rightPSeg, p1, p1, rightPSeg, cg3::Segment2d(upfirst, topRight), cg3::Segment2d(p1, rightPSeg));
                        insertionAfterInner=true;
                    }
                    else{
                        addPolygon(upfirst, topRight, rightPSeg, p1, p1, trap.getRightP(), cg3::Segment2d(upfirst, topRight), cg3::Segment2d(p1, rightPSeg));
                        innerTrap = trapezoid(p1, rightPSeg, bottomRight, downfirst, p1, bottomRight, cg3::Segment2d(p1, rightPSeg), cg3::Segment2d(downfirst, bottomRight));
                    }
                }
            }
            else if(trap.getLeftP() == topLeft && topLeft != bottomLeft){ //C2L
                if(topRight == bottomRight){ //C1R
                    //last trap
                    if(cg3::Segment2d(leftPSeg, bottomLeft) == cg3::Segment2d(innerTrap.getTrapezoid()[1], innerTrap.getTrapezoid()[2])){ //se i due lati coincidenti sono uguali
                        addPolygon(innerTrap.getTrapezoid()[0], p2, downsecond, innerTrap.getTrapezoid()[3], innerTrap.getLeftP(), p2, cg3::Segment2d(innerTrap.getTrapezoid()[0], p2), cg3::Segment2d(innerTrap.getTrapezoid()[3], downsecond));
                    }
                    if(insertionAfterInner){
                        for(int j=0; j<(int)insertAfterInner.size();j++){
                            addPolygon(insertAfterInner[j].getTrapezoid()[0], insertAfterInner[j].getTrapezoid()[1], insertAfterInner[j].getTrapezoid()[2],insertAfterInner[j].getTrapezoid()[3], insertAfterInner[j].getLeftP(), insertAfterInner[j].getRightP(), insertAfterInner[j].getTop(),insertAfterInner[j].getBottom());
                        }
                        insertionAfterInner=false;
                        insertAfterInner.clear();
                    }
                    addPolygon(trap.getLeftP(), upsecond, p2, leftPSeg, trap.getLeftP(), p2, cg3::Segment2d(trap.getLeftP(), upsecond), cg3::Segment2d(leftPSeg, p2));
                    if(p2 != trap.getRightP()){
                        addPolygon(upsecond, topRight, bottomRight, downsecond, p2, trap.getRightP(), cg3::Segment2d(upsecond, topRight), cg3::Segment2d(downsecond, bottomRight));
                    }
                }
                else if(trap.getRightP() == topRight && topRight != cg3::Point2d((1e+6), (1e+6))){ //C2R
                    if(i==0){
                        addPolygon(upfirst, topRight, rightPSeg, p1, p1, topRight, cg3::Segment2d(upfirst, topRight), cg3::Segment2d(p1, rightPSeg));
                        innerTrap = trapezoid(p1, rightPSeg, bottomRight, downfirst, p1, rightPSeg, cg3::Segment2d(p1, rightPSeg), cg3::Segment2d(downfirst, bottomRight));
                    }
                    else if(i==(int)TrapMapDag.size()-1){ //last trap
                        if(cg3::Segment2d(leftPSeg, bottomLeft) == cg3::Segment2d(innerTrap.getTrapezoid()[1], innerTrap.getTrapezoid()[2])){ //se i due lati coincidenti sono uguali
                            addPolygon(innerTrap.getTrapezoid()[0], p2, downsecond, innerTrap.getTrapezoid()[3], innerTrap.getLeftP(), p2, cg3::Segment2d(innerTrap.getTrapezoid()[0], p2), cg3::Segment2d(innerTrap.getTrapezoid()[3], downsecond));
                        }
                        if(insertionAfterInner){
                            for(int j=0; j<(int)insertAfterInner.size();j++){
                                addPolygon(insertAfterInner[j].getTrapezoid()[0], insertAfterInner[j].getTrapezoid()[1], insertAfterInner[j].getTrapezoid()[2],insertAfterInner[j].getTrapezoid()[3], insertAfterInner[j].getLeftP(), insertAfterInner[j].getRightP(), insertAfterInner[j].getTop(),insertAfterInner[j].getBottom());
                            }
                            insertionAfterInner=false;
                            insertAfterInner.clear();
                        }
                        addPolygon(trap.getLeftP(), upsecond, p2, leftPSeg, trap.getLeftP(), p2, cg3::Segment2d(trap.getLeftP(), upsecond), cg3::Segment2d(leftPSeg, p2));
                        if(p2 != trap.getRightP()){
                            addPolygon(upsecond, topRight, bottomRight, downsecond, p2, trap.getRightP(), cg3::Segment2d(upsecond, topRight), cg3::Segment2d(downsecond, bottomRight));
                        }
                    }
                    else{
                        if(cg3::Segment2d(leftPSeg, bottomLeft) == cg3::Segment2d(innerTrap.getTrapezoid()[1], innerTrap.getTrapezoid()[2])){ //se i due lati coincidenti sono uguali
                            innerTrap = trapezoid(innerTrap.getTrapezoid()[0], rightPSeg, bottomRight, innerTrap.getTrapezoid()[3], innerTrap.getLeftP(), trap.getRightP(), cg3::Segment2d(innerTrap.getTrapezoid()[0], rightPSeg), cg3::Segment2d(innerTrap.getTrapezoid()[3], bottomRight));
                        }
                        insertAfterInner.push_back(trapezoid(topLeft, topRight, rightPSeg, leftPSeg, topLeft, topRight, cg3::Segment2d(topLeft, topRight), cg3::Segment2d(leftPSeg, rightPSeg)));
                        insertionAfterInner=true;
                    }
                }
                else if(trap.getRightP() == bottomRight){ //C3R
                    if(i==0){
                        insertAfterInner.push_back(trapezoid(p1, rightPSeg, trap.getRightP(), downfirst, p1, trap.getRightP(), cg3::Segment2d(p1, rightPSeg), cg3::Segment2d(downfirst, trap.getRightP())));
                        insertionAfterInner=true;
                        innerTrap = trapezoid(topLeft, topRight, rightPSeg, leftPSeg, topLeft, rightPSeg, cg3::Segment2d(topLeft, topRight), cg3::Segment2d(leftPSeg, rightPSeg));
                    }
                    else if(i==(int)TrapMapDag.size()-1){ //last trap
                        if(cg3::Segment2d(leftPSeg, bottomLeft) == cg3::Segment2d(innerTrap.getTrapezoid()[1], innerTrap.getTrapezoid()[2])){ //se i due lati coincidenti sono uguali
                            addPolygon(innerTrap.getTrapezoid()[0], p2, downsecond, innerTrap.getTrapezoid()[3], innerTrap.getLeftP(), p2, cg3::Segment2d(innerTrap.getTrapezoid()[0], p2), cg3::Segment2d(innerTrap.getTrapezoid()[3], downsecond));
                        }
                        if(insertionAfterInner){
                            for(int j=0; j<(int)insertAfterInner.size();j++){
                                addPolygon(insertAfterInner[j].getTrapezoid()[0], insertAfterInner[j].getTrapezoid()[1], insertAfterInner[j].getTrapezoid()[2],insertAfterInner[j].getTrapezoid()[3], insertAfterInner[j].getLeftP(), insertAfterInner[j].getRightP(), insertAfterInner[j].getTop(),insertAfterInner[j].getBottom());
                            }
                            insertionAfterInner=false;
                            insertAfterInner.clear();
                        }
                        addPolygon(trap.getLeftP(), upsecond, p2, leftPSeg, trap.getLeftP(), p2, cg3::Segment2d(trap.getLeftP(), upsecond), cg3::Segment2d(leftPSeg, p2));
                        if(p2 != trap.getRightP()){
                            addPolygon(upsecond, topRight, bottomRight, downsecond, p2, trap.getRightP(), cg3::Segment2d(upsecond, topRight), cg3::Segment2d(downsecond, bottomRight));
                        }
                    }
                    else{
                        if(cg3::Segment2d(leftPSeg, bottomLeft) == cg3::Segment2d(innerTrap.getTrapezoid()[1], innerTrap.getTrapezoid()[2])){ //se i due lati coincidenti sono uguali
                            addPolygon(innerTrap.getTrapezoid()[0], rightPSeg, bottomRight, innerTrap.getTrapezoid()[3], innerTrap.getLeftP(), trap.getRightP(), cg3::Segment2d(innerTrap.getTrapezoid()[0], rightPSeg), cg3::Segment2d(innerTrap.getTrapezoid()[3], bottomRight));
                        }
                        innerTrap = trapezoid(topLeft, topRight, rightPSeg, leftPSeg, topLeft, topRight, cg3::Segment2d(topLeft, topRight), cg3::Segment2d(leftPSeg, rightPSeg));
                    }
                }
                else if(trap.getRightP().x() == topRight.x() && trap.getRightP() != topRight && trap.getRightP() != bottomRight){ //C4R
                    if(i==(int)TrapMapDag.size()-1){ //last trap
                        if(cg3::Segment2d(leftPSeg, bottomLeft) == cg3::Segment2d(innerTrap.getTrapezoid()[1], innerTrap.getTrapezoid()[2])){ //se i due lati coincidenti sono uguali
                            addPolygon(innerTrap.getTrapezoid()[0], p2, downsecond, innerTrap.getTrapezoid()[3], innerTrap.getLeftP(), p2, cg3::Segment2d(innerTrap.getTrapezoid()[0], p2), cg3::Segment2d(innerTrap.getTrapezoid()[3], downsecond));
                        }
                        if(insertionAfterInner){
                            for(int j=0; j<(int)insertAfterInner.size();j++){
                                addPolygon(insertAfterInner[j].getTrapezoid()[0], insertAfterInner[j].getTrapezoid()[1], insertAfterInner[j].getTrapezoid()[2],insertAfterInner[j].getTrapezoid()[3], insertAfterInner[j].getLeftP(), insertAfterInner[j].getRightP(), insertAfterInner[j].getTop(),insertAfterInner[j].getBottom());
                            }
                            insertionAfterInner=false;
                            insertAfterInner.clear();
                        }
                        addPolygon(trap.getLeftP(), upsecond, p2, leftPSeg, trap.getLeftP(), p2, cg3::Segment2d(trap.getLeftP(), upsecond), cg3::Segment2d(leftPSeg, p2));
                        if(p2 != trap.getRightP()){
                            addPolygon(upsecond, topRight, bottomRight, downsecond, p2, trap.getRightP(), cg3::Segment2d(upsecond, topRight), cg3::Segment2d(downsecond, bottomRight));
                        }
                    }
                    else{
                        if(dag.getDag()->determinant(segment, trap.getRightP()) < 0 ){ //sopra
                            if(i==0){
                                addPolygon(p1, rightPSeg, bottomRight, downfirst, p1, trap.getRightP(), cg3::Segment2d(p1, rightPSeg), cg3::Segment2d(downfirst, bottomRight));
                                innerTrap = trapezoid(upfirst, topRight, rightPSeg, p1, p1, rightPSeg, cg3::Segment2d(upfirst, topRight), cg3::Segment2d(p1, rightPSeg));
                            }
                            else{
                                if(cg3::Segment2d(leftPSeg, bottomLeft) == cg3::Segment2d(innerTrap.getTrapezoid()[1], innerTrap.getTrapezoid()[2])){ //se i due lati coincidenti sono uguali
                                    addPolygon(innerTrap.getTrapezoid()[0], rightPSeg, bottomRight, innerTrap.getTrapezoid()[3], innerTrap.getLeftP(), trap.getRightP(), cg3::Segment2d(innerTrap.getTrapezoid()[0], rightPSeg), cg3::Segment2d(innerTrap.getTrapezoid()[3], bottomRight));
                                }
                                innerTrap = trapezoid(trap.getLeftP(), topRight, rightPSeg, leftPSeg, trap.getLeftP(), rightPSeg, cg3::Segment2d(trap.getLeftP(), topRight), cg3::Segment2d(leftPSeg, rightPSeg));
                            }
                        }
                        else{
                            if(i==0){
                                addPolygon(upfirst, topRight, rightPSeg, p1, p1, rightPSeg, cg3::Segment2d(upfirst, topRight), cg3::Segment2d(p1, rightPSeg));
                                innerTrap = trapezoid(p1, rightPSeg, bottomRight, downfirst, p1, trap.getRightP(), cg3::Segment2d(p1, rightPSeg), cg3::Segment2d(downfirst, bottomRight));
                            }
                            else{
                                if(cg3::Segment2d(leftPSeg, bottomLeft) == cg3::Segment2d(innerTrap.getTrapezoid()[1], innerTrap.getTrapezoid()[2])){ //se i due lati coincidenti sono uguali
                                    innerTrap = trapezoid(innerTrap.getTrapezoid()[0], rightPSeg, bottomRight, innerTrap.getTrapezoid()[3], innerTrap.getLeftP(), trap.getRightP(), cg3::Segment2d(innerTrap.getTrapezoid()[0], rightPSeg), cg3::Segment2d(innerTrap.getTrapezoid()[3], bottomRight));
                                }
                                insertAfterInner.push_back(trapezoid(topLeft, topRight, rightPSeg, leftPSeg, topLeft, trap.getRightP(), cg3::Segment2d(topLeft, topRight), cg3::Segment2d(leftPSeg, rightPSeg)));
                                insertionAfterInner=true;
                            }
                        }
                    }
                }
                else if(topRight == cg3::Point2d((1e+6), (1e+6))){ //CR(Right)
                    //last trap
                    if(cg3::Segment2d(leftPSeg, bottomLeft) == cg3::Segment2d(innerTrap.getTrapezoid()[1], innerTrap.getTrapezoid()[2])){ //se i due lati coincidenti sono uguali
                        addPolygon(innerTrap.getTrapezoid()[0], p2, downsecond, innerTrap.getTrapezoid()[3], innerTrap.getLeftP(), p2, cg3::Segment2d(innerTrap.getTrapezoid()[0], p2), cg3::Segment2d(innerTrap.getTrapezoid()[3], downsecond));
                    }
                    if(insertionAfterInner){
                        for(int j=0; j<(int)insertAfterInner.size();j++){
                            addPolygon(insertAfterInner[j].getTrapezoid()[0], insertAfterInner[j].getTrapezoid()[1], insertAfterInner[j].getTrapezoid()[2],insertAfterInner[j].getTrapezoid()[3], insertAfterInner[j].getLeftP(), insertAfterInner[j].getRightP(), insertAfterInner[j].getTop(),insertAfterInner[j].getBottom());
                        }
                        insertionAfterInner=false;
                        insertAfterInner.clear();
                    }
                    addPolygon(trap.getLeftP(), upsecond, p2, leftPSeg, trap.getLeftP(), p2, cg3::Segment2d(trap.getLeftP(), upsecond), cg3::Segment2d(leftPSeg, p2));
                    if(p2 != trap.getRightP()){
                        addPolygon(upsecond, topRight, bottomRight, downsecond, p2, trap.getRightP(), cg3::Segment2d(upsecond, topRight), cg3::Segment2d(downsecond, bottomRight));
                    }
                }
            }
            else if(trap.getLeftP() == bottomLeft){ //C3L
                if(topRight == bottomRight){ //C1R
                    //last trap
                    if(cg3::Segment2d(topLeft, leftPSeg) == cg3::Segment2d(innerTrap.getTrapezoid()[1], innerTrap.getTrapezoid()[2])){ //se i due lati coincidenti sono uguali
                        addPolygon(innerTrap.getTrapezoid()[0], upsecond, p2, innerTrap.getTrapezoid()[3], innerTrap.getLeftP(), p2, cg3::Segment2d(innerTrap.getTrapezoid()[0], upsecond), cg3::Segment2d(innerTrap.getTrapezoid()[3], p2));
                    }
                    if(insertionAfterInner){
                        for(int j=0; j<(int)insertAfterInner.size();j++){
                            addPolygon(insertAfterInner[j].getTrapezoid()[0], insertAfterInner[j].getTrapezoid()[1], insertAfterInner[j].getTrapezoid()[2],insertAfterInner[j].getTrapezoid()[3], insertAfterInner[j].getLeftP(), insertAfterInner[j].getRightP(), insertAfterInner[j].getTop(),insertAfterInner[j].getBottom());
                        }
                        insertionAfterInner=false;
                        insertAfterInner.clear();
                    }
                    addPolygon(leftPSeg, p2, downsecond, trap.getLeftP(), trap.getLeftP(), p2, cg3::Segment2d(leftPSeg, p2), cg3::Segment2d(trap.getLeftP(), downsecond));
                    if(p2 != trap.getRightP()){
                        addPolygon(upsecond, topRight, bottomRight, downsecond, p2, trap.getRightP(), cg3::Segment2d(upsecond, topRight), cg3::Segment2d(downsecond, bottomRight));
                    }
                }
                else if(trap.getRightP() == topRight && topRight != cg3::Point2d((1e+6), (1e+6))){ //C2R
                    if(i==(int)TrapMapDag.size()-1){ //last trap
                        if(cg3::Segment2d(topLeft, leftPSeg) == cg3::Segment2d(innerTrap.getTrapezoid()[1], innerTrap.getTrapezoid()[2])){ //se i due lati coincidenti sono uguali
                            addPolygon(innerTrap.getTrapezoid()[0], upsecond, p2, innerTrap.getTrapezoid()[3], innerTrap.getLeftP(), p2, cg3::Segment2d(innerTrap.getTrapezoid()[0], upsecond), cg3::Segment2d(innerTrap.getTrapezoid()[3], p2));
                        }
                        if(insertionAfterInner){
                            for(int j=0; j<(int)insertAfterInner.size();j++){
                                addPolygon(insertAfterInner[j].getTrapezoid()[0], insertAfterInner[j].getTrapezoid()[1], insertAfterInner[j].getTrapezoid()[2],insertAfterInner[j].getTrapezoid()[3], insertAfterInner[j].getLeftP(), insertAfterInner[j].getRightP(), insertAfterInner[j].getTop(),insertAfterInner[j].getBottom());
                            }
                            insertionAfterInner=false;
                            insertAfterInner.clear();
                        }
                        addPolygon(leftPSeg, p2, downsecond, trap.getLeftP(), trap.getLeftP(), p2, cg3::Segment2d(leftPSeg, p2), cg3::Segment2d(trap.getLeftP(), downsecond));
                        if(p2 != trap.getRightP()){
                            addPolygon(upsecond, topRight, bottomRight, downsecond, p2, trap.getRightP(), cg3::Segment2d(upsecond, topRight), cg3::Segment2d(downsecond, bottomRight));
                        }
                    }
                    else{
                        if(cg3::Segment2d(topLeft, leftPSeg) == cg3::Segment2d(innerTrap.getTrapezoid()[1], innerTrap.getTrapezoid()[2])){ //se i due lati coincidenti sono uguali
                            addPolygon(innerTrap.getTrapezoid()[0], topRight, rightPSeg, innerTrap.getTrapezoid()[3], innerTrap.getLeftP(), trap.getRightP(), cg3::Segment2d(innerTrap.getTrapezoid()[0], topRight), cg3::Segment2d(innerTrap.getTrapezoid()[3], rightPSeg));
                        }
                        innerTrap = trapezoid(leftPSeg, rightPSeg, bottomRight, bottomLeft, bottomLeft, rightPSeg, cg3::Segment2d(leftPSeg, rightPSeg), cg3::Segment2d(bottomLeft, bottomRight));
                    }
                }
                else if(trap.getRightP() == bottomRight){ //C3R
                    if(i==0){
                        innerTrap = trapezoid(upfirst, topRight, rightPSeg, p1, p1, rightPSeg, cg3::Segment2d(upfirst, topRight), cg3::Segment2d(p1, rightPSeg));
                        insertAfterInner.push_back(trapezoid(p1, rightPSeg, bottomRight, downfirst, p1, trap.getRightP(), cg3::Segment2d(p1, rightPSeg), cg3::Segment2d(downfirst, bottomRight)));
                        insertionAfterInner=true;
                    }
                    else if(i==(int)TrapMapDag.size()-1){ //last trap
                        if(cg3::Segment2d(topLeft, leftPSeg) == cg3::Segment2d(innerTrap.getTrapezoid()[1], innerTrap.getTrapezoid()[2])){ //se i due lati coincidenti sono uguali
                            addPolygon(innerTrap.getTrapezoid()[0], upsecond, p2, innerTrap.getTrapezoid()[3], innerTrap.getLeftP(), p2, cg3::Segment2d(innerTrap.getTrapezoid()[0], upsecond), cg3::Segment2d(innerTrap.getTrapezoid()[3], p2));
                        }
                        if(insertionAfterInner){
                            for(int j=0; j<(int)insertAfterInner.size();j++){
                                addPolygon(insertAfterInner[j].getTrapezoid()[0], insertAfterInner[j].getTrapezoid()[1], insertAfterInner[j].getTrapezoid()[2],insertAfterInner[j].getTrapezoid()[3], insertAfterInner[j].getLeftP(), insertAfterInner[j].getRightP(), insertAfterInner[j].getTop(),insertAfterInner[j].getBottom());
                            }
                            insertionAfterInner=false;
                            insertAfterInner.clear();
                        }
                        addPolygon(leftPSeg, p2, downsecond, trap.getLeftP(), trap.getLeftP(), p2, cg3::Segment2d(leftPSeg, p2), cg3::Segment2d(trap.getLeftP(), downsecond));
                        if(p2 != trap.getRightP()){
                            addPolygon(upsecond, topRight, bottomRight, downsecond, p2, trap.getRightP(), cg3::Segment2d(upsecond, topRight), cg3::Segment2d(downsecond, bottomRight));
                        }
                    }
                    else{
                        if(cg3::Segment2d(topLeft, leftPSeg) == cg3::Segment2d(innerTrap.getTrapezoid()[1], innerTrap.getTrapezoid()[2])){ //se i due lati coincidenti sono uguali
                            innerTrap = trapezoid(innerTrap.getTrapezoid()[0], topRight, rightPSeg, innerTrap.getTrapezoid()[3], innerTrap.getLeftP(), trap.getRightP(), cg3::Segment2d(innerTrap.getTrapezoid()[0], topRight), cg3::Segment2d(innerTrap.getTrapezoid()[3], rightPSeg));
                        }
                        insertAfterInner.push_back(trapezoid(leftPSeg, rightPSeg, bottomRight, bottomLeft, bottomLeft, rightPSeg, cg3::Segment2d(leftPSeg, rightPSeg), cg3::Segment2d(bottomLeft, bottomRight)));
                        insertionAfterInner=true;
                    }
                }
                else if(trap.getRightP().x() == topRight.x() && trap.getRightP() != topRight && trap.getRightP() != bottomRight){ //C4R
                    if(i==(int)TrapMapDag.size()-1){ //last trap
                        if(cg3::Segment2d(topLeft, leftPSeg) == cg3::Segment2d(innerTrap.getTrapezoid()[1], innerTrap.getTrapezoid()[2])){ //se i due lati coincidenti sono uguali
                            addPolygon(innerTrap.getTrapezoid()[0], upsecond, p2, innerTrap.getTrapezoid()[3], innerTrap.getLeftP(), p2, cg3::Segment2d(innerTrap.getTrapezoid()[0], upsecond), cg3::Segment2d(innerTrap.getTrapezoid()[3], p2));
                        }
                        if(insertionAfterInner){
                            for(int j=0; j<(int)insertAfterInner.size();j++){
                                addPolygon(insertAfterInner[j].getTrapezoid()[0], insertAfterInner[j].getTrapezoid()[1], insertAfterInner[j].getTrapezoid()[2],insertAfterInner[j].getTrapezoid()[3], insertAfterInner[j].getLeftP(), insertAfterInner[j].getRightP(), insertAfterInner[j].getTop(),insertAfterInner[j].getBottom());
                            }
                            insertionAfterInner=false;
                            insertAfterInner.clear();
                        }
                        addPolygon(leftPSeg, p2, downsecond, trap.getLeftP(), trap.getLeftP(), p2, cg3::Segment2d(leftPSeg, p2), cg3::Segment2d(trap.getLeftP(), downsecond));
                        if(p2 != trap.getRightP()){
                            addPolygon(upsecond, topRight, bottomRight, downsecond, p2, trap.getRightP(), cg3::Segment2d(upsecond, topRight), cg3::Segment2d(downsecond, bottomRight));
                        }
                    }
                    else{
                        if(dag.getDag()->determinant(segment, trap.getRightP()) < 0 ){ //sopra
                            if(i==0){
                                innerTrap = trapezoid(upfirst, topRight, rightPSeg, p1, p1, rightPSeg, cg3::Segment2d(upfirst, topRight), cg3::Segment2d(p1, rightPSeg));
                                insertAfterInner.push_back(trapezoid(p1, rightPSeg, bottomRight, downfirst, p1, trap.getRightP(), cg3::Segment2d(p1, rightPSeg), cg3::Segment2d(downfirst, bottomRight)));
                                insertionAfterInner=true;
                            }
                            else{
                                if(cg3::Segment2d(topLeft, leftPSeg) == cg3::Segment2d(innerTrap.getTrapezoid()[1], innerTrap.getTrapezoid()[2])){ //se i due lati coincidenti sono uguali
                                    innerTrap = trapezoid(innerTrap.getTrapezoid()[0], topRight, rightPSeg, innerTrap.getTrapezoid()[3], innerTrap.getLeftP(), trap.getRightP(), cg3::Segment2d(innerTrap.getTrapezoid()[0], topRight), cg3::Segment2d(innerTrap.getTrapezoid()[3], rightPSeg));
                                }
                                insertAfterInner.push_back(trapezoid(leftPSeg, rightPSeg, bottomRight, bottomLeft, bottomLeft, rightPSeg, cg3::Segment2d(leftPSeg, rightPSeg), cg3::Segment2d(bottomLeft, bottomRight)));
                                insertionAfterInner=true;
                            }
                        }
                        else{
                            if(i==0){
                                addPolygon(upfirst, topRight, rightPSeg, p1, p1, trap.getRightP(), cg3::Segment2d(upfirst, topRight), cg3::Segment2d(p1, rightPSeg));
                                innerTrap = trapezoid(p1, rightPSeg, bottomRight, downfirst, p1, rightPSeg, cg3::Segment2d(p1, rightPSeg), cg3::Segment2d(downfirst, bottomRight));
                            }
                            else{
                                if(cg3::Segment2d(topLeft, leftPSeg) == cg3::Segment2d(innerTrap.getTrapezoid()[1], innerTrap.getTrapezoid()[2])){ //se i due lati coincidenti sono uguali
                                    addPolygon(innerTrap.getTrapezoid()[0], topRight, rightPSeg, innerTrap.getTrapezoid()[3], innerTrap.getLeftP(), trap.getRightP(), cg3::Segment2d(innerTrap.getTrapezoid()[0], topRight), cg3::Segment2d(innerTrap.getTrapezoid()[3], rightPSeg));
                                }
                                if(insertionAfterInner){
                                    for(int j=0; j<(int)insertAfterInner.size();j++){
                                        addPolygon(insertAfterInner[j].getTrapezoid()[0], insertAfterInner[j].getTrapezoid()[1], insertAfterInner[j].getTrapezoid()[2],insertAfterInner[j].getTrapezoid()[3], insertAfterInner[j].getLeftP(), insertAfterInner[j].getRightP(), insertAfterInner[j].getTop(),insertAfterInner[j].getBottom());
                                    }
                                    insertionAfterInner=false;
                                    insertAfterInner.clear();
                                }
                                innerTrap = trapezoid(leftPSeg, rightPSeg, bottomRight, bottomLeft, trap.getLeftP(), rightPSeg, cg3::Segment2d(leftPSeg, rightPSeg), cg3::Segment2d(bottomLeft, bottomRight));
                            }
                        }
                    }
                }
                else if(topRight == cg3::Point2d((1e+6), (1e+6))){ //CR(Right)
                    //last trap
                    if(cg3::Segment2d(topLeft, leftPSeg) == cg3::Segment2d(innerTrap.getTrapezoid()[1], innerTrap.getTrapezoid()[2])){ //se i due lati coincidenti sono uguali
                        addPolygon(innerTrap.getTrapezoid()[0], upsecond, p2, innerTrap.getTrapezoid()[3], innerTrap.getLeftP(), p2, cg3::Segment2d(innerTrap.getTrapezoid()[0], upsecond), cg3::Segment2d(innerTrap.getTrapezoid()[3], p2));
                    }
                    if(insertionAfterInner){
                        for(int j=0; j<(int)insertAfterInner.size();j++){
                            addPolygon(insertAfterInner[j].getTrapezoid()[0], insertAfterInner[j].getTrapezoid()[1], insertAfterInner[j].getTrapezoid()[2],insertAfterInner[j].getTrapezoid()[3], insertAfterInner[j].getLeftP(), insertAfterInner[j].getRightP(), insertAfterInner[j].getTop(),insertAfterInner[j].getBottom());
                        }
                        insertionAfterInner=false;
                        insertAfterInner.clear();
                    }
                    addPolygon(leftPSeg, p2, downsecond, bottomLeft, trap.getLeftP(), p2, cg3::Segment2d(leftPSeg, p2), cg3::Segment2d(bottomLeft, downsecond));
                    if(p2 != trap.getRightP()){
                        addPolygon(upsecond, topRight, bottomRight, downsecond, p2, trap.getRightP(), cg3::Segment2d(upsecond, topRight), cg3::Segment2d(downsecond, bottomRight));
                    }
                }
            }
            else if(trap.getLeftP().x() == topLeft.x() && trap.getLeftP() != topLeft && trap.getLeftP() != bottomLeft){ //C4L
                if(topRight == bottomRight){ //C1R
                    if(dag.getDag()->determinant(segment, trap.getLeftP()) < 0 ){ //sopra
                        if(cg3::Segment2d(topLeft, leftPSeg) == cg3::Segment2d(innerTrap.getTrapezoid()[1], innerTrap.getTrapezoid()[2])){ //se i due lati coincidenti sono uguali
                            addPolygon(innerTrap.getTrapezoid()[0], upsecond, p2, innerTrap.getTrapezoid()[3], innerTrap.getLeftP(), p2, cg3::Segment2d(innerTrap.getTrapezoid()[0], upsecond), cg3::Segment2d(innerTrap.getTrapezoid()[3], p2));
                        }
                        if(insertionAfterInner){
                            for(int j=0; j<(int)insertAfterInner.size();j++){
                                addPolygon(insertAfterInner[j].getTrapezoid()[0], insertAfterInner[j].getTrapezoid()[1], insertAfterInner[j].getTrapezoid()[2],insertAfterInner[j].getTrapezoid()[3], insertAfterInner[j].getLeftP(), insertAfterInner[j].getRightP(), insertAfterInner[j].getTop(),insertAfterInner[j].getBottom());
                            }
                            insertionAfterInner=false;
                            insertAfterInner.clear();
                        }
                        addPolygon(leftPSeg, p2, downsecond, trap.getLeftP(), trap.getLeftP(), p2, cg3::Segment2d(leftPSeg, p2), cg3::Segment2d(trap.getLeftP(), downsecond));
                    }
                    else{
                        if(cg3::Segment2d(leftPSeg, bottomLeft) == cg3::Segment2d(innerTrap.getTrapezoid()[1], innerTrap.getTrapezoid()[2])){ //se i due lati coincidenti sono uguali
                            addPolygon(innerTrap.getTrapezoid()[0], p2, downsecond, innerTrap.getTrapezoid()[3], innerTrap.getLeftP(), p2, cg3::Segment2d(innerTrap.getTrapezoid()[0], p2), cg3::Segment2d(innerTrap.getTrapezoid()[3], downsecond));
                        }
                        if(insertionAfterInner){
                            for(int j=0; j<(int)insertAfterInner.size();j++){
                                addPolygon(insertAfterInner[j].getTrapezoid()[0], insertAfterInner[j].getTrapezoid()[1], insertAfterInner[j].getTrapezoid()[2],insertAfterInner[j].getTrapezoid()[3], insertAfterInner[j].getLeftP(), insertAfterInner[j].getRightP(), insertAfterInner[j].getTop(),insertAfterInner[j].getBottom());
                            }
                            insertionAfterInner=false;
                            insertAfterInner.clear();
                        }
                        addPolygon(topLeft, upsecond, p2, leftPSeg, trap.getLeftP(), p2, cg3::Segment2d(topLeft, upsecond), cg3::Segment2d(leftPSeg, p2));
                    }
                    if(p2 != trap.getRightP()){
                        addPolygon(upsecond, topRight, bottomRight, downsecond, p2, trap.getRightP(), cg3::Segment2d(upsecond, topRight), cg3::Segment2d(downsecond, bottomRight));
                    }
                }
                else if(trap.getRightP() == topRight && topRight != cg3::Point2d((1e+6), (1e+6))){ //C2R
                    if(dag.getDag()->determinant(segment, trap.getLeftP()) < 0 ){ //sopra
                        if(i==0){
                            addPolygon(upfirst, topRight, rightPSeg, p1, p1, rightPSeg, cg3::Segment2d(upfirst, topRight), cg3::Segment2d(p1, rightPSeg));
                            innerTrap = trapezoid(p1, rightPSeg, bottomRight, downfirst, p1, rightPSeg, cg3::Segment2d(p1, rightPSeg), cg3::Segment2d(downfirst, bottomRight));
                        }
                        else if(i==(int)TrapMapDag.size()-1){ //last trap
                            if(cg3::Segment2d(topLeft, leftPSeg) == cg3::Segment2d(innerTrap.getTrapezoid()[1], innerTrap.getTrapezoid()[2])){ //se i due lati coincidenti sono uguali
                                addPolygon(innerTrap.getTrapezoid()[0], upsecond, p2, innerTrap.getTrapezoid()[3], innerTrap.getLeftP(), p2, cg3::Segment2d(innerTrap.getTrapezoid()[0], upsecond), cg3::Segment2d(innerTrap.getTrapezoid()[3], p2));
                            }
                            if(insertionAfterInner){
                                for(int j=0; j<(int)insertAfterInner.size();j++){
                                    addPolygon(insertAfterInner[j].getTrapezoid()[0], insertAfterInner[j].getTrapezoid()[1], insertAfterInner[j].getTrapezoid()[2],insertAfterInner[j].getTrapezoid()[3], insertAfterInner[j].getLeftP(), insertAfterInner[j].getRightP(), insertAfterInner[j].getTop(),insertAfterInner[j].getBottom());
                                }
                                insertionAfterInner=false;
                                insertAfterInner.clear();
                            }
                            addPolygon(leftPSeg, p2, downsecond, bottomLeft, trap.getLeftP(), p2, cg3::Segment2d(leftPSeg, p2), cg3::Segment2d(bottomLeft, downsecond));
                            if(p2 != trap.getRightP()){
                                addPolygon(upsecond, topRight, bottomRight, downsecond, p2, trap.getRightP(), cg3::Segment2d(upsecond, topRight), cg3::Segment2d(downsecond, bottomRight));
                            }
                        }
                        else{
                            if(cg3::Segment2d(topLeft, leftPSeg) == cg3::Segment2d(innerTrap.getTrapezoid()[1], innerTrap.getTrapezoid()[2])){ //se i due lati coincidenti sono uguali
                                addPolygon(innerTrap.getTrapezoid()[0], topRight, rightPSeg, innerTrap.getTrapezoid()[3], innerTrap.getLeftP(), trap.getRightP(), cg3::Segment2d(innerTrap.getTrapezoid()[0], topRight), cg3::Segment2d(innerTrap.getTrapezoid()[3], rightPSeg));
                            }
                            innerTrap = trapezoid(leftPSeg, rightPSeg, bottomRight, bottomLeft, bottomLeft, rightPSeg, cg3::Segment2d(leftPSeg, rightPSeg), cg3::Segment2d(bottomLeft, bottomRight));
                        }
                    }
                    else{
                        if(i==0){
                            addPolygon(upfirst, trap.getRightP(), rightPSeg, p1, p1, trap.getRightP(), cg3::Segment2d(upfirst, trap.getRightP()), cg3::Segment2d(p1, rightPSeg));
                            innerTrap = trapezoid(p1, rightPSeg, bottomRight, bottomLeft, p1, rightPSeg, cg3::Segment2d(p1, rightPSeg), cg3::Segment2d(bottomLeft, bottomRight));
                        }
                        else if(i==(int)TrapMapDag.size()-1){ //last trap
                            if(cg3::Segment2d(leftPSeg, bottomLeft) == cg3::Segment2d(innerTrap.getTrapezoid()[1], innerTrap.getTrapezoid()[2])){ //se i due lati coincidenti sono uguali
                                addPolygon(innerTrap.getTrapezoid()[0], p2, downsecond, innerTrap.getTrapezoid()[3], innerTrap.getLeftP(), p2, cg3::Segment2d(innerTrap.getTrapezoid()[0], p2), cg3::Segment2d(innerTrap.getTrapezoid()[3], downsecond));
                            }
                            if(insertionAfterInner){
                                for(int j=0; j<(int)insertAfterInner.size();j++){
                                    addPolygon(insertAfterInner[j].getTrapezoid()[0], insertAfterInner[j].getTrapezoid()[1], insertAfterInner[j].getTrapezoid()[2],insertAfterInner[j].getTrapezoid()[3], insertAfterInner[j].getLeftP(), insertAfterInner[j].getRightP(), insertAfterInner[j].getTop(),insertAfterInner[j].getBottom());
                                }
                                insertionAfterInner=false;
                                insertAfterInner.clear();
                            }
                            addPolygon(topLeft, upsecond, p2, leftPSeg, trap.getLeftP(), p2, cg3::Segment2d(trap.getLeftP(), upsecond), cg3::Segment2d(leftPSeg, p2));
                            if(p2 != trap.getRightP()){
                                addPolygon(upsecond, topRight, bottomRight, downsecond, p2, trap.getRightP(), cg3::Segment2d(upsecond, topRight), cg3::Segment2d(downsecond, bottomRight));
                            }
                        }
                        else{
                            if(cg3::Segment2d(leftPSeg, bottomLeft) == cg3::Segment2d(innerTrap.getTrapezoid()[1], innerTrap.getTrapezoid()[2])){ //se i due lati coincidenti sono uguali
                                innerTrap = trapezoid(innerTrap.getTrapezoid()[0], rightPSeg, bottomRight, innerTrap.getTrapezoid()[3], innerTrap.getLeftP(), trap.getRightP(), cg3::Segment2d(innerTrap.getTrapezoid()[0], rightPSeg), cg3::Segment2d(innerTrap.getTrapezoid()[3], bottomRight));
                            }
                            insertAfterInner.push_back(trapezoid(topLeft, topRight, rightPSeg, leftPSeg, topLeft, topRight, cg3::Segment2d(topLeft, topRight), cg3::Segment2d(leftPSeg, rightPSeg)));
                            insertionAfterInner=true;
                        }
                    }
                }
                else if(trap.getRightP() == bottomRight){ //C3R
                    if(dag.getDag()->determinant(segment, trap.getLeftP()) < 0 ){ //sopra
                        if(i==0){
                            innerTrap = trapezoid(upfirst, topRight, rightPSeg, p1, p1, rightPSeg, cg3::Segment2d(upfirst, topRight), cg3::Segment2d(p1, rightPSeg));
                            insertAfterInner.push_back(trapezoid(p1, rightPSeg, bottomRight, downfirst, p1, rightPSeg, cg3::Segment2d(p1, rightPSeg), cg3::Segment2d(downfirst, bottomRight)));
                            insertionAfterInner=true;
                        }
                        else if(i==(int)TrapMapDag.size()-1){ //last trap
                            if(cg3::Segment2d(topLeft, leftPSeg) == cg3::Segment2d(innerTrap.getTrapezoid()[1], innerTrap.getTrapezoid()[2])){ //se i due lati coincidenti sono uguali
                                addPolygon(innerTrap.getTrapezoid()[0], upsecond, p2, innerTrap.getTrapezoid()[3], innerTrap.getLeftP(), p2, cg3::Segment2d(innerTrap.getTrapezoid()[0], upsecond), cg3::Segment2d(innerTrap.getTrapezoid()[3], p2));
                            }
                            if(insertionAfterInner){
                                for(int j=0; j<(int)insertAfterInner.size();j++){
                                    addPolygon(insertAfterInner[j].getTrapezoid()[0], insertAfterInner[j].getTrapezoid()[1], insertAfterInner[j].getTrapezoid()[2],insertAfterInner[j].getTrapezoid()[3], insertAfterInner[j].getLeftP(), insertAfterInner[j].getRightP(), insertAfterInner[j].getTop(),insertAfterInner[j].getBottom());
                                }
                                insertionAfterInner=false;
                                insertAfterInner.clear();
                            }
                            addPolygon(leftPSeg, p2, downsecond, bottomLeft, trap.getLeftP(), p2, cg3::Segment2d(leftPSeg, p2), cg3::Segment2d(bottomLeft, downsecond));
                            if(p2 != trap.getRightP()){
                                addPolygon(upsecond, topRight, bottomRight, downsecond, p2, trap.getRightP(), cg3::Segment2d(upsecond, topRight), cg3::Segment2d(downsecond, bottomRight));
                            }
                        }
                        else{
                            if(cg3::Segment2d(topLeft, leftPSeg) == cg3::Segment2d(innerTrap.getTrapezoid()[1], innerTrap.getTrapezoid()[2])){ //se i due lati coincidenti sono uguali
                                innerTrap = trapezoid(innerTrap.getTrapezoid()[0], topRight, rightPSeg, innerTrap.getTrapezoid()[3], innerTrap.getLeftP(), trap.getRightP(), cg3::Segment2d(innerTrap.getTrapezoid()[0], topRight), cg3::Segment2d(innerTrap.getTrapezoid()[3], rightPSeg));
                            }
                            insertAfterInner.push_back(trapezoid(leftPSeg, rightPSeg, bottomRight, bottomLeft, bottomLeft, rightPSeg, cg3::Segment2d(leftPSeg, rightPSeg), cg3::Segment2d(bottomLeft, bottomRight)));
                            insertionAfterInner=true;
                        }
                    }
                    else{
                        if(i==0){
                            innerTrap = trapezoid(upfirst, topRight, rightPSeg, p1, p1, rightPSeg, cg3::Segment2d(upfirst, topRight), cg3::Segment2d(p1, rightPSeg));
                            insertAfterInner.push_back(trapezoid(p1, rightPSeg, bottomRight, downfirst, p1, trap.getRightP(), cg3::Segment2d(p1, rightPSeg), cg3::Segment2d(downfirst, bottomRight)));
                            insertionAfterInner=true;
                        }
                        else if(i==(int)TrapMapDag.size()-1){ //last trap
                            if(cg3::Segment2d(leftPSeg, bottomLeft) == cg3::Segment2d(innerTrap.getTrapezoid()[1], innerTrap.getTrapezoid()[2])){ //se i due lati coincidenti sono uguali
                                addPolygon(innerTrap.getTrapezoid()[0], p2, downsecond, innerTrap.getTrapezoid()[3], innerTrap.getLeftP(), p2, cg3::Segment2d(innerTrap.getTrapezoid()[0], p2), cg3::Segment2d(innerTrap.getTrapezoid()[3], downsecond));
                            }
                            if(insertionAfterInner){
                                for(int j=0; j<(int)insertAfterInner.size();j++){
                                    addPolygon(insertAfterInner[j].getTrapezoid()[0], insertAfterInner[j].getTrapezoid()[1], insertAfterInner[j].getTrapezoid()[2],insertAfterInner[j].getTrapezoid()[3], insertAfterInner[j].getLeftP(), insertAfterInner[j].getRightP(), insertAfterInner[j].getTop(),insertAfterInner[j].getBottom());
                                }
                                insertionAfterInner=false;
                                insertAfterInner.clear();
                            }
                            addPolygon(topLeft, upsecond, p2, leftPSeg, trap.getLeftP(), p2, cg3::Segment2d(topLeft, upsecond), cg3::Segment2d(leftPSeg, p2));
                            if(p2 != trap.getRightP()){
                                addPolygon(upsecond, topRight, bottomRight, downsecond, p2, trap.getRightP(), cg3::Segment2d(upsecond, topRight), cg3::Segment2d(downsecond, bottomRight));
                            }
                        }
                        else{
                            if(cg3::Segment2d(leftPSeg, bottomLeft) == cg3::Segment2d(innerTrap.getTrapezoid()[1], innerTrap.getTrapezoid()[2])){ //se i due lati coincidenti sono uguali
                                addPolygon(innerTrap.getTrapezoid()[0], rightPSeg, bottomRight, innerTrap.getTrapezoid()[3], innerTrap.getLeftP(), trap.getRightP(), cg3::Segment2d(innerTrap.getTrapezoid()[0], rightPSeg), cg3::Segment2d(innerTrap.getTrapezoid()[3], bottomRight));
                            }
                            innerTrap = trapezoid(topLeft, topRight, rightPSeg, leftPSeg, trap.getLeftP(), rightPSeg, cg3::Segment2d(topLeft, topRight), cg3::Segment2d(leftPSeg, rightPSeg));
                        }
                    }
                }
                else if(trap.getRightP().x() == topRight.x() && trap.getRightP() != topRight && trap.getRightP() != bottomRight){ //C4R
                    if(dag.getDag()->determinant(segment, trap.getLeftP()) < 0 ){ //sopra
                        if(i==(int)TrapMapDag.size()-1){ //last trap
                            if(cg3::Segment2d(topLeft, leftPSeg) == cg3::Segment2d(innerTrap.getTrapezoid()[1], innerTrap.getTrapezoid()[2])){ //se i due lati coincidenti sono uguali
                                addPolygon(innerTrap.getTrapezoid()[0], upsecond, p2, innerTrap.getTrapezoid()[3], innerTrap.getLeftP(), p2, cg3::Segment2d(innerTrap.getTrapezoid()[0], upsecond), cg3::Segment2d(innerTrap.getTrapezoid()[3], p2));
                            }
                            if(insertionAfterInner){
                                for(int j=0; j<(int)insertAfterInner.size();j++){
                                    addPolygon(insertAfterInner[j].getTrapezoid()[0], insertAfterInner[j].getTrapezoid()[1], insertAfterInner[j].getTrapezoid()[2],insertAfterInner[j].getTrapezoid()[3], insertAfterInner[j].getLeftP(), insertAfterInner[j].getRightP(), insertAfterInner[j].getTop(),insertAfterInner[j].getBottom());
                                }
                                insertionAfterInner=false;
                                insertAfterInner.clear();
                            }
                            addPolygon(leftPSeg, p2, downsecond, bottomLeft, trap.getLeftP(), p2, cg3::Segment2d(leftPSeg, p2), cg3::Segment2d(trap.getLeftP(), downsecond));
                            if(p2 != trap.getRightP()){
                                addPolygon(upsecond, topRight, bottomRight, downsecond, p2, trap.getRightP(), cg3::Segment2d(upsecond, topRight), cg3::Segment2d(downsecond, bottomRight));
                            }
                        }
                        else{
                            if(dag.getDag()->determinant(segment, trap.getRightP()) < 0 ){ //sopra
                                if(i==0){
                                    innerTrap = trapezoid(upfirst, topRight, rightPSeg, p1, p1, trap.getRightP(), cg3::Segment2d(upfirst, topRight), cg3::Segment2d(p1, rightPSeg));
                                    insertAfterInner.push_back(trapezoid(p1, rightPSeg, bottomRight, bottomLeft, p1, rightPSeg, cg3::Segment2d(p1, rightPSeg), cg3::Segment2d(bottomLeft, bottomRight)));
                                    insertionAfterInner=true;
                                }
                                else{
                                    if(cg3::Segment2d(topLeft, leftPSeg) == cg3::Segment2d(innerTrap.getTrapezoid()[1], innerTrap.getTrapezoid()[2])){ //se i due lati coincidenti sono uguali
                                        innerTrap = trapezoid(innerTrap.getTrapezoid()[0], topRight, rightPSeg, innerTrap.getTrapezoid()[3], innerTrap.getLeftP(), trap.getRightP(), cg3::Segment2d(innerTrap.getTrapezoid()[0], topRight), cg3::Segment2d(innerTrap.getTrapezoid()[3], rightPSeg));
                                    }
                                    insertAfterInner.push_back(trapezoid(leftPSeg, rightPSeg, bottomRight, bottomLeft, trap.getLeftP(), trap.getRightP(), cg3::Segment2d(leftPSeg, rightPSeg), cg3::Segment2d(bottomLeft, bottomRight)));
                                    insertionAfterInner=true;
                                }
                            }
                            else{
                                if(i==0){
                                    addPolygon(upfirst, topRight, rightPSeg, p1, p1, trap.getRightP(), cg3::Segment2d(upfirst, topRight), cg3::Segment2d(p1, rightPSeg));
                                    innerTrap = trapezoid(p1, rightPSeg, bottomRight, bottomLeft, p1, rightPSeg, cg3::Segment2d(p1, rightPSeg), cg3::Segment2d(bottomLeft, bottomRight));
                                }
                                else{
                                    if(cg3::Segment2d(topLeft, leftPSeg) == cg3::Segment2d(innerTrap.getTrapezoid()[1], innerTrap.getTrapezoid()[2])){ //se i due lati coincidenti sono uguali
                                        addPolygon(innerTrap.getTrapezoid()[0], topRight, rightPSeg, innerTrap.getTrapezoid()[3], innerTrap.getLeftP(), trap.getRightP(), cg3::Segment2d(innerTrap.getTrapezoid()[0], topRight), cg3::Segment2d(innerTrap.getTrapezoid()[3], rightPSeg));
                                    }
                                    if(insertionAfterInner){
                                        for(int j=0; j<(int)insertAfterInner.size();j++){
                                            addPolygon(insertAfterInner[j].getTrapezoid()[0], insertAfterInner[j].getTrapezoid()[1], insertAfterInner[j].getTrapezoid()[2],insertAfterInner[j].getTrapezoid()[3], insertAfterInner[j].getLeftP(), insertAfterInner[j].getRightP(), insertAfterInner[j].getTop(),insertAfterInner[j].getBottom());
                                        }
                                        insertionAfterInner=false;
                                        insertAfterInner.clear();
                                    }
                                    innerTrap = trapezoid(leftPSeg, rightPSeg, bottomRight, bottomLeft, trap.getLeftP(), rightPSeg, cg3::Segment2d(leftPSeg, rightPSeg), cg3::Segment2d(bottomLeft, bottomRight));
                                }
                            }
                        }
                    }
                    else{
                        if(i==0){
                            addPolygon(upfirst, topRight, rightPSeg, p1, p1, trap.getRightP(), cg3::Segment2d(upfirst, topRight), cg3::Segment2d(p1, rightPSeg));
                            innerTrap = trapezoid(p1, rightPSeg, bottomRight, bottomLeft, p1, rightPSeg, cg3::Segment2d(p1, rightPSeg), cg3::Segment2d(bottomLeft, bottomRight));
                        }
                        else if(i==(int)TrapMapDag.size()-1){ //last trap
                            if(cg3::Segment2d(leftPSeg, bottomLeft) == cg3::Segment2d(innerTrap.getTrapezoid()[1], innerTrap.getTrapezoid()[2])){ //se i due lati coincidenti sono uguali
                                addPolygon(innerTrap.getTrapezoid()[0], p2, downsecond, innerTrap.getTrapezoid()[3], innerTrap.getLeftP(), p2, cg3::Segment2d(innerTrap.getTrapezoid()[0], p2), cg3::Segment2d(innerTrap.getTrapezoid()[3], downsecond));
                            }
                            if(insertionAfterInner){
                                for(int j=0; j<(int)insertAfterInner.size();j++){
                                    addPolygon(insertAfterInner[j].getTrapezoid()[0], insertAfterInner[j].getTrapezoid()[1], insertAfterInner[j].getTrapezoid()[2],insertAfterInner[j].getTrapezoid()[3], insertAfterInner[j].getLeftP(), insertAfterInner[j].getRightP(), insertAfterInner[j].getTop(),insertAfterInner[j].getBottom());
                                }
                                insertionAfterInner=false;
                                insertAfterInner.clear();
                            }
                            addPolygon(topLeft, upsecond, p2, leftPSeg, trap.getLeftP(), p2, cg3::Segment2d(topLeft, upsecond), cg3::Segment2d(leftPSeg, p2));
                            if(p2 != trap.getRightP()){
                                addPolygon(upsecond, topRight, bottomRight, downsecond, p2, trap.getRightP(), cg3::Segment2d(upsecond, topRight), cg3::Segment2d(downsecond, bottomRight));
                            }
                        }
                        else{
                            if(dag.getDag()->determinant(segment, trap.getRightP()) < 0 ){ //sopra

                                if(cg3::Segment2d(leftPSeg, bottomLeft) == cg3::Segment2d(innerTrap.getTrapezoid()[1], innerTrap.getTrapezoid()[2])){ //se i due lati coincidenti sono uguali
                                    addPolygon(innerTrap.getTrapezoid()[0], rightPSeg, bottomRight, innerTrap.getTrapezoid()[3], innerTrap.getLeftP(), bottomRight, cg3::Segment2d(innerTrap.getTrapezoid()[0], rightPSeg), cg3::Segment2d(innerTrap.getTrapezoid()[3], bottomRight));
                                }
                                if(insertionAfterInner){
                                    for(int j=0; j<(int)insertAfterInner.size();j++){
                                        addPolygon(insertAfterInner[j].getTrapezoid()[0], insertAfterInner[j].getTrapezoid()[1], insertAfterInner[j].getTrapezoid()[2],insertAfterInner[j].getTrapezoid()[3], insertAfterInner[j].getLeftP(), insertAfterInner[j].getRightP(), insertAfterInner[j].getTop(),insertAfterInner[j].getBottom());
                                    }
                                    insertionAfterInner=false;
                                    insertAfterInner.clear();
                                }
                                innerTrap = trapezoid(topLeft, topRight, rightPSeg, leftPSeg, trap.getLeftP(), topRight, cg3::Segment2d(topLeft, topRight), cg3::Segment2d(leftPSeg, rightPSeg));
                            }
                            else{
                                if(cg3::Segment2d(leftPSeg, bottomLeft) == cg3::Segment2d(innerTrap.getTrapezoid()[1], innerTrap.getTrapezoid()[2])){ //se i due lati coincidenti sono uguali
                                    innerTrap = trapezoid(innerTrap.getTrapezoid()[0], rightPSeg, bottomRight, innerTrap.getTrapezoid()[3], innerTrap.getLeftP(), trap.getRightP(), cg3::Segment2d(innerTrap.getTrapezoid()[0], rightPSeg), cg3::Segment2d(innerTrap.getTrapezoid()[3], bottomRight));
                                }
                                insertAfterInner.push_back(trapezoid(topLeft, topRight, rightPSeg, leftPSeg, trap.getLeftP(), trap.getRightP(), cg3::Segment2d(topLeft, topRight), cg3::Segment2d(leftPSeg, rightPSeg)));
                                insertionAfterInner=true;
                            }
                        }
                    }
                }
                else if(topRight == cg3::Point2d((1e+6), (1e+6))){ //CR(Right)
                    if(dag.getDag()->determinant(segment, trap.getLeftP()) < 0 ){ //sopra
                        if(cg3::Segment2d(topLeft, leftPSeg) == cg3::Segment2d(innerTrap.getTrapezoid()[1], innerTrap.getTrapezoid()[2])){ //se i due lati coincidenti sono uguali
                            addPolygon(innerTrap.getTrapezoid()[0], upsecond, p2, innerTrap.getTrapezoid()[3], innerTrap.getLeftP(), p2, cg3::Segment2d(innerTrap.getTrapezoid()[0], upsecond), cg3::Segment2d(innerTrap.getTrapezoid()[3], p2));
                        }
                        if(insertionAfterInner){
                            for(int j=0; j<(int)insertAfterInner.size();j++){
                                addPolygon(insertAfterInner[j].getTrapezoid()[0], insertAfterInner[j].getTrapezoid()[1], insertAfterInner[j].getTrapezoid()[2],insertAfterInner[j].getTrapezoid()[3], insertAfterInner[j].getLeftP(), insertAfterInner[j].getRightP(), insertAfterInner[j].getTop(),insertAfterInner[j].getBottom());
                            }
                            insertionAfterInner=false;
                            insertAfterInner.clear();
                        }
                        addPolygon(leftPSeg, p2, downsecond, bottomLeft, trap.getLeftP(), p2, cg3::Segment2d(leftPSeg, p2), cg3::Segment2d(bottomLeft, downsecond));
                    }
                    else{
                        if(cg3::Segment2d(leftPSeg, bottomLeft) == cg3::Segment2d(innerTrap.getTrapezoid()[1], innerTrap.getTrapezoid()[2])){ //se i due lati coincidenti sono uguali
                            addPolygon(innerTrap.getTrapezoid()[0], p2, downsecond, innerTrap.getTrapezoid()[3], innerTrap.getLeftP(), p2, cg3::Segment2d(innerTrap.getTrapezoid()[0], p2), cg3::Segment2d(innerTrap.getTrapezoid()[3], downsecond));
                        }
                        if(insertionAfterInner){
                            for(int j=0; j<(int)insertAfterInner.size();j++){
                                addPolygon(insertAfterInner[j].getTrapezoid()[0], insertAfterInner[j].getTrapezoid()[1], insertAfterInner[j].getTrapezoid()[2],insertAfterInner[j].getTrapezoid()[3], insertAfterInner[j].getLeftP(), insertAfterInner[j].getRightP(), insertAfterInner[j].getTop(),insertAfterInner[j].getBottom());
                            }
                            insertionAfterInner=false;
                            insertAfterInner.clear();
                        }
                        addPolygon(topLeft, upsecond, p2, leftPSeg, trap.getLeftP(), p2, cg3::Segment2d(topLeft, upsecond), cg3::Segment2d(leftPSeg, p2));
                    }
                    if(p2 != trap.getRightP()){
                        addPolygon(upsecond, topRight, bottomRight, downsecond, p2, trap.getRightP(), cg3::Segment2d(upsecond, topRight), cg3::Segment2d(downsecond, bottomRight));
                    }
                }
            }
        }
        i++;
    }
}

void TrapezoidalMap::singleTrapezoid(const cg3::Point2d p1, const cg3::Point2d p2, const cg3::Point2d upfirst, const cg3::Point2d upsecond, const cg3::Point2d downfirst,
                                     const cg3::Point2d downsecond, trapezoid trap){

    topLeft = trap.getTrapezoid()[0];
    bottomLeft = trap.getTrapezoid()[3];
    topRight = trap.getTrapezoid()[1];
    bottomRight = trap.getTrapezoid()[2];

    /*
     * control for the triangles
     */
    if(trap.getLeftP() == p1 && trap.getRightP() == p2){
        addPolygon(upfirst, upsecond, p2, p1, p1, p2, cg3::Segment2d(upfirst, upsecond), cg3::Segment2d(p1, p2)); //B
        addPolygon(p1, p2, downsecond, downfirst, p1, p2, cg3::Segment2d(p1, p2), cg3::Segment2d(downfirst, downsecond)); //C
    }
    else if(trap.getLeftP() == p1){
        addPolygon(upfirst, upsecond, p2, p1, p1, p2, cg3::Segment2d(upfirst, upsecond), cg3::Segment2d(p1, p2)); //B
        addPolygon(p1, p2, downsecond, downfirst, p1, p2, cg3::Segment2d(p1, p2), cg3::Segment2d(downfirst, downsecond)); //C
        addPolygon(upsecond, topRight, bottomRight, downsecond, p2, trap.getRightP(), cg3::Segment2d(upsecond, topRight), cg3::Segment2d(downsecond, bottomRight)); //D
    }
    else if(trap.getRightP() == p2){
        addPolygon(topLeft, upfirst, downfirst, bottomLeft, trap.getLeftP(), p1, cg3::Segment2d(topLeft, upfirst), cg3::Segment2d(bottomLeft, downfirst)); //A
        addPolygon(upfirst, upsecond, p2, p1, p1, p2, cg3::Segment2d(upfirst, upsecond), cg3::Segment2d(p1, p2)); //B
        addPolygon(p1, p2, downsecond, downfirst, p1, p2, cg3::Segment2d(p1, p2), cg3::Segment2d(downfirst, downsecond)); //C
    }
    else{ //normal step
        addPolygon(topLeft, upfirst, downfirst, bottomLeft, trap.getLeftP(), p1, cg3::Segment2d(topLeft, upfirst), cg3::Segment2d(bottomLeft, downfirst)); //A
        addPolygon(upfirst, upsecond, p2, p1, p1, p2, cg3::Segment2d(upfirst, upsecond), cg3::Segment2d(p1, p2)); //B
        addPolygon(p1, p2, downsecond, downfirst, p1, p2, cg3::Segment2d(p1, p2), cg3::Segment2d(downfirst, downsecond)); //C
        addPolygon(upsecond, topRight, bottomRight, downsecond, p2, trap.getRightP(), cg3::Segment2d(upsecond, topRight), cg3::Segment2d(downsecond, bottomRight)); //D
    }
}


/*
 * @brief Method to add polygon in the structures of trapezoidal map
 * @param[in] 4 points Point for the new polygon
 */
void TrapezoidalMap::addPolygon(const cg3::Point2d p1, const cg3::Point2d p2, const cg3::Point2d p3, const cg3::Point2d p4, const cg3::Point2d leftp, const cg3::Point2d rightp,
                                const cg3::Segment2d top, const cg3::Segment2d bottom){

    trapezoidsList.push_back(trapezoid(p1, p2, p3, p4, leftp, rightp, top, bottom));
    dag.setTrapezoidToInsert(trapezoid(p1, p2, p3, p4, leftp, rightp, top, bottom), trapezoidsList.size());
}



/**
 * @brief Method to return the trapezoids
 * @param[out] vector of trapezoids
 */
std::list<trapezoid> TrapezoidalMap::getTrapezoids() const{
    return trapezoidsList;
}


/**
 * @brief Method to set the trapezoid after the query point: Point Location Query
 * @param[in] point Point
 */
void TrapezoidalMap::queryPoint(cg3::Point2d point){
    queryPointTrap = dag.findTrapezoid(point, cg3::Point2d(0,0), true, dag.getDag()).getTrapezoid();
}

/**
 * @brief Method to return the trapezoid after the point location query
 * @param[out] found Trapezoid
 */
std::array<cg3::Point2d, 4> TrapezoidalMap::getFoundTrapezoid() const{
    return queryPointTrap;
}

/**
 * @brief Cleared structures for the trapezoidal map
 */
void TrapezoidalMap::clearMap()
{
    boundingBox.setMin(cg3::Point2d(0,0));
    boundingBox.setMax(cg3::Point2d(0,0));
    trapezoidsList.clear();
}


