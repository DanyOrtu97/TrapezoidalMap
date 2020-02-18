#ifndef DAG_H
#define DAG_H

#include <unordered_map>
#include <vector>
#include <utility>

#include <cg3/geometry/point2.h>
#include <cg3/geometry/segment2.h>
#include <cg3/geometry/polygon2.h>


template <class T>
class Dag{

public:
    Dag();


    struct node {
        T value;
        struct node *leftChild;
        struct node *rightChild;
    };

    typedef std::pair<cg3::Point2d, cg3::Point2d> Segments;
    typedef cg3::Point2d Endpoints;


    void insertNewSegment(cg3::Segment2d segment);

    void insertSubTree(cg3::Segment2d segment, int indice, node *root);

    //da regolare il tipo (deve rendere un poligono, quadrante che contiene il punto)
    int findTrapezoidByPoint(cg3::Point2d);

    int getNumberOfTrapezoids();

    void clearDag();



private:
    std::vector<Segments> segments;
    std::vector<Endpoints> endpointsLeft;
    std::vector<Endpoints> endpointsRight;
    std::vector<int> leaves;

    int trapezoidIndex;

    node *structureDag;
};

#endif // DAG_H
