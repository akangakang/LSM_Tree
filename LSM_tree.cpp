//
// Created by LSC on 2020/3/30.
//

#include "LSM_tree.h"

std::string LSM_tree::get(uint64_t key) {


    //cout<<"get"<<key<<endl;
    //-----------------------------------先去memtable里找--------------------------//
    std::string result;
//    if ((result = memtable.get(key)) != nullptr) {
//        return *result;
//    }
    bool del = false;
    bool has_key = memtable.has_key(key, del, result);
    if (has_key) {
        if (del) {
            return "";
        } else
            return result;
    }

    //-----------------------------如果没有 再去disk里找----------------------------//

    int file_id = 0;
    int level_id = 0;


    int id = 0;
    int final_id=0;
    bool has = false;
    pairs ans(0, 0, 0, false);
    for (int i = 0; i < disk.size(); i++) {

        //每层里 一个文件一个文件找 -----在索引里
        if (disk.size() == 0) return "";
        Level level = *disk[i];
        for (int j = 0; j <level.file.size(); j++)                                                                       //这里file.size()不知道有没有问题  因为file可能会 把某一个变成null
        {

            if (!level.is_creat[j]) continue;
            if (level.file[j] == nullptr) {
                cout << "Error:level.file[j]==NULL\n";
                return "";
            }
            Sstable sstable = *(level.file[j]);




            //标记为删了的不能返回true哦！
            pairs temp(0, 0, 0, false);
            bool is_find = sstable.find(key, temp, id);

            if (is_find) {
                has = true;
                if (temp.timer > ans.timer) {
                    ans.timer = temp.timer;
                    ans.del = temp.del;
                    ans.offset = temp.offset;
                    ans.key = temp.key;
                    file_id = j;
                    level_id = i;
                    final_id=id;
                }

            }


        }


    }

    if (!has) {
        return "";
    } else {
        if (ans.del) return "";
        std::string result1 = disk[level_id]->get(ans, final_id, file_id);
        return result1;
    }


}

void LSM_tree::put(uint64_t key, const std::string &s, bool d) {
    int size = memtable.cap();

    //---------------------------如果加上这个会超size 就先flush_to_file----------------//
    if (size + s.size() + 1 + sizeof(uint64_t) > MAX_SIZE) {

        Level *level;
        if (disk.empty()) {

            level = new Level(0);                                                                                          //reset里析构level
            disk.push_back(level);
        } else
            level = disk[0];


        //把memtable里的东西 写道文件 并且把 索引存到sstable 由level统一管理
        //注意timer 因为这里都是最新的 timer统一

        timer++;
        level->flush_to_file(memtable, timer);//已经把memtable清空

        compaction();
      // memtable.clear();
        //memtable.my_clear();
    }

    memtable.put(key, s, d);
}

bool LSM_tree::del(uint64_t key) {
    //这个find是找 确实有没有这个 如果memtable有key 但标为被删就算没找到
    //如果要去文件里找看最大时间码的有没有标记未被删了的
    //cout<<"del"<<key<<endl;
    if (!find(key)) return false;
    std::string s;
    put(key, s, true);
    return true;
}

bool LSM_tree::find(uint64_t key) {
    //-----------------------------------先去memtable里找--------------------------//

    //声明：如果memtable里存了一个标记被删了的pair 那么memtable.get是会返回NULL的 也就是没找到
//    if ((memtable.get(key)) != nullptr) {
//        return true;
//    }
    std::string result;
//    if ((result = memtable.get(key)) != nullptr) {
//        return *result;
//    }
    bool del = false;
    bool has_key = memtable.has_key(key, del, result);
    if (has_key) {
        if (del) {
            return false;
        } else
            return true;
    }

    bool has = false;
    unsigned int ans_timer = 0;
    bool ans_del = false;
    for (int i = 0; i < disk.size(); i++) {

        if (disk.size() == 0) return false;
        //每层里 一个文件一个文件找 -----在索引里
        Level level = *disk[i];
        for (int j = 0; j < level.file.size(); j++) {
            if (level.file[j] == nullptr) continue;
            Sstable sstable = *(level.file[j]);

            pairs temp(0, 0, 0, false);

            //标记为删了的不能返回true哦！
            int id = 0;

            bool is_find = sstable.find(key, temp, id);

            if (is_find) {
                has = true;
                if (temp.timer > ans_timer) {
                    ans_timer = temp.timer;
                    ans_del = temp.del;

                }

            }

        }
    }

    if (!has) {
        return false;
    } else {
        if (ans_del) return false;
        return true;

    }


}

