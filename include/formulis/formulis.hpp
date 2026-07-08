#include <functional>
#include <memory>
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

#define BEFRIEND_BIN(name, t1, t2) \
  template<typename U> \
  friend auto operator name(t1& lhs, t2& rhs)->std::shared_ptr<formula<U>>;

#define BEFRIEND_UNARY(name, t1) \
  template<typename U> \
  friend auto operator~(t1& rhs)->std::shared_ptr<formula<U>>;

#define TERM_BEFRIEND_UNARY_OP(op) BEFRIEND_UNARY(op, std::shared_ptr<term<U>>)

#define FORMULA_BEFRIEND_UNARY_OP(op) \
  BEFRIEND_UNARY(op, std::shared_ptr<formula<U>>)

#define TERM_BEFRIEND_BIN_OP(op) \
  BEFRIEND_BIN(op, U, std::shared_ptr<term<U>>) \
  BEFRIEND_BIN(op, std::shared_ptr<term<U>>, U) \
  BEFRIEND_BIN(op, std::shared_ptr<term<U>>, std::shared_ptr<term<U>>) \
  BEFRIEND_BIN(op, std::shared_ptr<term<U>>, std::shared_ptr<formula<U>>) \
  BEFRIEND_BIN(op, std::shared_ptr<formula<U>>, std::shared_ptr<term<U>>)

#define FORMULA_BEFRIEND_BIN_OP(op) \
  BEFRIEND_BIN(op, U, std::shared_ptr<formula<U>>) \
  BEFRIEND_BIN(op, std::shared_ptr<formula<U>>, U) \
  BEFRIEND_BIN(op, std::shared_ptr<term<U>>, std::shared_ptr<formula<U>>) \
  BEFRIEND_BIN(op, std::shared_ptr<formula<U>>, std::shared_ptr<term<U>>) \
  BEFRIEND_BIN(op, std::shared_ptr<formula<U>>, std::shared_ptr<formula<U>>)

#define REGISTER_OVERLOAD_VAL_LEFT(name, t1, t2) \
  template<typename T> \
  auto operator name(t1& lhs, t2& rhs)->std::shared_ptr<formula<T>> \
  { \
    auto comb = [](const auto& lhss, const auto& rhss) -> T \
    { return lhss name rhss; }; \
    const bin_expr<T> new_expr = {std::make_shared<term<T>>(lhs), rhs, comb}; \
    std::shared_ptr<formula<T>> form = std::make_shared<formula<T>>(new_expr); \
    lhs->m_parents.push_back(form); \
    rhs->m_parents.push_back(form); \
    return form; \
  }

#define REGISTER_OVERLOAD_VAL_RIGHT(name, t1, t2) \
  template<typename T> \
  auto operator name(t1& lhs, t2& rhs)->std::shared_ptr<formula<T>> \
  { \
    auto comb = [](const auto& lhss, const auto& rhss) -> T \
    { return lhss name rhss; }; \
    const bin_expr<T> new_expr = {lhs, std::make_shared<term<T>>(rhs), comb}; \
    std::shared_ptr<formula<T>> form = std::make_shared<formula<T>>(new_expr); \
    lhs->m_parents.push_back(form); \
    rhs->m_parents.push_back(form); \
    return form; \
  }

#define REGISTER_BIN_OVERLOAD(name, t1, t2) \
  template<typename T> \
  auto operator name(t1& lhs, t2& rhs)->std::shared_ptr<formula<T>> \
  { \
    auto comb = [](const auto& lhss, const auto& rhss) -> T \
    { return lhss name rhss; }; \
    const bin_expr<T> new_expr = {lhs, rhs, comb}; \
    std::shared_ptr<formula<T>> form = std::make_shared<formula<T>>(new_expr); \
    lhs->m_parents.push_back(form); \
    rhs->m_parents.push_back(form); \
    return form; \
  }

#define REGISTER_BIN_OP(op) \
  REGISTER_OVERLOAD_VAL_LEFT(op, T, std::shared_ptr<term<T>>) \
  REGISTER_BIN_OVERLOAD( \
      op, std::shared_ptr<term<T>>, std::shared_ptr<term<T>>) \
  REGISTER_BIN_OVERLOAD( \
      op, std::shared_ptr<term<T>>, std::shared_ptr<formula<T>>) \
  REGISTER_BIN_OVERLOAD( \
      op, std::shared_ptr<formula<T>>, std::shared_ptr<term<T>>) \
  REGISTER_OVERLOAD_VAL_RIGHT(op, T, std::shared_ptr<formula<T>>) \
  REGISTER_BIN_OVERLOAD(op, std::shared_ptr<formula<T>>, T) \
  REGISTER_BIN_OVERLOAD( \
      op, std::shared_ptr<formula<T>>, std::shared_ptr<formula<T>>)

