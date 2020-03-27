#include "algorithms.h"

#include <data_structures/dag.h>


namespace GasAlgorithms {

    /**
     * @brief Method to find the split node in the dag structure
     * @param[in] segment Segment
     * @param[out] splitNode nodeDag*
     */
    nodeDag* findSplitNode(const cg3::Segment2d segment, nodeDag* dag){
        nodeDag* leftPoint = dag;
        nodeDag* rightPoint = dag;
        nodeDag* parent = nullptr;

        while((leftPoint->getType() != "PK4Leaf" || rightPoint->getType() != "PK4Leaf") && leftPoint == rightPoint){
            parent = leftPoint;

            /*
             * left endpoint
             */
            if(leftPoint->getType() == "PK1X"){

                leftPoint = *((X*)leftPoint)->pointToPoint(segment.p1());

            }
            else if(leftPoint->getType() == "PK1Y"){

                leftPoint = *((Y*)leftPoint)->pointToSegment(segment.p1());
            }

            /*
             * right endpoint
             */
            if(rightPoint->getType() == "PK1X"){

                rightPoint = *((X*)rightPoint)->pointToPoint(segment.p2());

            }
            else if(rightPoint->getType() == "PK1Y"){

                rightPoint = *((Y*)rightPoint)->pointToSegment(segment.p2());
            }
        }

        return parent;
    }


    /**
     * @brief Method to create a point
     * @param[in] p Point (old point), x and y double
     * @param[out] p Point (modified point)
     */
    cg3::Point2d createPoint(cg3::Point2d p, double x, double y){
        p.setXCoord(x);
        p.setYCoord(y);
        return p;
    }

    /**
     * @brief Method to handle the slope of segment (y coordinate)
     * @param[in] s Segment, p Point
     * @param[out] The y-coordinate of the segment computed y the point p (the projection of the point p in the segment s)
     */
    double handleSlopeSegment(cg3::Segment2d s, cg3::Point2d p){
        return (((p.x() - s.p1().x())/(s.p2().x() - s.p1().x()))*(s.p2().y() - s.p1().y()))+s.p1().y();
    }

    /**
     * @brief Method to handle the degerated cases (triangles)
     * @param[in] t Trapezoid
     * @param[out] true if the trapezoid is a triangle, false otherwise
     */
    bool degeneratedTrapezoid(const Trapezoid t){
        if(t[0] == t[1]){
            return true;
        }
        else if(t[1] == t[2]){
            return true;
        }
        else if(t[2] == t[3]){
            return true;
        }
        else if(t[3] == t[0]){
            return true;
        }
        else{
            return false;
        }
    }

    /**
     * @brief Method to handle the degerated cases (triangles) by find the 3 points of the Trapezoid
     * @param[in] t Trapezoid
     * @param[out] triangle Triangle formed by the input trapezoid
     */
    std::array<cg3::Point2d, 3> findTriangleByQuad(const Trapezoid t){
        std::array<cg3::Point2d, 3> triangle;
        if(t[0] == t[1]){
            triangle = {t[1], t[2], t[3]};
        }
        else if(t[1] == t[2]){
            triangle = {t[2], t[3], t[0]};
        }
        else if(t[2] == t[3]){
            triangle = {t[3], t[0], t[1]};
        }
        else if(t[3] == t[0]){
            triangle = {t[0], t[1], t[2]};
        }
        return triangle;
    }




} //namespace GasAlgorithms
