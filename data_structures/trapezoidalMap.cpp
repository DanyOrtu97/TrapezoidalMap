#include "trapezoidalMap.h"
#include "dag.h"

#include <drawables/drawableTrapezoidalMap.h>

cg3::Point2d topLeft;
cg3::Point2d bottomLeft;
cg3::Point2d topRight;
cg3::Point2d bottomRight;
Dag dag;

TrapezoidalMap::TrapezoidalMap() :
    boundingBox(cg3::Point2d(0,0),cg3::Point2d(0,0))

{

}

void TrapezoidalMap::inizialize(){

    topLeft = createPoint(topLeft, -(1e+6), (1e+6));
    bottomLeft = createPoint(bottomLeft, -(1e+6), -(1e+6));
    topRight = createPoint(topRight, (1e+6), (1e+6));
    bottomRight = createPoint(bottomRight, (1e+6), -(1e+6));

    //inizialize the trapezoidal map with the bounding box
    addPolygon(topLeft, topRight, bottomRight, bottomLeft);

    std::vector<std::array<cg3::Point2d, 4>> trapezoids;
    std::array<cg3::Point2d, 4> trapezoid = {topLeft, topRight, bottomRight, bottomLeft};
    trapezoids.push_back(trapezoid);
    cg3::Segment2d seg;

    InsertNeighbors(trapezoids, seg ,0);

    //inizialize the dag with the pointer of the first polygon (the bounding box)
    dag.inizializeDag(trapezoids.front());

}


void TrapezoidalMap::trapezoidalMapAlgorithm(cg3::Segment2d segment){

    dag.updateDag(segment);

    std::vector<std::array<cg3::Point2d, 4>> foundTrapezoids = followSegment(trapezoids, /*dag*/ segment);

    std::map<Trapezoid, size_t> delet;

    std::map<cg3::Segment2d, size_t> deletSeg;

    bool found;

    std::cout<<"Trapezoidi trovati: "<<std::endl;
    std::cout << foundTrapezoids.size() << std::endl;


    //modificare trapezoidi trovati e inserire i nuovi
    /*
      Casi:
      1 - completamente dentro un trapezoide
      2 - due o più trapezoidi
      3 -
      */

    //caso 1
    if (foundTrapezoids.size() == 1){
        //tra.clear();
        delet.clear();
        deletSeg.clear();

        delet = findTrapezoid(foundTrapezoids.front(), found);
        deletSeg.insert(std::make_pair(cg3::Segment2d(delet.begin()->first[0], delet.begin()->first[1]), delet.begin()->second));

        trapezoidsMap.erase(delet.begin()->first);

        update();

        //Inserisci e/o aggiorna vicini...
        CompletelyInsideTrapezoid(foundTrapezoids.front(), segment);

        leftpMap.erase(delet.begin()->second);
        rightpMap.erase(delet.begin()->second);
        topMap.erase(deletSeg.begin()->first);
        bottomMap.erase(deletSeg.begin()->first);

        InsertNeighbors(trapezoids, segment, 1);
    }
    else{
        for (Trapezoid t : foundTrapezoids){
            delet.clear();
            deletSeg.clear();

            delet = findTrapezoid(t, found);
            deletSeg.insert(std::make_pair(cg3::Segment2d(delet.begin()->first[0], delet.begin()->first[1]), delet.begin()->second));

            trapezoidsMap.erase(delet.begin()->first);

            update();

            multipleTrapezoid(trapezoids, t, segment);

            leftpMap.erase(delet.begin()->second);
            rightpMap.erase(delet.begin()->second);
            topMap.erase(deletSeg.begin()->first);
            bottomMap.erase(deletSeg.begin()->first);

        }    

        //Inserisci e/o aggiorna vicini...
        InsertNeighbors(trapezoids, segment, 2);
    }
}


