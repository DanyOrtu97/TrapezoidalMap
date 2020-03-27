#ifndef ALGORITHMS_H
#define ALGORITHMS_H

#include <data_structures/nodedag.h>

namespace GasAlgorithms {
    typedef std::array<cg3::Point2d, 4> Trapezoid;

    /*
     * dag algorithms
     */
    nodeDag* findSplitNode(const cg3::Segment2d segment, nodeDag* dag);


    /*
     * trapezoidal map algorithms
     */
    cg3::Point2d createPoint(cg3::Point2d p, double x, double y);
    double handleSlopeSegment(cg3::Segment2d s, cg3::Point2d p);

    bool degeneratedTrapezoid(const Trapezoid t);
    std::array<cg3::Point2d, 3> findTriangleByQuad(const Trapezoid trap);



} //namespace GasAlgorithms

#endif // ALGORITHMS_H


