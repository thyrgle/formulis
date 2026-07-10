#include <algorithm>
#include <functional>
#include <variant>
#include <vector>

// See below for docs on this.
template<class... Ts>
struct overloaded : Ts...
{
  using Ts::operator()...;
};
// For C++17 compatibility.
/**
 * Helper to combine multiple lambdas, used for "pattern matching."
 */
template<class... Ts>
overloaded(Ts...) -> overloaded<Ts...>;

/**
 * Helper macro to generate operator overloads. In particular, this creates
 * friend declarations arithmetic binary operations.
 */
#define BEFRIEND_BIN(name, t1, t2) \
  template<typename U> \
  friend auto operator name(t1& lhs, t2& rhs)->formula<U>&;

/**
 * Helper macro to generate operator overloads. In particular, this creates
 * friend declarations for boolean binary operations.
 */
#define BEFRIEND_BIN_BOOL(name, t1, t2) \
  template<typename U> \
  friend auto operator name(t1& lhs, t2& rhs)->formula<bool>&;

/**
 * Helper macro to generate operator overloads. In particular, this creates
 * friend declarations for arithmetic unary operations.
 */
#define BEFRIEND_UNARY(name, t1) \
  template<typename U> \
  friend auto operator~(t1& rhs)->formula<U>&;

/**
 * Helper macro to generate operator overloads. In particular, this creates
 * friend declarations for boolean unary operations.
 */
#define BEFRIEND_UNARY_BOOL(name, t1) \
  template<typename U> \
  friend auto operator~(t1& rhs)->formula<bool>&;

/**
 * Helper macro to generate operator overloads. In particular, this creates
 * friend declarations for unary arithmetic operators in the term class.
 */
#define TERM_BEFRIEND_UNARY_OP(op) BEFRIEND_UNARY(op, term<U>)

/**
 * Helper macro to generate operator overloads. In particular, this creates
 * friend declarations for unary boolean operators in the term class.
 */
#define TERM_BEFRIEND_UNARY_OP_BOOL(op) BEFRIEND_UNARY_BOOL(op, term<U>)

/**
 * Helper macro to generate operator overloads. In particular, this creates
 * friend declarations for unary arithmetic operators in the formula class.
 */
#define FORMULA_BEFRIEND_UNARY_OP(op) BEFRIEND_UNARY(op, formula<U>)

/**
 * Helper macro to generate operator overloads. In particular, this creates
 * friend declarations for unary boolean operators in the formula class.
 */
#define FORMULA_BEFRIEND_UNARY_OP_BOOL(op) BEFRIEND_UNARY_BOOL(op, formula<U>)

/**
 * Helper macro to generate operator overloads. In particular, this creates
 * friend declarations for binary arithmetic operators in the term class.
 */
#define TERM_BEFRIEND_BIN_OP(op) \
  BEFRIEND_BIN(op, const U, term<U>) \
  BEFRIEND_BIN(op, term<U>, const U) \
  BEFRIEND_BIN(op, term<U>, term<U>) \
  BEFRIEND_BIN(op, term<U>, formula<U>) \
  BEFRIEND_BIN(op, formula<U>, term<U>)

/**
 * Helper macro to generate operator overloads. In particular, this creates
 * friend declarations for binary boolean operators in the term class.
 */
#define TERM_BEFRIEND_BIN_OP_BOOL(op) \
  BEFRIEND_BIN_BOOL(op, const U, term<U>) \
  BEFRIEND_BIN_BOOL(op, term<U>, const U) \
  BEFRIEND_BIN_BOOL(op, term<U>, term<U>) \
  BEFRIEND_BIN_BOOL(op, term<U>, formula<U>) \
  BEFRIEND_BIN_BOOL(op, formula<U>, term<U>)

/**
 * Helper macro to generate operator overloads. In particular, this creates
 * friend declarations for binary arithmetic operators in the formula class.
 */
#define FORMULA_BEFRIEND_BIN_OP(op) \
  BEFRIEND_BIN(op, const U, formula<U>) \
  BEFRIEND_BIN(op, formula<U>, const U) \
  BEFRIEND_BIN(op, term<U>, formula<U>) \
  BEFRIEND_BIN(op, formula<U>, term<U>) \
  BEFRIEND_BIN(op, formula<U>, formula<U>)

/**
 * Helper macro to generate operator overloads. In particular, this creates
 * friend declarations for binary boolean operators in the formula class.
 */
