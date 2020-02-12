#include "trapezoidalMap.h"

//code for the trapezoidal map structure
//vertical lines and polygons

TrapezoidalMapDataset tmd;

TrapezoidalMap::TrapezoidalMap(){}



size_t TrapezoidalMap::addVerticalLine(const cg3::Segment2d& verticalLine, bool& verticalLineInserted){
    size_t id;

    bool found;
    tmd.findSegment(verticalLine, found);

    bool degenerate = verticalLine.p1() == verticalLine.p2();

    verticalLineInserted = false;
    id = std::numeric_limits<size_t>::max();

    if (!degenerate && !found) {
        bool generalPosition = true;

        bool foundPoint1;
        size_t id1 = tmd.findPoint(verticalLine.p1(), foundPoint1);
        bool foundPoint2;
        size_t id2 = tmd.findPoint(verticalLine.p2(), foundPoint2);

        if (generalPosition) {
            bool intersecting = intersectionChecker.checkIntersections(verticalLine);

            if (!intersecting) {
                verticalLineInserted = true;

                id = VerticalLines.size();

                if (!foundPoint1) {
                    bool insertedPoint1;
                    id1 = tmd.addPoint(verticalLine.p1(), insertedPoint1);
                    assert(insertedPoint1);
                }

                if (!foundPoint2) {
                    bool insertedPoint2;
                    id2 = tmd.addPoint(verticalLine.p2(), insertedPoint2);
                    assert(insertedPoint2);
                }
                assert(id1 != id2 && id1 < tmd.getPoints().size() && id2 < tmd.getPoints().size());

                VerticalLines.push_back(VerticalLines2D(id1, id2));

                LineMap.insert(std::make_pair(VerticalLines2D(id1, id2), id));

                intersectionChecker.insert(verticalLine);
            }
        }
    }

    return id;

}

size_t TrapezoidalMap::addIndexedVerticalLine(const VerticalLines2D& verticalLine, bool& verticalLineInserted){
    bool found;
    size_t id;

    findIndexedVerticalLine(verticalLine, found);

    verticalLineInserted = false;
    id = std::numeric_limits<size_t>::max();

    bool degenerate = verticalLine.first == verticalLine.second;

    if (!degenerate && !found) {
        bool intersecting = intersectionChecker.checkIntersections(cg3::Segment2d(tmd.getPoints()[verticalLine.first], tmd.getPoints()[verticalLine.second]));

        if (!intersecting) {
            verticalLineInserted = true;

            id = VerticalLines.size();

            VerticalLines.push_back(VerticalLines2D(verticalLine.first, verticalLine.second));

            LineMap.insert(std::make_pair(VerticalLines2D(verticalLine.first, verticalLine.second), id));

            intersectionChecker.insert(cg3::Segment2d(tmd.getPoints()[verticalLine.first], tmd.getPoints()[verticalLine.second]));
        }
    }

    return id;
}

//we need it to delete vertical line for new insertion
/*size_t TrapezoidalMap::deleteVerticalLine(const cg3::Segment2d& verticalLine, bool& verticalLineDeleted){


}*/

//before deletion we need to search
size_t TrapezoidalMap::findVerticalLine(const cg3::Segment2d& verticalLine, bool& found){
    found = false;

    bool foundPoint1;
    size_t id1 = tmd.findPoint(verticalLine.p1(), foundPoint1);
    if (!foundPoint1)
        return std::numeric_limits<size_t>::max();

    bool foundPoint2;
    size_t id2 = tmd.findPoint(verticalLine.p2(), foundPoint2);
    if (!foundPoint2)
        return std::numeric_limits<size_t>::max();

    return findIndexedVerticalLine(VerticalLines2D(id1, id2), found);
   }

size_t TrapezoidalMap::findIndexedVerticalLine(const VerticalLines2D& verticalLine, bool& found){
    std::unordered_map<VerticalLines2D, size_t>::iterator it = LineMap.find(verticalLine);

    //Segment already in the data structure
    if (it != LineMap.end()) {
        found = true;
        return it->second;
    }
    //Segment not in the data structure
    else {
        found = false;
        return std::numeric_limits<size_t>::max();
    }

}

std::vector<cg3::Segment2d> TrapezoidalMap::getVerticalLines() const
{
    std::vector<cg3::Segment2d> verticalLines;
    for (size_t i = 0; i < VerticalLines.size(); i++) {
        verticalLines.push_back(getVerticalLine(i));
    }
    return verticalLines;
}

cg3::Segment2d TrapezoidalMap::getVerticalLine(size_t id) const
{
    return cg3::Segment2d(tmd.getPoints()[tmd.getIndexedSegments()[id].first], tmd.getPoints()[tmd.getIndexedSegments()[id].second]);
}


void TrapezoidalMap::clearMap()
{
    VerticalLines.clear();
    LineMap.clear();
    //Polygons.clear();
    //PolygonsMap.clear();
}


