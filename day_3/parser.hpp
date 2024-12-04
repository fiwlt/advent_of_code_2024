#include <stdint.h>
#include <vector>

class parser {
  enum state {
    state_function_name,
    state_function_start,
    state_parameter,
    state_par_separator,
    state_function_end
  };

  enum type { _mul, _do, _dont, _undef };

  const std::vector<char> nums{'0', '1', '2', '3', '4',
                               '5', '6', '7', '8', '9'};
  const std::vector<std::vector<char>> func_names{
      {'m', 'u', 'l'}, {'d', 'o'}, {'d', 'o', 'n', '\'', 't'}};
  const size_t max_pars = 2;
  const uint8_t max_par_length = 3;

  state current_state;
  type f_type;
  std::vector<char> f_name;
  std::vector<std::vector<char>> pars;
  size_t par_idx;
  int64_t acc_value;
  bool exe_enabled;

  void function_name(const char &c);

  void function_mul(const char &c);

  void function_do(const char &c);

  void function_dont(const char &c);

  void function_undef(const char &c);

  void function_start(const char &c);

  void parameter(const char &c);

  void par_separator(const char &c);

  void function_end(const char &c);

  void run_instruction();

  void clear_parser_state();

public:
  bool can_be_disabled;

  parser()
      : current_state(state_function_name), f_type(_undef), f_name(), pars(),
        par_idx(), acc_value(), exe_enabled(true), can_be_disabled(false) {};

  void init();

  void step(const char &c);

  int64_t get_value() { return this->acc_value; }
};
