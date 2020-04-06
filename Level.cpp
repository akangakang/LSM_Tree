//
// Created by LSC on 2020/3/30.
//

#include "Level.h"

void Sstable::clear() {
    int size=buffer.size();
    for(int i=0;i<size;i++)
    {
        if(buffer[i])
        {
            delete buffer[i];
        }
    }

    buffer.clear();



}
bool Sstable::find(uint64_t key,pairs &temp,int &id) {

    int left = 0;
    int right = buffer.size() - 1;


    while (left <= right) {
        int mid = (right + left) / 2;
        if (buffer[mid]->key == key) {
            temp.timer = buffer[mid]->timer;
            temp.del = buffer[mid]->del;
            temp.key=buffer[mid]->key;
            temp.offset=buffer[mid]->offset;
            id=mid;
            return true;
        } else if (buffer[mid]->key < key)
            left = mid + 1;
        else if (buffer[mid]->key > key)
            right = mid - 1;
    }

    return false;
}

std::string Level::get(const pairs &ans,int id,int file_id) {
    std::string file_name= get_file_name(level_id,file_id);
    ifstream in(file_name,ios::in|ios::binary);
    in.seekg(ans.offset+ sizeof(uint64_t),ios::beg);

    unsigned int size;
    //如果是这个文件的最后一个元素
    if(id+1==file[file_id]->buffer.size())
    {
        size=file[file_id]->end-ans.offset- sizeof(uint64_t);
    } else
    {
        unsigned int a=(file[file_id]->buffer[id+1])->offset;
        unsigned int b=(file[file_id]->buffer[id])->offset;
        if(a==b)
        {
            size=file[file_id]->end-ans.offset- sizeof(uint64_t);
        } else
        size=a-b- sizeof(uint64_t);
    }


    char*c=new char[size];
    in.read(c,size);
    std::string result=c;
    delete[]c;
    return result;
}
bool Level::flush_to_file(Skiplist &memtable, unsigned int timer) {

    //找到空的文件写进去！
    int i=0;
    if(!std::filesystem::exists("DATA"))
    {
        bool fail=std::filesystem::create_directory("DATA");
        if(!fail) cout<<"create DATA failed\n";
        cout<<"new LSM Tree "<<endl;
    }
    if(!std::filesystem::exists("DATA\\0"))
    {
        std::filesystem::create_directory("DATA\\0");

    }
    for(;i<(1<<(level_id+1));i++)
    {
        if(is_creat[i]) continue;

        break;
    }
    //确定文件名！
    is_creat[i]=true;
    std::string file_name ="DATA\\0\\" +to_string(level_id) + "_" + to_string(i) + ".txt";
    Sstable* my_sstable =new Sstable;

    file[i]=my_sstable;
    ofstream out(file_name,ios::out|ios::binary);
    if(!out.is_open())
    {
        cout<<"Error:open file"<<file_name<<endl;
        return false;
    }
    //开始写拉
    int size=memtable.size();
    for(int m=0;m<size;m++)
    {
        uint64_t k;
        std::string s;
        bool d= false;
        memtable.read_and_remove_first(k,s,d);

        unsigned int o1=out.tellp();
        pairs * my_pair =new pairs(k,o1,timer,d);
        my_sstable->buffer.push_back(my_pair);

        out.write((char *)&(k), sizeof(uint64_t));
        int o2=out.tellp();
        const char * c=s.c_str();
        out.write(c, s.size()+1);
        int o3=out.tellp();

//        out<<s;

    }

    my_sstable->end=out.tellp();

    out.seekp(INDEX_BEGIN,ios::beg);
    out.write((char*)&my_sstable->end, sizeof(unsigned int ));

    for(int m=0;m<my_sstable->buffer.size();m++)
    {
        out.write((char *) &(my_sstable->buffer[m]->key), sizeof(uint64_t));
        out.write((char *) &(my_sstable->buffer[m]->offset), sizeof(unsigned int));
        out.write((char *) &(my_sstable->buffer[m]->timer), sizeof(unsigned int));
        out.write((char *) &(my_sstable->buffer[m]->del), sizeof(bool));

    }

    out.close();
    return true;

}

bool Level::clear() {
    int size=this->size();
    for(int j=0;j<size;j++)
    {
        //把文件删了
        if(!is_creat[j]) continue;

        //把索引删了
        file[j]->clear();

    }
    file.clear();
    std::string level="DATA\\"+to_string(level_id);
    std::filesystem::remove_all(level);
    delete []is_creat;
    return true;
}

std::string Level::get_level_name(int level_id) {
    std::string ans="DATA\\"+to_string(level_id);
    return ans;
}
std::string Level::get_file_name(int level_id, int file_id) {
    std::string ans="DATA\\"+to_string(level_id)+"\\"+to_string(level_id)+"_"+to_string(file_id)+".txt";
    return ans;
}