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

enum direction { unknown, right, down_right, down, down_left };

struct word_inst {
  char word_key{};
  size_t char_pos{};
  std::vector<direction> dir{};
};

auto update_position(word_inst &p, const char &c, const std::string_view &s,
                     int64_t &count,
                     std::vector<std::vector<word_inst>> &c_line,
                     const size_t &line_idx, direction dir) -> void {
  if (p.char_pos > 0 &&
      std::find(p.dir.begin(), p.dir.end(), dir) == p.dir.end()) {
    return;
  }
  ++p.char_pos;
  if (std::find(p.dir.begin(), p.dir.end(), dir) == p.dir.end()) {
    p.dir.push_back(dir);
  }
  if (p.char_pos == s.size() - 1) {
    ++count;
  } else {
    c_line[line_idx].push_back(p);
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

  std::map<char, std::string_view> word_list;
  word_list['X'] = "XMAS";
  word_list['S'] = "SAMX";
  std::vector<std::vector<word_inst>> p_line{};
  std::vector<std::vector<word_inst>> c_line{};

  while (std::getline(file, buf)) {
    std::vector<int> pline_size{};
    for (const auto &c : buf) {
      std::vector<word_inst> c_set{};
      if (!c_line.empty() && !c_line.back().empty()) {
        std::copy(c_line.back().begin(), c_line.back().end(),
                  std::back_inserter(c_set));
      }
       c_line.push_back({});
      if (word_list.contains(c)) {
        word_inst tmp{};
        tmp.word_key = c;
        c_line.back().push_back(tmp);
      }

      size_t line_idx{c_line.size() - 1};
      for (auto &p : c_set) {
        std::string_view s{word_list.at(p.word_key)};
        if (c == s.at(p.char_pos + 1)) {
          update_position(p, c, s, results.at(0), c_line, line_idx, right);
        }
      }

      // check previous line
      if (!p_line.empty()) {
        std::vector<word_inst> p_set{};
        int offset{-1};
        if (line_idx > 0) {
          std::copy(p_line.at(line_idx + offset).begin(),
                    p_line.at(line_idx + offset).end(),
                    std::back_inserter(p_set));

          for (auto &p : p_set) {
            std::string_view s{word_list.at(p.word_key)};
            if (c == s.at(p.char_pos + 1)) {
              update_position(p, c, s, results.at(0), c_line, line_idx,
                              down_right);
            }
          }
        }
        ++offset;
        p_set.clear();
        std::copy(p_line.at(line_idx + offset).begin(),
                  p_line.at(line_idx + offset).end(),
                  std::back_inserter(p_set));

        for (auto &p : p_set) {
          std::string_view s{word_list.at(p.word_key)};
          if (c == s.at(p.char_pos + 1)) {
            update_position(p, c, s, results.at(0), c_line, line_idx, down);
          }
        }

        p_set.clear();
        ++offset;
        if (line_idx < p_line.size() - 1) {
          std::copy(p_line.at(line_idx + offset).begin(),
                    p_line.at(line_idx + offset).end(),
                    std::back_inserter(p_set));

          for (auto &p : p_set) {
            std::string_view s{word_list.at(p.word_key)};
            if (c == s.at(p.char_pos + 1)) {
              update_position(p, c, s, results.at(0), c_line, line_idx,
                              down_left);

            }
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
