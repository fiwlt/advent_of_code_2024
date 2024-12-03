// result part 1: 1889772
// result part 2: 23228917

#include <iostream>
#include <fstream>
#include <deque>
#include <filesystem>
#include <iterator>
#include <vector>
#include <algorithm>

auto parse_file(const std::filesystem::path &, std::vector<int64_t> &, std::vector<int64_t> &) -> void;

auto calc_sorted_distance(std::vector<int64_t> &, std::vector<int64_t> &) -> int64_t;

auto calc_similarity_score(const std::vector<int64_t> &, const std::vector<int64_t> &) -> int64_t;

int main(const int argc, const char *argv[])
{
  std::deque<std::string_view> args(argv, argv + argc);
  args.pop_front();
  if (args.empty()) {
    std::cout << "no file name given!" << std::endl;
    return 1;
  }

  std::filesystem::path path{args.at(0)};
  if (!std::filesystem::exists(path)) {
    std::cout << "file not found: " << path << std::endl;
    return 1;
  }

  std::vector<int64_t> list_1;
  std::vector<int64_t> list_2;

  parse_file(path, list_1, list_2);

  std::cout << "Day 1, Part 1 result: " << calc_sorted_distance(list_1, list_2) << std::endl;
  std::cout << "Day 1, Part 2 result: " << calc_similarity_score(list_1, list_2) << std::endl;

  return 0;
}

//based on GCC C++20 implementation of std::back_insert_iterator
//turns assignment into accumulation
class accumulating_iterator
{
  std::shared_ptr<int64_t> sum;
public:
  explicit accumulating_iterator(const std::shared_ptr<int64_t> &value): sum(value) {}
  accumulating_iterator &operator=(int64_t &value) { *this->sum += value; return *this; }
  accumulating_iterator &operator=(const int64_t &value) { *this->sum += value; return *this; }
  accumulating_iterator &operator*() { return *this; }
  accumulating_iterator &operator++() { return *this; } //accumulated value, iterator does not move
  accumulating_iterator &operator++(int) { return *this; }
};

auto parse_file(const std::filesystem::path &path, std::vector<int64_t> &list_1, std::vector<int64_t> &list_2) -> void
{
  std::ifstream file{path};
  int64_t num_1{};
  int64_t num_2{};
  while (file >> num_1 >> num_2) {
    list_1.push_back(num_1);
    list_2.push_back(num_2);
  }
}

auto calc_sorted_distance(std::vector<int64_t> &list_1, std::vector<int64_t> &list_2) -> int64_t
{
  std::ranges::sort(list_1.begin(), list_1.end());
  std::ranges::sort(list_2.begin(), list_2.end());
  auto sum{std::make_shared<int64_t>(0)};
  std::transform(
    list_1.begin(), list_1.end(),
    list_2.begin(),
    accumulating_iterator(sum),
    [](int64_t x, int64_t y) { return std::abs(x - y); }
  );
  return *sum;
}

auto sorted_count(const std::vector<int64_t> &sorted_vec, const int64_t &e) -> int64_t
{
  auto l_it{std::ranges::lower_bound(sorted_vec.begin(), sorted_vec.end(), e)};
  auto u_it{std::ranges::upper_bound(l_it, sorted_vec.end(), e)};
  return std::ranges::distance(l_it, u_it);
}

auto calc_similarity_score(const std::vector<int64_t> &list_1, const std::vector<int64_t> &list_2) -> int64_t
{
  auto score{std::make_shared<int64_t>(0)};
  std::transform(
    list_1.begin(), list_1.end(),
    accumulating_iterator(score),
    [&list_2](int64_t x) { return x * sorted_count(list_2, x); }
  );
  return *score;
}
