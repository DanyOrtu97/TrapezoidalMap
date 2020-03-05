#ifndef LEAF_H
#define LEAF_H

#include <vector>
#include <utility>
#include <cg3/geometry/point2.h>
#include <data_structures/dag.h>

class Leaf : public Dag{

public:
    std::array<cg3::Point2d, 4> *leaf;

};

#endif // LEAF_H
