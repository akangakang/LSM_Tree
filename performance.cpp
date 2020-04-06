//
// Created by LSC on 2020/4/5.
//
#include "LSM_tree.h"
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <time.h>
#define PUT_TEST 1024*15

using namespace std;
void get_test()
{

    LSM_tree store;
    for(int i=0;i<1024*32;i++)
    {

        store.put(i,std::string(i+1, 's'));

        if(i%1024==0)
        {
            unsigned int test=0;
            double all_time=0;
            for(int m=0;m<1024*2;m++)
            {


                clock_t start = clock();
                store.get(m);
                clock_t ends = clock();

                test++;
                all_time+=(double)(ends - start);


            }
            cout<<i<<" "<<(all_time/test)/ CLOCKS_PER_SEC<<endl;
        }

    }

    unsigned int test=0;
    double all_time=0;
    for(int i=0;i<1024*2;i++)
    {


        clock_t start = clock();
        store.get(i);
        clock_t ends = clock();

        test++;
        all_time+=(double)(ends - start);


    }
    cout<<test<<" "<<all_time<<" "<<(all_time/test)/ CLOCKS_PER_SEC<<endl;

}

void delete_test()
{
    LSM_tree store;
    for(int i=0;i<1024*12;i++)
    {

        store.put(i,std::string(i+1, 's'));
    }

    unsigned int test=0;
    double all_time=0;
    for(int i=0;i<1024*2;i++)
    {
        //uint64_t p=rand();

        store.compact= false;
        clock_t start = clock();
        store.del(i);
        clock_t ends = clock();

        if(!store.compact)
        {
            test++;
            all_time+=(double)(ends - start);

        }


    }
    cout<<test<<" "<<all_time<<" "<<(all_time/test)/ CLOCKS_PER_SEC<<endl;
}
void put_test()
{

//    for(int k=50;k<=1000;k+=50)
//    {
//        LSM_tree store;
//        unsigned int test=0;
//        double all_time=0;
//
//        for(int i=0;i<k;i++)
//        {
//            store.compact= false;
//            uint64_t p=rand();
//
//
//            clock_t start = clock();
//            store.put(p,std::string(p+1, 's'));
//            clock_t ends = clock();
//
//            //只有没有发生合并的时候才记录
//            if(!store.compact)
//            {
//                test++;
//                all_time+=(double)(ends - start)/ CLOCKS_PER_SEC;
//            }
//        }
//
//        cout<<test<<" "<<all_time<<" "<<all_time/test<<endl;
//    }
    LSM_tree store;
    uint64_t test=0,i=0;
        double all_time=0;
        double old_time=0;
        int count=0;
        clock_t start = clock();
    while(i!=1024*16)
    {

        store.compact=false;

        int p=rand();
        int q=rand();
            store.put(p,std::string(q+1, 's'));
            clock_t ends = clock();

            double now=(double)(ends-start)/CLOCKS_PER_SEC;
            //只有没有发生合并的时候才记录
//            if(store.compact)
//            {
//                cout<<"!"<<endl;
//
//            }
     i++;
            //test++;
        //all_time+=(double)(ends - start)/ CLOCKS_PER_SEC;
        count++;
        if(now-old_time>0.005)
        {
            cout<<now<<" "<<count/(now-old_time)<<endl;
            old_time=now;
            count=0;
        }

    }
   // cout<<test<<" "<<(all_time/test)/ CLOCKS_PER_SEC<<endl;




//    for(int k=1024;k<PUT_TEST;k+=4048)
//    {
//        LSM_tree store;
//        unsigned int test=0;
//        double all_time=0;
//
//        for(int i=0;i<k;i++)
//        {
//            store.compact= false;
//            uint64_t p=rand();
//
//
//            clock_t start = clock();
//            store.put(p,std::string(p+1, 's'));
//            clock_t ends = clock();
//
//            //只有没有发生合并的时候才记录
//            if(!store.compact)
//            {
//                test++;
//                all_time+=(double)(ends - start)/ CLOCKS_PER_SEC;
//            }
//        }
//
//        cout<<test<<" "<<all_time<<" "<<all_time/test<<endl;
//    }


//    LSM_tree store;
//        unsigned int test=0;
//        double all_time=0;
//    for(int i=0;i<1024*16;i++)
//        {
//            store.compact= false;
//            uint64_t p=rand();
//
//            cout<<i<<" "<<p<<endl;
//
//            clock_t start = clock();
//            store.put(p,std::string(p+1, 's'));
//            clock_t ends = clock();
//
//            //只有没有发生合并的时候才记录
//            if(!store.compact)
//            {
//                test++;
//                all_time+=(double)(ends - start)/ CLOCKS_PER_SEC;
//            }
//        }
//
//        cout<<test<<" "<<all_time<<" "<<all_time/test<<endl;

}
int main()
{
   // srand(time(NULL));

 put_test();
//get_test();
//delete_test();


}