#include <iostream>
#include <random>
#include <cstdlib>
#include <chrono>
#include <set>
#include "hnsw/hnswlib.h"
#include "hnsw/knn_graph.h"

size_t
calculate_overlap(const std::vector<uint32_t>& vec1, std::vector<uint32_t>& vec2, int K) {
  int size1 = std::min(K, static_cast<int>(vec1.size()));
  int size2 = std::min(K, static_cast<int>(vec2.size()));

  std::vector<uint32_t> top_k_vec1(vec1.begin(), vec1.begin() + size1);
  std::vector<uint32_t> top_k_vec2(vec2.begin(), vec2.begin() + size2);

  std::sort(top_k_vec1.rbegin(), top_k_vec1.rend());
  std::sort(top_k_vec2.rbegin(), top_k_vec2.rend());

  std::set<uint32_t> set1(top_k_vec1.begin(), top_k_vec1.end());
  std::set<uint32_t> set2(top_k_vec2.begin(), top_k_vec2.end());

  std::set<uint32_t> intersection;
  std::set_intersection(set1.begin(),
                        set1.end(),
                        set2.begin(),
                        set2.end(),
                        std::inserter(intersection, intersection.begin()));
  return intersection.size();
}

int main() {

  std::random_device rd;
  std::uniform_real_distribution float_distr;
  std::mt19937 rng(rd());
  int data_num = 10000;
  size_t data_dim = 64;
  int max_degree = 24;
  auto vectors = std::shared_ptr<float[]>(new float[data_num * data_dim]);
  for (int i = 0; i < data_num * data_dim; ++i) {
    vectors[i] = float_distr(rng);
  }

  knngraph::Graph g(vectors, data_dim, data_num, max_degree);
  g.BuildGraph();
  int hit_edge_count = 0;
  auto graph = g.GetGraph();
  for (int i = 0; i < data_num; ++i) {
    std::vector<std::pair<float, uint32_t>> ground_truths;
    for (int j = 0; j < data_num; ++j) {
      if (i != j) {
        ground_truths.emplace_back(knngraph::L2Sqr(vectors.get() + i * data_dim, vectors.get() + j * data_dim, &data_dim),
                                   j);
      }
    }
    std::sort(ground_truths.begin(), ground_truths.end());
    std::vector<uint32_t> truths_edges(max_degree);
    for (int j = 0; j < max_degree; ++j) {
      truths_edges[j] = ground_truths[j].second;
    }
    hit_edge_count += calculate_overlap(truths_edges, graph[i], max_degree);
  }
  std::cout << hit_edge_count / (float) (data_num * max_degree) << std::endl;
  return 0;
}
