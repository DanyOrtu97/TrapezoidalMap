#ifndef TRAPEZOIDALMAP_H
#define TRAPEZOIDALMAP_H

#include <unordered_map>
#include <vector>
#include <utility>


#include <cg3/geometry/point2.h>
#include <cg3/geometry/segment2.h>
#include <cg3/geometry/polygon2.h>
#include <cg3/geometry/bounding_box2.h>

#include "data_structures/trapezoidalmap_dataset.h"


/**
 * @brief This class allows to build the trapezoidal map step by step (incrementally)
 */
class TrapezoidalMap {

public:
    typedef std::pair<size_t, size_t> VerticalLines2D;
    // typedef ... Polygons2D;

    TrapezoidalMap();



    //we need it to draw vertical line
    size_t addVerticalLine(const cg3::Segment2d& segment, bool& segmentInserted);
    size_t addIndexedVerticalLine(const VerticalLines2D& segment, bool& segmentInserted);

    //we need it to delete vertical line fro new insertion
    //size_t deleteVerticalLine(const cg3::Segment2d& segment, bool& segmentInserted);

    //before deletion we need to search
    size_t findVerticalLine(const cg3::Segment2d& segment, bool& found);
    size_t findIndexedVerticalLine(const VerticalLines2D& indexedSegment, bool& found);

    //mancano i metodi per creare e distruggere poligoni, derivanti dalle vertical line

    //..... code here


    void clearMap();

private:

    //list of vertical lines. I update it every time that is necessary
    std::vector<VerticalLines2D> VerticalLines;

    //map of vertical lines
    std::unordered_map<VerticalLines2D, size_t> LineMap;

    //list of polygons
    //std::vector<Polygons2D> Polygons;

    //map of polygons
    //std...

    SegmentIntersectionChecker intersectionChecker;

};

#endif // TRAPEZOIDALMAP_H
