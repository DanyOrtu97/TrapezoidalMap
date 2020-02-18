#include "trapezoidalMap.h"
#include "dag.h"


TrapezoidalMap::TrapezoidalMap() :
    boundingBox(cg3::Point2d(0,0),cg3::Point2d(0,0))
{


}


//next step is to add controls for avoid the intersection ...
/**
 * @brief Create from the segment 4 vertical lines; two for each endpoint.
 * @param[in] segment
 */
void TrapezoidalMap::addVerticalLines(const cg3::Segment2d& segment){

    cg3::Point2d p1 = segment.p1();
    cg3::Point2d p2 = segment.p2();


    bool segmentInserted;
    cg3::Point2d intersectionPoint;

    cg3::Point2d upfirst;
    upfirst.setXCoord(p1.x());
    upfirst.setYCoord(1e+6);
    cg3::Point2d downfirst;
    downfirst.setXCoord(p1.x());
    downfirst.setYCoord(-(1e+6));

    cg3::Point2d upsecond;
    upsecond.setXCoord(p2.x());
    upsecond.setYCoord(1e+6);
    cg3::Point2d downsecond;
    downsecond.setXCoord(p2.x());
    downsecond.setYCoord(-(1e+6));

    cg3::Segment2d s1p1;
    s1p1.set(p1, upfirst);
    cg3::Segment2d s2p1;
    s2p1.set(p1, downfirst);
    cg3::Segment2d s1p2;
    s1p2.set(p2, upsecond);
    cg3::Segment2d s2p2;
    s2p2.set(p2, downsecond);

    /*
    bool intersecting1 = intersectionChecker.checkIntersections(s1p1);
    bool intersecting2 = intersectionChecker.checkIntersections(s2p1);
    bool intersecting3 = intersectionChecker.checkIntersections(s1p2);
    bool intersecting4 = intersectionChecker.checkIntersections(s2p2);
    */

    addVerticalLine(s1p1, segmentInserted);
    addVerticalLine(s2p1, segmentInserted);
    addVerticalLine(s1p2, segmentInserted);
    addVerticalLine(s2p2, segmentInserted);

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
}


