#ifndef NODEDAG_H
#define NODEDAG_H


#include <cg3/geometry/point2.h>
#include <cg3/geometry/segment2.h>
#include <data_structures/Trapezoidalmap.h>


enum Types{X, Y, Leaf};


class nodeDag{

public:
    nodeDag() = default;

    virtual Types getType() const=0;

    nodeDag *getLeftChild() const;
    void setLeftChild(nodeDag *leftChild);
    nodeDag *getRightChild() const;
    void setRightChild(nodeDag *rightChild);

    nodeDag** getLeftChildPointer();
    nodeDag** getRightChildPointer();

    //auxiliary functions
    long double determinant(const cg3::Segment2d seg, const cg3::Point2d point);



    virtual ~nodeDag();

private:
    nodeDag *leftChildNode;
    nodeDag *rightChildNode;
};


class X: public nodeDag{

public:
    X() = default;
    explicit X(cg3::Point2d p);
    ~X() override = default;

    Types getType() const override;
    cg3::Point2d getPoint() const;
    void setPoint(cg3::Point2d point);
    nodeDag* pointToPoint(const cg3::Point2d point);


private:
    cg3::Point2d point;

};


class Y: public nodeDag{
public:
    Y() = default;
    explicit Y(cg3::Segment2d s);
    ~Y() override = default;

    Types getType() const override;
    cg3::Segment2d getSegment() const;
    void setSegment(cg3::Segment2d segment);
    nodeDag* pointToSegment(const cg3::Point2d point);

private:
    cg3::Segment2d segment;

};


class Leaf: public nodeDag{
    typedef std::array<cg3::Point2d, 4> Trapezoid;

public:
    Leaf() = default;
    explicit Leaf(Trapezoid t);
    ~Leaf() override = default;

    Types getType() const override;
    Trapezoid getTrapezoid() const;
    void setTrapezoid(Trapezoid trapezoid);

private:
    Trapezoid trapezoid;

};

#endif // NODEDAG_H