void TrapezoidalMap::CompletelyInsideTrapezoid(Trapezoid t, const cg3::Segment2d& segment){
    topLeft = t[0];
    bottomLeft = t[3];
    topRight = t[1];
    bottomRight = t[2];

    cg3::Segment2d topS;
    topS.set(topLeft, topRight);
    cg3::Segment2d bottomS;
    bottomS.set(bottomLeft, bottomRight);

    cg3::Point2d p1 = segment.p1();
    cg3::Point2d p2 = segment.p2();
    cg3::Point2d upfirst;
    cg3::Point2d upsecond;
    cg3::Point2d downfirst;
    cg3::Point2d downsecond;

    std::map<cg3::Segment2d, size_t>::iterator top = topMap.find(topS);
    std::map<cg3::Segment2d, size_t>::iterator bottom = bottomMap.find(bottomS);


    //Segment already in the data structure (top -> 1e+6, bottom -> -1e+6 )
    if (top != topMap.end()) {
        //std::cout<<"trova top mappa:"<<std::endl;
       // std::cout<< top->first.p1().y() << " " << top->first.p2().y()<<std::endl;

        upfirst = createPoint(upfirst, p1.x(), handleSlopeSegment(top->first, p1));
        upsecond = createPoint(upsecond, p2.x(), handleSlopeSegment(top->first, p2));
    }

    if (bottom != bottomMap.end()) {
        //std::cout<<"trova bottom mappa:"<<std::endl;
        //std::cout<< bottom->first.p1().y() << " " << bottom->first.p2().y()<<std::endl;

        downfirst = createPoint(downfirst, p1.x(), handleSlopeSegment(bottom->first, p1));
        downsecond = createPoint(downsecond, p2.x(), handleSlopeSegment(bottom->first, p2));
    }

    //AGGIUNTA POLIGONI
    addPolygon(topLeft, upfirst, downfirst, bottomLeft); //A
    addPolygon(upfirst, upsecond, p2, p1); //B
    addPolygon(p1, p2, downsecond, downfirst); //C
    addPolygon(upsecond, topRight, bottomRight, downsecond); //D
}


void TrapezoidalMap::multipleTrapezoid(std::vector<Trapezoid> trapezoids, Trapezoid t, const cg3::Segment2d& segment){

}

//verify and return the trapezoid in which intersect the segment
std::vector<std::array<cg3::Point2d, 4>> TrapezoidalMap::followSegment(std::vector<Trapezoid> trapezoids, /*Dag*/ cg3::Segment2d segment){
    cg3::Point2d p1 = segment.p1();
    cg3::Point2d p2 = segment.p2();


    std::vector<std::array<cg3::Point2d, 4>> foundTrapezoids;

    std::array<cg3::Point2d, 4> firstTrapezoid = findTrapezoid(p1, trapezoids);
    std::array<cg3::Point2d, 4> lastTrapezoid = findTrapezoid(p2, trapezoids);

    int j = findIndexedTrapezoid(p1, trapezoids);

    std::cout << "\n indice primo trapezoide" <<std::endl;
    std::cout << j <<std::endl;
    std::cout << firstTrapezoid[0] << " " << firstTrapezoid[1] << " " << firstTrapezoid[2] << " "
                                   << firstTrapezoid[3] <<std::endl;


    //puttanaio da rivedere
    /*
    if(trapezoids.size() > 1){
        foundTrapezoids.push_back(firstTrapezoid);
        while(isToTheRight(p2, findRightp(trapezoids[j])) && j < (int)trapezoids.size()-1){
            if(LiesAbove(findRightp(trapezoids[j]), segment)){
                j++;
                foundTrapezoids.push_back(trapezoids[j]);
                j++;
                std::cout<< "a"<<std::endl;
            }
            else{
                if(j+1 == (int)trapezoids.size()-1){
                    j++;
                    foundTrapezoids.push_back(trapezoids[j]);
                    j++;
                    std::cout<< "b"<<std::endl;
                }
                else if (j+1 < (int)trapezoids.size()-1){
                    j+=2;
                    foundTrapezoids.push_back(trapezoids[j]);
                    std::cout<< "c"<<std::endl;
                }

            }
            std::cout << j <<std::endl;
        }
    }
    else{
        foundTrapezoids.push_back(firstTrapezoid);
    }
    */
    if(firstTrapezoid == lastTrapezoid){
        foundTrapezoids.push_back(firstTrapezoid);
    }else{
        if(trapezoids.size() == 4){
            foundTrapezoids.push_back(firstTrapezoid);
            while(isToTheRight(p2, findRightp(trapezoids[j])) && j < (int)trapezoids.size()-1){
                if(LiesAbove(findRightp(trapezoids[j]), segment)){
                    j++;
                    foundTrapezoids.push_back(trapezoids[j]);
                    j++;
                    std::cout<< "a"<<std::endl;
                }
                else{
                    if(j+1 == (int)trapezoids.size()-1){
                        j++;
                        foundTrapezoids.push_back(trapezoids[j]);
                        j++;
                        std::cout<< "b"<<std::endl;
                    }
                    else if (j+1 < (int)trapezoids.size()-1){
                        j+=2;
                        foundTrapezoids.push_back(trapezoids[j]);
                        std::cout<< "c"<<std::endl;
                    }

                }
                std::cout << j <<std::endl;
            }
        }
        else{
            foundTrapezoids.push_back(firstTrapezoid);
            foundTrapezoids.push_back(lastTrapezoid);
        }
    }


    return foundTrapezoids;
}

