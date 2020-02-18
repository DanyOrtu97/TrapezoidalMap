#include "dag.h"


template <class T>
Dag<T>::Dag():
    structureDag(nullptr),
    trapezoidIndex(0)
{

}


template <class T>
void Dag<T>::insertNewSegment(cg3::Segment2d segment){
    int trapezoid;
    if (structureDag == nullptr){
       insertSubTree(segment, 1, structureDag);
    }
    else{
        node *insertion;
        trapezoid = findTrapezoidByPoint(segment.p1());
        for (T el: structureDag){
            if(el == trapezoid){
                insertion = *el;
            }
        }
        insertSubTree(segment, trapezoid, insertion);
    }

}


template <class T>
void Dag<T>::insertSubTree(cg3::Segment2d segment, int indice, node *root){
    cg3::Point2d p1 = segment.p1();
    cg3::Point2d p2 = segment.p2();

    root->value = p1;
    root->leftChild = indice;
    trapezoidIndex++;
    root->rightChild = p2;

    node *rr = root->rightChild;
    node *rl = root->rightChild;

    rl->leftChild = segment;

    node *rlr = rl->rightChild;
    node *rll = rl->leftChild;

    rlr->leftChild = trapezoidIndex++;
    rll->rightChild = trapezoidIndex++;

    rr->rightChild = trapezoidIndex++;
}


template <class T>
int Dag<T>::findTrapezoidByPoint(cg3::Point2d){
    int result=0;
    for (T el: structureDag){
        //code to retrieve when point2d is inside poligon
        //prova per eliminare warning
        if(el == 3){
            result = el;
        }
    }
    return result;
}


template <class T>
int Dag<T>::getNumberOfTrapezoids(){
    return trapezoidIndex;
}

template <class T>
void Dag<T>::printDag(){
    for (T el: structureDag){
        std::cout<< el << ", " <<std::endl;
    }
}

template <class T>
void Dag<T>::clearDag(){
    structureDag = nullptr;
}
