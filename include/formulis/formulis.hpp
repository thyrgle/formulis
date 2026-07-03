#include <functional>
#include <variant>
#include <vector>

// Helper to combine multiple lambdas, used for "pattern matching."
template<class... Ts>
struct overloaded : Ts...
{
  using Ts::operator()...;
};
// For C++17 compatibility.
template<class... Ts>
overloaded(Ts...) -> overloaded<Ts...>;

template<typename T>
struct unary_expr;

template<typename T>
struct bin_expr;

template<typename T>
class formula;

template<typename T>
class term
{
  std::vector<formula<T>*> m_parents;
  std::vector<std::function<void(T, T)>> m_on_change;
  T m_value;

  auto change_detected(T old_value)
  {
    // Things changed. Send message to parents.
    for (const auto& func : m_on_change) {
      func(old_value, m_value);
    }
    // Propagate changes to parents.
    for (auto& parent : m_parents) {
      parent->set_needs_update();
      parent->update();
    }
  }

public:
  explicit term(T value)
      : m_parents({})
      , m_on_change({})
      , m_value(value)
  {
  }

  auto unwrap() -> T { return m_value; }

  auto set(formula<T> form) -> void
  {
    T old_value = m_value;
    m_value = form.eval();
    // Nothing changed, early exit.
    if (old_value == m_value) {
      return;
    }
    // Make changes now that change happened.
    change_detected(old_value);
  }

  auto set(term<T> the_term) -> void
  {
    T old_value = m_value;
    m_value = the_term.unwrap();
    // Nothing changed, early exit.
    if (old_value == m_value) {
      return;
    }
    // Make changes now that change happened.
    change_detected(old_value);
  }

  auto set(T value) -> void
  {
    T old_value = m_value;
    m_value = value;
    // Nothing changed, early exit.
    if (old_value == m_value) {
      return;
    }
    // Make changes now that change happened.
    change_detected(old_value);
  }
};

template<typename T>
auto operator+(term<T>& lhs, term<T>& rhs) -> formula<T>
{
  auto add = [](const auto& lhss, const auto& rhss) -> T
  { return lhss + rhss; };
  const bin_expr<T> new_expr = {&lhs, &rhs, add};
  return formula(new_expr);
}

template<typename T>
auto operator+(const term<T>& lhs, const formula<T>& rhs) -> formula<T>
{
  auto add = [](const auto& lhss, const auto& rhss) -> T
  { return lhss + rhss; };
  const bin_expr<T> new_expr = {&lhs, &rhs, add};
  return formula(new_expr);
}

template<typename T>
using stmt = std::variant<term<T>*, formula<T>*>;

template<typename T>
struct unary_expr
{
  stmt<T> rhs;
  std::function<T(T)> op;
};

template<typename T>
struct bin_expr
{
  stmt<T> lhs;
  stmt<T> rhs;
  std::function<T(T, T)> op;
};

template<typename T>
class formula
{
  bool m_needs_update;
  std::variant<unary_expr<T>, bin_expr<T>> m_expr;
  T m_cached_val;
  std::vector<formula<T>*> m_parents;
  std::vector<std::function<T(T, T)>> m_on_change;

public:
  explicit formula(unary_expr<T> expr)
      : m_needs_update(true)
      , m_expr(expr)
      , m_cached_val(eval())
  {
  }

  explicit formula(bin_expr<T> expr)
      : m_needs_update(true)
      , m_expr(expr)
      , m_cached_val(eval())
  {
  }

  auto set_needs_update() -> void { m_needs_update = true; }

  auto eval() -> T
  {
    if (!m_needs_update) {
      return m_cached_val;
    }
    return std::visit(
        overloaded {
            [](unary_expr<T> expression) -> T
            {
              auto inner = std::visit(
                  overloaded {[](term<T>* val) -> T { return val->unwrap(); },
                              [](formula<T>* ast) -> T
                              {
                                return (ast->m_needs_update
                                            ? ast->eval()
                                            : ast->m_cached_val);
                              }},
                  expression.rhs);
              return expression.op(inner);
            },
            [](bin_expr<T> operand) -> T
            {
              auto inner_lhs = std::visit(
                  overloaded {[](term<T>* val) -> T { return val->unwrap(); },
                              [](formula<T>* ast) -> T { return ast->eval(); }},
                  operand.lhs);
              auto inner_rhs = std::visit(
                  overloaded {[](term<T>* val) -> T { return val->unwrap(); },
                              [](formula<T>* ast) -> T { return ast->eval(); }},
                  operand.rhs);
              return operand.op(inner_lhs, inner_rhs);
            }},
        m_expr);
  }

  auto update() -> void
  {
    T old_val = m_cached_val;
    m_cached_val = eval();
    if (old_val != m_cached_val) {
      for (const auto& func : m_on_change) {
        func(old_val, m_cached_val);
      }
      for (auto& parent : m_parents) {
        parent->update();
      }
    }
    m_needs_update = false;
  }
};
