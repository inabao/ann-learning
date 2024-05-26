#include <iostream>
#include <random>
#include <cstdlib>

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
    //s->get_dist_func() (data, data + data_dim, &data_dim);

    int correct = 0;
    for (int i = 0; i < data_num; ++i) {
        auto result = hnsw.searchKnn(data + i * data_dim, 1);
        if (result.top().second == i) {
            correct ++;
        }
    }
    std::cout << correct / (float) data_num << std::endl;

    return 0;
}