#define REGISTER_UNARY_OVERLOAD(name, t1) \
  template<typename T> \
  auto operator name(t1& rhs)->std::shared_ptr<formula<T>> \
  { \
    auto comb = [](const auto& rhss) -> T { return name rhss; }; \
    const unary_expr<T> new_expr = {rhs, comb}; \
    std::shared_ptr<formula<T>> form = std::make_shared<formula<T>>(new_expr); \
    rhs->m_parents.push_back(form); \
    return form; \
  }

#define REGISTER_UNARY_OP(op) \
  REGISTER_UNARY_OVERLOAD(op, std::shared_ptr<term<T>>) \
  REGISTER_UNARY_OVERLOAD(op, std::shared_ptr<formula<T>>)

template<typename T>
struct unary_expr;

template<typename T>
struct bin_expr;

template<typename T>
class formula;

/**
 * `term` objects and `formula` objects are two of the fundamental types.
 * `term` objects store a value that can be changed over time. Importantly,
 * when they change, parent formula are also changed *automatically*. In this
 * context, a "parent" formula simply uses the term. So if `formula z = x + y;`
 * then `x` and `y` are terms that may change and `z` stores the result of
 * adding `x` and `y`.
 */
template<typename T>
class term
{
  std::vector<std::shared_ptr<formula<T>>> m_parents;
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

  /**
   * Get the current value of the term. (For `formula` use `eval`).
   */
  auto unwrap() const -> T { return m_value; }

  /**
   * Dereferencing -> an alias for unwrap.
   */
  auto operator*() const -> const T& { return unwrap(); }

  /**
   * Set the value of a term to the *current* value of a formula.
   * @param form The formula to be evaluated and set this term to.
   */
  auto set(const formula<T>& form) -> void
  {
    T old_value = m_value;
    m_value = form->eval();
    // Nothing changed, early exit.
    if (old_value == m_value) {
      return;
    }
    // Make changes now that change happened.
    change_detected(old_value);
  }

  /**
   * Set the value of a term to the *current* value of a term.
   * @param the_term The term to evaluate and set this term to.
   */
  auto set(const term<T>& the_term) -> void
  {
    T old_value = m_value;
    m_value = the_term->unwrap();
    // Nothing changed, early exit.
    if (old_value == m_value) {
      return;
    }
    // Make changes now that change happened.
    change_detected(old_value);
  }

  /**
   * Set the value of a term to a specified value (of the appropriate type.
   * @param value The value to set this term to.
   */
  auto set(const T value) -> void
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

  TERM_BEFRIEND_BIN_OP(+)
  TERM_BEFRIEND_BIN_OP(-)
  TERM_BEFRIEND_BIN_OP(*)
  TERM_BEFRIEND_BIN_OP(/)
  TERM_BEFRIEND_BIN_OP(%)
  TERM_BEFRIEND_BIN_OP(^)
  TERM_BEFRIEND_BIN_OP(&)
  TERM_BEFRIEND_BIN_OP(|)
  TERM_BEFRIEND_BIN_OP(&&)
  TERM_BEFRIEND_BIN_OP(||)

  TERM_BEFRIEND_UNARY_OP(~)
  TERM_BEFRIEND_UNARY_OP(!)
};

/**
 * A "stmt" is either a term or formula. Used extensively in `eval` to determine
 * how to evaluate an expression. (Note: Sister library `forcamla` does *not*
 * make this distinction.)
 */
template<typename T>
using stmt =
    std::variant<std::shared_ptr<term<T>>, std::shared_ptr<formula<T>>>;

/**
 * Keep track of a unary operation applied to some inner formula (the "rhs").
 */
template<typename T>
struct unary_expr
{
  /** The operand. For instance, in the expression `!x`, `x` is `rhs`. */
  stmt<T> rhs;
  /** The operator. For instance, in the expression `!x`, `!` is `op`. */
  std::function<T(T)> op;
};

/**
 * Keep track of a binary operation applied to two (sub-)formula (the lhs and
 * rhs).
 */
