#ifndef DAG_H
#define DAG_H

#include "data_structures/nodedag.h"


class Dag{

public:
    Dag();

    /*funzioni:
     * 1 . inizializzazione(primo trapezoide)
     * 2 . ricerca punto in trapezoide
     * 3 . ricerca segmento in trapezoidi
     * 4 . inserimento in singolo trapezoide
     * 5 . inserimento multi trapezoide
     */

    void inizializeDag(const std::array<cg3::Point2d, 4> boundingBox);

    void updateDag(const cg3::Segment2d segment);

    std::array<cg3::Point2d, 4> findTrapezoid(const cg3::Point2d point);

    std::vector<std::array<cg3::Point2d, 4>> findTrapezoids(const cg3::Segment2d segment);

    void insertSingleTrapezoid(const std::array<cg3::Point2d, 4>* trapezoid, const cg3::Segment2d segment);

    void insertMultipleTrapezoids(const std::vector<std::array<cg3::Point2d, 4>>* foundTrapezoids, const cg3::Segment2d segment);



    //auxiliary functions
    nodeDag* pointToPoint(class X *node ,const cg3::Point2d point);
    nodeDag* pointToSegment(class Y *node, const cg3::Point2d point);
    double determinant(const cg3::Segment2d seg, const cg3::Point2d point);


private:
    nodeDag* dag;

};
#endif // DAG_H
