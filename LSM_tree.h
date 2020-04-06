//
// Created by LSC on 2020/3/30.
//

#ifndef PROJECT1_KVSTORE_LSM_TREE_H
#define PROJECT1_KVSTORE_LSM_TREE_H
//
#define MAX_SIZE 2097152
#define INDEX_BEGIN 2097152
//#define MAX_SIZE 33
//#define INDEX_BEGIN 33
#include <iostream>
#include <fstream>
#include "Level.h"
#include "Skiplist.h"
#include <list>
#include <filesystem>
namespace fs = std::filesystem;
class LSM_tree {

public:
    vector<Level*> disk;
    unsigned int timer;
    Skiplist memtable;

    //for test
    //bool compact;

private:

    void recover();
    void compaction();
    void compaction_level_0();
    void compaction_level(int level,vector<sortNode*>* temp);
    bool find(uint64_t key);
    void file_to_mem(int level,int file,vector<sortNode*>* t);
    std::string get_file_name(int level_id, int file_id);
    std::string get_level_name(int level_id);
    vector<sortNode *>* sort_merge(vector<vector<sortNode*>*>& final);
public:
    LSM_tree();
    ~LSM_tree(){reset();};
    void put(uint64_t key, const std::string &s ,bool d= false);

    std::string get(uint64_t key);

    bool del(uint64_t key) ;

    void reset();
};


#endif //PROJECT1_KVSTORE_LSM_TREE_H
