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
      : data_(data), dim_(dim), num_(num), max_degree_(max_degree) {}

  void BuildGraph() {
    init_graph();
    check_turn();
    for (int i = 0; i < 10; ++i) {
      iter();
      check_turn();
    }
  }

  std::vector<std::vector<uint32_t>> GetGraph() {
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

  void check_turn() {
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
    new_edges_.resize(num_);
    for (int i = 0; i < num_; ++i) {
      auto neighbors = edges_[i];
      for (int j = 0; j < neighbors.size(); ++j) {
        for (int k = j + 1; k < neighbors.size(); ++k) {
          auto dist = getDistance(neighbors[j].second, neighbors[k].second);
          new_edges_[neighbors[j].second].emplace_back(dist, neighbors[k].second);
          new_edges_[neighbors[k].second].emplace_back(dist, neighbors[j].second);
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
};

} // namespace knngraph