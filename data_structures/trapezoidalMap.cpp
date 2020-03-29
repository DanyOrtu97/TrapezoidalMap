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

    cg3::Segment2d top;
    cg3::Segment2d bottom;
    cg3::Point2d leftp;
    cg3::Point2d rightp;

    size_t id2;

    //inizialize the trapezoidal map with the bounding box
    //addPolygon(topLeft, topRight, bottomRight, bottomLeft);

    //std::vector<std::array<cg3::Point2d, 4>>
    std::array<cg3::Point2d, 4> trap = {topLeft, topRight, bottomRight, bottomLeft};

    leftp.set(trap[0].x(), trap[0].y());
    rightp.set(trap[1].x(), trap[1].y());
    top.set(trap[0], trap[1]);
    bottom.set(trap[3],trap[2]);

    id2 = std::numeric_limits<size_t>::max();

    /*
    topMap.insert(std::make_pair(cg3::Segment2d(top), id2));
    bottomMap.insert(std::make_pair(cg3::Segment2d(bottom), id2));
    leftpList.push_back(leftp);
    rightpList.push_back(rightp);
    */
    trapezoid trapez = trapezoid(topLeft, topRight, bottomRight, bottomLeft, leftp, rightp, top, bottom);

    trapezoidsMap.insert(std::make_pair(trapez, id2));
    trapezoids.push_back(trapez);

    //inizialize the dag with the pointer of the first polygon (the bounding box)
    dag.inizializeDag(trapez);

}

/**
 * @brief Method to update the trapezoidal map by the new trapezoids
 * @param[in] segment Segment
 */
void TrapezoidalMap::trapezoidalMapAlgorithm(cg3::Segment2d segment){

    trapezoid firstTrapezoid = dag.findTrapezoid(segment.p1(), false, dag.getDag());
    trapezoid lastTrapezoid = dag.findTrapezoid(segment.p2(), false, dag.getDag());

    std::vector<trapezoid> foundTrapezoids;

    if (firstTrapezoid.getTrapezoid() == lastTrapezoid.getTrapezoid()){

        dag.clearTraps();

        CompletelyInsideTrapezoid(firstTrapezoid, segment);

        dag.updateDag(segment);

    }
    else{
        //split node
        nodeDag* splitNode= GasAlgorithms::findSplitNode(segment, dag.getDag());

        dag.findTrapezoids(segment, splitNode, nullptr);

        std::map<trapezoid, nodeDag**> TrapMapDag = dag.getPointerMap();

        for(std::map<trapezoid, nodeDag**>::iterator it = TrapMapDag.begin(); it!=TrapMapDag.end(); it++){
            foundTrapezoids.push_back(it->first);
        }

        dag.clearTraps();

        multipleTrapezoid(segment, foundTrapezoids);

        dag.updateDag(segment);


    }

}

/**
 * @brief Method to update the trapezoidal map in the case of single trapezoid insertion
 * @param[in] t Trapezoid, segment Segment
 */
