#include "trapezoidalMap.h"
#include "dag.h"
#include <drawables/drawableTrapezoidalMap.h>

cg3::Point2d topLeft;
cg3::Point2d bottomLeft;
cg3::Point2d topRight;
cg3::Point2d bottomRight;
Dag dag;

TrapezoidalMap::TrapezoidalMap() :
    boundingBox(cg3::Point2d(0,0),cg3::Point2d(0,0))
{
}

void TrapezoidalMap::inizialize(){

    topLeft = createPoint(topLeft, -(1e+6), (1e+6));
    bottomLeft = createPoint(bottomLeft, -(1e+6), -(1e+6));
    topRight = createPoint(topRight, (1e+6), (1e+6));
    bottomRight = createPoint(bottomRight, (1e+6), -(1e+6));

    cg3::Segment2d top;
    cg3::Segment2d bottom;
    cg3::Point2d leftp;
    cg3::Point2d rightp;

    //inizialize the trapezoidal map with the bounding box
    addPolygon(topLeft, topRight, bottomRight, bottomLeft);

    std::vector<std::array<cg3::Point2d, 4>> trapezoids;
    std::array<cg3::Point2d, 4> trapezoid = {topLeft, topRight, bottomRight, bottomLeft};

    size_t id2;

    leftp.set(trapezoid[0].x(), trapezoid[0].y());
    rightp.set(trapezoid[1].x(), trapezoid[1].y());
    top.set(trapezoid[0], trapezoid[1]);
    bottom.set(trapezoid[3],trapezoid[2]);

    id2 = std::numeric_limits<size_t>::max();

    topMap.insert(std::make_pair(cg3::Segment2d(top), id2));
    bottomMap.insert(std::make_pair(cg3::Segment2d(bottom), id2));
    leftpList.push_back(leftp);
    rightpList.push_back(rightp);

    //inizialize the dag with the pointer of the first polygon (the bounding box)
    trapezoids.push_back(trapezoid);
    dag.inizializeDag(trapezoids.front());

}


void TrapezoidalMap::trapezoidalMapAlgorithm(cg3::Segment2d segment){

    std::array<cg3::Point2d, 4> firstTrapezoid = dag.findTrapezoid(segment.p1(), false, dag.getDag());
    std::array<cg3::Point2d, 4> lastTrapezoid = dag.findTrapezoid(segment.p2(), false, dag.getDag());

    std::vector<std::array<cg3::Point2d, 4>> foundTrapezoids;

    if (firstTrapezoid == lastTrapezoid){   

        dag.clearTraps();

        CompletelyInsideTrapezoid(firstTrapezoid, segment);

        dag.updateDag(segment);

    }
    else{
        //split node
        nodeDag* splitNode= dag.findSplitNode(segment);
        dag.findTrapezoids(segment, splitNode);

        std::vector<std::array<cg3::Point2d, 4>> foundTrapezoids;

        std::map<std::array<cg3::Point2d, 4>, nodeDag*> TrapMapDag = dag.getPointerMap();

        for(std::map<std::array<cg3::Point2d, 4>, nodeDag*>::iterator it = TrapMapDag.begin(); it!=TrapMapDag.end(); it++){
            foundTrapezoids.push_back(it->first);
        }

        dag.clearTraps();

        multipleTrapezoid(segment, foundTrapezoids);

        dag.updateDag(segment);


    }

}


