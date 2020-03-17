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

    //caso 1
    if (firstTrapezoid == lastTrapezoid){   

        //Inserisci e/o aggiorna vicini...
        CompletelyInsideTrapezoid(firstTrapezoid, segment);

        dag.updateDag(segment);
        dag.clearTraps();
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

        multipleTrapezoid(segment, foundTrapezoids);

        dag.updateDag(segment);
        dag.clearTraps();

    }

}


void TrapezoidalMap::CompletelyInsideTrapezoid(Trapezoid t, const cg3::Segment2d& segment){
    std::map<Trapezoid, size_t> delet;

    std::map<cg3::Segment2d, size_t> deletSegTop, deletSegBottom;

    std::list<cg3::Point2d> deletPoint;

    bool found;

    delet = findTrapezoid(t, found);

    deletSegTop.insert(std::make_pair(cg3::Segment2d(delet.begin()->first[0], delet.begin()->first[1]), delet.begin()->second));
    deletSegBottom.insert(std::make_pair(cg3::Segment2d(delet.begin()->first[3], delet.begin()->first[2]), delet.begin()->second));

    trapezoidsMap.erase(delet.begin()->first);

    update();

    size_t id2;

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
    id2 = std::numeric_limits<size_t>::max();
    addPolygon(topLeft, upfirst, downfirst, bottomLeft); //A
    topMap.insert(std::make_pair(cg3::Segment2d(topLeft, upfirst), id2));
    bottomMap.insert(std::make_pair(cg3::Segment2d(downfirst, bottomLeft), id2));
    leftpList.push_back(topLeft);
    rightpList.push_back(segment.p1());

    id2 = std::numeric_limits<size_t>::max();
    addPolygon(upfirst, upsecond, p2, p1); //B
    topMap.insert(std::make_pair(cg3::Segment2d(upfirst, upsecond), id2));
    bottomMap.insert(std::make_pair(cg3::Segment2d(p1, p2), id2));
    leftpList.push_back(segment.p1());
    rightpList.push_back(segment.p2());

    id2 = std::numeric_limits<size_t>::max();
    addPolygon(p1, p2, downsecond, downfirst); //C
    topMap.insert(std::make_pair(cg3::Segment2d(p1, p2), id2));
    bottomMap.insert(std::make_pair(cg3::Segment2d(downfirst, downsecond), id2));
    leftpList.push_back(segment.p1());
    rightpList.push_back(segment.p2());

    id2 = std::numeric_limits<size_t>::max();
    addPolygon(upsecond, topRight, bottomRight, downsecond); //D
    topMap.insert(std::make_pair(cg3::Segment2d(upsecond, topRight), id2));
    bottomMap.insert(std::make_pair(cg3::Segment2d(downsecond, bottomRight), id2));
    leftpList.push_back(segment.p2());
    rightpList.push_back(topRight);

}