void LSM_tree::compaction() {
   //cout << "compaction" << endl;
    Level *my_level = disk[0];
    if (my_level->size() <= (2)) {
        return;
    }
    //compact=true;
    compaction_level_0();
}

vector<sortNode *> *LSM_tree::sort_merge(vector<vector<sortNode *> *> &need_to_cpct) {
    //cout << "sort_merge" << endl;
    vector<sortNode *> final;
    int size = need_to_cpct.size() - 1;
    for (int i = 0; i < size; i++) {
        vector<sortNode *> *a = (need_to_cpct)[i];
        vector<sortNode *> *b = (need_to_cpct)[i + 1];

        int m = 0, n = 0;
        while (m < a->size() && n < b->size()) {


            if ((*a)[m]->key < (*b)[n]->key) {
                final.push_back((*a)[m]);
                m++;
            } else if ((*a)[m]->key > (*b)[n]->key) {
                final.push_back((*b)[n]);
                n++;
            } else {
                if ((*a)[m]->timer > (*b)[n]->timer) {
                    final.push_back((*a)[m]);
                    m++;
                    n++;

                } else if ((*a)[m]->timer < (*b)[n]->timer) {
                    final.push_back((*b)[n]);
                    m++;
                    n++;

                } else {
                    cout << "Error: two node with same key and timer";
                    exit(0);
                }
            }

        }

        while (m < a->size()) {
            final.push_back((*a)[m]);
            m++;
        }
        while (n < b->size()) {
            final.push_back((*b)[n]);
            n++;
        }
       // cout << "sort_merge" << "" << i << endl;
        b->clear();

        a->clear();
        //delete a;
        (need_to_cpct)[i] = nullptr;

        for (int k = 0; k < final.size(); k++) {
            b->push_back(final[k]);
        }
        (need_to_cpct)[i+1] =b;
        final.clear();

    }

    return (need_to_cpct)[need_to_cpct.size() - 1];
}

void LSM_tree::compaction_level_0() {
    vector<vector<sortNode *> *> need_to_cpct;
    Level *my_level = disk[0];

    //---------------------------------把需要合并的文件都读到内存---------------------------------//
    for (int i = 0; i < 3; i++) {
        vector<sortNode *> *t = new vector<sortNode *>;

        //cout << "file_to_mem " << i << endl;
        //-----------------------------把文件写进内存----------------------------//
        file_to_mem(0, i, t);//这步之后t里面的每个节点的空间都是new出来的
        need_to_cpct.push_back(t);//t的空间也是new出来的 need_to_cpct的每个节点的空间也都是new出来的
        my_level->is_creat[i] = false;

        std::string file_name=get_file_name(0,i);
        if(!fs::remove(file_name))
        {
            cout<<"remove"<<file_name<<"fail"<<endl;
        }

        //---------------在索引区把这个文件删了---说明这个文件不存在了-------------//
        Sstable *remove = my_level->file[i];                                                                           //我觉得这里要析构 但是不知道改怎么析构

        //析构Sstable.buffer的空间
//        int buffer_size=remove->buffer.size();
//        for(int i=0;i<buffer_size;i++)
//        {
//            delete remove->buffer[i];
//        }
        remove->clear();//清空并析构
       // delete remove;                                                                                                  //!!

        my_level->file[i] = nullptr;//my_level里的指针也析构？？

    }
    //cout << "compaction level  " << 0 << " " << "to merge" << endl;
    //--------------------------------------归并------------------------------------------------//
    vector<sortNode *> *temp;
    temp = sort_merge(need_to_cpct);
    if (temp == nullptr) {
        cout << "Error: sort_merge";
        exit(0);

    }

    compaction_level(1, temp);

}

