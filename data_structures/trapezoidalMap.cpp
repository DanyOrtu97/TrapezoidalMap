#include "trapezoidalMap.h"
#include "dag.h"


TrapezoidalMap::TrapezoidalMap() :
    boundingBox(cg3::Point2d(0,0),cg3::Point2d(0,0))
{

}


void TrapezoidalMap::trapezoidalMapAlgorithm(cg3::Segment2d segment){
    //Punti bounding box
    cg3::Point2d topLeft;
    cg3::Point2d bottomLeft;
    cg3::Point2d topRight;
    cg3::Point2d bottomRight;

    topLeft = createPoint(topLeft, -(1e+6), (1e+6));
    bottomLeft = createPoint(bottomLeft, -(1e+6), -(1e+6));
    topRight = createPoint(topRight, (1e+6), (1e+6));
    bottomRight = createPoint(bottomRight, (1e+6), -(1e+6));

    //inizialize the trapezoidal map with the bounding box

    //addPolygon(topLeft, bottomLeft, topRight, bottomRight);

    //inizialize the dag with the pointer of the first polygon (the bounding box)


    //std::vector<std::array<cg3::Point2d, 4>> foundTrapezoids = followSegment(trapezoids, /*dag*/ segment);

    //control in dag e trapezoidal map and after do it
    /*

     some stuff


    */



    cg3::Point2d p1 = segment.p1();
    cg3::Point2d p2 = segment.p2();

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
    //addPolygon(upfirst, upsecond, p2, p1); //B
    //addPolygon(p1, p2, downsecond, downfirst); //C
    //addPolygon(upsecond, topRight, bottomRight, downsecond); //D


    //AGGIUNTA VERTICAL LINE
    addVerticalLine(s1p1, segmentInserted);
    addVerticalLine(s2p1, segmentInserted);
    addVerticalLine(s1p2, segmentInserted);
    addVerticalLine(s2p2, segmentInserted);
}


std::vector<std::array<cg3::Point2d, 4>> TrapezoidalMap::followSegment(std::vector<std::array<cg3::Point2d, 4>> trapezoids, /*Dag*/ cg3::Segment2d segment){
    //verify in which trapezoid intersect the segment



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

        intersectionChecker.insert(segment);

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
    std::array<cg3::Point2d, 4> trapezoid = {p1, p2, p3, p4};
    trapezoids.push_back(Trapezoid(trapezoid));
    /*
    std::cout<< trapezoidsNumber() <<std::endl;
    for ( Trapezoid t : trapezoids){
        std::cout << "primo trapezoide: " << std::endl;
        std::cout << t.at(0) << std::endl;
        std::cout << t.at(1) << std::endl;
        std::cout << t.at(2) << std::endl;
        std::cout << t.at(3) << std::endl;
    }
    */
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
    intersectionChecker.clear();
    trapezoids.clear();
    trapezoidsMap.clear();
}


