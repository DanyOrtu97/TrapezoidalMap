#ifndef NODEDAG_H
#define NODEDAG_H


#include <cg3/geometry/point2.h>
#include <cg3/geometry/segment2.h>
#include <data_structures/trapezoid.h>

class nodeDag{

public:
    nodeDag() = default;

    virtual std::string  getType() const=0;

    nodeDag *getLeftChild() const;
    nodeDag*& getLeftChildRef();
    void setLeftChild(nodeDag *leftChild);

    nodeDag *getRightChild() const;
    nodeDag*& getRightChildRef();
    void setRightChild(nodeDag *rightChild);

    nodeDag **getLeftChildP();
    nodeDag **getRightChildP();

    long double determinant(const cg3::Segment2d seg, const cg3::Point2d point);

    //rules of three and five
    virtual ~nodeDag();
    nodeDag& operator = (nodeDag& other);
    nodeDag (nodeDag&& in);
    nodeDag& operator = (nodeDag&& in);

protected:
    nodeDag *leftChildNode;
    nodeDag *rightChildNode;
};


class X: public nodeDag{

public:
    X() = default;
    explicit X(cg3::Point2d p, bool right);
    ~X() override = default;

    std::string  getType() const override;
    cg3::Point2d getPoint() const;
    void setPoint(cg3::Point2d point);
    nodeDag** pointToPoint(const cg3::Point2d point, const cg3::Point2d point2);

    bool getValueEndpoint();
    void setValueEndpoint(bool value);

private:
    cg3::Point2d point;
    bool rightEndpoint;
};


class Y: public nodeDag{
public:
    Y() = default;
    explicit Y(cg3::Segment2d s);
    ~Y() override = default;

    std::string  getType() const override;
    cg3::Segment2d getSegment() const;
    void setSegment(cg3::Segment2d segment);
    nodeDag** pointToSegment(const cg3::Point2d point, const cg3::Point2d point2);

private:
    cg3::Segment2d segment;
};


class Leaf: public nodeDag{

public:
    //Leaf() = default;
    explicit Leaf(trapezoid& t);
    ~Leaf() override = default;

    std::string  getType() const override;
    trapezoid& getTrapezoid();
    void setTrapezoid(trapezoid& trapezoid);

private:
    trapezoid& trapezoide;
};

#endif // NODEDAG_H
