#include <iostream>
#include <random>
#include <cstdlib>
#include <chrono>
#include "hnsw/hnswlib.h"

int main() {
    size_t data_num = 10000;
    size_t data_dim = 128;
    auto data = new float[data_num * data_dim];
    auto ids = new int[data_num];

    for (int i = 0; i < data_num; ++i) {
        ids[i] = i;
        for (int j = 0; j < data_dim; ++j) {
            data[i * data_dim + j] = static_cast<float>(rand()) / RAND_MAX;
        }
    }

    hnswlib::SpaceInterface<float>* s = new hnswlib::L2Space(data_dim);

    hnswlib::HierarchicalNSW<float> hnsw(s, data_num);
    for (int i = 0; i < data_num; ++i) {
        hnsw.addPoint(data + i * data_dim, ids[i]);
    }

    auto start_hnsw = std::chrono::high_resolution_clock::now();
    hnsw.ef_ = 200;
    int correct = 0;
    for (int i = 0; i < data_num; ++i) {
        auto result = hnsw.searchKnn(data + i * data_dim, 1);
        if (result.top().second == i) {
            correct ++;
        }
    }
    auto end_hnsw = std::chrono::high_resolution_clock::now();
    auto duration_hnsw = std::chrono::duration_cast<std::chrono::microseconds>(end_hnsw - start_hnsw);
    std::cout << "hnsw cost: " << duration_hnsw.count() << " " << "hnsw correct:" << correct / (float) data_num  << std::endl;

    correct = 0;
    auto start_brute_force = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < data_num; ++i) {
        auto query = data + i * data_dim;
        float min_distance = s->get_dist_func() (query, data, &data_dim);
        int id = 0;
        for (int j = 1; j < data_num; ++j) {
            float distance = s->get_dist_func() (query, data + j * data_dim, &data_dim);
            if (distance < min_distance) {
                min_distance = distance;
                id = j;
            }
        }
        if (id == i) {
            correct ++;
        }
    }
    auto end_brute_force = std::chrono::high_resolution_clock::now();
    auto duration_brute_force = std::chrono::duration_cast<std::chrono::microseconds>(end_brute_force - start_brute_force);
    std::cout << "brute force cost: " << duration_brute_force.count() << " " << "brute force correct:" << correct / (float) data_num  << std::endl;

    return 0;
}
