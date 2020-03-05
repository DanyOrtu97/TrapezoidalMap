#ifndef TRAPEZOIDALMAP_H
#define TRAPEZOIDALMAP_H

#include <unordered_map>
#include <vector>
#include <utility>

#include <GL/gl.h>
#include <GL/glu.h>

#include <cg3/geometry/point2.h>
#include <cg3/geometry/segment2.h>
#include <cg3/geometry/bounding_box2.h>


#include "data_structures/trapezoidalmap_dataset.h"
#include "data_structures/segment_intersection_checker.h"


/**
 * @brief This class allows to build the trapezoidal map step by step (incrementally)
 */
class TrapezoidalMap {

public:
    /*
    struct trapezoid{
        cg3::Point2d ul;
        cg3::Point2d ur;
        cg3::Point2d lr;
        cg3::Point2d ll;
    };
    */

    typedef std::pair<cg3::Point2d, cg3::Point2d> VerticalLines2D;
    typedef std::array<cg3::Point2d, 4> Trapezoid;

    TrapezoidalMap();

    void inizialize();

    //methods to create and handle the vertical lines
    size_t addVerticalLine(const cg3::Segment2d& segment, bool& segmentInserted);
    size_t findVerticalLine(const cg3::Segment2d& segment, bool& found);
    size_t findIndexedVerticalLine(const VerticalLines2D& indexedSegment, bool& found);
    std::vector<cg3::Segment2d> getVerticalLines() const;
    cg3::Segment2d getVerticalLine(size_t id) const;


    //metodi per creare e distruggere poligoni, derivanti dalle vertical line
    void addPolygon(cg3::Point2d p1, cg3::Point2d p2, cg3::Point2d p3, cg3::Point2d p4);
    std::unordered_map<Trapezoid, size_t> findTrapezoid(const Trapezoid t, bool& found);
    std::unordered_map<Trapezoid, size_t> findIndexedTrapezoid(const Trapezoid& indexedTrapezoid, bool& found);
    std::vector<Trapezoid> getTrapezoids() const;
    size_t trapezoidsNumber();


    //method to update dag e trapezoidal map
    void trapezoidalMapAlgorithm(cg3::Segment2d segment);

    //methods to find the trapezoidal in which the point p (the left endpoint of the segment) intersect
    std::vector<std::array<cg3::Point2d, 4>> followSegment(std::vector<Trapezoid> trapezoids, /*Dag*/ cg3::Segment2d segment);
    bool isToTheRight(cg3::Point2d p, cg3::Point2d rightP);
    cg3::Point2d findLeftp(Trapezoid t);
    cg3::Point2d findRightp(Trapezoid t);
    bool LiesAbove(cg3::Point2d p, cg3::Segment2d segment);

    //method to fill vectors of adjacency
    void InsertNeighbors(std::vector<Trapezoid> t, cg3::Segment2d segment);


    //methods to handle points
    cg3::Point2d createPoint(cg3::Point2d p, double x, double y);

    void clearMap();

private:
    std::vector<VerticalLines2D> verticalLines;
    std::unordered_map<VerticalLines2D, size_t> verticalLinesMap;
    cg3::BoundingBox2 boundingBox;

    std::vector<Trapezoid> trapezoids;
    std::unordered_map<Trapezoid, size_t> trapezoidsMap;
    std::unordered_map<cg3::Point2d, size_t> leftp;
    std::unordered_map<cg3::Point2d, size_t> rightp;
    std::unordered_map<cg3::Segment2d, size_t> top;
    std::unordered_map<cg3::Segment2d, size_t> bottom;
};

#endif // TRAPEZOIDALMAP_H