void LSM_tree::compaction_level(int level, vector<sortNode *> *temp) {
    //cout << "compaction level " << level << endl;
    if (temp == nullptr)
        return;

    vector<vector<sortNode *> *> need_to_cpct;

    Level *my_level;
    if (level >= disk.size()) {

        my_level = new Level(level);
        std::string level_name=get_level_name(level);
        fs::create_directory(level_name);
        disk.push_back(my_level);
    } else {
        my_level = disk[level];


        bool *is_dup = new bool[(1 << (level + 1))];//别忘了析构
        for (int i = 0; i < (1 << (level + 1)); i++) { is_dup[i] = false; }

        //cout << "finding file to merge" << endl;
        //---------------------------------寻找要归并的文件------------------------------------------//
        uint64_t key_end = (*temp)[temp->size() - 1]->key;
        uint64_t key_begin = (*temp)[0]->key;

//        if(key_begin==0)
//        {
//            cout<<"?";
//        }
        for (int i = 0; i < (1 << (level + 1)); i++) {
            if (!my_level->is_creat[i]) continue;

            Sstable *my_sstable = my_level->file[i];
            uint64_t begin = my_sstable->buffer[0]->key;
            uint64_t end = my_sstable->buffer[my_sstable->buffer.size() - 1]->key;

            if ((begin >= key_begin && begin <= key_end) || (end >= key_begin && end <= key_end)) {
                is_dup[i] = true;
            }

        }

        //---------------------------------把需要合并的文件都读到内存---------------------------------//
        for (int i = 0; i < (1 << (level + 1)); i++) {
            if (!is_dup[i]) continue;
            // cout  << "file_to_mem" << endl;
            //该文件需要读进来
            vector<sortNode *> *t = new vector<sortNode *>;

            //-----------------------------把文件写进内存----------------------------//
            file_to_mem(level, i, t);//这步之后t里面的每个节点的空间都是new出来的
            need_to_cpct.push_back(t);//t的空间也是new出来的 need_to_cpct的每个节点的空间也都是new出来的

            my_level->is_creat[i] = false;
            std::string file_name=get_file_name(level,i);
            if(!fs::remove(file_name))
            {
                cout<<"remove"<<file_name<<"fail"<<endl;
            }
            //---------------在索引区把这个文件删了---说明这个文件不存在了-------------//
            Sstable *remove = my_level->file[i];                                                                           //我觉得这里要析构 但是不知道改怎么析构

            remove->clear();//清空并析构

            // delete remove;
            my_level->file[i] = nullptr;//my_level里的指针也析构？？
        }
        //这里要看vector构造的时候传的是引用还是 复制个新的

        //--------------------------------------归并------------------------------------------------//
        if (need_to_cpct.size() != 0) {
            // cout << "compaction level  " << level << " " << "to merge" << endl;
            need_to_cpct.push_back(temp);
            temp = sort_merge(need_to_cpct);
            if (temp == nullptr) {
                cout << "Error: sort_merge";
                exit(0);

            }
        } else {
            // cout << "no need to sort merge" << endl;
        }
        delete[]is_dup;

    }
    int temp_index = 0;//指着需要被写的的那个
    //----------------------------看看有没有空位--写一点到这层的文件里----------------------------//
    for (int i = 0; (1 << (my_level->level_id + 1)) > i; i++) {

        if (my_level->is_creat[i]) continue;


        //cout << "has empty file " << i << endl;
        //找到空文件  确定文件名
        int l = my_level->level_id;
        int f = i;
        std::string file_name = get_file_name(l,f);
        ofstream out(file_name, ios::out | ios::binary);//默认清空原来的并从开头开始写

        out.seekp(0,ios::beg);
        Sstable *my_sstable = nullptr;
        my_sstable = new  Sstable;



        my_level->is_creat[i] = true;
        //往文件里写 并生成索引
        for (; temp_index < temp->size(); temp_index++) {
            sortNode *now = (*temp)[temp_index];
            int size = now->value.size();
            int pos = out.tellp();


            int k=now->key;
            //如果是要删掉的元素 还是要写进文件 万一下面的层也有记录
            //写入这个不会超范围 写入
            if (pos + size + 8 + 1 < MAX_SIZE) {

                //索引   一定要先写索引再写文件  pos
                pairs *my_pair = new pairs(now->key, pos, now->timer, now->del);
                my_sstable->buffer.push_back(my_pair);

                //文件
                out.write((char *) &(now->key), sizeof(uint64_t));
                const char *c = (now->value).c_str();
                out.write(c, (now->value).size() + 1);
//                    out<<now->value;                                                                                     //!!!!

                //一边写一边delete
               // delete now;
                (*temp)[temp_index] = nullptr;


            } else {
                //如果写了这个会超出范围就不写了
                //关上文件
                my_sstable->end = pos;

               // cout<<"write index"<<" tempt_index="<<temp_index<<endl;
                //-------------把索引区写进文件------------//
                out.seekp(INDEX_BEGIN, ios::beg);
                //先把end写进去
                out.write((char *)&(my_sstable->end), sizeof(unsigned int));
                for (int k = 0; k < my_sstable->buffer.size(); k++) {
                    out.write((char *) &(my_sstable->buffer[k]->key), sizeof(uint64_t));
                    out.write((char *) &(my_sstable->buffer[k]->offset), sizeof(unsigned int));
                    out.write((char *) &(my_sstable->buffer[k]->timer), sizeof(unsigned int));
                    out.write((char *) &(my_sstable->buffer[k]->del), sizeof(bool));
                }


                //--------------写完了--------------------//

                (my_level->file)[i] = my_sstable;  //???
                //-----------------标记文件---------------//
                my_level->is_creat[i] = true;
                out.close();

                break;


            }


        }

        //如果temp都被写完了
        if (temp_index == temp->size()) {
            //cout<<"temp is empty"<<endl;
            my_sstable->end = out.tellp();

            //-------------把索引区写进文件------------//
            out.seekp(INDEX_BEGIN, ios::beg);
            //先把end写进去
            out.write((char *) &(my_sstable->end), sizeof(unsigned int));
            int size = my_sstable->buffer.size();
            for (int k = 0; k < size; k++) {
                out.write((char *) &(my_sstable->buffer[k]->key), sizeof(uint64_t));
                out.write((char *) &(my_sstable->buffer[k]->offset), sizeof(unsigned int));
                out.write((char *) &(my_sstable->buffer[k]->timer), sizeof(unsigned int));
                out.write((char *) &(my_sstable->buffer[k]->del), sizeof(bool));
            }
            my_level->is_creat[i] = true;
            out.close();

               //---------------析构temp------------------//

//                for(int k=0;k<temp->size();k++)
//                {
//                    delete (*temp)[k];
//                }
            //delete temp;
            (my_level->file)[i] = my_sstable;  //???
            return;

        }

    }

   // cout<<"delete"<<endl;

    //运行到这里应该说明文件写满了要去下一层了
    if(temp_index==0)
    {
        //cout<<"go to next level directly"<<endl;
        compaction_level(level + 1, temp);
    }
    else{
        vector<sortNode *> new_temp;                                                           //change
        for (int k = temp_index; k < temp->size(); k++) {
            new_temp.push_back((*temp)[k]);
        }
        //delete temp;

        //如果还没有下一层 要新建一层

        if (level + 1 >= disk.size()) {
            Level *level_next = new Level(level + 1);
            std::string level_name=get_level_name(level+1);
            fs::create_directory(level_name);
            disk.push_back(level_next);
        }


        compaction_level(level + 1, &new_temp);
    }





}