void TrapezoidalMap::multipleTrapezoid(const cg3::Segment2d& segment, std::vector<Trapezoid> foundTrapezoids){
    std::map<Trapezoid, size_t> delet;

    std::map<cg3::Segment2d, size_t> deletSegTop, deletSegBottom;

    std::list<cg3::Point2d> deletPoint;

    cg3::Point2d p1 = segment.p1();
    cg3::Point2d p2 = segment.p2();
    cg3::Point2d upfirst;
    cg3::Point2d upsecond;
    cg3::Point2d downfirst;
    cg3::Point2d downsecond;
    cg3::Segment2d left;
    cg3::Segment2d right;

    cg3::Point2d rightPSeg;
    cg3::Point2d leftPSeg;

    bool found;



    for (int i=0; i<(int)foundTrapezoids.size(); i++){
        delet.clear();
        deletSegTop.clear();
        deletSegBottom.clear();

        topLeft = foundTrapezoids[i][0];
        bottomLeft = foundTrapezoids[i][3];
        topRight = foundTrapezoids[i][1];
        bottomRight = foundTrapezoids[i][2];
        left.set(foundTrapezoids[i][0], foundTrapezoids[i][3]);
        right.set(foundTrapezoids[i][1], foundTrapezoids[i][2]);

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

        if(foundTrapezoids.size() == 2){
            if(i==0){
                upfirst=createPoint(upfirst, p1.x(), handleSlopeSegment(deletSegTop.begin()->first, p1));
                downfirst=createPoint(downfirst, p1.x(), handleSlopeSegment(deletSegBottom.begin()->first, p1));

                addPolygon(topLeft, upfirst, downfirst, bottomLeft);
            }
            else if(i == (int)foundTrapezoids.size()-1){
                upsecond=createPoint(upsecond, p2.x(), handleSlopeSegment(deletSegTop.begin()->first, p2));
                downsecond=createPoint(downsecond, p2.x(), handleSlopeSegment(deletSegBottom.begin()->first, p2));
                leftPSeg = createPoint(leftPSeg, itL->x(), handleSlopeSegment(segment, *itL));

                addPolygon(upsecond, topRight, bottomRight, downsecond);
                if(dag.getDag()->determinant(segment, *itL) < 0){
                    addPolygon(leftPSeg, p2, downsecond, foundTrapezoids[i][3]);
                }
                else{
                    addPolygon(foundTrapezoids[i][0], upsecond, p2, leftPSeg);
                }
            }

            if(p2.x() > itR->x()){ //sfrutto trapezoidi e vertical lines già presenti
                if(dag.getDag()->determinant(segment, *itR) < 0){
                    rightPSeg = createPoint(rightPSeg, itR->x(), handleSlopeSegment(segment, *itR));
                    upsecond=createPoint(upsecond, p2.x(), handleSlopeSegment(deletSegTop.begin()->first, p2));
                    addPolygon(upfirst, upsecond, p2, p1);
                    addPolygon(p1, rightPSeg, foundTrapezoids[i][2], downfirst);
                }
                else{
                    rightPSeg = createPoint(rightPSeg, itR->x(), handleSlopeSegment(segment, *itR));
                    addPolygon(upfirst, foundTrapezoids[i][1], rightPSeg ,p1);
                    downsecond=createPoint(downsecond, p2.x(), handleSlopeSegment(deletSegBottom.begin()->first, p2));
                    addPolygon(p1, p2, downsecond, downfirst);
                }
            }
        }
        else{ //Trovati più di 2 trapezoidi
            if(i==0){
                upfirst=createPoint(upfirst, p1.x(), handleSlopeSegment(deletSegTop.begin()->first, p1));
                downfirst=createPoint(downfirst, p1.x(), handleSlopeSegment(deletSegBottom.begin()->first, p1));

                addPolygon(topLeft, upfirst, downfirst, bottomLeft);
            }
            else if(i == (int)foundTrapezoids.size()-1){
                upsecond=createPoint(upsecond, p2.x(), handleSlopeSegment(deletSegTop.begin()->first, p2));
                downsecond=createPoint(downsecond, p2.x(), handleSlopeSegment(deletSegBottom.begin()->first, p2));
                leftPSeg = createPoint(leftPSeg, itL->x(), handleSlopeSegment(segment, *itL));
                addPolygon(upsecond, topRight, bottomRight, downsecond);
                if(dag.getDag()->determinant(segment, *itL) < 0){
                    addPolygon(leftPSeg, p2, downsecond, foundTrapezoids[i][3]);
                }
                else{
                    addPolygon(foundTrapezoids[i][0], upsecond, p2, leftPSeg);
                }
            }
            if(p2.x() > itR->x()){ //sfrutto trapezoidi e vertical lines già presenti
                if(dag.getDag()->determinant(segment, *itR) < 0){
                    upsecond=createPoint(upsecond, p2.x(), handleSlopeSegment(deletSegTop.begin()->first, p2));
                    rightPSeg = createPoint(rightPSeg, itR->x(), handleSlopeSegment(segment, *itR));
                    downfirst=createPoint(downfirst, p1.x(), handleSlopeSegment(deletSegBottom.begin()->first, p1));

                    addPolygon(upfirst, upsecond, p2, p1);

                    if(p1.x() < itL->x() && p2.x() > itR->x()){
                        leftPSeg = createPoint(leftPSeg, itL->x(), handleSlopeSegment(segment, *itL));
                        rightPSeg = createPoint(rightPSeg, itR->x(), handleSlopeSegment(segment, *itR));
                        addPolygon(leftPSeg, rightPSeg, foundTrapezoids[i][2], foundTrapezoids[i][3]);
                    }
                    else{
                        addPolygon(p1, rightPSeg, foundTrapezoids[i][2], downfirst);
                    }
                }
                else{ //sotto
                    rightPSeg = createPoint(rightPSeg, itR->x(), handleSlopeSegment(segment, *itR));
                    downsecond=createPoint(downsecond, p2.x(), handleSlopeSegment(deletSegBottom.begin()->first, p2));
                    addPolygon(p1,p2, downsecond, downfirst);

                    if(p1.x() < itL->x() && p2.x() > itR->x()){
                        addPolygon(foundTrapezoids[i][0], foundTrapezoids[i][1], rightPSeg, foundTrapezoids[i][3]);
                    }
                    else{
                        addPolygon(upfirst, foundTrapezoids[i][1], rightPSeg, p1);
                    }

                }
            }
        }

        /*
        deletPoint.clear();
        deletPoint.insert(deletPoint.begin(), delet.begin()->first[0]);
        leftpList.erase(deletPoint.begin());
        deletPoint.clear();
        deletPoint.insert(deletPoint.begin(), delet.begin()->first[1]);
        */

        topMap.erase(deletSegTop.begin()->first);
        bottomMap.erase(deletSegBottom.begin()->first);
    }
    leftpList.clear();
    rightpList.clear();
}


void TrapezoidalMap::addPolygon(cg3::Point2d p1, cg3::Point2d p2, cg3::Point2d p3, cg3::Point2d p4){
    size_t id;

    std::array<cg3::Point2d, 4> trapezoid = {p1, p2, p3, p4};

    id = std::numeric_limits<size_t>::max();

    trapezoidsMap.insert(std::make_pair(Trapezoid(trapezoid), id));

    dag.setTrapezoidToInsert(trapezoid, trapezoids.size());

    update();
}

std::map<std::array<cg3::Point2d, 4>, size_t> TrapezoidalMap::findTrapezoid(const Trapezoid t, bool& found){
    found = false;
    return findIndexedTrapezoid(Trapezoid(t), found);
}

std::map<std::array<cg3::Point2d, 4>, size_t> TrapezoidalMap::findIndexedTrapezoid(const Trapezoid& indexedTrapezoid, bool& found){
    std::map<Trapezoid, size_t>::iterator it = trapezoidsMap.find(indexedTrapezoid);

    std::map<std::array<cg3::Point2d, 4>, size_t> map;

    //Segment already in the data structure
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


