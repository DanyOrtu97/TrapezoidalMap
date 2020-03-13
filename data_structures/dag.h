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

    std::array<cg3::Point2d, 4> findTrapezoid(const cg3::Point2d point, bool queryPoint);

    std::vector<std::array<cg3::Point2d, 4>> findTrapezoids(const cg3::Segment2d segment);

    void insertSingleTrapezoid(const cg3::Segment2d segment);

    void insertMultipleTrapezoids(const cg3::Segment2d segment);

    void setTrapezoidToInsert (std::vector<std::array<cg3::Point2d, 4>> traps);

    nodeDag* getDag();


private:
    nodeDag* dag;
    std::vector<std::array<cg3::Point2d, 4>> traps;
    std::map<std::array<cg3::Point2d, 4>, nodeDag**> pointersMap;

};
#endif // DAG_H
