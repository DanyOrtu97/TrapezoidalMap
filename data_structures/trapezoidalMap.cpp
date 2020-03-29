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

    upfirst = GasAlgorithms::createPoint(upfirst, p1.x(), GasAlgorithms::handleSlopeSegment(topS, p1));
    upsecond = GasAlgorithms::createPoint(upsecond, p2.x(), GasAlgorithms::handleSlopeSegment(topS, p2));

    downfirst = GasAlgorithms::createPoint(downfirst, p1.x(), GasAlgorithms::handleSlopeSegment(bottomS, p1));
    downsecond = GasAlgorithms::createPoint(downsecond, p2.x(), GasAlgorithms::handleSlopeSegment(bottomS, p2));

    trapezoids.remove(t);

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
    cg3::Point2d p1 = segment.p1();
    cg3::Point2d p2 = segment.p2();

    cg3::Point2d upfirst, upsecond, downfirst, downsecond;

    cg3::Point2d rightPSeg, leftPSeg;
    cg3::Point2d midpointR, midpointL, tmp, tmpL;

    int countNonBoxUp=0, countNonBoxDown=0;

    bool firstTrap=false;

    upfirst=GasAlgorithms::createPoint(upfirst, p1.x(), GasAlgorithms::handleSlopeSegment(foundTrapezoids[0].getTop(), p1));
    downfirst=GasAlgorithms::createPoint(downfirst, p1.x(), GasAlgorithms::handleSlopeSegment(foundTrapezoids[0].getBottom(), p1));
    upsecond=GasAlgorithms::createPoint(upsecond, p2.x(), GasAlgorithms::handleSlopeSegment(foundTrapezoids[foundTrapezoids.size()-1].getTop(), p2));
    downsecond=GasAlgorithms::createPoint(downsecond, p2.x(), GasAlgorithms::handleSlopeSegment(foundTrapezoids[foundTrapezoids.size()-1].getBottom(), p2));

    for (int i=0; i<(int)foundTrapezoids.size(); i++){
        trapezoid trap = foundTrapezoids[i];

        topLeft = trap.getTrapezoid()[0];
        bottomLeft = trap.getTrapezoid()[3];
        topRight = trap.getTrapezoid()[1];
        bottomRight = trap.getTrapezoid()[2];


        trapezoids.remove(trap);


        leftPSeg = GasAlgorithms::createPoint(leftPSeg, trap.getLeftP().x(), GasAlgorithms::handleSlopeSegment(segment, trap.getLeftP()));
        rightPSeg = GasAlgorithms::createPoint(rightPSeg, trap.getRightP().x(), GasAlgorithms::handleSlopeSegment(segment, trap.getRightP()));

        if(foundTrapezoids.size() == 2){
            if(i==0){
                midpointR = trap.getRightP();
                addPolygon(topLeft, upfirst, downfirst, bottomLeft, trap.getLeftP(), p1, cg3::Segment2d(topLeft, upfirst), cg3::Segment2d(bottomLeft, downfirst));
            }
            else if(i == (int)foundTrapezoids.size()-1){                 
                if(dag.getDag()->determinant(segment, trap.getLeftP()) < 0){
                    addPolygon(leftPSeg, p2, downsecond, trap.getTrapezoid()[3], trap.getLeftP(), p2, cg3::Segment2d(leftPSeg, p2), cg3::Segment2d(trap.getTrapezoid()[3], downsecond));
                }
                else{
                    addPolygon(trap.getTrapezoid()[0], upsecond, p2, leftPSeg, trap.getLeftP(), p2, cg3::Segment2d(trap.getTrapezoid()[0], upsecond), cg3::Segment2d(leftPSeg, p2));
                }
                addPolygon(upsecond, topRight, bottomRight, downsecond, p2, trap.getRightP(), cg3::Segment2d(upsecond, topRight), cg3::Segment2d(downsecond, bottomRight));
            }
            if(p2.x() > trap.getRightP().x()){
                if(dag.getDag()->determinant(segment, trap.getRightP()) < 0){
                    addPolygon(upfirst, upsecond, p2, p1, p1, p2, cg3::Segment2d(upfirst, upsecond), cg3::Segment2d(p1, p2));
                    addPolygon(p1, rightPSeg, trap.getTrapezoid()[2], downfirst, p1, trap.getRightP(),cg3::Segment2d(p1, rightPSeg), cg3::Segment2d(downfirst, trap.getTrapezoid()[2]));
                }
                else{
                    addPolygon(upfirst, trap.getTrapezoid()[1], rightPSeg, p1, p1, trap.getRightP(), cg3::Segment2d(upfirst, trap.getTrapezoid()[1]), cg3::Segment2d(p1, rightPSeg));
                    addPolygon(p1, p2, downsecond, downfirst, p1, p2, cg3::Segment2d(p1, p2), cg3::Segment2d(downfirst, downsecond));
                }
            }
        }
        else{ //foundTrapezoids > 2
            if(i==0){
                firstTrap=true;
                addPolygon(topLeft, upfirst, downfirst, bottomLeft, topLeft, p1, cg3::Segment2d(topLeft, upfirst), cg3::Segment2d(bottomLeft, downfirst));
            }
            else if(i == (int)foundTrapezoids.size()-1){
                if(dag.getDag()->determinant(segment, trap.getLeftP()) < 0){ //sopra
                    if(countNonBoxUp == 0){
                        addPolygon(upfirst, upsecond, p2, p1, p1, p2, cg3::Segment2d(upfirst, upsecond), cg3::Segment2d(p1, p2));
                    }
                    addPolygon(leftPSeg, p2, downsecond, trap.getTrapezoid()[3], trap.getLeftP(), p2, cg3::Segment2d(leftPSeg, p2), cg3::Segment2d(trap.getTrapezoid()[3], downsecond));
                }
                else if(dag.getDag()->determinant(segment, trap.getLeftP()) > 0){
                    if(countNonBoxDown == 0){
                        addPolygon(p1,p2, downsecond, downfirst, p1, p2, cg3::Segment2d(p1, p2), cg3::Segment2d(downfirst, downsecond));
                    }
                    addPolygon(trap.getTrapezoid()[0], upsecond, p2, leftPSeg, trap.getLeftP(), p2, cg3::Segment2d(trap.getTrapezoid()[0], upsecond), cg3::Segment2d(leftPSeg, p2));
                }

                addPolygon(upsecond, topRight, bottomRight, downsecond, p2, topRight, cg3::Segment2d(upsecond, topRight), cg3::Segment2d(downsecond, bottomRight));

            }

            if(p2.x() > trap.getRightP().x()){
                if(trap.getTop().p1().y() != (1e+6)){
                    countNonBoxUp++;
                }
                if(trap.getBottom().p1().y() != -(1e+6)){
                    countNonBoxDown++;
                }

                if(dag.getDag()->determinant(segment, trap.getRightP()) < 0 ){ //sopra
                    if(dag.getDag()->determinant(segment, trap.getLeftP()) < 0){

                        if(p1.x() < trap.getLeftP().x() && p2.x() > trap.getRightP().x()){
                            addPolygon(leftPSeg, rightPSeg, trap.getTrapezoid()[2], trap.getTrapezoid()[3], trap.getTrapezoid()[3], trap.getTrapezoid()[2], cg3::Segment2d(leftPSeg, rightPSeg), cg3::Segment2d(trap.getTrapezoid()[3], trap.getTrapezoid()[2]));
                        }

                        if(countNonBoxUp==1 && countNonBoxDown==1){
                            addPolygon(p1, rightPSeg, trap.getRightP(), downfirst, p1, trap.getRightP(), cg3::Segment2d(p1, rightPSeg), cg3::Segment2d(downfirst, trap.getRightP()));
                        }

                        if(countNonBoxUp==1 && countNonBoxDown==0){
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
                            leftPSeg = GasAlgorithms::createPoint(leftPSeg, tmp.x(), GasAlgorithms::handleSlopeSegment(segment, tmp));
                            addPolygon(leftPSeg, rightPSeg, trap.getRightP(), tmp, tmp, trap.getRightP(), cg3::Segment2d(leftPSeg, rightPSeg), cg3::Segment2d(trap.getRightP(), tmp));
                        }
                        else{
                            leftPSeg = GasAlgorithms::createPoint(leftPSeg, tmp.x(), GasAlgorithms::handleSlopeSegment(segment, tmp));
                            addPolygon(leftPSeg, rightPSeg, trap.getRightP(), tmp, tmp, trap.getRightP(), cg3::Segment2d(leftPSeg, rightPSeg), cg3::Segment2d(trap.getRightP(), tmp));
                        }

                        if(countNonBoxUp >=1 && trap.getTrapezoid()[0].y() == (1e+6)){
                            leftPSeg = GasAlgorithms::createPoint(leftPSeg, trap.getLeftP().x(), GasAlgorithms::handleSlopeSegment(segment, trap.getLeftP()));
                            addPolygon(trap.getTrapezoid()[0], upsecond, p2, leftPSeg, trap.getLeftP(), p2, cg3::Segment2d(trap.getTrapezoid()[0], upsecond), cg3::Segment2d(leftPSeg, p2));
                        }
                        else if(countNonBoxUp == 1 && trap.getTrapezoid()[0].y() != (1e+6)){
                            if(foundTrapezoids[(int)foundTrapezoids.size()-1].getTrapezoid()[0].y() != (1e+6)){
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
                    if(p1.x() < trap.getLeftP().x() && p2.x() > trap.getRightP().x()){
                        addPolygon(trap.getTrapezoid()[0], trap.getTrapezoid()[1], rightPSeg, leftPSeg, trap.getLeftP(), trap.getRightP(), cg3::Segment2d(trap.getTrapezoid()[0], trap.getTrapezoid()[1]), cg3::Segment2d(leftPSeg, rightPSeg));
                    }
                    else{
                        addPolygon(upfirst, trap.getTrapezoid()[1], rightPSeg, p1, p1, trap.getRightP(), cg3::Segment2d(upfirst, trap.getTrapezoid()[1]), cg3::Segment2d(p1, rightPSeg));
                    }

                }
                else if(dag.getDag()->determinant(segment, trap.getRightP()) > 0 && dag.getDag()->determinant(segment, trap.getLeftP()) < 0){ //sotto r e sopra l
                    if(countNonBoxDown > 0 && foundTrapezoids[(foundTrapezoids.size()-1)].getTrapezoid()[3].y() != -(1e+6)){
                       addPolygon(leftPSeg, p2, downsecond, trap.getTrapezoid()[3], trap.getLeftP(), p2, cg3::Segment2d(leftPSeg, p2), cg3::Segment2d(trap.getTrapezoid()[3], downsecond));
                    }

                    if(countNonBoxDown == 1 && countNonBoxUp==0){
                        addPolygon(upfirst, trap.getTrapezoid()[1], rightPSeg, p1, p1, trap.getRightP(), cg3::Segment2d(upfirst, trap.getTrapezoid()[1]), cg3::Segment2d(p1, rightPSeg));
                    }
                    if(countNonBoxUp >= 1 && trap.getTrapezoid()[0].y() != (1e+6)){
                        if(countNonBoxUp==1 || countNonBoxUp > 2){
                            if(countNonBoxUp > 2  && trap.getTrapezoid()[2].y() == -(1e+6) && upfirst.y() != (1e+6)){
                                addPolygon(upfirst, trap.getRightP(), rightPSeg, p1, p1, trap.getRightP(), cg3::Segment2d(upfirst, trap.getRightP()), cg3::Segment2d(p1, rightPSeg));
                            }
                            else{
                                //addPolygon(midpointR, trap.getRightP() ,rightPSeg, leftPSeg,  midpointR, trap.getRightP(), cg3::Segment2d(midpointR, trap.getRightP()), cg3::Segment2d(leftPSeg, rightPSeg));
                            }
                        }

                        if(countNonBoxUp==2 && countNonBoxDown>1){
                            leftPSeg = GasAlgorithms::createPoint(leftPSeg, tmpL.x(), GasAlgorithms::handleSlopeSegment(segment, tmpL));
                            addPolygon(tmpL, trap.getRightP(), rightPSeg, leftPSeg, tmpL, trap.getRightP(), cg3::Segment2d(tmpL, trap.getRightP()), cg3::Segment2d(leftPSeg, rightPSeg));
                        }
                        else if(countNonBoxUp==2 && countNonBoxDown<=1){
                            leftPSeg = GasAlgorithms::createPoint(leftPSeg, tmpL.x(), GasAlgorithms::handleSlopeSegment(segment, tmpL));
                            addPolygon(tmpL ,trap.getRightP(), rightPSeg, leftPSeg, tmpL, trap.getRightP(), cg3::Segment2d(tmpL, trap.getRightP()), cg3::Segment2d(leftPSeg, rightPSeg));
                        }

                    }
                    if(countNonBoxUp >= 1 && trap.getTrapezoid()[2].y() == -(1e+6)){
                        addPolygon(leftPSeg, p2, downsecond, trap.getTrapezoid()[3], trap.getLeftP(), p2, cg3::Segment2d(leftPSeg, p2), cg3::Segment2d(trap.getTrapezoid()[3], downsecond));
                    }
                }
            }

        }
    }
}


/*
 * @brief Method to add polygon in the structures of trapezoidal map
 * @param[in] 4 points Point for the new polygon
 */
void TrapezoidalMap::addPolygon(const cg3::Point2d p1, const cg3::Point2d p2, const cg3::Point2d p3, const cg3::Point2d p4, const cg3::Point2d leftp, const cg3::Point2d rightp, const cg3::Segment2d top, const cg3::Segment2d bottom){

    trapezoids.push_back(trapezoid(p1, p2, p3, p4, leftp, rightp, top, bottom));

    dag.setTrapezoidToInsert(trapezoid(p1, p2, p3, p4, leftp, rightp, top, bottom), trapezoids.size());
}

/**
 * @brief Method to return the trapezoids
 * @param[out] vector of trapezoids
 */
std::list<trapezoid> TrapezoidalMap::getTrapezoids() const{
    return trapezoids;
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
}


