//
// Created by LSC on 2020/3/30.
//

#ifndef PROJECT1_KVSTORE_LEVEL_H
#define PROJECT1_KVSTORE_LEVEL_H
#include <cstdint>
#include <string>
#include "Mynode.h"
#include <iostream>
#include <fstream>
#include <filesystem>
//#define MAX_SIZE 33
//#define INDEX_BEGIN 33
#define MAX_SIZE 2097152
#define INDEX_BEGIN 2097152
using namespace std;

#include "Skiplist.h"
#include <vector>
struct pairs{
    uint64_t key;
    unsigned int offset;
    unsigned int timer;
    bool del;

    pairs(uint64_t k,int o,uint64_t t,bool d=false):key(k),offset(o),timer(t),del(d){}
    pairs(){key=0;offset=0;timer=0;del= false;}
};

struct sortNode{
    uint64_t key;
    std::string value;
    unsigned int timer;
    bool del;

    sortNode(){key=0;value="";timer=0;del= false;}
};

class Sstable{

public:
    unsigned int end;//存最后一个kv对结束的位置
    vector<pairs*> buffer;

public:
    Sstable(){end=0;}

    bool find(uint64_t key,pairs &temp,int &id); //记得检查标记是否是删了的

    void clear();//清空并析构
};
class Level {
public:
    int level_id;

    bool *is_creat;
    vector<Sstable*> file;


    Level(int id){
        level_id=id;

        is_creat =new bool[(1<<(level_id+1))+1];
        for(int i=0;i<=(1<<(level_id+1));i++)
        {
            is_creat[i]= false;
            file.push_back(nullptr);
        }
    }

    std::string get(const pairs &ans,int id,int file_id);//从文件里取出offset
    //vector<Content*> * compaction(vector<Content*> *temp);
    std::string get_file_name(int level_id, int file_id);
    std::string get_level_name(int level_id);
    bool flush_to_file(Skiplist &,unsigned int timer);
    int size()
    {
        int sum=0;
        for(int i=0;i<(1<<(level_id+1))+1;i++)
        {
            if(is_creat[i])sum++;
        }
        return sum;
    }
    bool clear();

};


#endif //PROJECT1_KVSTORE_LEVEL_H
