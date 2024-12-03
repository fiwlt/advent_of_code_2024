#include "parser.hpp"

#include <algorithm>
#include <charconv>
#include <cstdint>
#include <functional>
#include <numeric>
#include <string_view>

void parser::init() {
  this->acc_value = 0;
  this->clear_parser_state();
}

void parser::clear_parser_state() {
  this->current_state = state_function_name;
  this->f_type = _undef;
  this->f_name.clear();
  this->par_idx = 0;
  this->pars.clear();
}

void parser::step(const char &c) {
  switch (this->current_state) {
  case state_function_name:
    this->function_name(c);
    break;
  case state_function_start:
    this->function_start(c);
    break;
  case state_parameter:
    this->parameter(c);
    break;
  case state_par_separator:
    this->par_separator(c);
    break;
  case state_function_end:
    this->function_end(c);
    break;
  }
}

void parser::function_name(const char &c) {
  switch (this->f_type) {
  case _undef:
    this->function_undef(c);
    break;
  case _mul:
    this->function_mul(c);
    break;
  case _do:
  case _dont:
    this->function_do(c);
    break;
  }
}

void parser::function_undef(const char &c) {
  if (c == 'm') {
    this->f_type = _mul;
    this->function_mul(c);
  } else if (c == 'd') {
    this->f_type = _do;
    this->function_do(c);
  }
}

void parser::function_mul(const char &c) {
  auto size{this->f_name.size()};
  if (size < this->func_names.at(_mul).size()) {
    if (c == this->func_names.at(_mul).at(size)) {
      this->f_name.push_back(c);
    } else {
      this->clear_parser_state();
    }
  } else {
    this->current_state = state_function_start;
    this->function_start(c);
  }
}

void parser::function_do(const char &c) {
  auto size{this->f_name.size()};
  if (size < this->func_names.at(_do).size()) {
    if (c == this->func_names.at(_do).at(size)) {
      this->f_name.push_back(c);
    } else {
      this->clear_parser_state();
    }
  } else {
    this->f_type = _dont;
    this->function_dont(c);
  }
}

void parser::function_dont(const char &c) {
  auto size{this->f_name.size()};
  if (size < this->func_names.at(_dont).size()) {
    if (c == this->func_names.at(_dont).at(size)) {
      this->f_name.push_back(c);
    } else {
      this->current_state = state_function_start;
      this->function_start(c);
    }
  } else {
    this->current_state = state_function_start;
    this->function_start(c);
  }
}

void parser::function_start(const char &c) {
  if (c == '(') {
    if (this->f_type == _mul) {
      this->current_state = state_parameter;
      this->par_idx = 0;
    } else {
      this->current_state = state_function_end;
    }
  } else {
    this->clear_parser_state();
    this->step(c);
  }
}

void parser::parameter(const char &c) {
  if (this->pars.size() < this->par_idx + 1) {
    this->pars.push_back(std::vector<char>());
  }
  auto size{this->pars.at(this->par_idx).size()};
  if (std::any_of(this->nums.begin(), this->nums.end(),
                  [&c](const char n) { return n == c; })) {
    if (size < this->max_par_length) {
      this->pars.at(this->par_idx).push_back(c);
    } else if (size == this->max_par_length) {
      this->pars.at(this->par_idx).push_back(c);
      if (this->pars.size() == this->max_pars) {
        this->current_state = state_function_end;
      } else {
        this->current_state = state_par_separator;
      }
    }
  } else {
    if (size == 0) {
      this->clear_parser_state();
      this->step(c);
    } else if (this->par_idx < this->max_pars - 1) {
      this->current_state = state_par_separator;
      this->par_separator(c);
    } else if (this->par_idx == this->max_pars - 1) {
      this->current_state = state_function_end;
      this->function_end(c);
    }
  }
}

void parser::par_separator(const char &c) {
  if (c == ',') {
    this->par_idx++;
    this->current_state = state_parameter;
  } else {
    this->clear_parser_state();
    this->step(c);
  }
}

void parser::function_end(const char &c) {
  this->current_state = state_function_name;
  if (c == ')') {
    this->run_instruction();
  } else {
    this->clear_parser_state();
    this->step(c);
  }
}

void parser::run_instruction() {
  if (this->exe_enabled &&
      "mul" == std::string_view(this->f_name.data(),
                                this->f_name.data() + this->f_name.size())) {
    int64_t tmp_m;
    std::vector<int64_t> m{};
    for (const auto &p : this->pars) {
      std::from_chars(p.data(), p.data() + p.size(), tmp_m);
      m.push_back(tmp_m);
    }
    this->acc_value +=
        std::accumulate(m.begin(), m.end(), 1, std::multiplies());
  } else if (this->can_be_disabled) {
    if ("do" == std::string_view(this->f_name.data(),
                                 this->f_name.data() + this->f_name.size())) {
      this->exe_enabled = true;
    }
    if ("don't" ==
        std::string_view(this->f_name.data(),
                         this->f_name.data() + this->f_name.size())) {
      this->exe_enabled = false;
    }
  }
  this->clear_parser_state();
}