void LSM_tree::file_to_mem(int level, int file, vector<sortNode *> *t) {
    //确定层数
    Level *my_level = disk[level];

    //确定文件名
    std::string file_name =get_file_name(level,file);

    ifstream in(file_name, ios::in | ios::binary);
    if (!in.is_open()) {
       cout << "Error:open file  " << file_name << endl;
        exit(0);
    }

    vector<pairs *> my_buffer = (my_level->file)[file]->buffer;
    //-------------------把一个文件都出来--------------------------//
    //把除了倒数第一个全读出来
    int j = 0;
    int size = my_buffer.size();
    for (; j <= size - 2; j++) {
        in.seekg(my_buffer[j]->offset, ios::beg);
        sortNode *my_pair = new sortNode;


        //读key
        int o1 = in.tellg();
        in.read((char *) &(my_pair->key), sizeof(uint64_t));
//        if(my_pair->key==323)
//        {
//            cout<<"!"<<endl;
//        }
        int o2 = in.tellg();
        //读string
        char *ans = new char[my_buffer[j + 1]->offset - my_buffer[j]->offset - sizeof(uint64_t)];

        in.read(ans, my_buffer[j + 1]->offset - my_buffer[j]->offset - sizeof(uint64_t));//要加1！！！！！！！！          //!!!!
        int o3 = in.tellg();
        my_pair->value = ans;
        delete[]ans;
        //存del timer
        my_pair->del = my_buffer[j]->del;
        my_pair->timer = my_buffer[j]->timer;

        t->push_back(my_pair);
    }
    in.seekg(my_buffer[j]->offset, ios::beg);
    sortNode *my_pair = new sortNode;
    in.read((char *) &(my_pair->key), sizeof(uint64_t));
    int x = (my_level->file)[file]->end;
    int y = my_buffer[j]->offset;
    int o1 = in.tellg();
    char *ans = new char[(my_level->file)[file]->end - my_buffer[j]->offset - sizeof(uint64_t)];
    in.read(ans, (my_level->file)[file]->end - my_buffer[j]->offset - sizeof(uint64_t));
    int o2 = in.tellg();
    my_pair->value = ans;
    delete[]ans;
    my_pair->del = my_buffer[j]->del;
    my_pair->timer = my_buffer[j]->timer;
    t->push_back(my_pair);


    in.close();
    //-------------------把一个文件都出来--finish-----------------//
}



