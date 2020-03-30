#ifndef TRAPEZOIDALMAP_H
#define TRAPEZOIDALMAP_H

#include <vector>
#include <utility>

#include <cg3/geometry/point2.h>
#include <cg3/geometry/segment2.h>
#include <cg3/geometry/bounding_box2.h>
#include <data_structures/dag.h>
#include "data_structures/trapezoidalmap_dataset.h"
#include "data_structures/segment_intersection_checker.h"
#include "data_structures/trapezoid.h"


/**
 * @brief This class allows to build the trapezoidal map step by step (incrementally)
 */
class TrapezoidalMap {

public:
    typedef std::array<cg3::Point2d, 4> Trapezoid;

    TrapezoidalMap();

    void inizialize();

    void addPolygon(const cg3::Point2d p1, const cg3::Point2d p2, const cg3::Point2d p3, const cg3::Point2d p4,
                    const cg3::Point2d left, const cg3::Point2d right, const cg3::Segment2d top, const cg3::Segment2d bottom);

    std::list<trapezoid> getTrapezoids() const;

    //methods to update dag e trapezoidal map
    void trapezoidalMapAlgorithm(cg3::Segment2d segment);
    void updateTrapezoid(const cg3::Segment2d& segment, std::vector<trapezoid> foundTrapezoid);

    //methods for point location
    void queryPoint(cg3::Point2d point);
    Trapezoid getFoundTrapezoid() const;

    void clearMap();

private:
    cg3::BoundingBox2 boundingBox;

    Trapezoid queryPointTrap;

    std::list<trapezoid> trapezoids;
};

#endif // TRAPEZOIDALMAP_H
