#ifndef DAG_H
#define DAG_H

#include "data_structures/nodedag.h"
#include <data_structures/trapezoidalMap.h>
#include <data_structures/trapezoid.h>


class Dag{

public:
    Dag();

    void inizializeDag(const trapezoid boundingBox);
    void updateDag(const cg3::Segment2d segment);

    trapezoid findTrapezoid(const cg3::Point2d point, bool queryPoint, nodeDag* root);
    void findTrapezoids(const cg3::Segment2d segment, nodeDag* node, nodeDag* temp);

    void insertSingleTrapezoid(const cg3::Segment2d segment);
    void insertMultipleTrapezoids(const cg3::Segment2d segment, nodeDag* splitNode);

    void setTrapezoidToInsert(trapezoid trap, int num);

    nodeDag* getDag();

    std::map<trapezoid, nodeDag**> getPointerMap();

    void clearDag();
    void clearTraps();


private:
    int nTrapezoids;
    nodeDag* dag;
    std::vector<trapezoid> traps;
    std::map<trapezoid, nodeDag**> pointersMap;

};
#endif // DAG_H
