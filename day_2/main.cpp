// result part 1: 549
// result part 2: 589

#include <iostream>
#include <fstream>
#include <deque>
#include <filesystem>
#include <ranges>
#include <vector>

auto is_report_safe(const std::vector<int64_t> &report, const bool &dampener_on = false) -> bool;

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

  std::ifstream file{path};
  std::string buf{};
  int safe_reps{};
  int safe_reps_dampener{};

  while (std::getline(file, buf)) {
    auto tokens = buf
                  | std::views::split(' ')
                  | std::views::transform(
                    [](auto t) { return std::string_view(t.begin(), t.end()); }
                  );
    //first: report value
    std::vector<int64_t> report{};
    for (const auto &t: tokens) {
      int64_t n{};
      std::from_chars(t.data(), t.data() + t.size(), n);
      report.push_back(n);
    }
    if (is_report_safe(report, false)) { ++safe_reps; }
    if (is_report_safe(report, true)) { ++safe_reps_dampener; }
  }

  std::cout << "Day 2, Part 1 result: " << safe_reps << std::endl;
  std::cout << "Day 2, Part 2 result: " << safe_reps_dampener << std::endl;

  return 0;
}

struct patch_pos
{
  size_t index;
  bool patches;
};

auto is_report_safe(const std::vector<int64_t> &report, const bool &dampener_on) -> bool
{
  bool is_sorted{false};
  bool is_patched{false};
  std::pair<patch_pos, patch_pos> patch_pair{};
  auto it = std::is_sorted_until(report.begin(), report.end(), std::less());
  if (it == report.end()) {
    is_sorted = true;
  } else {
    if (dampener_on) {
      patch_pair.first.index = std::distance(report.begin(), it - 1);
      patch_pair.second.index = std::distance(report.begin(), it);
      auto tmp = report;
      tmp.erase(tmp.begin() + patch_pair.first.index);
      if (std::is_sorted(tmp.begin(), tmp.end(), std::less())) {
        patch_pair.first.patches = true;
      }
      tmp = report;
      tmp.erase(tmp.begin() + patch_pair.second.index);
      if (std::is_sorted(tmp.begin(), tmp.end(), std::less())) {
        patch_pair.second.patches = true;
      }
      if (patch_pair.first.patches || patch_pair.second.patches) {
        is_sorted = true;
        is_patched = true;
      }
    }
  }

  if (!is_sorted) {
    it = std::is_sorted_until(report.begin(), report.end(), std::greater());
    if (it != report.end()) {
      if (dampener_on) {
        patch_pair.first.index = std::distance(report.begin(), it - 1);
        patch_pair.second.index = std::distance(report.begin(), it);
        auto tmp = report;
        tmp.erase(tmp.begin() + patch_pair.first.index);
        if (std::is_sorted(tmp.begin(), tmp.end(), std::greater())) {
          patch_pair.first.patches = true;
          is_sorted = true;
          is_patched = true;
        }
        tmp = report;
        tmp.erase(tmp.begin() + patch_pair.second.index);
        if (std::is_sorted(tmp.begin(), tmp.end(), std::greater())) {
          patch_pair.second.patches = true;
          is_sorted = true;
          is_patched = true;
        }
      }
    } else { is_sorted = true; }
  }

  if (!is_sorted) { return false; }

  if (dampener_on) {
    if (is_patched) {
      //check patched report
      //only check valid patches
      auto tmp = report;
      if (patch_pair.first.patches) {
        tmp.erase(tmp.begin() + patch_pair.first.index);
        auto el = std::ranges::adjacent_find(
          tmp,
          [](int64_t x, int64_t y) { return std::abs(x - y) < 1 || std::abs(x - y) > 3; }
        );
        if (el == tmp.end()) { return true; }
      }
      tmp = report;
      if (patch_pair.second.patches) {
        tmp.erase(tmp.begin() + patch_pair.second.index);
        auto el = std::ranges::adjacent_find(
          tmp,
          [](int64_t x, int64_t y) { return std::abs(x - y) < 1 || std::abs(x - y) > 3; }
        );
        if (el == tmp.end()) { return true; }
      }
      return false;
    } //end is_patched

    auto tmp = report;
    auto el = std::ranges::adjacent_find(
      tmp,
      [](int64_t x, int64_t y) { return std::abs(x - y) < 1 || std::abs(x - y) > 3; }
    );
    if (el != tmp.end()) {
      //try to patch
      patch_pair.first.index = std::distance(tmp.begin(), el);
      patch_pair.second.index = std::distance(tmp.begin(), el + 1);
      tmp.erase(tmp.begin() + patch_pair.first.index);
      auto loc_el = std::ranges::adjacent_find(
        tmp,
        [](int64_t x, int64_t y) { return std::abs(x - y) < 1 || std::abs(x - y) > 3; }
      );
      if (loc_el == tmp.end()) { return true; }
      tmp = report;
      tmp.erase(tmp.begin() + patch_pair.second.index);
      loc_el = std::ranges::adjacent_find(
        tmp,
        [](int64_t x, int64_t y) { return std::abs(x - y) < 1 || std::abs(x - y) > 3; }
      );
      if (loc_el == tmp.end()) { return true; }
      return false;
    }
    return true;
  } //end dampener_on
  auto el = std::ranges::adjacent_find(
    report,
    [](int64_t x, int64_t y) { return std::abs(x - y) < 1 || std::abs(x - y) > 3; }
  );
  if (el != report.end()) { return false; }
  return true;
}
