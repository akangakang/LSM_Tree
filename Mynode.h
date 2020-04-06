//
// Created by LSC on 2020/3/18.
//
#include <cstdint>
#include <string>
#ifndef PROJECT1_KVSTORE_MYNODE_H
#define PROJECT1_KVSTORE_MYNODE_H
#define QuadlistnodeP Quadlistnode*


struct Entry{
    uint64_t key; std::string  value;
    Entry(){key=0;value="";}
    Entry(uint64_t k , std::string v ) :key(k), value(v){}
    Entry(const Entry& e) :key(e.key), value(e.value){}
    bool operator<(const Entry& e) { return key < e.key; }
    bool operator>(const Entry& e) { return key > e.key;  }
    bool operator==(const Entry& e) { return key == e.key;  }
    bool operator!=(const Entry& e) { return key != e.key; }
};


struct Quadlistnode{
    Entry entry;
    bool del;
    QuadlistnodeP pred;
    QuadlistnodeP succ;
    QuadlistnodeP above;
    QuadlistnodeP below;
    Quadlistnode(Entry e=Entry(),QuadlistnodeP p= nullptr,QuadlistnodeP s=NULL,QuadlistnodeP a=NULL,QuadlistnodeP b=NULL,bool d= false):entry(e),pred(p),succ(s),above(a),below(b),del(d){}
    QuadlistnodeP insertAsSuccAbove(Entry const& e, QuadlistnodeP b=NULL,bool d= false)
    {
        QuadlistnodeP x=new Quadlistnode(e,this,succ,nullptr,b,d);
        succ->pred = x;
        succ =x;
        if(b) b->above =x;
        return x;
    }
};

#endif //PROJECT1_KVSTORE_MYNODE_H
