//
// Created by LSC on 2020/3/18.
//

#include "Skiplist.h"
#include <cstdlib>

Quadlist* Skiplist::top() {
    return front();
}

Quadlist* Skiplist::bottom() {
    return back();
}
bool Skiplist::put(uint64_t k,const std::string &v,bool d) {
    Entry e;

    e.key=k;
    e.value=v;


    if(empty())
    {
        Quadlist * bottomm = new Quadlist();
        push_front(bottomm);

    }

    list::iterator qlist=begin();   //顶层那条四联表  用迭代器！！！
    Quadlistnode * p=(*qlist)->first();//顶层四联表的第一个元素

    //如果找到这个元素 改变他的值 为v
    //memtable里的k不重复 插入相同k直接覆盖
    if(skipSearch1(qlist,p,k))  //这里传的都是引用哦！！
    {
//        big-=p->entry.value.size();
      //  big+=v.size();
        p->entry.value=v;
        p->del=d;
        while (p->below)
        {
            p=p->below;
            p->entry.value=v;
            p->del=d;
        }
        return true;

    }


    //skipSearch之后 如果没找到 qlist就是底层，p是比k小的最大的那个
    list::reverse_iterator r_qlist=rbegin();
    Quadlistnode * b = (*r_qlist)->insertAfterAbove(e,p,nullptr,d); //新底座
    while(rand()&1)
    {
        //找到这条链比k小的 并且头上有东西的
        while ((*r_qlist)->valid(p) && !p->above) p=p->pred;

        //如果pred是header 就说明 比k小的没有这么高的了
        if(!(*r_qlist)->valid(p))
        {
            reverse_iterator temp =r_qlist;
            temp++;
            if(temp==rend()) //说明是顶层了
            {
                //新建一层
                Quadlist * newTop = new Quadlist();
                push_front(newTop);

                p=newTop->first()->pred;
            } else {
                reverse_iterator temp = r_qlist;
                temp++;
                p = (*temp)->first()->pred;  //让p等于上一层的header (private)}
            }

        }
        else{
            p=p->above;
        }

        r_qlist++;//上升一层
        b= (*r_qlist)->insertAfterAbove(e,p,b,d);
    }


    return true;
}

bool Skiplist::skipSearch(list::iterator &qlist, Quadlistnode *&p, uint64_t k) {
    while(true)
    {
        while (p->succ && (p->entry.key <= k))
        {
            p=p->succ;
        }

        p=p->pred;
        if(p->pred && (k==p->entry.key) && p->del==0) return true;

        qlist++;
        if(qlist == end()) return false;
        p= (p->pred)? p->below:(*qlist)->first();
    }
}
bool Skiplist::skipSearch1(list::iterator &qlist, Quadlistnode *&p, uint64_t k) {
    while(true)
    {
        while (p->succ && (p->entry.key <= k))
        {
            p=p->succ;
        }

        p=p->pred;
        if(p->pred && (k==p->entry.key) ) return true;

        qlist++;
        if(qlist == end()) return false;
        p= (p->pred)? p->below:(*qlist)->first();
    }
}
bool Skiplist::remove(uint64_t k) {
    if(empty())
        return false;
    list::iterator qlist=begin();
    Quadlistnode * p = (*qlist)->first();
    if(!skipSearch1(qlist,p,k))return false;

    do {
        Quadlistnode *lower = p->below;
        (*qlist)->remove(p);
        p = lower;
        qlist++;
    }while(qlist!=end());

    //清除已经空了的层 会吧
    while(!(empty()) && top()->empty())
    {
        if(top()==bottom())
            return true;
        list::remove(Skiplist::top());

    }
    return true;
}

bool Skiplist::remove2(uint64_t k) {
    if(empty())
        return false;
    list::iterator qlist=begin();
    Quadlistnode * p = (*qlist)->first();
    if(!skipSearch(qlist,p,k))return false;

    put(k,"",true);
    return true;

}
bool Skiplist::read_and_remove_first(uint64_t &k, std::string &s,bool & d) {
    if(empty())
        return false;
    Quadlist* b=bottom();

    k=b->first()->entry.key;
    s=b->first()->entry.value;

    d=b->first()->del;

    if(!remove(k))
    {
        cout<<"read and remove the first error!\n";
        return false;
    }

    return true;

}
int Skiplist::level() {
    int result=0;

    for(iterator it=begin();it != end();it++)
    {
        result++;
    }

    return result;

}

int Skiplist::size() {
//    int result=0;
//    for(iterator it= begin();it !=end();it++)
//    {
//        result+=(*it)->size();
//    }
//    return result;

    return empty()?0:bottom()->size();
}

bool Skiplist::empty() {
    return  list::empty();
//    return bottom()->empty();
//    if(level()==0 ) return true;
    //if(size()==0) return true;

}
std::string* Skiplist::get(uint64_t k) {
    if(empty())
    {
        return nullptr;
    }

    iterator qlist = begin();
    Quadlistnode * p= (*qlist)->first();
    return skipSearch(qlist,p,k)? &(p->entry.value) : nullptr;




}

bool Skiplist::has_key(uint64_t k, bool &del, std::string &s) {
    if(empty())
    {
        return false;
    }
    iterator qlist = begin();
    Quadlistnode * p= (*qlist)->first();
    bool has=skipSearch1(qlist,p,k);
    del=p->del;
    s=p->entry.value;
    return has;


}
int Skiplist::cap() {
    Quadlist* q=bottom();
    if(q== nullptr)return 0;
    Quadlistnode* p=q->first();


    int cap=0;
    while(p->succ!= nullptr )
    {
        cap+=8;
        cap+=p->entry.value.size()+1;
        p=p->succ;
    }

    return cap;
}
bool Skiplist::my_clear() {
    if(empty())
        return true;


    while(!empty())
    {
        Quadlist* b=bottom();


        uint64_t k=b->first()->entry.key;

        if(!remove(k))
        {
            cout<<"my clear error!"<<k<<endl;
            return false ;
        }


    }
    return true;


}