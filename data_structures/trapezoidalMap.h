#ifndef TRAPEZOIDALMAP_H
#define TRAPEZOIDALMAP_H


#include <cg3/geometry/point2.h>
#include <cg3/geometry/segment2.h>
#include <cg3/geometry/bounding_box2.h>

#include "data_structures/trapezoidalmap_dataset.h"


/**
 * @brief This class allows to build the trapezoidal map step by step (incrementally)
 */
class TrapezoidalMap {

public:
    typedef std::pair<size_t, size_t> VerticalLines2D;


    TrapezoidalMap();



    //we need it to draw vertical line
    size_t addVerticalLIne(const cg3::Segment2d& segment, bool& segmentInserted);

    //we need it to delete vertical line fro new insertion
    size_t deleteVerticalLIne(const cg3::Segment2d& segment, bool& segmentInserted);


    size_t findVerticalLine(const cg3::Segment2d& segment, bool& found);


private:

    //list of vertical lines. I update it every time that is necessary
    std::vector<VerticalLines2D> VerticalLines;

};

#endif // TRAPEZOIDALMAP_H
