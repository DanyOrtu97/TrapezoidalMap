#ifndef DAG_H
#define DAG_H

#include "data_structures/nodedag.h"
#include <data_structures/trapezoidalMap.h>
#include <data_structures/trapezoid.h>


class Dag{

public:
    typedef std::array<cg3::Point2d, 4> Trapezoid;

    Dag();

    void inizializeDag(trapezoid& boundingBox);

    //Methods to find trapezoids, single and multiple, the first is used also for the point location query
    trapezoid findTrapezoid(const cg3::Point2d point, const cg3::Point2d auxiliaryPoint, bool queryPoint, nodeDag*& root);
    void findTrapezoids(const cg3::Segment2d segment, nodeDag*& node, nodeDag*& temp);

    //Methods to update the dag
    void updateDag(const cg3::Segment2d segment);
    void insertSingleTrapezoid(const cg3::Segment2d segment);
    void insertMultipleTrapezoids(const cg3::Segment2d segment);
    void setTrapezoidToInsert(trapezoid& trap, int num);
    void multipleReferences(nodeDag*& node, trapezoid& trap, nodeDag*& segment1b, nodeDag*& point2, cg3::Point2d& p2);

    //getter
    nodeDag*& getDag();
    std::map<trapezoid, nodeDag**>& getPointerMap();

    void clearDag();
    void clearTraps();

    //rules of three and five
    ~Dag();
    Dag& operator = (Dag other);
    void swap(Dag& other);
    Dag (Dag&& in);
    Dag& operator = (Dag&& in);

private:
    int nTrapezoids;
    nodeDag* dag;

    std::vector<std::reference_wrapper<trapezoid>> traps;
    std::map<trapezoid, nodeDag**> pointersMap;
    std::map<long int, std::map<int, nodeDag**>> multipleAdresses;

};
#endif // DAG_H