bool TrapezoidalMap::isToTheRight(cg3::Point2d p, cg3::Point2d rightP){
    if (p.x() >= rightP.x()){
        return true;
    }
    else{
        return false;
    }
}

cg3::Point2d TrapezoidalMap::findLeftp(Trapezoid t){
    bool found;
    size_t idL = findTrapezoid(t, found).begin()->second;
    return leftpMap[idL];
}

cg3::Point2d TrapezoidalMap::findRightp(Trapezoid t){
    bool found;
    size_t idR = findTrapezoid(t, found).begin()->second;
    return rightpMap[idR];
}

bool TrapezoidalMap::LiesAbove(cg3::Point2d p, cg3::Segment2d segment){

    /*int det;
    det = ((segment.p1().x() * ((segment.p2().y() * 1) - (1 * p.y())))-
           (segment.p1().y() * ((segment.p2().x() * 1) - (1 * p.x())))+
           (1 * ((segment.p2().x() * p.y()) - (segment.p2().y() * p.x()))));
    std::cout<< det << std::endl;
    if(det<0){*/
    if(segment.p1().y() <= p.y() && segment.p2().y() <= p.y()){
        return true;
    }
    else{
        return false;
    }
}

void TrapezoidalMap::InsertNeighbors(std::vector<Trapezoid> t, cg3::Segment2d segment, int cases){
    cg3::Segment2d top;
    cg3::Segment2d bottom;
    cg3::Point2d leftp;
    cg3::Point2d rightp;

    size_t id2;

    switch (cases) {
        case 0:
            //init
            leftp.set(t[0][0].x(), t[0][0].y());
            rightp.set(t[0][1].x(), t[0][1].y());
            top.set(t[0][0], t[0][1]);
            bottom.set(t[0][3], t[0][2]);

            id2 = std::numeric_limits<size_t>::max();

            topMap.insert(std::make_pair(cg3::Segment2d(top), id2));
            bottomMap.insert(std::make_pair(cg3::Segment2d(bottom), id2));
            leftpMap.insert(std::make_pair(cg3::Point2d(leftp), id2));
            rightpMap.insert(std::make_pair(cg3::Point2d(rightp), id2));

            break;
        case 1:
            static int index=0;
            for (Trapezoid trap : t){
                top.set(trap[0], trap[1]);
                bottom.set(trap[3], trap[2]);

                if(index == 0){
                    leftp.set(trap[0].x(), trap[0].y());
                    rightp.set(segment.p1().x(), segment.p1().y());
                }
                else if(index == (int)t.size()-1){
                    leftp.set(segment.p2().x(), segment.p2().y());
                    rightp.set(trap[1].x(), trap[1].y());
                }
                else{
                    leftp.set(segment.p1().x(), segment.p1().y());
                    rightp.set(segment.p2().x(), segment.p2().y());
                }

                id2 = std::numeric_limits<size_t>::max();

                topMap.insert(std::make_pair(cg3::Segment2d(top), id2));
                bottomMap.insert(std::make_pair(cg3::Segment2d(bottom), id2));
                leftpMap.insert(std::make_pair(cg3::Point2d(leftp), id2));
                rightpMap.insert(std::make_pair(cg3::Point2d(rightp), id2));

                index++;
            }
            break;
        case 2:
            //caso più trapezoidi

            id2 = std::numeric_limits<size_t>::max();
            break;
    }
}