void TrapezoidalMap::CompletelyInsideTrapezoid(trapezoid t, const cg3::Segment2d& segment){
    std::map<trapezoid, size_t> delet;
    //std::map<cg3::Segment2d, size_t> deletSegTop, deletSegBottom;
    std::list<cg3::Point2d> deletPoint;
    bool found;
    //bool up=false;

    delet = findTrapezoid(t, found);
    /*
    deletSegTop.insert(std::make_pair(cg3::Segment2d(delet.begin()->first[0], delet.begin()->first[1]), delet.begin()->second));
    deletSegBottom.insert(std::make_pair(cg3::Segment2d(delet.begin()->first[3], delet.begin()->first[2]), delet.begin()->second));
    */

    topLeft = t.getTrapezoid()[0];
    bottomLeft = t.getTrapezoid()[3];
    topRight = t.getTrapezoid()[1];
    bottomRight = t.getTrapezoid()[2];

    cg3::Segment2d topS;
    topS.set(topLeft, topRight);
    cg3::Segment2d bottomS;
    bottomS.set(bottomLeft, bottomRight);

    cg3::Point2d p1 = segment.p1();
    cg3::Point2d p2 = segment.p2();
    cg3::Point2d upfirst;
    cg3::Point2d upsecond;
    cg3::Point2d downfirst;
    cg3::Point2d downsecond;

    /*
    std::map<cg3::Segment2d, size_t>::iterator top = topMap.find(topS);
    std::map<cg3::Segment2d, size_t>::iterator bottom = bottomMap.find(bottomS);
    */

    //if (top != topMap.end()) {
        upfirst = GasAlgorithms::createPoint(upfirst, p1.x(), GasAlgorithms::handleSlopeSegment(topS, p1));
        upsecond = GasAlgorithms::createPoint(upsecond, p2.x(), GasAlgorithms::handleSlopeSegment(topS, p2));
    //}

    //if (bottom != bottomMap.end()) {
        downfirst = GasAlgorithms::createPoint(downfirst, p1.x(), GasAlgorithms::handleSlopeSegment(bottomS, p1));
        downsecond = GasAlgorithms::createPoint(downsecond, p2.x(), GasAlgorithms::handleSlopeSegment(bottomS, p2));
    //}

    //deletPoint.clear();
    //deletPoint.insert(deletPoint.begin(), delet.begin()->first[0]);
    //leftpList.erase(deletPoint.begin());
    //deletPoint.clear();
    //deletPoint.insert(deletPoint.begin(), delet.begin()->first[1]);
    //rightpList.erase(deletPoint.begin());
    //topMap.erase(deletSegTop.begin()->first);
    //bottomMap.erase(deletSegBottom.begin()->first);

    trapezoidsMap.erase(delet.begin()->first);
    update();

    addPolygon(topLeft, upfirst, downfirst, bottomLeft, t.getLeftP(), p1, cg3::Segment2d(topLeft, upfirst), cg3::Segment2d(bottomLeft, downfirst)); //A

    addPolygon(upfirst, upsecond, p2, p1, p1, p2, cg3::Segment2d(upfirst, upsecond), cg3::Segment2d(p1, p2)); //B

    addPolygon(p1, p2, downsecond, downfirst, p1, p2, cg3::Segment2d(p1, p2), cg3::Segment2d(downfirst, downsecond)); //C

    addPolygon(upsecond, topRight, bottomRight, downsecond, p2, t.getRightP(), cg3::Segment2d(upsecond, topRight), cg3::Segment2d(downsecond, bottomRight)); //D
}

/**
 * @brief Method to update the trapezoidal map in the case of multiple trapezoid insertion
 * @param[in] segment Segment, vector of Trapezoid: foundTrapezoids
 */
