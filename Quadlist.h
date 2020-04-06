//
// Created by LSC on 2020/3/18.
//
#include "Mynode.h"

#ifndef PROJECT1_KVSTORE_QUADLIST_H
#define PROJECT1_KVSTORE_QUADLIST_H


class Quadlist {
private:
    int _size;
    QuadlistnodeP header;
    QuadlistnodeP trailer;
    void init();
    int clear();

public:

    Quadlist(){init();}
    ~Quadlist(){clear();delete header; delete trailer;}

    int size() const {return _size;}
    bool empty(){return _size<=0;}
    QuadlistnodeP first(){return header->succ;}
    QuadlistnodeP last(){ return trailer->pred;}
    bool valid(QuadlistnodeP p){return p&&(trailer!=p)&&(header!=p);}

    Entry remove(QuadlistnodeP p);
    QuadlistnodeP insertAfterAbove(Entry const& e,QuadlistnodeP p,QuadlistnodeP b=nullptr,bool d= false);


};


#endif //PROJECT1_KVSTORE_QUADLIST_H