void LSM_tree::reset() {


    if(!fs::exists("DATA"))
    {
        disk.clear();
        return;
    }

    for (int i = 0; i < disk.size(); i++) {

        //int size=disk[i]->size();
        Level* level=disk[i];
        level->clear();


    }
    if(!std::filesystem::remove_all("DATA"))
    {
        cout<<"fail to remove DATA"<<endl;
    }

}
std::string LSM_tree::get_level_name(int level_id) {
    std::string ans="DATA\\"+to_string(level_id);
    return ans;
}
std::string LSM_tree::get_file_name(int level_id, int file_id) {
    std::string ans="DATA\\"+to_string(level_id)+"\\"+to_string(level_id)+"_"+to_string(file_id)+".txt";
    return ans;
}
LSM_tree::LSM_tree()
{
    timer=0;
    //compact=false;
    if(std::filesystem::exists("DATA"))
    {
        cout<<"exixst old file.....recover"<<endl;
        recover();
    } else
    {
        bool fail=fs::create_directory("DATA");
        if(!fail) cout<<"create DATA failed\n";
        //cout<<"new LSM Tree "<<endl;
    }


}
void LSM_tree::recover() {
    int level_id=0;


    while(true)
    {
        std::string level_name=get_level_name(level_id);
        if(!fs::exists(level_name)) break;
        Level* my_level=new Level(level_id);
        disk.push_back(my_level);

        //看看这层由上面文件 读一下
        for(int file_id=0;file_id<(1<<(level_id+1));file_id++)
        {
            std::string file_name=get_file_name(level_id,file_id);
            //如果这个文件存在 就都进去
            if(!fs::exists(file_name)){
                my_level->is_creat[file_id]=false;
                continue;
            }
            else my_level->is_creat[file_id]=true;

            //打开文件
            ifstream in(file_name, ios::in | ios::binary);
            if (!in.is_open()) {
                cout << "Error:open file" << file_name << endl;
                exit(0);
            }

            //新建索引区
            Sstable * my_sstable =new Sstable;
            //找到索引区起始位置
            in.seekg(INDEX_BEGIN,ios::beg);
            int o4=in.tellg();
            in.read((char*)&my_sstable->end, sizeof(unsigned int));
           int o5=in.tellg();
            in.seekg(INDEX_BEGIN+ sizeof(unsigned int),ios::beg);
            int o6=in.tellg();
            while (!in.eof())
            {
                uint64_t key;
                unsigned int offset;
                unsigned int my_timer;
                bool del;

                int o0=in.tellg();
                in.read((char*)&key,sizeof(uint64_t));
                int o1=in.tellg();
                in.read((char*)&offset,sizeof(unsigned int));
                int o2=in.tellg();
                in.read((char*)&my_timer, sizeof(unsigned int));
                int o3=in.tellg();
                in.read((char*)&del,sizeof(bool));
                int o4=in.tellg();

                pairs* my_pair=new pairs(key,offset,my_timer,del);
                my_sstable->buffer.push_back(my_pair);
            }
            in.close();

            //把sstable写进level
            my_level->file[file_id]=my_sstable;



        }
        level_id++;

    }

    cout<<"recover finished\n";

}