void TrapezoidalMap::multipleTrapezoid(const cg3::Segment2d& segment, std::vector<trapezoid> foundTrapezoids){
    std::map<trapezoid, size_t> delet;

    //std::map<cg3::Segment2d, size_t> deletSegTop, deletSegBottom;

    //std::list<cg3::Point2d> deletPoint;

    cg3::Point2d p1 = segment.p1();
    cg3::Point2d p2 = segment.p2();

    cg3::Point2d upfirst, upsecond, downfirst, downsecond;

    cg3::Point2d rightPSeg, leftPSeg;
    cg3::Point2d midpointR, midpointL, tmp, tmpL;

    int countNonBoxUp=0, countNonBoxDown=0;

    bool firstTrap=false;

    for (int i=0; i<(int)foundTrapezoids.size(); i++){
        trapezoid trap = foundTrapezoids[i];

        bool found;

        delet.clear();
        /*
        deletSegTop.clear();
        deletSegBottom.clear();
        deletPoint.clear();
        */

        topLeft = trap.getTrapezoid()[0];
        bottomLeft = trap.getTrapezoid()[3];
        topRight = trap.getTrapezoid()[1];
        bottomRight = trap.getTrapezoid()[2];

        delet = findTrapezoid(trap, found);
        //deletSegTop.insert(std::make_pair(cg3::Segment2d(delet.begin()->first[0], delet.begin()->first[1]), delet.begin()->second));
        //deletSegBottom.insert(std::make_pair(cg3::Segment2d(delet.begin()->first[3], delet.begin()->first[2]), delet.begin()->second));

        /*
        std::list<cg3::Point2d>::iterator itR = rightpList.begin();
        std::list<cg3::Point2d>::iterator itL = leftpList.begin();

        while(itR != rightpList.end()){
            if (itR->x() == topRight.x()){
                break;
            }
            else{
                itR++;
            }
        }
        while(itL != leftpList.end()){
            if (itL->x() == topLeft.x()){
                break;
            }
            else{
                itL++;
            }
        }
        */


        trapezoidsMap.erase(delet.begin()->first);
        update();

        /*
        deletPoint.clear();
        deletPoint.insert(deletPoint.begin(), *itL);
        leftpList.erase(deletPoint.begin());
        deletPoint.clear();
        deletPoint.insert(deletPoint.begin(), *itR);
        rightpList.erase(deletPoint.begin());
        topMap.erase(deletSegTop.begin()->first);
        bottomMap.erase(deletSegBottom.begin()->first);
        */

        if(foundTrapezoids.size() == 2){
            if(i==0){
                midpointR = trap.getRightP();
                upfirst=GasAlgorithms::createPoint(upfirst, p1.x(), GasAlgorithms::handleSlopeSegment(trap.getTop(), p1));
                downfirst=GasAlgorithms::createPoint(downfirst, p1.x(), GasAlgorithms::handleSlopeSegment(trap.getBottom(), p1));
                addPolygon(topLeft, upfirst, downfirst, bottomLeft, trap.getLeftP(), p1, cg3::Segment2d(topLeft, upfirst), cg3::Segment2d(bottomLeft, downfirst));
            }
            else if(i == (int)foundTrapezoids.size()-1){
                upsecond=GasAlgorithms::createPoint(upsecond, p2.x(), GasAlgorithms::handleSlopeSegment(trap.getTop(), p2));
                downsecond=GasAlgorithms::createPoint(downsecond, p2.x(), GasAlgorithms::handleSlopeSegment(trap.getBottom(), p2));
                leftPSeg = GasAlgorithms::createPoint(leftPSeg, trap.getLeftP().x(), GasAlgorithms::handleSlopeSegment(segment, trap.getLeftP()));
                addPolygon(upsecond, topRight, bottomRight, downsecond, p2, trap.getRightP(), cg3::Segment2d(upsecond, topRight), cg3::Segment2d(downsecond, bottomRight));

                if(dag.getDag()->determinant(segment, trap.getLeftP()) < 0){
                    addPolygon(leftPSeg, p2, downsecond, trap.getTrapezoid()[3], trap.getLeftP(), p2, cg3::Segment2d(leftPSeg, p2), cg3::Segment2d(trap.getTrapezoid()[3], downsecond));
                }
                else{
                    addPolygon(trap.getTrapezoid()[0], upsecond, p2, leftPSeg, trap.getLeftP(), p2, cg3::Segment2d(trap.getTrapezoid()[0], upsecond), cg3::Segment2d(leftPSeg, p2));
                }
            }
            if(p2.x() > trap.getRightP().x()){
                if(dag.getDag()->determinant(segment, trap.getRightP()) < 0){
                    rightPSeg = GasAlgorithms::createPoint(rightPSeg, trap.getRightP().x(), GasAlgorithms::handleSlopeSegment(segment, trap.getRightP()));
                    upsecond=GasAlgorithms::createPoint(upsecond, p2.x(), GasAlgorithms::handleSlopeSegment(trap.getTop(), p2));
                    addPolygon(upfirst, upsecond, p2, p1, p1, p2, cg3::Segment2d(upfirst, upsecond), cg3::Segment2d(p1, p2));
                    addPolygon(p1, rightPSeg, trap.getTrapezoid()[2], downfirst, p1, trap.getRightP(),cg3::Segment2d(p1, rightPSeg), cg3::Segment2d(downfirst, trap.getTrapezoid()[2]));
                }
                else{
                    rightPSeg = GasAlgorithms::createPoint(rightPSeg, trap.getRightP().x(), GasAlgorithms::handleSlopeSegment(segment, trap.getRightP()));
                    addPolygon(upfirst, trap.getTrapezoid()[1], rightPSeg, p1, p1, trap.getRightP(), cg3::Segment2d(upfirst, trap.getTrapezoid()[1]), cg3::Segment2d(p1, rightPSeg));
                    downsecond=GasAlgorithms::createPoint(downsecond, p2.x(), GasAlgorithms::handleSlopeSegment(trap.getBottom(), p2));
                    addPolygon(p1, p2, downsecond, downfirst, p1, p2, cg3::Segment2d(p1, p2), cg3::Segment2d(downfirst, downsecond));
                }
            }
        }
        else{ //foundTrapezoids > 2
            if(i==0){
                firstTrap=true;
                upfirst=GasAlgorithms::createPoint(upfirst, p1.x(), GasAlgorithms::handleSlopeSegment(trap.getTop(), p1));
                downfirst=GasAlgorithms::createPoint(downfirst, p1.x(), GasAlgorithms::handleSlopeSegment(trap.getBottom(), p1));
                addPolygon(topLeft, upfirst, downfirst, bottomLeft, topLeft, p1, cg3::Segment2d(topLeft, upfirst), cg3::Segment2d(bottomLeft, downfirst));
            }
            else if(i == (int)foundTrapezoids.size()-1){
                upsecond=GasAlgorithms::createPoint(upsecond, p2.x(), GasAlgorithms::handleSlopeSegment(trap.getTop(), p2));
                downsecond=GasAlgorithms::createPoint(downsecond, p2.x(), GasAlgorithms::handleSlopeSegment(trap.getBottom(), p2));
                addPolygon(upsecond, topRight, bottomRight, downsecond, p2, topRight, cg3::Segment2d(upsecond, topRight), cg3::Segment2d(downsecond, bottomRight));

                if(dag.getDag()->determinant(segment, trap.getLeftP()) < 0){ //sopra
                    leftPSeg = GasAlgorithms::createPoint(leftPSeg, trap.getLeftP().x(), GasAlgorithms::handleSlopeSegment(segment, trap.getLeftP()));
                    addPolygon(leftPSeg, p2, downsecond, trap.getTrapezoid()[3], trap.getLeftP(), p2, cg3::Segment2d(leftPSeg, p2), cg3::Segment2d(trap.getTrapezoid()[3], downsecond));
                    if(countNonBoxUp > 0){

                    }
                    else{
                        addPolygon(upfirst, upsecond, p2, p1, p1, p2, cg3::Segment2d(upfirst, upsecond), cg3::Segment2d(p1, p2));
                    }
                }
                else if(dag.getDag()->determinant(segment, trap.getLeftP()) > 0){
                    leftPSeg = GasAlgorithms::createPoint(leftPSeg, trap.getLeftP().x(), GasAlgorithms::handleSlopeSegment(segment, trap.getLeftP()));
                    addPolygon(trap.getTrapezoid()[0], upsecond, p2, leftPSeg, trap.getLeftP(), p2, cg3::Segment2d(trap.getTrapezoid()[0], upsecond), cg3::Segment2d(leftPSeg, p2));
                    if(countNonBoxDown > 0){

                    }
                    else{
                        addPolygon(p1,p2, downsecond, downfirst, p1, p2, cg3::Segment2d(p1, p2), cg3::Segment2d(downfirst, downsecond));
                    }
                }
            }

            if(p2.x() > trap.getRightP().x()){
                if(trap.getTop().p1().y() != (1e+6)){
                    countNonBoxUp++;
                }
                if(trap.getBottom().p1().y() != -(1e+6)){
                    countNonBoxDown++;
                }
                /*
                if(dag.getDag()->determinant(segment, trap.getRightP()) < 0 ){ //sopra
                    upsecond=GasAlgorithms::createPoint(upsecond, p2.x(), GasAlgorithms::handleSlopeSegment(trap.getTop(), p2));
                    rightPSeg = GasAlgorithms::createPoint(rightPSeg, trap.getRightP().x(), GasAlgorithms::handleSlopeSegment(segment, trap.getRightP()));
                    downfirst=GasAlgorithms::createPoint(downfirst, p1.x(), GasAlgorithms::handleSlopeSegment(trap.getBottom(), p1));
                    midpointL= trap.getLeftP();
                    if(dag.getDag()->determinant(segment, trap.getLeftP()) < 0){
                        if(p1.x() < trap.getLeftP().x() && p2.x() > trap.getRightP().x()){
                            leftPSeg = GasAlgorithms::createPoint(leftPSeg, trap.getLeftP().x(), GasAlgorithms::handleSlopeSegment(segment, trap.getLeftP()));
                            rightPSeg = GasAlgorithms::createPoint(rightPSeg, trap.getRightP().x(),GasAlgorithms:: handleSlopeSegment(segment, trap.getRightP()));
                            addPolygon(leftPSeg, rightPSeg, trap.getTrapezoid()[2], trap.getTrapezoid()[3], trap.getTrapezoid()[3], trap.getTrapezoid()[2], cg3::Segment2d(leftPSeg, rightPSeg), cg3::Segment2d(trap.getTrapezoid()[3], trap.getTrapezoid()[2]));
                        }

                        if(countNonBoxUp==1 && countNonBoxDown==1){
                            rightPSeg = GasAlgorithms::createPoint(rightPSeg, trap.getRightP().x(), GasAlgorithms::handleSlopeSegment(segment, trap.getRightP()));
                            downfirst=GasAlgorithms::createPoint(downfirst, p1.x(), GasAlgorithms::handleSlopeSegment(trap.getBottom(), p1));
                            addPolygon(p1, rightPSeg, trap.getRightP(), downfirst, p1, trap.getRightP(), cg3::Segment2d(p1, rightPSeg), cg3::Segment2d(downfirst, trap.getRightP()));

                        }

                        if(countNonBoxUp==1 && countNonBoxDown==0){
                            rightPSeg = GasAlgorithms::createPoint(rightPSeg, trap.getRightP().x(), GasAlgorithms::handleSlopeSegment(segment, trap.getRightP()));
                            downfirst=GasAlgorithms::createPoint(downfirst, p1.x(), GasAlgorithms::handleSlopeSegment(trap.getBottom(), p1));
                            addPolygon(p1, rightPSeg, trap.getTrapezoid()[2], downfirst, p1, trap.getRightP(), cg3::Segment2d(p1, rightPSeg), cg3::Segment2d(downfirst,  trap.getTrapezoid()[2]));
                        }

                    }
                    else{
                        if((p1 <= trap.getTrapezoid()[1] && trap.getTrapezoid()[3].y() == -(1e+6)) || firstTrap){
                            firstTrap=false;
                            addPolygon(p1, rightPSeg, trap.getTrapezoid()[2], downfirst, p1, trap.getRightP(), cg3::Segment2d(p1, rightPSeg), cg3::Segment2d(downfirst, trap.getTrapezoid()[2]));
                            tmp = trap.getRightP();
                        }
                        else if(trap.getTrapezoid()[2].y() != -(1e+6) && trap.getTrapezoid()[1].y() == (1e+6)){
                            rightPSeg = GasAlgorithms::createPoint(rightPSeg, trap.getRightP().x(), GasAlgorithms::handleSlopeSegment(segment, trap.getRightP()));
                            leftPSeg = GasAlgorithms::createPoint(leftPSeg, tmp.x(), GasAlgorithms::handleSlopeSegment(segment, tmp));
                            addPolygon(leftPSeg, rightPSeg, trap.getRightP(), tmp, tmp, trap.getRightP(), cg3::Segment2d(leftPSeg, rightPSeg), cg3::Segment2d(trap.getRightP(), tmp));
                        }
                        else{
                            leftPSeg = GasAlgorithms::createPoint(leftPSeg, tmp.x(), GasAlgorithms::handleSlopeSegment(segment, tmp));
                            addPolygon(leftPSeg, rightPSeg, trap.getRightP(), tmp, tmp, trap.getRightP(), cg3::Segment2d(leftPSeg, rightPSeg), cg3::Segment2d(trap.getRightP(), tmp));
                        }

                        if(countNonBoxUp >=1 && trap.getTrapezoid()[0].y() == (1e+6)){
                            upsecond=GasAlgorithms::createPoint(upsecond, p2.x(), GasAlgorithms::handleSlopeSegment(trap.getTop(), p2));
                            leftPSeg = GasAlgorithms::createPoint(leftPSeg, trap.getLeftP().x(), GasAlgorithms::handleSlopeSegment(segment, trap.getLeftP()));
                            addPolygon(trap.getTrapezoid()[0], upsecond, p2, leftPSeg, trap.getLeftP(), p2, cg3::Segment2d(trap.getTrapezoid()[0], upsecond), cg3::Segment2d(leftPSeg, p2));
                        }
                        else if(countNonBoxUp == 1 && trap.getTrapezoid()[0].y() != (1e+6)){
                            if(foundTrapezoids[(int)foundTrapezoids.size()-1].getTrapezoid()[0].y() != (1e+6)){
                                upsecond=GasAlgorithms::createPoint(upsecond, p2.x(), GasAlgorithms::handleSlopeSegment(trap.getTop(), p2));
                                leftPSeg = GasAlgorithms::createPoint(leftPSeg, trap.getLeftP().x(), GasAlgorithms::handleSlopeSegment(segment, trap.getLeftP()));
                                addPolygon(trap.getLeftP(), upsecond, p2, leftPSeg, trap.getLeftP(), p2, cg3::Segment2d(trap.getLeftP(), upsecond), cg3::Segment2d(leftPSeg, p2));
                            }
                            else{
                                midpointR = trap.getLeftP();
                                tmpL= trap.getLeftP();
                            }

                        }

                    }
                }
                else if (dag.getDag()->determinant(segment, trap.getRightP()) > 0 && dag.getDag()->determinant(segment, trap.getLeftP()) > 0){ //sotto
                    rightPSeg = GasAlgorithms::createPoint(rightPSeg, trap.getRightP().x(), GasAlgorithms::handleSlopeSegment(segment, trap.getRightP()));
                    leftPSeg = GasAlgorithms::createPoint(leftPSeg, trap.getLeftP().x(), GasAlgorithms::handleSlopeSegment(segment, trap.getLeftP()));
                    downsecond=GasAlgorithms::createPoint(downsecond, p2.x(), GasAlgorithms::handleSlopeSegment(trap.getBottom(), p2));


                    if(p1.x() < trap.getLeftP().x() && p2.x() > trap.getRightP().x()){
                        addPolygon(trap.getTrapezoid()[0], trap.getTrapezoid()[1], rightPSeg, leftPSeg, trap.getLeftP(), trap.getRightP(), cg3::Segment2d(trap.getTrapezoid()[0], trap.getTrapezoid()[1]), cg3::Segment2d(leftPSeg, rightPSeg));
                    }
                    else{
                        addPolygon(upfirst, trap.getTrapezoid()[1], rightPSeg, p1, p1, trap.getRightP(), cg3::Segment2d(upfirst, trap.getTrapezoid()[1]), cg3::Segment2d(p1, rightPSeg));
                    }

                }
                else if(dag.getDag()->determinant(segment, trap.getRightP()) > 0 && dag.getDag()->determinant(segment, trap.getLeftP()) < 0){ //sotto r e sopra l
                    if(countNonBoxDown > 0 && foundTrapezoids[(foundTrapezoids.size()-1)].getTrapezoid()[3].y() != -(1e+6)){
                        leftPSeg = GasAlgorithms::createPoint(leftPSeg, trap.getLeftP().x(), GasAlgorithms::handleSlopeSegment(segment, trap.getLeftP()));
                        downsecond=GasAlgorithms::createPoint(downsecond, p2.x(), GasAlgorithms::handleSlopeSegment(trap.getBottom(), p2));
                        addPolygon(leftPSeg, p2, downsecond, trap.getTrapezoid()[3], trap.getLeftP(), p2, cg3::Segment2d(leftPSeg, p2), cg3::Segment2d(trap.getTrapezoid()[3], downsecond));
                    }

                    if(countNonBoxDown == 1 && countNonBoxUp==0){
                        rightPSeg = GasAlgorithms::createPoint(rightPSeg, trap.getRightP().x(), GasAlgorithms::handleSlopeSegment(segment, trap.getRightP()));
                        addPolygon(upfirst, trap.getTrapezoid()[1], rightPSeg, p1, p1, trap.getRightP(), cg3::Segment2d(upfirst, trap.getTrapezoid()[1]), cg3::Segment2d(p1, rightPSeg));
                    }
                    if(countNonBoxUp >= 1 && trap.getTrapezoid()[0].y() != (1e+6)){
                        if(countNonBoxUp==1 || countNonBoxUp > 2){

                            if(countNonBoxUp > 2  && trap.getTrapezoid()[2].y() == -(1e+6) && upfirst.y() != (1e+6)){
                                upfirst=GasAlgorithms::createPoint(upfirst, p1.x(), GasAlgorithms::handleSlopeSegment(trap.getTop(), p1));
                                rightPSeg = GasAlgorithms::createPoint(rightPSeg, trap.getRightP().x(), GasAlgorithms::handleSlopeSegment(segment, trap.getRightP()));
                                addPolygon(upfirst, trap.getRightP(), rightPSeg, p1, p1, trap.getRightP(), cg3::Segment2d(upfirst, trap.getRightP()), cg3::Segment2d(p1, rightPSeg));
                            }
                            else{
                                leftPSeg = GasAlgorithms::createPoint(leftPSeg, midpointR.x(), GasAlgorithms::handleSlopeSegment(segment, midpointR));
                                rightPSeg = GasAlgorithms::createPoint(rightPSeg, trap.getRightP().x(), GasAlgorithms::handleSlopeSegment(segment, trap.getRightP()));
                                addPolygon(midpointR, trap.getRightP() ,rightPSeg, leftPSeg,  midpointR, trap.getRightP(), cg3::Segment2d(midpointR, trap.getRightP()), cg3::Segment2d(leftPSeg, rightPSeg));
                            }
                        }

                        if(countNonBoxUp==2 && countNonBoxDown>1){
                            leftPSeg = GasAlgorithms::createPoint(leftPSeg, tmpL.x(), GasAlgorithms::handleSlopeSegment(segment, tmpL));
                            rightPSeg = GasAlgorithms::createPoint(rightPSeg, trap.getRightP().x(), GasAlgorithms::handleSlopeSegment(segment, trap.getRightP()));
                            addPolygon(tmpL, trap.getRightP(), rightPSeg, leftPSeg, tmpL, trap.getRightP(), cg3::Segment2d(tmpL, trap.getRightP()), cg3::Segment2d(leftPSeg, rightPSeg));
                        }
                        else if(countNonBoxUp==2 && countNonBoxDown<=1){
                            rightPSeg = GasAlgorithms::createPoint(rightPSeg, trap.getRightP().x(), GasAlgorithms::handleSlopeSegment(segment, trap.getRightP()));
                            leftPSeg = GasAlgorithms::createPoint(leftPSeg, tmpL.x(), GasAlgorithms::handleSlopeSegment(segment, tmpL));
                            addPolygon(tmpL ,trap.getRightP(), rightPSeg, leftPSeg, tmpL, trap.getRightP(), cg3::Segment2d(tmpL, trap.getRightP()), cg3::Segment2d(leftPSeg, rightPSeg));
                        }

                    }
                    if(countNonBoxUp >= 1 && trap.getTrapezoid()[2].y() == -(1e+6)){
                        downsecond=GasAlgorithms::createPoint(downsecond, p2.x(), GasAlgorithms::handleSlopeSegment(trap.getBottom(), p2));
                        leftPSeg = GasAlgorithms::createPoint(leftPSeg, trap.getLeftP().x(), GasAlgorithms::handleSlopeSegment(segment, trap.getLeftP()));
                        addPolygon(leftPSeg, p2, downsecond, trap.getTrapezoid()[3], trap.getLeftP(), p2, cg3::Segment2d(leftPSeg, p2), cg3::Segment2d(trap.getTrapezoid()[3], downsecond));
                    }
                }*/
            }
        }
    }
}

