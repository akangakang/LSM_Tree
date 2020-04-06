//
// Created by LSC on 2020/3/18.
//
#include <list>
#include "Quadlist.h"
#include <iostream>
#ifndef PROJECT1_KVSTORE_SKIPLIST_H
#define PROJECT1_KVSTORE_SKIPLIST_H

using namespace std;
class Skiplist: public list<Quadlist*> {


public:
   // unsigned int big=0;
    bool skipSearch(list::iterator &qlist,Quadlistnode * &p,uint64_t k);
    bool skipSearch1(list::iterator &qlist,Quadlistnode * &p,uint64_t k);
    Quadlist* top();
    Quadlist* bottom();
    int level();
    int size();
    bool empty();//全部层都空了  也就是一层都没有了
    int cap();

    bool put(uint64_t,const std::string &,bool d= false);
    std::string * get(uint64_t k);
    bool has_key(uint64_t k,bool & del,std::string &s);
    bool remove(uint64_t k);
    bool remove2(uint64_t k);
    bool read_and_remove_first(uint64_t &k,std::string &s,bool & d);
  bool my_clear();



};


#endif //PROJECT1_KVSTORE_SKIPLIST_H
