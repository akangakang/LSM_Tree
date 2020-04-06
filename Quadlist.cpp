//
// Created by LSC on 2020/3/18.
//

#include "Quadlist.h"

void Quadlist::init() {
    header = new Quadlistnode();
    trailer = new Quadlistnode();
    header ->succ = trailer;
    header ->pred =nullptr;
    header ->above=nullptr;
    header ->below=nullptr;

    trailer ->pred=header;
    trailer ->below=nullptr;
    trailer ->above=nullptr;
    trailer ->succ=nullptr;

    _size=0;
}

int Quadlist::clear() {
    int old_size =_size;
    while (0<_size) remove(header->succ);
    return old_size;
}

Entry Quadlist::remove(Quadlistnode *p) {
    p->pred->succ=p->succ;
    p->succ->pred=p->pred;
    _size--;
    Entry e=p->entry;
    delete p;
    return e;
}

Quadlistnode * Quadlist::insertAfterAbove(Entry const &e, Quadlistnode *p, Quadlistnode *b,bool d) {
    _size++;
    return p->insertAsSuccAbove(e,b,d);
}
