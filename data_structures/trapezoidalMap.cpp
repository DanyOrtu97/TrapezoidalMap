#include "trapezoidalMap.h"
#include "dag.h"

#include <drawables/drawableTrapezoidalMap.h>

cg3::Point2d topLeft;
cg3::Point2d bottomLeft;
cg3::Point2d topRight;
cg3::Point2d bottomRight;

TrapezoidalMap::TrapezoidalMap() :
    boundingBox(cg3::Point2d(0,0),cg3::Point2d(0,0))

{

}

void TrapezoidalMap::inizialize(){

    topLeft = createPoint(topLeft, -(1e+6), (1e+6));
    bottomLeft = createPoint(bottomLeft, -(1e+6), -(1e+6));
    topRight = createPoint(topRight, (1e+6), (1e+6));
    bottomRight = createPoint(bottomRight, (1e+6), -(1e+6));

    //inizialize the trapezoidal map with the bounding box
    addPolygon(topLeft, topRight, bottomRight, bottomLeft);

    //inizialize the dag with the pointer of the first polygon (the bounding box)
    //std::vector<std::array<cg3::Point2d, 4>> trapezoidsDag;
    //trapezoidsDag[0] = {topLeft, topRight, bottomRight, bottomLeft};
    //dag.inizializeDag(trapezoids);
}


void TrapezoidalMap::trapezoidalMapAlgorithm(cg3::Segment2d segment){

    topLeft = createPoint(topLeft, -(1e+6), (1e+6));
    bottomLeft = createPoint(bottomLeft, -(1e+6), -(1e+6));
    topRight = createPoint(topRight, (1e+6), (1e+6));
    bottomRight = createPoint(bottomRight, (1e+6), -(1e+6));


    std::vector<std::array<cg3::Point2d, 4>> foundTrapezoids = followSegment(trapezoids, /*dag*/ segment);

    cg3::Point2d p1 = segment.p1();
    cg3::Point2d p2 = segment.p2();

    std::cout<<"trapezoidi trovati"<<std::endl;
    //control in dag e trapezoidal map and after do it
    for (Trapezoid t : foundTrapezoids){
        std::vector<std::array<cg3::Point2d, 4>> tra;
        tra.clear();

        //debug  
        std::cout << t[0] << " " <<t[1] << " "<<t[2] << " "<<t[3] <<"   \n"<<std::endl;

        bool found;
        std::unordered_map<Trapezoid, size_t> delet;
        delet.clear();

        delet = findTrapezoid(t, found);

        tra.push_back(t);

        trapezoidsMap.erase(delet.begin()->first);
        trapezoids.erase(tra.begin());

        leftp.erase(leftp[delet.begin()->second]);
        rightp.erase(rightp[delet.begin()->second]);
        /*
        top.erase(top[delet.begin()->second]);
        bottom.erase(bottom[delet.begin()->second]);
        */

        //modificare trapezoidi trovati e inserire i nuovi
        //creation of vertical lines
        bool segmentInserted;

        cg3::Point2d upfirst;
        upfirst = createPoint(upfirst, p1.x(), 1e+6);
        cg3::Point2d downfirst;
        downfirst = createPoint(downfirst, p1.x(), -(1e+6));
        cg3::Point2d upsecond;
        upsecond = createPoint(upsecond, p2.x(), 1e+6);
        cg3::Point2d downsecond;
        downsecond = createPoint(downsecond, p2.x(), -(1e+6));

        cg3::Segment2d s1p1;
        s1p1.set(p1, upfirst);
        cg3::Segment2d s2p1;
        s2p1.set(p1, downfirst);
        cg3::Segment2d s1p2;
        s1p2.set(p2, upsecond);
        cg3::Segment2d s2p2;
        s2p2.set(p2, downsecond);


        //AGGIUNTA POLIGONI
        addPolygon(topLeft, upfirst, downfirst, bottomLeft); //A
        addPolygon(upfirst, upsecond, p2, p1); //B
        addPolygon(p1, p2, downsecond, downfirst); //C
        addPolygon(upsecond, topRight, bottomRight, downsecond); //D


        //AGGIUNTA VERTICAL LINE
        addVerticalLine(s1p1, segmentInserted);
        addVerticalLine(s2p1, segmentInserted);
        addVerticalLine(s1p2, segmentInserted);
        addVerticalLine(s2p2, segmentInserted);

    }
    InsertNeighbors(trapezoids, segment);

}


//verify and return the trapezoid in which intersect the segment
std::vector<std::array<cg3::Point2d, 4>> TrapezoidalMap::followSegment(std::vector<std::array<cg3::Point2d, 4>> trapezoids, /*Dag*/ cg3::Segment2d segment){
    cg3::Point2d p1 = segment.p1();
    cg3::Point2d p2 = segment.p2();

    std::vector<std::array<cg3::Point2d, 4>> foundTrapezoids;

    std::array<cg3::Point2d, 4> firstTrapezoid;

    std::cout<<"dim: \n"<<std::endl;
    std::cout<<trapezoids.size()<<std::endl;

    //ricerca del trapezoide in cui é p1 che andrà sostituita con la ricerca nel DAG
    for (Trapezoid trap : trapezoids){
        if (trap[0].x() <= p1.x() && trap[0].y() >= p1.y() &&
            trap[1].x() >= p1.x() && trap[1].y() >= p1.y() &&
            trap[2].x() >= p1.x() && trap[2].y() <= p1.y() &&
            trap[3].x() <= p1.x() && trap[3].y() <= p1.y()){
            firstTrapezoid=trap;
        }
    }


    if(trapezoids.size() > 1){
        int j=0;
        while(isToTheRight(p2, findRightp(trapezoids[j]))){
            if(LiesAbove(findRightp(trapezoids[j]), segment)){
                //non ho capito cosa ci va
            }
            else{
                //non ho capito cosa ci va
            }
            foundTrapezoids.push_back(trapezoids[j]);
            j++;
        }
    }
    else{
        foundTrapezoids.push_back(firstTrapezoid);
    }

    return foundTrapezoids;
}

