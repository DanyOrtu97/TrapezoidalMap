#ifndef DAG_H
#define DAG_H

#include <unordered_map>
#include <vector>
#include <utility>

#include <cg3/geometry/point2.h>
#include <cg3/geometry/segment2.h>




class Dag{

public:
    Dag();

    struct node {
       /*nodo nodeValue;*/
        node *leftChild;
        node *rightChild;

    };


    void insertNewSegment(cg3::Segment2d segment);

    void insertSubTree(cg3::Segment2d segment, int indice, node *root);

    //da regolare il tipo (deve rendere un poligono, quadrante che contiene il punto)
    int findTrapezoidByPoint(cg3::Point2d);

    int getNumberOfTrapezoids();

    void printDag();

    void clearDag();



private:
    int trapezoidIndex;
    node *structureDag;
};

#endif // DAG_H