void TrapezoidalMap::CompletelyInsideTrapezoid(Trapezoid t, const cg3::Segment2d& segment){
    std::map<Trapezoid, size_t> delet;
    std::map<cg3::Segment2d, size_t> deletSegTop, deletSegBottom;
    std::list<cg3::Point2d> deletPoint;
    bool found;
    bool up=false;

    delet = findTrapezoid(t, found);
    deletSegTop.insert(std::make_pair(cg3::Segment2d(delet.begin()->first[0], delet.begin()->first[1]), delet.begin()->second));
    deletSegBottom.insert(std::make_pair(cg3::Segment2d(delet.begin()->first[3], delet.begin()->first[2]), delet.begin()->second));

    if(t == trapezoids[2]){
        up=false;
    }
    if(t == trapezoids[1]){
        up=true;
    }

    trapezoidsMap.erase(delet.begin()->first);
    update();

    topLeft = t[0];
    bottomLeft = t[3];
    topRight = t[1];
    bottomRight = t[2];

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

    std::map<cg3::Segment2d, size_t>::iterator top = topMap.find(topS);
    std::map<cg3::Segment2d, size_t>::iterator bottom = bottomMap.find(bottomS);


    if (top != topMap.end()) {
        upfirst = createPoint(upfirst, p1.x(), handleSlopeSegment(top->first, p1));
        upsecond = createPoint(upsecond, p2.x(), handleSlopeSegment(top->first, p2));
    }

    if (bottom != bottomMap.end()) {
        downfirst = createPoint(downfirst, p1.x(), handleSlopeSegment(bottom->first, p1));
        downsecond = createPoint(downsecond, p2.x(), handleSlopeSegment(bottom->first, p2));
    }

    deletPoint.clear();
    deletPoint.insert(deletPoint.begin(), delet.begin()->first[0]);
    leftpList.erase(deletPoint.begin());
    deletPoint.clear();
    deletPoint.insert(deletPoint.begin(), delet.begin()->first[1]);
    rightpList.erase(deletPoint.begin());
    topMap.erase(deletSegTop.begin()->first);
    bottomMap.erase(deletSegBottom.begin()->first);

    //AGGIUNTA
    addPolygon(topLeft, upfirst, downfirst, bottomLeft); //A
    if(up){
        updateNeighbors(cg3::Segment2d(topLeft, upfirst), cg3::Segment2d(bottomLeft, downfirst), bottomLeft, segment.p1());
    }
    else{
        updateNeighbors(cg3::Segment2d(topLeft, upfirst), cg3::Segment2d(bottomLeft, downfirst), topLeft, segment.p1());
    }

    addPolygon(upfirst, upsecond, p2, p1); //B
    updateNeighbors(cg3::Segment2d(upfirst, upsecond), cg3::Segment2d(p1, p2), segment.p1(), segment.p2());

    addPolygon(p1, p2, downsecond, downfirst); //C
    updateNeighbors(cg3::Segment2d(p1, p2), cg3::Segment2d(downfirst, downsecond), segment.p1(), segment.p2());

    addPolygon(upsecond, topRight, bottomRight, downsecond); //D
    if(up){
        updateNeighbors(cg3::Segment2d(upsecond, topRight), cg3::Segment2d(downsecond, bottomRight), segment.p2(), bottomRight);
    }
    else{
        updateNeighbors(cg3::Segment2d(upsecond, topRight), cg3::Segment2d(downsecond, bottomRight), segment.p2(), topRight);
    }


}

