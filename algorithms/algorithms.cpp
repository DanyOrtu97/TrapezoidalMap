#include "algorithms.h"

#include <data_structures/dag.h>
#include <data_structures/trapezoid.h>


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

                leftPoint = *((X*)leftPoint)->pointToPoint(segment.p1(), segment.p2());

            }
            else if(leftPoint->getType() == "PK1Y"){

                leftPoint = *((Y*)leftPoint)->pointToSegment(segment.p1(), segment.p2());
            }

            /*
             * right endpoint
             */
            if(rightPoint->getType() == "PK1X"){

                rightPoint = *((X*)rightPoint)->pointToPoint(segment.p2(), segment.p1());

            }
            else if(rightPoint->getType() == "PK1Y"){

                rightPoint = *((Y*)rightPoint)->pointToSegment(segment.p2(), segment.p1());
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
    bool degeneratedTrapezoid(trapezoid t){
        if(round(t.getTrapezoid()[0].x()) == round(t.getTrapezoid()[1].x()) && round(t.getTrapezoid()[0].y()) == round(t.getTrapezoid()[1].y())){
            return true;
        }
        else if(round(t.getTrapezoid()[1].x()) == round(t.getTrapezoid()[2].x()) && round(t.getTrapezoid()[1].y()) == round(t.getTrapezoid()[2].y())){
            return true;
        }
        else if(round(t.getTrapezoid()[2].x()) == round(t.getTrapezoid()[3].x()) && round(t.getTrapezoid()[2].y()) == round(t.getTrapezoid()[3].y())){
            return true;
        }
        else if(round(t.getTrapezoid()[3].x()) == round(t.getTrapezoid()[0].x()) && round(t.getTrapezoid()[3].y()) == round(t.getTrapezoid()[0].y())){
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
    std::array<cg3::Point2d, 3> findTriangleByQuad(trapezoid t){
        std::array<cg3::Point2d, 3> triangle;
        if(round(t.getTrapezoid()[0].x()) == round(t.getTrapezoid()[1].x()) && round(t.getTrapezoid()[0].y()) == round(t.getTrapezoid()[1].y())){
            triangle = {t.getTrapezoid()[1], t.getTrapezoid()[2], t.getTrapezoid()[3]};
        }
        else if(round(t.getTrapezoid()[1].x()) == round(t.getTrapezoid()[2].x()) && round(t.getTrapezoid()[1].y()) == round(t.getTrapezoid()[2].y())){
            triangle = {t.getTrapezoid()[2], t.getTrapezoid()[3], t.getTrapezoid()[0]};
        }
        else if(round(t.getTrapezoid()[2].x()) == round(t.getTrapezoid()[3].x()) && round(t.getTrapezoid()[2].y()) == round(t.getTrapezoid()[3].y())){
            triangle = {t.getTrapezoid()[3], t.getTrapezoid()[0], t.getTrapezoid()[1]};
        }
        else if(round(t.getTrapezoid()[3].x()) == round(t.getTrapezoid()[0].x()) && round(t.getTrapezoid()[3].y()) == round(t.getTrapezoid()[0].y())){
            triangle = {t.getTrapezoid()[0], t.getTrapezoid()[1], t.getTrapezoid()[2]};
        }
        return triangle;
    }




} //namespace GasAlgorithms
