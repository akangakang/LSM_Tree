cmake_minimum_required(VERSION 3.13)
project(project1-kvstore)

set(CMAKE_CXX_STANDARD 17)

include_directories(.)

add_executable(project1-kvstore
        kvstore.cc
        kvstore.h
        kvstore_api.h
        correctness.cc
        test.h
        Quadlist.cpp Quadlist.h Mynode.h Skiplist.cpp Skiplist.h
        LSM_tree.cpp LSM_tree.h Level.cpp Level.h
        )