void TrapezoidalMap::multipleTrapezoid(const cg3::Segment2d& segment, std::vector<Trapezoid> foundTrapezoids){
    std::map<Trapezoid, size_t> delet;

    std::map<cg3::Segment2d, size_t> deletSegTop, deletSegBottom;

    std::list<cg3::Point2d> deletPoint;

    cg3::Point2d p1 = segment.p1();
    cg3::Point2d p2 = segment.p2();

    cg3::Point2d upfirst, upsecond, downfirst, downsecond;

    cg3::Point2d rightPSeg, leftPSeg;
    cg3::Point2d midpointR, midpointL;

    int countNonBoxUp=0, countNonBoxDown=0;

    for (int i=0; i<(int)foundTrapezoids.size(); i++){
        bool found;

        delet.clear();
        deletSegTop.clear();
        deletSegBottom.clear();
        deletPoint.clear();

        topLeft = foundTrapezoids[i][0];
        bottomLeft = foundTrapezoids[i][3];
        topRight = foundTrapezoids[i][1];
        bottomRight = foundTrapezoids[i][2];

        delet = findTrapezoid(foundTrapezoids[i], found);
        deletSegTop.insert(std::make_pair(cg3::Segment2d(delet.begin()->first[0], delet.begin()->first[1]), delet.begin()->second));
        deletSegBottom.insert(std::make_pair(cg3::Segment2d(delet.begin()->first[3], delet.begin()->first[2]), delet.begin()->second));

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


        trapezoidsMap.erase(delet.begin()->first);

        update();

        deletPoint.clear();
        deletPoint.insert(deletPoint.begin(), *itL);
        leftpList.erase(deletPoint.begin());
        deletPoint.clear();
        deletPoint.insert(deletPoint.begin(), *itR);
        rightpList.erase(deletPoint.begin());
        topMap.erase(deletSegTop.begin()->first);
        bottomMap.erase(deletSegBottom.begin()->first);


        if(foundTrapezoids.size() == 2){
            if(i==0){
                midpointR = *itR;
                upfirst=createPoint(upfirst, p1.x(), handleSlopeSegment(deletSegTop.begin()->first, p1));
                downfirst=createPoint(downfirst, p1.x(), handleSlopeSegment(deletSegBottom.begin()->first, p1));

                addPolygon(topLeft, upfirst, downfirst, bottomLeft);
                updateNeighbors(cg3::Segment2d(topLeft, upfirst), cg3::Segment2d(bottomLeft, downfirst), topLeft, p1);
            }
            else if(i == (int)foundTrapezoids.size()-1){
                upsecond=createPoint(upsecond, p2.x(), handleSlopeSegment(deletSegTop.begin()->first, p2));
                downsecond=createPoint(downsecond, p2.x(), handleSlopeSegment(deletSegBottom.begin()->first, p2));
                leftPSeg = createPoint(leftPSeg, itL->x(), handleSlopeSegment(segment, *itL));

                addPolygon(upsecond, topRight, bottomRight, downsecond);
                updateNeighbors(cg3::Segment2d(upsecond, topRight), cg3::Segment2d(downsecond, bottomRight), p2, topRight);

                if(dag.getDag()->determinant(segment, *itL) < 0){
                    addPolygon(leftPSeg, p2, downsecond, foundTrapezoids[i][3]);
                    updateNeighbors(cg3::Segment2d(leftPSeg, p2), cg3::Segment2d(foundTrapezoids[i][3], downsecond), *itL, p2);
                }
                else{
                    addPolygon(foundTrapezoids[i][0], upsecond, p2, leftPSeg);
                    updateNeighbors(cg3::Segment2d(foundTrapezoids[i][0], upsecond), cg3::Segment2d(leftPSeg, p2), *itL, p2);
                }
            }
            if(p2.x() > itR->x()){
                if(dag.getDag()->determinant(segment, *itR) < 0){
                    rightPSeg = createPoint(rightPSeg, itR->x(), handleSlopeSegment(segment, *itR));
                    upsecond=createPoint(upsecond, p2.x(), handleSlopeSegment(deletSegTop.begin()->first, p2));
                    addPolygon(upfirst, upsecond, p2, p1);
                    updateNeighbors(cg3::Segment2d(upfirst, upsecond), cg3::Segment2d(p1, p2), p1, p2);
                    addPolygon(p1, rightPSeg, foundTrapezoids[i][2], downfirst);
                    updateNeighbors(cg3::Segment2d(p1, rightPSeg), cg3::Segment2d(downfirst, foundTrapezoids[i][2]), p1, *itR);

                }
                else{
                    rightPSeg = createPoint(rightPSeg, itR->x(), handleSlopeSegment(segment, *itR));
                    addPolygon(upfirst, foundTrapezoids[i][1], rightPSeg, p1);
                    updateNeighbors(cg3::Segment2d(upfirst, foundTrapezoids[i][1]), cg3::Segment2d(p1, rightPSeg), p1, *itR);
                    downsecond=createPoint(downsecond, p2.x(), handleSlopeSegment(deletSegBottom.begin()->first, p2));
                    addPolygon(p1, p2, downsecond, downfirst);
                    updateNeighbors(cg3::Segment2d(p1, p2), cg3::Segment2d(downfirst, downsecond), p1, p2);
                }
            }
        }
        else{ //Trovati più di 2 trapezoidi
            if(i==0){
                upfirst=createPoint(upfirst, p1.x(), handleSlopeSegment(deletSegTop.begin()->first, p1));
                downfirst=createPoint(downfirst, p1.x(), handleSlopeSegment(deletSegBottom.begin()->first, p1));
                addPolygon(topLeft, upfirst, downfirst, bottomLeft);
                updateNeighbors(cg3::Segment2d(topLeft, upfirst), cg3::Segment2d(bottomLeft, downfirst), topLeft, p1);
            }
            else if(i == (int)foundTrapezoids.size()-1){
                upsecond=createPoint(upsecond, p2.x(), handleSlopeSegment(deletSegTop.begin()->first, p2));
                downsecond=createPoint(downsecond, p2.x(), handleSlopeSegment(deletSegBottom.begin()->first, p2));
                addPolygon(upsecond, topRight, bottomRight, downsecond);
                updateNeighbors(cg3::Segment2d(upsecond, topRight), cg3::Segment2d(downsecond, bottomRight), p2, topRight);

                if(dag.getDag()->determinant(segment, *itL) < 0){ //sopra
                    leftPSeg = createPoint(leftPSeg, itL->x(), handleSlopeSegment(segment, *itL));
                    addPolygon(leftPSeg, p2, downsecond, foundTrapezoids[i][3]);
                    updateNeighbors(cg3::Segment2d(leftPSeg, p2), cg3::Segment2d(foundTrapezoids[i][3], downsecond), *itL, p2);

                    if(countNonBoxUp > 0){

                    }
                    else{
                        addPolygon(upfirst, upsecond, p2, p1);
                        updateNeighbors(cg3::Segment2d(upfirst, upsecond), cg3::Segment2d(p1, p2), p1, p2);
                    }

                }
                else if(dag.getDag()->determinant(segment, *itL) > 0){
                    leftPSeg = createPoint(leftPSeg, itL->x(), handleSlopeSegment(segment, *itL));
                    addPolygon(foundTrapezoids[i][0], upsecond, p2, leftPSeg);
                    updateNeighbors(cg3::Segment2d(foundTrapezoids[i][0], upsecond), cg3::Segment2d(leftPSeg, p2), *itL, p2);

                    if(countNonBoxDown > 0){

                    }
                    else{
                        addPolygon(p1,p2, downsecond, downfirst);
                        updateNeighbors(cg3::Segment2d(p1, p2), cg3::Segment2d(downfirst, downsecond), p1, p2);
                    }

                }
            }

            if(p2.x() > itR->x()){
                if(deletSegTop.begin()->first.p1().y() != (1e+6)){
                    countNonBoxUp++;
                }
                if(deletSegBottom.begin()->first.p1().y() != -(1e+6)){
                    countNonBoxDown++;
                }

                if(dag.getDag()->determinant(segment, *itR) < 0 ){ //sopra
                    upsecond=createPoint(upsecond, p2.x(), handleSlopeSegment(deletSegTop.begin()->first, p2));
                    rightPSeg = createPoint(rightPSeg, itR->x(), handleSlopeSegment(segment, *itR));
                    downfirst=createPoint(downfirst, p1.x(), handleSlopeSegment(deletSegBottom.begin()->first, p1));
                    midpointL= *itL;
                    if(dag.getDag()->determinant(segment, *itL) < 0){
                        if(p1.x() < itL->x() && p2.x() > itR->x()){
                            leftPSeg = createPoint(leftPSeg, itL->x(), handleSlopeSegment(segment, *itL));
                            rightPSeg = createPoint(rightPSeg, itR->x(), handleSlopeSegment(segment, *itR));
                            addPolygon(leftPSeg, rightPSeg, foundTrapezoids[i][2], foundTrapezoids[i][3]);
                            updateNeighbors(cg3::Segment2d(leftPSeg, rightPSeg), cg3::Segment2d(foundTrapezoids[i][3], foundTrapezoids[i][2]), foundTrapezoids[i][3], foundTrapezoids[i][2]);

                        }

                        if(countNonBoxUp==1 && countNonBoxDown==1){
                            rightPSeg = createPoint(rightPSeg, itR->x(), handleSlopeSegment(segment, *itR));
                            downfirst=createPoint(downfirst, p1.x(), handleSlopeSegment(deletSegBottom.begin()->first, p1));
                            addPolygon(p1, rightPSeg, *itR, downfirst);
                            updateNeighbors(cg3::Segment2d(p1, rightPSeg), cg3::Segment2d(downfirst, *itR), p1, *itR);


                        }

                        if(countNonBoxUp==1 && countNonBoxDown==0){
                            rightPSeg = createPoint(rightPSeg, itR->x(), handleSlopeSegment(segment, *itR));
                            downfirst=createPoint(downfirst, p1.x(), handleSlopeSegment(deletSegBottom.begin()->first, p1));
                            addPolygon(p1, rightPSeg, foundTrapezoids[i][2], downfirst);
                            updateNeighbors(cg3::Segment2d(p1, rightPSeg), cg3::Segment2d(downfirst,  foundTrapezoids[i][2]), p1, *itR);
                        }

                    }
                    else{
                        addPolygon(p1, rightPSeg, foundTrapezoids[i][2], downfirst);
                        updateNeighbors(cg3::Segment2d(p1, rightPSeg), cg3::Segment2d(downfirst, foundTrapezoids[i][2]), p1, *itR);
                        if(countNonBoxUp >=1 && foundTrapezoids[i][0].y() == (1e+6)){
                            upsecond=createPoint(upsecond, p2.x(), handleSlopeSegment(deletSegTop.begin()->first, p2));
                            leftPSeg = createPoint(leftPSeg, itL->x(), handleSlopeSegment(segment, *itL));
                            addPolygon(foundTrapezoids[i][0], upsecond, p2, leftPSeg);
                            updateNeighbors(cg3::Segment2d(foundTrapezoids[i][0], upsecond), cg3::Segment2d(leftPSeg, p2), *itL, p2);
                        }
                        else if(countNonBoxUp == 1 && foundTrapezoids[i][0].y() != (1e+6)){
                            if(foundTrapezoids[(int)foundTrapezoids.size()-1][0].y() != (1e+6)){
                                upsecond=createPoint(upsecond, p2.x(), handleSlopeSegment(deletSegTop.begin()->first, p2));
                                leftPSeg = createPoint(leftPSeg, itL->x(), handleSlopeSegment(segment, *itL));
                                addPolygon(*itL, upsecond, p2, leftPSeg);
                                updateNeighbors(cg3::Segment2d(*itL, upsecond), cg3::Segment2d(leftPSeg, p2), *itL, p2);
                            }
                            else{
                                midpointR = *itL;
                            }

                        }
                    }
                }
                else if (dag.getDag()->determinant(segment, *itR) > 0 && dag.getDag()->determinant(segment, *itL) > 0){ //sotto
                    rightPSeg = createPoint(rightPSeg, itR->x(), handleSlopeSegment(segment, *itR));
                    leftPSeg = createPoint(leftPSeg, itL->x(), handleSlopeSegment(segment, *itL));
                    downsecond=createPoint(downsecond, p2.x(), handleSlopeSegment(deletSegBottom.begin()->first, p2));


                    if(p1.x() < itL->x() && p2.x() > itR->x()){
                        addPolygon(foundTrapezoids[i][0], foundTrapezoids[i][1], rightPSeg, leftPSeg);
                        updateNeighbors(cg3::Segment2d(foundTrapezoids[i][0], foundTrapezoids[i][1]), cg3::Segment2d(leftPSeg, rightPSeg), *itL, *itR);
                    }
                    else{
                        addPolygon(upfirst, foundTrapezoids[i][1], rightPSeg, p1);
                        updateNeighbors(cg3::Segment2d(upfirst, foundTrapezoids[i][1]), cg3::Segment2d(p1, rightPSeg), p1, *itR);
                    }

                }
                else if(dag.getDag()->determinant(segment, *itR) > 0 && dag.getDag()->determinant(segment, *itL) < 0){ //sotto r e sopra l
                    if(countNonBoxDown > 0 && foundTrapezoids[(foundTrapezoids.size()-1)][3].y() != -(1e+6)){
                        leftPSeg = createPoint(leftPSeg, itL->x(), handleSlopeSegment(segment, *itL));
                        downsecond=createPoint(downsecond, p2.x(), handleSlopeSegment(deletSegBottom.begin()->first, p2));
                        addPolygon(leftPSeg, p2, downsecond, foundTrapezoids[i][3]);
                        updateNeighbors(cg3::Segment2d(leftPSeg, p2), cg3::Segment2d(foundTrapezoids[i][3], downsecond), *itL, p2);
                    }

                    if(countNonBoxDown == 1 && countNonBoxUp==0){
                        rightPSeg = createPoint(rightPSeg, itR->x(), handleSlopeSegment(segment, *itR));
                        addPolygon(upfirst, foundTrapezoids[i][1], rightPSeg, p1);
                        updateNeighbors(cg3::Segment2d(upfirst, foundTrapezoids[i][1]), cg3::Segment2d(p1, rightPSeg), p1, *itR);

                    }
                    if(countNonBoxUp >= 1 && foundTrapezoids[i][0].y() != (1e+6)){
                        if(countNonBoxUp==1 || countNonBoxUp > 2){
                            if(countNonBoxUp > 2  && foundTrapezoids[i][2].y() == -(1e+6)){
                                upfirst=createPoint(upfirst, p1.x(), handleSlopeSegment(deletSegTop.begin()->first, p1));
                                rightPSeg = createPoint(rightPSeg, itR->x(), handleSlopeSegment(segment, *itR));
                                addPolygon(upfirst, *itR, rightPSeg, p1);
                                updateNeighbors(cg3::Segment2d(upfirst, *itR), cg3::Segment2d(p1, rightPSeg), p1, *itR);
                            }
                            else{
                                leftPSeg = createPoint(leftPSeg, midpointR.x(), handleSlopeSegment(segment, midpointR));
                                rightPSeg = createPoint(rightPSeg, itR->x(), handleSlopeSegment(segment, *itR));
                                addPolygon(midpointR, *itR ,rightPSeg, leftPSeg);
                                updateNeighbors(cg3::Segment2d(midpointR, *itR), cg3::Segment2d(leftPSeg, rightPSeg), midpointR, *itR);
                            }
                        }

                        if(countNonBoxUp==2){
                            upfirst=createPoint(upfirst, p1.x(), handleSlopeSegment(deletSegTop.begin()->first, p1));
                            rightPSeg = createPoint(rightPSeg, itR->x(), handleSlopeSegment(segment, *itR));
                            addPolygon(upfirst, *itR, rightPSeg, p1);
                            updateNeighbors(cg3::Segment2d(upfirst, *itR), cg3::Segment2d(p1, rightPSeg), midpointR, *itR);

                        }

                    }
                    if(countNonBoxUp >= 1 && foundTrapezoids[i][2].y() == -(1e+6)){
                        downsecond=createPoint(downsecond, p2.x(), handleSlopeSegment(deletSegBottom.begin()->first, p2));
                        leftPSeg = createPoint(leftPSeg, itL->x(), handleSlopeSegment(segment, *itL));
                        addPolygon(leftPSeg, p2, downsecond, foundTrapezoids[i][3]);
                        updateNeighbors(cg3::Segment2d(leftPSeg, p2), cg3::Segment2d(foundTrapezoids[i][3], downsecond), *itL, p2);
                    }

                }

            }

        }
    }
}

