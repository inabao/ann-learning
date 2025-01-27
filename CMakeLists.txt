cmake_minimum_required(VERSION 3.20)
project(ann)

set(CMAKE_CXX_STANDARD 17)

include_directories(hnsw)

add_executable(ann main.cpp
        hnsw/knn_graph.h)