/*
  @brief Method to update the neighbors of each trapezoid
  @param[in] Segment top and bottom, Segment left and right (pleft, pright)

void TrapezoidalMap::updateNeighbors(cg3::Segment2d top, cg3::Segment2d bottom, cg3::Point2d left, cg3::Point2d right){
    size_t id2 = std::numeric_limits<size_t>::max();
    topMap.insert(std::make_pair(cg3::Segment2d(top), id2));
    bottomMap.insert(std::make_pair(cg3::Segment2d(bottom), id2));
    leftpList.push_back(left);
    rightpList.push_back(right);
}
*/


/*
 * @brief Method to add polygon in the structures of trapezoidal map
 * @param[in] 4 points Point for the new polygon
 */
void TrapezoidalMap::addPolygon(const cg3::Point2d p1, const cg3::Point2d p2, const cg3::Point2d p3, const cg3::Point2d p4,
                    const cg3::Point2d leftp, const cg3::Point2d rightp, const cg3::Segment2d top, const cg3::Segment2d bottom){
    size_t id;

    trapezoid trapez = trapezoid(p1, p2, p3, p4, leftp, rightp, top, bottom);

    id = std::numeric_limits<size_t>::max();

    trapezoidsMap.insert(std::make_pair(trapez, id));

    trapezoids.push_back(trapez);

    dag.setTrapezoidToInsert(trapez, trapezoids.size());
}