#define FORMULA_BEFRIEND_BIN_OP_BOOL(op) \
  BEFRIEND_BIN_BOOL(op, const U, formula<U>) \
  BEFRIEND_BIN_BOOL(op, formula<U>, const U) \
  BEFRIEND_BIN_BOOL(op, term<U>, formula<U>) \
  BEFRIEND_BIN_BOOL(op, formula<U>, term<U>) \
  BEFRIEND_BIN_BOOL(op, formula<U>, formula<U>)

/**
 * Helper macro to generate operator overloads. In particular, this creates
 * operator overloading when the lhs is a numeric. As in `auto z = 1 + x`.
 */
#define REGISTER_OVERLOAD_VAL_LEFT(name, t1, t2) \
  template<typename T> \
  auto operator name(t1& lhs, t2& rhs)->formula<T>& \
  { \
    auto comb = [](const auto& lhss, const auto& rhss) -> T \
    { return lhss name rhss; }; \
    term<T>* new_term = new term<T>(lhs); \
    const bin_expr<T> new_expr = {new_term, &rhs, comb}; \
    formula<T>* form = new formula<T>(new_expr); \
    new_term->m_parents.push_back(form); \
    rhs.m_parents.push_back(form); \
    return *form; \
  }

/**
 * Helper macro to generate operator overloads. In particular, this creates
 * operator overloading when the lhs is a numeric/bool. As in `auto z = true ||
 * x`.
 */
#define REGISTER_OVERLOAD_VAL_LEFT_BOOL(name, t1, t2) \
  template<typename T> \
  auto operator name(t1& lhs, t2& rhs)->formula<bool>& \
  { \
    auto comb = [](const auto& lhss, const auto& rhss) -> bool \
    { return lhss name rhss; }; \
    term<T>* new_term = new term<T>(lhs); \
    const bin_expr<T> new_expr = {new_term, &rhs, comb}; \
    formula<T>* form = new formula<T>(new_expr); \
    new_term->m_parents.push_back(form); \
    rhs.m_parents.push_back(form); \
    return *form; \
  }

/**
 * Helper macro to generate operator overloads. In particular, this creates
 * operator overloading when the rhs is a numeric. As in `auto z = x + 1`.
 */
#define REGISTER_OVERLOAD_VAL_RIGHT(name, t1, t2) \
  template<typename T> \
  auto operator name(t1& lhs, t2& rhs)->formula<T>& \
  { \
    auto comb = [](const auto& lhss, const auto& rhss) -> T \
    { return lhss name rhss; }; \
    term<T>* new_term = new term<T>(rhs); \
    const bin_expr<T> new_expr = {&lhs, new_term, comb}; \
    formula<T>* form = new formula<T>(new_expr); \
    lhs.m_parents.push_back(form); \
    new_term->m_parents.push_back(form); \
    return *form; \
  }

/**
 * Helper macro to generate operator overloads. In particular, this creates
 * operator overloading when the rhs is a numeric/bool. As in `auto z = x ||
 * true`.
 */
#define REGISTER_OVERLOAD_VAL_RIGHT_BOOL(name, t1, t2) \
  template<typename T> \
  auto operator name(t1& lhs, t2& rhs)->formula<bool>& \
  { \
    auto comb = [](const auto& lhss, const auto& rhss) -> bool \
    { return lhss name rhss; }; \
    term<T>* new_term = new term<T>(rhs); \
    const bin_expr<T> new_expr = {&lhs, new_term, comb}; \
    formula<T>* form = new formula<T>(new_expr); \
    lhs.m_parents.push_back(form); \
    new_term->m_parents.push_back(form); \
    return *form; \
  }

/**
 * Helper macro to generate operator overloads. In particular, this creates a
 * particular arithmetic binary operator overload given the operator and two
 * types.
 */
#define REGISTER_BIN_OVERLOAD(name, t1, t2) \
  template<typename T> \
  auto operator name(t1& lhs, t2& rhs)->formula<T>& \
  { \
    auto comb = [](const auto& lhss, const auto& rhss) -> T \
    { return lhss name rhss; }; \
    const bin_expr<T> new_expr = {&lhs, &rhs, comb}; \
    formula<T>* form = new formula<T>(new_expr); \
    lhs.m_parents.push_back(form); \
    rhs.m_parents.push_back(form); \
    return *form; \
  }

/**
 * Helper macro to generate operator overloads. In particular, this creates a
 * particular boolean binary operator overload given the operator and two types.
 */