void TrapezoidalMap::addPolygon(cg3::Point2d p1, cg3::Point2d p2, cg3::Point2d p3, cg3::Point2d p4){
    size_t id;

    std::array<cg3::Point2d, 4> trapezoid = {p1, p2, p3, p4};

    id = std::numeric_limits<size_t>::max();

    //trapezoids.push_back(Trapezoid(trapezoid));

    trapezoidsMap.insert(std::make_pair(Trapezoid(trapezoid), id));

    update();
}

std::map<std::array<cg3::Point2d, 4>, size_t> TrapezoidalMap::findTrapezoid(const Trapezoid t, bool& found){
    found = false;
    return findIndexedTrapezoid(Trapezoid(t), found);
}

std::map<std::array<cg3::Point2d, 4>, size_t> TrapezoidalMap::findIndexedTrapezoid(const Trapezoid& indexedTrapezoid, bool& found){
    std::map<Trapezoid, size_t>::iterator it = trapezoidsMap.find(indexedTrapezoid);

    std::map<std::array<cg3::Point2d, 4>, size_t> map;

    //Segment already in the data structure
    if (it != trapezoidsMap.end()) {
        found = true;
        map.insert(std::make_pair(it->first, it->second));
    }
    return map;
}

std::vector<std::array<cg3::Point2d, 4>> TrapezoidalMap::getTrapezoids() const{
    return trapezoids;
}

cg3::Point2d TrapezoidalMap::createPoint(cg3::Point2d p, double x, double y){
    p.setXCoord(x);
    p.setYCoord(y);
    return p;
}

double TrapezoidalMap::handleSlopeSegment(cg3::Segment2d s, cg3::Point2d p){
    return (((p.x() - s.p1().x())/(s.p2().x() - s.p1().x()))*(s.p2().y() - s.p1().y()))+s.p1().y();
}

void TrapezoidalMap::update(){
    trapezoids.clear();
    for (std::map<Trapezoid, size_t>::iterator it=trapezoidsMap.begin(); it!=trapezoidsMap.end(); ++it){
        trapezoids.push_back(it->first);
    }
}

std::array<cg3::Point2d, 4> TrapezoidalMap::findTrapezoid(cg3::Point2d p1, std::vector<Trapezoid> trapezoids){
    std::array<cg3::Point2d, 4> firstTrapezoid;

    //ricerca del trapezoide in cui é p1 che andrà sostituita con la ricerca nel DAG
    //migliorare se capita in un punto della vertical line
    for (Trapezoid trap : trapezoids){
        if (trap[0].x() <= p1.x() && trap[0].y() >= p1.y() &&
            trap[1].x() >= p1.x() && trap[1].y() >= p1.y() &&
            trap[2].x() >= p1.x() && trap[2].y() <= p1.y() &&
            trap[3].x() <= p1.x() && trap[3].y() <= p1.y()){
            firstTrapezoid=trap;
        }
    }
    return firstTrapezoid;
}

int TrapezoidalMap::findIndexedTrapezoid(cg3::Point2d p1, std::vector<Trapezoid> trapezoids){
    int index=0, j=0;

    //ricerca del trapezoide in cui é p1 che andrà sostituita con la ricerca nel DAG
    //migliorare se capita in un punto della vertical line
    for (Trapezoid trap : trapezoids){
        if (trap[0].x() <= p1.x() && trap[0].y() >= p1.y() &&
            trap[1].x() >= p1.x() && trap[1].y() >= p1.y() &&
            trap[2].x() >= p1.x() && trap[2].y() <= p1.y() &&
            trap[3].x() <= p1.x() && trap[3].y() <= p1.y()){
            j = index;
        }
        index++;
    }
    return j;
}

void TrapezoidalMap::queryPoint(cg3::Point2d point){
    found = dag.findTrapezoid(point);
    //dag.findTrapezoid(point);
    //findTrapezoid(point, trapezoids);
}

std::array<cg3::Point2d, 4> TrapezoidalMap::getFoundTrapezoid() const{
    std::cout<< found.begin()[0] <<std::endl;
    return found;
}


/**
 * @brief Cleared structures for the trapezoidal map
 * @param[in]
 */
void TrapezoidalMap::clearMap()
{

    boundingBox.setMin(cg3::Point2d(0,0));
    boundingBox.setMax(cg3::Point2d(0,0));
    trapezoids.clear();
    trapezoidsMap.clear();
    rightpMap.clear();
    leftpMap.clear();
    topMap.clear();
    bottomMap.clear();
}


