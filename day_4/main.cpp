// result part 1: 2554
// result part 2: 1916

#include <algorithm>
#include <cassert>
#include <deque>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <map>
#include <unordered_map>
#include <vector>

enum direction { down_right = -1, down, down_left, right };

struct point {
  size_t row{};
  size_t col{};
  explicit point() : row(), col() {};
  point(size_t r, size_t c) : row(r), col(c) {};
};
bool operator==(const point &lhs, const point &rhs) {
  if (lhs.row == rhs.row && lhs.col == rhs.col) {
    return true;
  }
  return false;
}

template <> struct std::hash<point> {
  size_t operator()(const point &other) const noexcept {
    return (other.row << 3) + other.col;
  }
};

struct word_inst {
  char word_key{};
  size_t char_pos{};
  point midpoint{};
  std::vector<direction> dir{};
};

auto update_position_1(std::vector<word_inst> &set, const char &c,
                       int64_t &count,
                       std::vector<std::vector<word_inst>> &c_line,
                       size_t &col_idx, size_t &row_idx, direction dir,
                       const std::map<char, std::string_view> &word_list)
    -> void {
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
        c_line[col_idx].push_back(w);
      }
    }
  }
}

auto update_position_2(std::vector<word_inst> &set, const char &c,
                       int64_t &count,
                       std::vector<std::vector<word_inst>> &c_line,
                       size_t &col_idx, size_t &row_idx, direction dir,
                       const std::map<char, std::string_view> &word_list,
                       std::unordered_map<point, int> &midpoints) -> void {
  for (auto &w : set) {
    std::string_view s{word_list.at(w.word_key)};
    if (c == s.at(w.char_pos + 1)) {
      bool dir_unknown =
          std::find(w.dir.begin(), w.dir.end(), dir) == w.dir.end();
      if (w.char_pos > 0 && dir_unknown) {
        continue;
      }
      ++w.char_pos;
      if (w.char_pos == 1) {
        point mp(row_idx, col_idx);
        w.midpoint = mp;
      }
      if (w.char_pos == s.size() - 1) {
        if (midpoints.contains(w.midpoint)) {
          ++midpoints.at(w.midpoint);
        } else {
          midpoints[w.midpoint] = 1;
        }

        if (midpoints.at(w.midpoint) == 2) {
          ++count;
        }
      } else {
        if (dir_unknown) {
          w.dir.push_back(dir);
        }
        c_line[col_idx].push_back(w);
      }
    }
  }
}

auto calc_part_1(std::vector<word_inst> &c_set, std::vector<word_inst> &p_set,
                 std::string &buf, int64_t &result, size_t row_idx,
                 std::vector<std::vector<word_inst>> &c_line,
                 std::vector<std::vector<word_inst>> &p_line,
                 const std::map<char, std::string_view> &word_list) -> void {
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

    size_t col_idx{c_line.size() - 1};
    update_position_1(c_set, c, result, c_line, col_idx, row_idx, right,
                      word_list);

    // check previous line
    if (!p_line.empty()) {
      for (auto &offset : {-1, 0, 1}) {
        if (col_idx + offset >= 0 && col_idx + offset < p_line.size()) {
          p_set = p_line.at(col_idx + offset);
          update_position_1(p_set, c, result, c_line, col_idx, row_idx,
                            direction(offset), word_list);
        }
      }
    }
  }
  p_line.swap(c_line);
  c_line.clear();
}

auto calc_part_2(std::vector<word_inst> &c_set, std::vector<word_inst> &p_set,
                 std::string &buf, int64_t &result, size_t row_idx,
                 std::vector<std::vector<word_inst>> &c_line,
                 std::vector<std::vector<word_inst>> &p_line,
                 const std::map<char, std::string_view> &word_list,
                 std::unordered_map<point, int> &midpoints) -> void {
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

    size_t col_idx{c_line.size() - 1};
    //    update_position_2(c_set, c, result, c_line, col_idx, row_idx, right,
    //                      word_list, midpoints);

    // check previous line
    if (!p_line.empty()) {
      for (auto &offset : {-1, 1}) {
        if (col_idx + offset >= 0 && col_idx + offset < p_line.size()) {
          p_set = p_line.at(col_idx + offset);
          update_position_2(p_set, c, result, c_line, col_idx, row_idx,
                            direction(offset), word_list, midpoints);
        }
      }
    }
  }
  p_line.swap(c_line);
  c_line.clear();
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

  const std::map<char, std::string_view> word_list = {{'X', "XMAS"},
                                                      {'S', "SAMX"}};
  const std::map<char, std::string_view> x_word_list = {{'M', "MAS"},
                                                        {'S', "SAM"}};

  std::ifstream file{path};
  std::string buf{};
  std::vector<int64_t> results{0, 0};

  std::vector<std::vector<word_inst>> p_line{};
  std::vector<std::vector<word_inst>> c_line{};
  std::vector<word_inst> p_set{};
  std::vector<word_inst> c_set{};

  std::vector<std::vector<word_inst>> x_p_line{};
  std::vector<std::vector<word_inst>> x_c_line{};
  std::vector<word_inst> x_p_set{};
  std::vector<word_inst> x_c_set{};
  std::unordered_map<point, int> midpoints{};

  size_t row_idx{};
  while (std::getline(file, buf)) {
    calc_part_1(c_set, p_set, buf, results.at(0), row_idx, c_line, p_line,
                word_list);
    calc_part_2(x_c_set, x_p_set, buf, results.at(1), row_idx, x_c_line,
                x_p_line, x_word_list, midpoints);
    ++row_idx;
  }
  std::cout << "Day 4, Part 1 result: " << results.at(0) << std::endl;
  std::cout << "Day 4, Part 2 result: " << results.at(1) << std::endl;

  return 0;
}