bool TrapezoidalMap::isToTheRight(cg3::Point2d p, cg3::Point2d rightP){
    if (p.x() >= rightP.x()){
        return true;
    }
    else{
        return false;
    }
}

cg3::Point2d TrapezoidalMap::findLeftp(Trapezoid t){
    bool found;
    size_t id = findTrapezoid(t, found).begin()->second;
    return leftp[id];
}


cg3::Point2d TrapezoidalMap::findRightp(Trapezoid t){
    bool found;
    size_t id = findTrapezoid(t, found).begin()->second;
    return rightp[id];
}

bool TrapezoidalMap::LiesAbove(cg3::Point2d p, cg3::Segment2d segment){
    //da rivedere
    if(segment.p1().y() <= p.y() && segment.p2().y() <= p.y()){
        return true;
    }
    else{
        return false;
    }
}


void TrapezoidalMap::InsertNeighbors(std::vector<Trapezoid> t, cg3::Segment2d segment){
    for (Trapezoid trap : t){

    }
}


/**
 * @brief Add vertical line into vector and into map
 * @param[in] segment, segmentInserted
 */
size_t TrapezoidalMap::addVerticalLine(const cg3::Segment2d& segment, bool& segmentInserted){
    size_t id;

    bool found;


    findVerticalLine(segment, found);

    bool degenerate = segment.p1() == segment.p2();

    segmentInserted = false;
    id = std::numeric_limits<size_t>::max();

    if (!degenerate && !found) {
        segmentInserted=true;
        verticalLines.push_back(VerticalLines2D(segment.p1(), segment.p2()));

        verticalLinesMap.insert(std::make_pair(VerticalLines2D(segment.p1(), segment.p2()), id));

    }

    return id;
}


/**
 * @brief Find vertical line into structure
 * @param[in] segment, found
 */
size_t TrapezoidalMap::findVerticalLine(const cg3::Segment2d& segment, bool& found){
    found = false;

    return findIndexedVerticalLine(VerticalLines2D(segment.p1(), segment.p2()), found);
}

/**
 * @brief Retrieve vertical line position and free position
 * @param[in] indexedSegment, found
 */
size_t TrapezoidalMap::findIndexedVerticalLine(const VerticalLines2D& indexedSegment, bool& found){
    std::unordered_map<VerticalLines2D, size_t>::iterator it = verticalLinesMap.find(indexedSegment);
    //Segment already in the data structure
    if (it != verticalLinesMap.end()) {
        found = true;
        return it->second;
    }
    //Segment not in the data structure
    else {
        found = false;
        return std::numeric_limits<size_t>::max();
    }
}

/**
 * @brief For each vertical line cal the procedure getVerticalLine
 * @param[in]
 */
std::vector<cg3::Segment2d> TrapezoidalMap::getVerticalLines() const
{
    std::vector<cg3::Segment2d> segments;
    for (size_t i = 0; i < verticalLines.size(); i++) {
        segments.push_back(getVerticalLine(i));
    }
    return segments;
}

/**
 * @brief Returned vertical line at position id
 * @param[in] id
 */
cg3::Segment2d TrapezoidalMap::getVerticalLine(size_t id) const
{
    return cg3::Segment2d(verticalLines[id].first, verticalLines[id].second);
}


void TrapezoidalMap::addPolygon(cg3::Point2d p1, cg3::Point2d p2, cg3::Point2d p3, cg3::Point2d p4){
    size_t id;

    std::array<cg3::Point2d, 4> trapezoid = {p1, p2, p3, p4};

    id = std::numeric_limits<size_t>::max();


    trapezoids.push_back(Trapezoid(trapezoid));

    trapezoidsMap.insert(std::make_pair(Trapezoid(trapezoid), id));
}


std::unordered_map<std::array<cg3::Point2d, 4>, size_t> TrapezoidalMap::findTrapezoid(const Trapezoid t, bool& found){
    found = false;

    return findIndexedTrapezoid(Trapezoid(t), found);
}

std::unordered_map<std::array<cg3::Point2d, 4>, size_t> TrapezoidalMap::findIndexedTrapezoid(const Trapezoid& indexedTrapezoid, bool& found){
    std::unordered_map<Trapezoid, size_t>::iterator it = trapezoidsMap.find(indexedTrapezoid);

    std::unordered_map<std::array<cg3::Point2d, 4>, size_t> map;

    //Segment already in the data structure
    if (it != trapezoidsMap.end()) {
        found = true;
        map.insert(std::make_pair(it->first, it->second));
        return map;
    }
}



std::vector<std::array<cg3::Point2d, 4>> TrapezoidalMap::getTrapezoids() const{
    return trapezoids;
}


size_t TrapezoidalMap::trapezoidsNumber(){
    return trapezoids.size();
}

cg3::Point2d TrapezoidalMap::createPoint(cg3::Point2d p, double x, double y){
    p.setXCoord(x);
    p.setYCoord(y);
    return p;
}

/**
 * @brief Cleared structures for the trapezoidal map
 * @param[in]
 */
void TrapezoidalMap::clearMap()
{

    verticalLines.clear();
    verticalLinesMap.clear();
    boundingBox.setMin(cg3::Point2d(0,0));
    boundingBox.setMax(cg3::Point2d(0,0));
    trapezoids.clear();
    trapezoidsMap.clear();
    rightp.clear();
    leftp.clear();
    top.clear();
    bottom.clear();
}


