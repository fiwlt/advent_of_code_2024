// result part 1: 2554
// result part 2:

#include <algorithm>
#include <cassert>
#include <deque>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <map>
#include <vector>

enum direction { down_right = -1, down, down_left, right };

struct word_inst {
  char word_key{};
  size_t char_pos{};
  std::vector<direction> dir{};
};

const std::map<char, std::string_view> word_list = {{'X', "XMAS"},
                                                    {'S', "SAMX"}};

auto update_position(std::vector<word_inst> &set, const char &c, int64_t &count,
                     std::vector<std::vector<word_inst>> &c_line,
                     const size_t &line_idx, direction dir) -> void {
  for (auto &w : set) {
    std::string_view s{word_list.at(w.word_key)};
    if (c == s.at(w.char_pos + 1)) {
      bool dir_unknown =
          std::find(w.dir.begin(), w.dir.end(), dir) == w.dir.end();
      if (w.char_pos > 0 && dir_unknown) {
        continue;
      }
      ++w.char_pos;
      if (w.char_pos == s.size() - 1) {
        ++count;
      } else {
        if (dir_unknown) {
          w.dir.push_back(dir);
        }
        c_line[line_idx].push_back(w);
      }
    }
  }
}

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
  std::vector<int64_t> results{0, 0};

  std::vector<std::vector<word_inst>> p_line{};
  std::vector<std::vector<word_inst>> c_line{};
  std::vector<word_inst> p_set{};
  std::vector<word_inst> c_set{};

  while (std::getline(file, buf)) {
    for (const auto &c : buf) {
      if (!c_line.empty() && !c_line.back().empty()) {
        c_set = c_line.back();
      } else {
        c_set = {};
      }
      c_line.push_back({});
      if (word_list.contains(c)) {
        word_inst tmp{};
        tmp.word_key = c;
        c_line.back().push_back(tmp);
      }

      size_t line_idx{c_line.size() - 1};
      update_position(c_set, c, results.at(0), c_line, line_idx, right);

      // check previous line
      if (!p_line.empty()) {
        for (auto &offset : {-1, 0, 1}) {
          if (line_idx + offset >= 0 && line_idx + offset < p_line.size()) {
            p_set = p_line.at(line_idx + offset);
            update_position(p_set, c, results.at(0), c_line, line_idx,
                            direction(offset));
          }
        }
      }
    }
    p_line.swap(c_line);
    c_line.clear();
  }
  std::cout << "Day 4, Part 1 result: " << results.at(0) << std::endl;
  std::cout << "Day 4, Part 2 result: " << results.at(1) << std::endl;

  return 0;
}