void TrapezoidalMap::updateNeighbors(cg3::Segment2d top, cg3::Segment2d bottom, cg3::Point2d left, cg3::Point2d right){
    size_t id2 = std::numeric_limits<size_t>::max();
    topMap.insert(std::make_pair(cg3::Segment2d(top), id2));
    bottomMap.insert(std::make_pair(cg3::Segment2d(bottom), id2));
    leftpList.push_back(left);
    rightpList.push_back(right);
}

void TrapezoidalMap::addPolygon(cg3::Point2d p1, cg3::Point2d p2, cg3::Point2d p3, cg3::Point2d p4){
    size_t id;

    std::array<cg3::Point2d, 4> trapezoid = {p1, p2, p3, p4};

    id = std::numeric_limits<size_t>::max();

    bool noInsertion=false;

    /*
     * Verificare che funzioni e che non dia errori nel dag
     */

    if(p1 == p2 && p3 == p4){
        noInsertion= true;
    }

    if(!noInsertion){
        trapezoidsMap.insert(std::make_pair(Trapezoid(trapezoid), id));
        update();
    }

    dag.setTrapezoidToInsert(trapezoid, trapezoids.size());


}

std::map<std::array<cg3::Point2d, 4>, size_t> TrapezoidalMap::findTrapezoid(const Trapezoid t, bool& found){
    found = false;
    return findIndexedTrapezoid(Trapezoid(t), found);
}