/**
 * @brief Method to find trapezoid in the map
 * @param[in] t Trapezoid, found bool
 * @param[out] map of <Trapezoid, unsigned_int>
 */
std::map<trapezoid, size_t> TrapezoidalMap::findTrapezoid(const trapezoid t, bool& found){
    found = false;
    return findIndexedTrapezoid(trapezoid(t), found);
}

/**
 * @brief Method to find trapezoid in the map by index in constant time
 * @param[in] indexedTrapezoid Trapezoid, found bool
 * @param[out] map of <Trapezoid, unsigned_int>
 */
std::map<trapezoid, size_t> TrapezoidalMap::findIndexedTrapezoid(const trapezoid& indexedTrapezoid, bool& found){
    std::map<trapezoid, size_t>::iterator it = trapezoidsMap.find(indexedTrapezoid);

    std::map<trapezoid, size_t> map;

    if (it != trapezoidsMap.end()) {
        found = true;
        map.insert(std::make_pair(it->first, it->second));
    }
    return map;
}

/**
 * @brief Method to return the trapezoids
 * @param[out] vector of trapezoids
 */
std::vector<trapezoid> TrapezoidalMap::getTrapezoids() const{
    return trapezoids;
}

/**
 * @brief Method to update the trapezoids
 */
void TrapezoidalMap::update(){
    trapezoids.clear();
    for (std::map<trapezoid, size_t>::iterator it=trapezoidsMap.begin(); it!=trapezoidsMap.end(); ++it){
        trapezoids.push_back(it->first);
    }
}

/**
 * @brief Method to set the trapezoid after the query point: Point Location Query
 * @param[in] point Point
 */
void TrapezoidalMap::queryPoint(cg3::Point2d point){
    queryPointTrap = dag.findTrapezoid(point, true, dag.getDag()).getTrapezoid();
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
    trapezoids.clear();
    trapezoidsMap.clear();
}


