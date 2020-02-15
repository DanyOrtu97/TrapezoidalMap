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
#include "data_structures/segment_intersection_checker.h"

/**
 * @brief This class allows to build the trapezoidal map step by step (incrementally)
 */
class TrapezoidalMap {

public:
    typedef std::pair<cg3::Point2d, cg3::Point2d> VerticalLines2D;
    // typedef ... Polygons2D;

    TrapezoidalMap();

    void addVerticalLines(const cg3::Segment2d& segment);

    size_t addVerticalLine(const cg3::Segment2d& segment, bool& segmentInserted);

    size_t findVerticalLine(const cg3::Segment2d& segment, bool& found);
    size_t findIndexedVerticalLine(const VerticalLines2D& indexedSegment, bool& found);

    std::vector<cg3::Segment2d> getVerticalLines() const;
    cg3::Segment2d getVerticalLine(size_t id) const;


    //mancano i metodi per creare e distruggere poligoni, derivanti dalle vertical line

    //..... code here


    void clearMap();

private:
    std::vector<VerticalLines2D> verticalLines;
    std::unordered_map<VerticalLines2D, size_t> verticalLinesMap;
    cg3::BoundingBox2 boundingBox;

    SegmentIntersectionChecker intersectionChecker;
};

#endif // TRAPEZOIDALMAP_H