#define REGISTER_BIN_OVERLOAD_BOOL(name, t1, t2) \
  template<typename T> \
  auto operator name(t1& lhs, t2& rhs)->formula<bool>& \
  { \
    auto comb = [](const auto& lhss, const auto& rhss) -> bool \
    { return lhss name rhss; }; \
    const bin_expr<T> new_expr = {&lhs, &rhs, comb}; \
    formula<T>* form = formula<T>(new_expr); \
    lhs.m_parents.push_back(form); \
    rhs.m_parents.push_back(form); \
    return *form; \
  }

/**
 * Helper macro to gnerate operator overloads. In particular, generates every
 * operator overload for a particular binary arithmetic operator.
 */
#define REGISTER_BIN_OP(op) \
  REGISTER_BIN_OVERLOAD(op, term<T>, formula<T>) \
  REGISTER_BIN_OVERLOAD(op, formula<T>, term<T>) \
  REGISTER_BIN_OVERLOAD(op, formula<T>, formula<T>) \
  REGISTER_OVERLOAD_VAL_LEFT(op, const T, formula<T>) \
  REGISTER_OVERLOAD_VAL_RIGHT(op, formula<T>, const T) \
  REGISTER_OVERLOAD_VAL_LEFT(op, const T, term<T>) \
  REGISTER_OVERLOAD_VAL_RIGHT(op, term<T>, const T) \
  REGISTER_BIN_OVERLOAD(op, term<T>, term<T>)

/**
 * Helper macro to gnerate operator overloads. In particular, generates every
 * operator overload for a particular binary boolean operator.
 */
#define REGISTER_BIN_OP_BOOL(op) \
  REGISTER_BIN_OVERLOAD_BOOL(op, term<T>, formula<T>) \
  REGISTER_BIN_OVERLOAD_BOOL(op, formula<T>, term<T>) \
  REGISTER_BIN_OVERLOAD_BOOL(op, formula<T>, formula<T>) \
  REGISTER_OVERLOAD_VAL_LEFT_BOOL(op, const T, formula<T>) \
  REGISTER_OVERLOAD_VAL_RIGHT_BOOL(op, formula<T>, const T) \
  REGISTER_OVERLOAD_VAL_LEFT_BOOL(op, const T, term<T>) \
  REGISTER_OVERLOAD_VAL_RIGHT_BOOL(op, term<T>, const T) \
  REGISTER_BIN_OVERLOAD_BOOL(op, term<T>, term<T>)

/**
 * Helper macro to generate operator overloads. In particular, this creates a
 * particular arithmetic unary operator overload given the operator and a type.
 */
#define REGISTER_UNARY_OVERLOAD(name, t1) \
  template<typename T> \
  auto operator name(t1& rhs)->formula<T>& \
  { \
    auto comb = [](const auto& rhss) -> T { return name rhss; }; \
    const unary_expr<T> new_expr = {&rhs, comb}; \
    formula<T>* form = new formula<T>(new_expr); \
    rhs.m_parents.push_back(form); \
    return *form; \
  }

/**
 * Helper macro to generate operator overloads. In particular, this creates a
 * particular boolean unary operator overload given the operator and a type.
 */
#define REGISTER_UNARY_OVERLOAD_BOOL(name, t1) \
  template<typename T> \
  auto operator name(t1& rhs)->formula<bool>& \
  { \
    auto comb = [](const auto& rhss) -> bool { return name rhss; }; \
    const unary_expr<T> new_expr = {&rhs, comb}; \
    formula<T>* form = new formula<T>(new_expr); \
    rhs->m_parents.push_back(form); \
    return *form; \
  }

/**
 * Helper macro to gnerate operator overloads. In particular, generates every
 * operator overload for a particular unary arithmetic operator.
 */
#define REGISTER_UNARY_OP(op) \
  REGISTER_UNARY_OVERLOAD(op, term<T>) \
  REGISTER_UNARY_OVERLOAD(op, formula<T>)

/**
 * Helper macro to gnerate operator overloads. In particular, generates every
 * operator overload for a particular unary boolean operator.
 */
#define REGISTER_UNARY_OP_BOOL(op) \
  REGISTER_UNARY_OVERLOAD_BOOL(op, term<T>) \
  REGISTER_UNARY_OVERLOAD_BOOL(op, formula<T>)

