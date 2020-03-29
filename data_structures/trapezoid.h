#ifndef TRAPEZOID_H
#define TRAPEZOID_H

#include <vector>
#include <utility>

#include <cg3/geometry/point2.h>
#include <cg3/geometry/segment2.h>


class trapezoid{
public:
    typedef std::array<cg3::Point2d, 4> Trapezoid;

    trapezoid();

    trapezoid(const cg3::Point2d p1, const cg3::Point2d p2, const cg3::Point2d p3, const cg3::Point2d p4,
              const cg3::Point2d left, const cg3::Point2d right, const cg3::Segment2d top, const cg3::Segment2d bottom);

    bool operator<(const trapezoid &other) const;

    bool operator==(const trapezoid &other) const;

    void setTrapezoid(Trapezoid trapezoid);
    Trapezoid getTrapezoid();

    void setLeftP(const cg3::Point2d left);
    cg3::Point2d getLeftP();

    void setRightP(const cg3::Point2d right);
    cg3::Point2d getRightP();

    void setTop(const cg3::Segment2d top);
    cg3::Segment2d getTop();

    void setBottom(const cg3::Segment2d bottom);
    cg3::Segment2d getBottom();


private:
    Trapezoid trapezoide;

    cg3::Point2d leftp;
    cg3::Point2d rightp;
    cg3::Segment2d tops;
    cg3::Segment2d bottoms;

    /*
    trapezoid* upleft;
    trapezoid* downleft;
    trapezoid* upright;
    trapezoid* downright;
    */

};

#endif // TRAPEZOID_H
