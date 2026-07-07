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
  auto unwrap() -> T { return m_value; }

  /**
   * Set the value of a term to the *current* value of a formula.
   * @param form The formula to be evaluated and set this term to.
   */
  auto set(std::shared_ptr<formula<T>> form) -> void
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
  auto set(std::shared_ptr<term<T>> the_term) -> void
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

  template<typename U>
  friend auto operator+(std::shared_ptr<term<U>>& lhs,
                        std::shared_ptr<term<U>>& rhs)
      -> std::shared_ptr<formula<U>>;

  template<typename U>
  friend auto operator+(std::shared_ptr<term<U>>& lhs,
                        std::shared_ptr<formula<U>>& rhs)
      -> std::shared_ptr<formula<U>>;

  template<typename U>
  friend auto operator-(std::shared_ptr<term<U>>& lhs,
                        std::shared_ptr<term<U>>& rhs)
      -> std::shared_ptr<formula<U>>;

  template<typename U>
  friend auto operator-(std::shared_ptr<term<U>>& lhs,
                        std::shared_ptr<formula<U>>& rhs)
      -> std::shared_ptr<formula<U>>;

  template<typename U>
  friend auto operator*(std::shared_ptr<term<U>>& lhs,
                        std::shared_ptr<term<U>>& rhs)
      -> std::shared_ptr<formula<U>>;

  template<typename U>
  friend auto operator*(std::shared_ptr<term<U>>& lhs,
                        std::shared_ptr<formula<U>>& rhs)
      -> std::shared_ptr<formula<U>>;

  template<typename U>
  friend auto operator/(std::shared_ptr<term<U>>& lhs,
                        std::shared_ptr<term<U>>& rhs)
      -> std::shared_ptr<formula<U>>;

  template<typename U>
  friend auto operator/(std::shared_ptr<term<U>>& lhs,
                        std::shared_ptr<formula<U>>& rhs)
      -> std::shared_ptr<formula<U>>;

  template<typename U>
  friend auto operator%(std::shared_ptr<term<U>>& lhs,
                        std::shared_ptr<term<U>>& rhs)
      -> std::shared_ptr<formula<U>>;

  template<typename U>
  friend auto operator%(std::shared_ptr<term<U>>& lhs,
                        std::shared_ptr<formula<U>>& rhs)
      -> std::shared_ptr<formula<U>>;

};

/**
 * Overloading to allow two terms to be added together.
 */
template<typename T>
auto operator+(std::shared_ptr<term<T>>& lhs, std::shared_ptr<term<T>>& rhs)
    -> std::shared_ptr<formula<T>>
{
  auto add = [](const auto& lhss, const auto& rhss) -> T
  { return lhss + rhss; };
  const bin_expr<T> new_expr = {lhs, rhs, add};
  std::shared_ptr<formula<T>> form = std::make_shared<formula<T>>(new_expr);
  lhs->m_parents.push_back(form);
  rhs->m_parents.push_back(form);
  return form;
}

/**
 * Overloading to allow a term and formula to be added together.
 */
template<typename T>
auto operator+(std::shared_ptr<term<T>>& lhs, std::shared_ptr<formula<T>>& rhs)
    -> std::shared_ptr<formula<T>>
{
  auto add = [](const auto& lhss, const auto& rhss) -> T
  { return lhss + rhss; };
  const bin_expr<T> new_expr = {lhs, rhs, add};
  std::shared_ptr<formula<T>> form = std::make_shared<formula<T>>(new_expr);
  lhs->m_parents.push_back(form);
  rhs->m_parents.push_back(form);
  return form;
}

/**
 * Overloading to allow two terms to subtract from on another (lhs - rhs).
 */
template<typename T>
auto operator-(std::shared_ptr<term<T>>& lhs, std::shared_ptr<term<T>>& rhs)
    -> std::shared_ptr<formula<T>>
{
  auto add = [](const auto& lhss, const auto& rhss) -> T
  { return lhss - rhss; };
  const bin_expr<T> new_expr = {lhs, rhs, add};
  std::shared_ptr<formula<T>> form = std::make_shared<formula<T>>(new_expr);
  lhs->m_parents.push_back(form);
  rhs->m_parents.push_back(form);
  return form;
}

/**
 * Overloading to allow a term and formula to be added together (lhs - rhs).
 */
