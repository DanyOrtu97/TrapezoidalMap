#ifndef DAG_H
#define DAG_H

#include <unordered_map>
#include <vector>
#include <utility>

#include <cg3/geometry/point2.h>
#include <cg3/geometry/segment2.h>
#include <cg3/geometry/polygon2.h>


class Dag{

public:
    Dag();


    struct node {
        int value; //da modificare come generico
        struct node *leftChild;
        struct node *rightChild;
    };

    typedef std::pair<cg3::Point2d, cg3::Point2d> Segments;
    typedef cg3::Point2d Endpoints;


    void insertNewSegment(cg3::Segment2d segment);


    //da regolare il tipo (deve rendere un poligono, quadrante che contiene il punto)
    void findPoint(cg3::Point2d);


    void clearDag();



private:
    std::vector<Segments> segments;
    std::vector<Endpoints> endpointsLeft;
    std::vector<Endpoints> endpointsRight;
    std::vector<int> leaves;

    //non sono sicuro
    std::vector<node> structureDag;
};

#endif // DAG_H
