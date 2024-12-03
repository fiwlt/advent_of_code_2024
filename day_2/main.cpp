// result part 1: 549
// result part 2: 589

#include <algorithm>
#include <charconv>
#include <deque>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <ranges>
#include <vector>

auto is_report_safe(const std::vector<int64_t> &report,
                    const bool &dampener_on = false) -> bool;

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
  int safe_reps{};
  int safe_reps_dampener{};

  while (std::getline(file, buf)) {
    auto tokens =
        buf | std::views::split(' ') | std::views::transform([](auto t) {
          return std::string_view(t.begin(), t.end());
        });
    // first: report value
    std::vector<int64_t> report{};
    for (const auto &t : tokens) {
      int64_t n{};
      std::from_chars(t.data(), t.data() + t.size(), n);
      report.push_back(n);
    }
    if (is_report_safe(report, false)) {
      ++safe_reps;
    }
    if (is_report_safe(report, true)) {
      ++safe_reps_dampener;
    }
  }

  std::cout << "Day 2, Part 1 result: " << safe_reps << std::endl;
  std::cout << "Day 2, Part 2 result: " << safe_reps_dampener << std::endl;

  return 0;
}

struct patch_pos {
  size_t index;
  bool patches;
};

auto is_report_safe(const std::vector<int64_t> &report, const bool &dampener_on)
    -> bool {
  bool is_sorted{false};
  bool is_patched{false};
  std::vector<patch_pos> patch_pair{patch_pos(), patch_pos()};
  auto it = std::is_sorted_until(report.begin(), report.end(), std::less());

  auto tmp = report;
  if (it == report.end()) {
    is_sorted = true;
  } else if (dampener_on) {
    patch_pair.at(0).index = std::distance(report.begin(), it - 1);
    patch_pair.at(1).index = std::distance(report.begin(), it);
    for (auto &p : patch_pair) {
      tmp = report;
      tmp.erase(tmp.begin() + p.index);
      if (std::is_sorted(tmp.begin(), tmp.end(), std::less())) {
        p.patches = true;
      }
    }
    if (patch_pair.at(0).patches || patch_pair.at(1).patches) {
      is_sorted = true;
      is_patched = true;
    }
  }

  if (!is_sorted) {
    it = std::is_sorted_until(report.begin(), report.end(), std::greater());
    if (it == report.end()) {
      is_sorted = true;
    } else if (dampener_on) {
      patch_pair.at(0).index = std::distance(report.begin(), it - 1);
      patch_pair.at(1).index = std::distance(report.begin(), it);
      for (auto &p : patch_pair) {
        tmp = report;
        tmp.erase(tmp.begin() + p.index);
        if (std::is_sorted(tmp.begin(), tmp.end(), std::greater())) {
          p.patches = true;
        }
      }
      if (patch_pair.at(0).patches || patch_pair.at(1).patches) {
        is_sorted = true;
        is_patched = true;
      }
    }
  }

  if (!is_sorted) {
    return false;
  }

  if (is_patched) {
    // check patched report
    // only check valid patches
    for (const auto &p : patch_pair) {
      if (p.patches == false) {
        continue;
      }
      tmp = report;
      tmp.erase(tmp.begin() + p.index);
      auto el = std::ranges::adjacent_find(tmp, [](int64_t x, int64_t y) {
        return std::abs(x - y) < 1 || std::abs(x - y) > 3;
      });
      if (el == tmp.end()) {
        return true;
      }
    }
    return false;
  } else {
    auto el = std::ranges::adjacent_find(report, [](int64_t x, int64_t y) {
      return std::abs(x - y) < 1 || std::abs(x - y) > 3;
    });
    if (el == report.end()) {
      return true;
    } else if (dampener_on) {
      // try to patch
      patch_pair.at(0).index = std::distance(report.begin(), el);
      patch_pair.at(1).index = std::distance(report.begin(), el + 1);
      for (const auto &p : patch_pair) {
        tmp = report;
        tmp.erase(tmp.begin() + p.index);
        auto loc_el = std::ranges::adjacent_find(tmp, [](int64_t x, int64_t y) {
          return std::abs(x - y) < 1 || std::abs(x - y) > 3;
        });
        if (loc_el == tmp.end()) {
          return true;
        }
      }
    }
  }
  return false;
}