#define REGISTER_INPLACE(name) \
  auto operator name(const term<T>& other)->term<T>& \
  { \
    T val = this->unwrap(); \
    val name other.unwrap(); \
    this->set(val); \
    return *this; \
  } \
  auto operator name(const formula<T>& other)->term<T>& \
  { \
    T val = this->unwrap(); \
    val name other.eval(); \
    this->set(val); \
    return *this; \
  } \
  auto operator name(const T& other)->term<T>& \
  { \
    T val = this->unwrap(); \
    val name other; \
    this->set(val); \
    return *this; \
  }

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

  template<typename U>
  auto remove_parent(formula<U>* parent) -> void
  {
    m_parents.erase(std::remove(m_parents.begin(), m_parents.end(), parent));
  }

  ~term()
  {
    for (auto* parent : m_parents) {
      parent->~formula();
    }
  }

  /**
   * Get the current value of the term. (For `formula` use `eval`).
   */
  auto unwrap() const -> T { return m_value; }

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
  TERM_BEFRIEND_BIN_OP_BOOL(&&)
  TERM_BEFRIEND_BIN_OP_BOOL(||)

  TERM_BEFRIEND_UNARY_OP_BOOL(~)
  TERM_BEFRIEND_UNARY_OP_BOOL(!)

  REGISTER_INPLACE(+=)
  REGISTER_INPLACE(-=)
  REGISTER_INPLACE(*=)
  REGISTER_INPLACE(/=)
  REGISTER_INPLACE(%=)
  REGISTER_INPLACE(^=)
  REGISTER_INPLACE(&=)
  REGISTER_INPLACE(|=)

  /**
   * Overload increment. Note: Returns the previous value of type `T` not
   * `term<T>`.
   */
  auto operator++()
  {
    T new_value = this->unwrap() + 1;
    this->set(new_value);
    return *this;
  }

  /**
   * Overload decrement. Note: Returns the previous value of type `T` not
   * `term<T>`.
   */
  auto operator--()
  {
    T new_value = this->unwrap() - 1;
    this->set(new_value);
    return *this;
  }

  /**
   * Overload increment. Note: Returns the previous value of type `T` not
   * `term<T>`.
   */
  auto operator++(int)
  {
    T return_value = this->unwrap();
    ++(*this);
    return return_value;
  }

  /**
   * Overload decrement. Note: Returns the previous value of type `T` not
   * `term<T>`.
   */
  auto operator--(int)
  {
    T return_value = this->unwrap();
    --(*this);
    return return_value;
  }
};

/**
 * A "stmt" is either a term or formula. Used extensively in `eval` to determine
 * how to evaluate an expression. (Note: Sister library `forcamla` does *not*
 * make this distinction.)
 */
template<typename T>
using stmt = std::variant<term<T>*, formula<T>*>;

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
  std::vector<formula<T>*> m_parents;
  std::vector<stmt<T>> m_children;
  std::vector<std::function<void(T, T)>> m_on_change;

public:
  explicit formula(unary_expr<T> expr)
      : m_needs_update(true)
      , m_expr(expr)
      , m_cached_val(eval())
      , m_parents({})
      , m_on_change({})
  {
    m_children.push_back(expr.rhs);
  }

  explicit formula(bin_expr<T> expr)
      : m_needs_update(true)
      , m_expr(expr)
      , m_cached_val(eval())
      , m_parents({})
      , m_on_change({})
  {
    m_children.push_back(expr.lhs);
    m_children.push_back(expr.rhs);
  }

  template<typename U>
  auto remove_parent(formula<U>* parent) -> void
  {
    m_parents.erase(std::remove(m_parents.begin(), m_parents.end(), parent));
  }

  ~formula()
  {
    // Mark parent formula for deletion.
    for (auto* parent : m_parents) {
      parent->~formula();
    }
    for (auto child : m_children) {
      std::visit(overloaded {[this](term<T>* child) -> void
                             { child->remove_parent(this); },
                             [this](formula<T>* child) -> void
                             { child->remove_parent(this); }},
                 child);
    }
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
        parent->set_needs_update();
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
  FORMULA_BEFRIEND_BIN_OP_BOOL(&&)
  FORMULA_BEFRIEND_BIN_OP_BOOL(||)

  FORMULA_BEFRIEND_UNARY_OP_BOOL(~)
  FORMULA_BEFRIEND_UNARY_OP_BOOL(!)
};

REGISTER_BIN_OP(+)
REGISTER_BIN_OP(-)
REGISTER_BIN_OP(*)
REGISTER_BIN_OP(/)
REGISTER_BIN_OP(%)
REGISTER_BIN_OP(^)
REGISTER_BIN_OP(&)
REGISTER_BIN_OP(|)
REGISTER_BIN_OP_BOOL(&&)
REGISTER_BIN_OP_BOOL(||)

REGISTER_UNARY_OP(~)
REGISTER_UNARY_OP_BOOL(!)