std::map<std::array<cg3::Point2d, 4>, size_t> TrapezoidalMap::findIndexedTrapezoid(const Trapezoid& indexedTrapezoid, bool& found){
    std::map<Trapezoid, size_t>::iterator it = trapezoidsMap.find(indexedTrapezoid);

    std::map<std::array<cg3::Point2d, 4>, size_t> map;

    if (it != trapezoidsMap.end()) {
        found = true;
        map.insert(std::make_pair(it->first, it->second));
    }
    return map;
}

std::vector<std::array<cg3::Point2d, 4>> TrapezoidalMap::getTrapezoids() const{
    return trapezoids;
}

cg3::Point2d TrapezoidalMap::createPoint(cg3::Point2d p, double x, double y){
    p.setXCoord(x);
    p.setYCoord(y);
    return p;
}

double TrapezoidalMap::handleSlopeSegment(cg3::Segment2d s, cg3::Point2d p){
    return (((p.x() - s.p1().x())/(s.p2().x() - s.p1().x()))*(s.p2().y() - s.p1().y()))+s.p1().y();
}

void TrapezoidalMap::update(){
    trapezoids.clear();
    for (std::map<Trapezoid, size_t>::iterator it=trapezoidsMap.begin(); it!=trapezoidsMap.end(); ++it){
        trapezoids.push_back(it->first);
    }
}

void TrapezoidalMap::queryPoint(cg3::Point2d point){
    found = dag.findTrapezoid(point, true, dag.getDag());
}

std::array<cg3::Point2d, 4> TrapezoidalMap::getFoundTrapezoid() const{
    return found;
}

bool TrapezoidalMap::degeneratedTrapezoid(const Trapezoid t){
    if(t[0] == t[1]){
        return true;
    }
    else if(t[1] == t[2]){
        return true;
    }
    else if(t[2] == t[3]){
        return true;
    }
    else if(t[3] == t[0]){
        return true;
    }
    else{
        return false;
    }
}

/**
 * @brief Cleared structures for the trapezoidal map
 * @param[in]
 */
void TrapezoidalMap::clearMap()
{

    boundingBox.setMin(cg3::Point2d(0,0));
    boundingBox.setMax(cg3::Point2d(0,0));
    trapezoids.clear();
    trapezoidsMap.clear();
    rightpList.clear();
    leftpList.clear();
    topMap.clear();
    bottomMap.clear();
}


