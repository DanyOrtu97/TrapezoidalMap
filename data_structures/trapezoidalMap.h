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


/**
 * @brief This class allows to build the trapezoidal map step by step (incrementally)
 */
class TrapezoidalMap {

public:
    typedef std::array<cg3::Point2d, 4> Trapezoid;

    TrapezoidalMap();

    void inizialize();

    //metodi per creare e distruggere poligoni
    void addPolygon(cg3::Point2d p1, cg3::Point2d p2, cg3::Point2d p3, cg3::Point2d p4);
    std::map<Trapezoid, size_t> findTrapezoid(const Trapezoid t, bool& found);
    std::map<Trapezoid, size_t> findIndexedTrapezoid(const Trapezoid& indexedTrapezoid, bool& found);

    void update();
    std::vector<Trapezoid> getTrapezoids() const;


    //methods to update dag e trapezoidal map
    void trapezoidalMapAlgorithm(cg3::Segment2d segment);
    void CompletelyInsideTrapezoid(Trapezoid t, const cg3::Segment2d& segment);
    void multipleTrapezoid(const cg3::Segment2d& segment, std::vector<Trapezoid> foundTrapezoid);
    void updateNeighbors(cg3::Segment2d top, cg3::Segment2d bottom, cg3::Point2d left, cg3::Point2d right);

    //methods to handle points
    cg3::Point2d createPoint(cg3::Point2d p, double x, double y);
    double handleSlopeSegment(cg3::Segment2d s, cg3::Point2d p);

    //methods for point location
    void queryPoint(cg3::Point2d point);
    Trapezoid getFoundTrapezoid() const;

    //methods to handle the trapeoids that are triangles
    bool degeneratedTrapezoid(const Trapezoid t);
    std::array<cg3::Point2d, 3> findTriangleByQuad(const Trapezoid trap);

    void clearMap();

private:
    cg3::BoundingBox2 boundingBox;

    Trapezoid found;
    std::vector<Trapezoid> trapezoids;
    std::map<Trapezoid, size_t> trapezoidsMap;

    std::list<cg3::Point2d> leftpList;
    std::list<cg3::Point2d> rightpList;
    std::map<cg3::Segment2d, size_t> topMap;
    std::map<cg3::Segment2d, size_t> bottomMap;
};

#endif // TRAPEZOIDALMAP_H
