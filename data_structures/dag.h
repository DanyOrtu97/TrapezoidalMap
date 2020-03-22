#ifndef DAG_H
#define DAG_H

#include "data_structures/nodedag.h"
#include <data_structures/trapezoidalMap.h>


class Dag{

public:
    Dag();

    void inizializeDag(const std::array<cg3::Point2d, 4> boundingBox);

    void updateDag(const cg3::Segment2d segment);

    std::array<cg3::Point2d, 4> findTrapezoid(const cg3::Point2d point, bool queryPoint, nodeDag* root);

    void findTrapezoids(const cg3::Segment2d segment, nodeDag* node, nodeDag* temp);

    nodeDag* findSplitNode(const cg3::Segment2d segment);

    void insertSingleTrapezoid(const cg3::Segment2d segment);

    void insertMultipleTrapezoids(const cg3::Segment2d segment, nodeDag* splitNode);

    void setTrapezoidToInsert (std::array<cg3::Point2d, 4> trap, int num);

    nodeDag* getDag();

    void addMapElement(const std::pair<std::array<cg3::Point2d, 4>, nodeDag**> element);

    std::map<std::array<cg3::Point2d, 4>, nodeDag**> getPointerMap();

    void clearDag();

    void clearTraps();


private:
    int nTrapezoids;
    nodeDag* dag;
    std::vector<std::array<cg3::Point2d, 4>> traps;
    std::map<std::array<cg3::Point2d, 4>, nodeDag**> pointersMap;

};
#endif // DAG_H