template<typename T>
auto operator-(std::shared_ptr<term<T>>& lhs, std::shared_ptr<formula<T>>& rhs)
    -> std::shared_ptr<formula<T>>
{
  auto add = [](const auto& lhss, const auto& rhss) -> T
  { return lhss - rhss; };
  const bin_expr<T> new_expr = {lhs, rhs, add};
  std::shared_ptr<formula<T>> form = std::make_shared<formula<T>>(new_expr);
  lhs->m_parents.push_back(form);
  rhs->m_parents.push_back(form);
  return form;
}

/**
 * Overloading to allow two terms to multiply.
 */
template<typename T>
auto operator*(std::shared_ptr<term<T>>& lhs, std::shared_ptr<term<T>>& rhs)
    -> std::shared_ptr<formula<T>>
{
  auto add = [](const auto& lhss, const auto& rhss) -> T
  { return lhss * rhss; };
  const bin_expr<T> new_expr = {lhs, rhs, add};
  std::shared_ptr<formula<T>> form = std::make_shared<formula<T>>(new_expr);
  lhs->m_parents.push_back(form);
  rhs->m_parents.push_back(form);
  return form;
}

/**
 * Overloading to allow a term and formula to be multiplied together.
 */
template<typename T>
auto operator*(std::shared_ptr<term<T>>& lhs, std::shared_ptr<formula<T>>& rhs)
    -> std::shared_ptr<formula<T>>
{
  auto add = [](const auto& lhss, const auto& rhss) -> T
  { return lhss * rhss; };
  const bin_expr<T> new_expr = {lhs, rhs, add};
  std::shared_ptr<formula<T>> form = std::make_shared<formula<T>>(new_expr);
  lhs->m_parents.push_back(form);
  rhs->m_parents.push_back(form);
  return form;
}

/**
 * Overloading to allow two terms to divide. (lhs / rhs).
 */
template<typename T>
auto operator/(std::shared_ptr<term<T>>& lhs, std::shared_ptr<term<T>>& rhs)
    -> std::shared_ptr<formula<T>>
{
  auto add = [](const auto& lhss, const auto& rhss) -> T
  { return lhss / rhss; };
  const bin_expr<T> new_expr = {lhs, rhs, add};
  std::shared_ptr<formula<T>> form = std::make_shared<formula<T>>(new_expr);
  lhs->m_parents.push_back(form);
  rhs->m_parents.push_back(form);
  return form;
}

/**
 * Overloading to allow a term and formula to be divided by the other.
 */
template<typename T>
auto operator/(std::shared_ptr<term<T>>& lhs, std::shared_ptr<formula<T>>& rhs)
    -> std::shared_ptr<formula<T>>
{
  auto add = [](const auto& lhss, const auto& rhss) -> T
  { return lhss / rhss; };
  const bin_expr<T> new_expr = {lhs, rhs, add};
  std::shared_ptr<formula<T>> form = std::make_shared<formula<T>>(new_expr);
  lhs->m_parents.push_back(form);
  rhs->m_parents.push_back(form);
  return form;
}

/**
 * Take modulo of lhs by rhs.
 */
template<typename T>
auto operator%(std::shared_ptr<term<T>>& lhs, std::shared_ptr<term<T>>& rhs)
    -> std::shared_ptr<formula<T>>
{
  auto add = [](const auto& lhss, const auto& rhss) -> T
  { return lhss % rhss; };
  const bin_expr<T> new_expr = {lhs, rhs, add};
  std::shared_ptr<formula<T>> form = std::make_shared<formula<T>>(new_expr);
  lhs->m_parents.push_back(form);
  rhs->m_parents.push_back(form);
  return form;
}

/**
 * Overloading to allow a term and formula to be modulo each other.
 */
template<typename T>
auto operator%(std::shared_ptr<term<T>>& lhs, std::shared_ptr<formula<T>>& rhs)
    -> std::shared_ptr<formula<T>>
{
  auto add = [](const auto& lhss, const auto& rhss) -> T
  { return lhss % rhss; };
  const bin_expr<T> new_expr = {lhs, rhs, add};
  std::shared_ptr<formula<T>> form = std::make_shared<formula<T>>(new_expr);
  lhs->m_parents.push_back(form);
  rhs->m_parents.push_back(form);
  return form;
}

/**
 * A "stmt" is either a term or formula. (Note: Sister library `forcamla` does
 * *not* make this distinction.)
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
  stmt<T> rhs;
  std::function<T(T)> op;
};

/**
 * Keep track of a binary operation applied to two (sub-)formula (the lhs and
 * rhs).
 */
template<typename T>
struct bin_expr
{
  stmt<T> lhs;
  stmt<T> rhs;
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
  auto eval() -> T
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
};
