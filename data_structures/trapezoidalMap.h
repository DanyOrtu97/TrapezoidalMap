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
    std::map<trapezoid, size_t> findTrapezoid(const trapezoid t, bool& found);
    std::map<trapezoid, size_t> findIndexedTrapezoid(const trapezoid& indexedTrapezoid, bool& found);

    void update();
    std::vector<trapezoid> getTrapezoids() const;

    //methods to update dag e trapezoidal map
    void trapezoidalMapAlgorithm(cg3::Segment2d segment);
    void CompletelyInsideTrapezoid(trapezoid t, const cg3::Segment2d& segment);
    void multipleTrapezoid(const cg3::Segment2d& segment, std::vector<trapezoid> foundTrapezoid);
    //void updateNeighbors(cg3::Segment2d top, cg3::Segment2d bottom, cg3::Point2d left, cg3::Point2d right);

    //methods for point location
    void queryPoint(cg3::Point2d point);
    Trapezoid getFoundTrapezoid() const;

    void clearMap();

private:
    cg3::BoundingBox2 boundingBox;

    Trapezoid queryPointTrap;

    std::vector<trapezoid> trapezoids;
    std::map<trapezoid, size_t> trapezoidsMap;

    /*
    std::list<cg3::Point2d> leftpList;
    std::list<cg3::Point2d> rightpList;
    std::map<cg3::Segment2d, size_t> topMap;
    std::map<cg3::Segment2d, size_t> bottomMap;
    */
};

#endif // TRAPEZOIDALMAP_H
