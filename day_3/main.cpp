// result part 1: 188741603
// result part 2: 67269798

#include <cassert>
#include <deque>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <vector>

#include "parser.hpp"

int main(const int argc, const char *argv[]) {
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

  std::ifstream file{path};
  std::string buf{};

  auto p1{parser()};
  auto p2{parser()};
  p2.can_be_disabled = true;
  std::vector<int64_t> results{0, 0};
  while (std::getline(file, buf)) {
    for (const auto &c : buf) {
      p1.step(c);
      p2.step(c);
    }
    results.at(0) += p1.get_value();
    results.at(1) += p2.get_value();
    p1.init();
    p2.init();
  }

  std::cout << "Day 3, Part 1 result: " << results.at(0) << std::endl;
  std::cout << "Day 3, Part 2 result: " << results.at(1) << std::endl;

  return 0;
}
