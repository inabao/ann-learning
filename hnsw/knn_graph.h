#pragma once

#include <iostream>
#include <memory>
#include <vector>
#include <random>
#include <unordered_set>
#include <algorithm>

namespace knngraph {

static float
L2Sqr(const void *pVect1v, const void *pVect2v, const void *qty_ptr) {
  float *pVect1 = (float *) pVect1v;
  float *pVect2 = (float *) pVect2v;
  size_t qty = *((size_t *) qty_ptr);

  float res = 0;
  for (size_t i = 0; i < qty; i++) {
    float t = *pVect1 - *pVect2;
    pVect1++;
    pVect2++;
    res += t * t;
  }
  return (res);
}


class Graph {

public:
  Graph(std::shared_ptr<float[]> data, size_t dim, size_t num,
        size_t max_degree)
      : data_(data), dim_(dim), num_(num), max_degree_(max_degree) {
    rd = new std::random_device;
    float_distr = new std::uniform_real_distribution<float>;
    rng = new std::mt19937((*rd)());
  }

  void BuildGraph() {
    init_graph();
    check_turn();
    for (int i = 0; i < 20; ++i) {
      iter();
      check_turn();
    }
  }

  std::vector<std::vector<uint32_t>> GetGraph() const {
    std::vector<std::vector<uint32_t>> graphs;
    for (const auto &item : edges_) {
      std::vector<uint32_t> edge;
      for (const auto &node : item) {
        edge.push_back(node.second);
      }
      graphs.push_back(edge);
    }
    return graphs;
  }



private:
  void init_graph() {
    std::random_device rd;
    std::uniform_int_distribution<int> k_generate(0, num_ - 1);
    std::mt19937 rng(rd());
    for (int i = 0; i < num_; ++i) {
      std::vector<std::pair<float, uint32_t>> single_edge;
      std::unordered_set<uint32_t> edge_ids;
      edge_ids.insert(i);
      for (int j = 0; j < max_degree_; ++j) {
        auto id = k_generate(rng);
        while (edge_ids.find(id) != edge_ids.end()) {
          id = k_generate(rng);
        }
        auto dist = getDistance(i, id);
        single_edge.emplace_back(dist, id);
      }
      edges_.push_back(single_edge);
    }
  }

  void check_turn() const {
    int edge_count = 0;
    float edge_avg_dist = 0;
    for (int i = 0; i < num_; ++i) {
      edge_count += edges_[i].size();
      for (int j = 0; j < edges_[i].size(); ++j) {
        edge_avg_dist += edges_[i][j].first;
      }
    }
    std::cout << "edge_count:" << edge_count << "   edge_avg_dist:" << edge_avg_dist / edge_count << std::endl;
  }

  void iter() {
    std::vector<std::vector<std::pair<float, uint32_t>>> new_edges_;
    std::vector<std::vector<uint32_t>> neighbors;
    new_edges_.resize(num_);
    neighbors.resize(num_);
    for (int i = 0; i < num_; ++i) {
      auto &edges_i = edges_[i];
      for (auto & j : edges_i) {
        if ((*float_distr)(*rng) < 0.3) {
          neighbors[i].push_back(j.second);
          neighbors[j.second].push_back(i);
        }
      }
    }
    for (int i = 0; i < num_; ++i) {
      // 考虑反向边
      for (int j = 0; j < neighbors[i].size(); ++j) { // 采样部分点进行计算
        for (int k = j + 1; k < neighbors[i].size(); ++k) { // 在无更新时依旧有较多的计算量
          auto dist = getDistance(neighbors[i][j], neighbors[i][k]);
          new_edges_[neighbors[i][j]].emplace_back(dist, neighbors[i][k]);
          new_edges_[neighbors[i][k]].emplace_back(dist, neighbors[i][j]);
        }
      }
    }
    for (int i = 0; i < num_; ++i) {
      edges_[i].insert(edges_[i].end(), new_edges_[i].begin(), new_edges_[i].end());
      std::sort(edges_[i].begin(), edges_[i].end());
      edges_[i].erase(std::unique(edges_[i].begin(), edges_[i].end()), edges_[i].end());
      if (edges_[i].size() > max_degree_) {
        edges_[i].resize(max_degree_);
      }
    }
  }


  float getDistance(size_t id1, size_t id2) {
    return L2Sqr(data_.get() + id1 * dim_, data_.get() + id2 * dim_, &dim_);
  }


  std::shared_ptr<float[]> data_{nullptr};
  size_t dim_{0};
  size_t num_{0};
  size_t max_degree_{0};
  std::vector<std::vector<std::pair<float, uint32_t>>> edges_;
  std::random_device *rd;
  std::uniform_real_distribution<float> *float_distr;
  std::mt19937 *rng;
};

} // namespace knngraph