template<typename T>
struct bin_expr
{
  /** The "left hand" operand. For instance, in the expression `x + y`, `x` is
   * `lhs`. */
  stmt<T> lhs;
  /** The "right hand" operand. For instance, in the expression `x + y`, `y` is
   * `rhs`. */
  stmt<T> rhs;
  /** The operator. For instance, in the expression `x + y`, `+` is `op`. */
  std::function<T(T, T)> op;
};

/**
 * `term` objects and `formula` objects are two of the fundamental types.
 * `formula` objects are `term` objects combined with operations to represent
 * a mathematical formula. They always evaluate to the most up-to-date value of
 * each `term` they use in their expression.
 */
template<typename T>
class formula
{
  bool m_needs_update;
  std::variant<unary_expr<T>, bin_expr<T>> m_expr;
  T m_cached_val;
  std::vector<std::shared_ptr<formula<T>>> m_parents;
  std::vector<std::function<void(T, T)>> m_on_change;

public:
  explicit formula(unary_expr<T> expr)
      : m_needs_update(true)
      , m_expr(expr)
      , m_cached_val(eval())
      , m_parents({})
      , m_on_change({})
  {
  }

  explicit formula(bin_expr<T> expr)
      : m_needs_update(true)
      , m_expr(expr)
      , m_cached_val(eval())
      , m_parents({})
      , m_on_change({})
  {
  }

  /**
   * Indicate the formula's cached value is out of date and an update is needed
   */
  auto set_needs_update() -> void { m_needs_update = true; }

  /**
   * Get the current value of the formula.
   */
  auto eval() const -> T
  {
    if (!m_needs_update) {
      return m_cached_val;
    }
    return std::visit(
        overloaded {[](unary_expr<T> expression) -> T
                    {
                      auto inner = std::visit(
                          overloaded {[](std::shared_ptr<term<T>> val) -> T
                                      { return val->unwrap(); },
                                      [](std::shared_ptr<formula<T>> ast) -> T
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
                          overloaded {[](std::shared_ptr<term<T>> val) -> T
                                      { return val->unwrap(); },
                                      [](std::shared_ptr<formula<T>> ast) -> T
                                      { return ast->eval(); }},
                          operand.lhs);
                      auto inner_rhs = std::visit(
                          overloaded {[](std::shared_ptr<term<T>> val) -> T
                                      { return val->unwrap(); },
                                      [](std::shared_ptr<formula<T>> ast) -> T
                                      { return ast->eval(); }},
                          operand.rhs);
                      return operand.op(inner_lhs, inner_rhs);
                    }},
        m_expr);
  }

  /**
   * Dereferencing -> an alias for eval.
   */
  auto operator*() const -> const T& { return eval(); }

  /**
   * Update the cached value of the formula and also perform function calls
   * if the value was changed.
   */
  auto update() -> void
  {
    T old_val = m_cached_val;
    m_cached_val = eval();
    if (old_val != m_cached_val) {
      for (const auto& func : m_on_change) {
        func(old_val, m_cached_val);
      }
      m_needs_update = false;
      for (auto& parent : m_parents) {
        parent->update();
      }
    }
  }

  /**
   * Execute a function when the value of this formula changes.
   * @param func The function to execute. Takes the old and current value of
   * the formula. (The user is free to do with the old and current values as
   * they wish.)
   */
  auto on_change(std::function<void(T, T)> func) -> void
  {
    m_on_change.push_back(func);
  }

  FORMULA_BEFRIEND_BIN_OP(+)
  FORMULA_BEFRIEND_BIN_OP(-)
  FORMULA_BEFRIEND_BIN_OP(*)
  FORMULA_BEFRIEND_BIN_OP(/)
  FORMULA_BEFRIEND_BIN_OP(%)
  FORMULA_BEFRIEND_BIN_OP(^)
  FORMULA_BEFRIEND_BIN_OP(&)
  FORMULA_BEFRIEND_BIN_OP(|)
  FORMULA_BEFRIEND_BIN_OP(&&)
  FORMULA_BEFRIEND_BIN_OP(||)

  FORMULA_BEFRIEND_UNARY_OP(~)
  FORMULA_BEFRIEND_UNARY_OP(!)
};

REGISTER_BIN_OP(+)
REGISTER_BIN_OP(-)
REGISTER_BIN_OP(*)
REGISTER_BIN_OP(/)
REGISTER_BIN_OP(%)
REGISTER_BIN_OP(^)
REGISTER_BIN_OP(&)
REGISTER_BIN_OP(|)
REGISTER_BIN_OP(&&)
REGISTER_BIN_OP(||)

REGISTER_UNARY_OP(~)
REGISTER_UNARY_OP(!)
