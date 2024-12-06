// result part 1: 3608
// result part 2:

#include <cassert>
#include <charconv>
#include <deque>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <map>
#include <ranges>
#include <vector>
#include <algorithm>

auto valid_update(const std::string &buf,
                  std::map<uint32_t, std::vector<uint32_t>> &f_pmap,
                  std::map<uint32_t, std::vector<uint32_t>> &r_pmap,
                  int64_t &score) -> bool {
  auto tokens =
      buf | std::views::split(',') | std::views::transform([](auto t) {
        return std::string_view(t.begin(), t.end());
      });
  std::vector<uint32_t> p{};
  for (const auto &t : tokens) {
    auto s{std::string_view(t.begin(), t.end())};
    uint32_t u;
    std::from_chars(s.data(), s.data() + s.size(), u);
    p.push_back(u);
  }
  for (const auto &v: p){
  std::cout << v << " ";
  }
  std::cout << std::endl;
  std::map<uint32_t, std::vector<uint32_t>> t_f_pmap{};
  std::map<uint32_t, std::vector<uint32_t>> t_r_pmap{};

  //tailor maps
  for(const auto &i: p){
    for(const auto &v: f_pmap[i]){
      if (std::find(p.begin(), p.end(), v) != p.end()){
        t_f_pmap[i].push_back(v);
      }
    }
    for(const auto &v: r_pmap[i]){
      if (std::find(p.begin(), p.end(), v) != p.end()){
        t_r_pmap[i].push_back(v);
      }
    }
  }

  bool is_valid{true};
  for(const auto &i: p){
    std::cout << t_f_pmap[i].empty() << " ";
    if (!t_f_pmap[i].empty()){is_valid = false; break;}
    for (auto &ri: t_r_pmap[i]){
      t_f_pmap[ri].pop_back();
    }
  }
  std::cout << std::endl;
  if (is_valid){
    score += p.at((p.size()-1)/2);
  }
  return false;
}

auto parse_rule(const std::string &buf,
                std::map<uint32_t, std::vector<uint32_t>> &f_pmap,
                std::map<uint32_t, std::vector<uint32_t>> &r_pmap) -> void {
  auto tokens =
      buf | std::views::split('|') | std::views::transform([](auto t) {
        return std::string_view(t.begin(), t.end());
      });
  std::vector<uint32_t> p{};
  for (const auto &t : tokens) {
    auto s{std::string_view(t.begin(), t.end())};
    uint32_t u;
    std::from_chars(s.data(), s.data() + s.size(), u);
    p.push_back(u);
  }

  f_pmap[p.at(1)].push_back(p.at(0));
  r_pmap[p.at(0)].push_back(p.at(1));
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

  std::map<uint32_t, std::vector<uint32_t>> f_pmap{};
  std::map<uint32_t, std::vector<uint32_t>> r_pmap{};

  bool eval_updates{false};
  while (std::getline(file, buf)) {
    if (eval_updates) {
      // evaluate updates
      valid_update(buf, f_pmap, r_pmap, results.at(0));
    } else {
      // read page pairs
      if (buf.empty()) {
        // on empty line switch to evaluating updates
        eval_updates = true;
        continue;
      }
      parse_rule(buf, f_pmap, r_pmap);
    }
  }
  std::cout << "Day 5, Part 1 result: " << results.at(0) << std::endl;
  std::cout << "Day 5, Part 2 result: " << results.at(1) << std::endl;

  return 0;
}
