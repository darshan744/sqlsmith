/// @file
/// @brief grammar: Value expression productions

#ifndef EXPR_HH
#define EXPR_HH

#include "prod.hh"
#include <string>


#include "schema.hh"
using std::shared_ptr;
using std::vector;
using std::string;


/// Base class for expressions
struct value_expr: prod {
  sqltype *type;
  virtual void out(std::ostream &out) = 0;
  virtual ~value_expr() { }
  value_expr(prod *p) : prod(p) { }
  static shared_ptr<value_expr> factory(prod *p, sqltype *type_constraint = 0);
};

/// creates a express with CASE <CONDITION> THEN VALUE ELSE VALUE
struct case_expr : value_expr {
  shared_ptr<value_expr> condition;
  shared_ptr<value_expr> true_expr;
  shared_ptr<value_expr> false_expr;
  case_expr(prod *p, sqltype *type_constraint = 0);
  virtual void out(std::ostream &out);
  virtual void accept(prod_visitor *v);
};

/// Aggregate functions or anything else
struct funcall : value_expr {
  routine *proc;
  bool is_aggregate;
  vector<shared_ptr<value_expr> > parms;
  virtual void out(std::ostream &out);
  virtual ~funcall() { }
  funcall(prod *p, sqltype *type_constraint = 0, bool agg = 0);
  virtual void accept(prod_visitor *v) {
    v->visit(this);
    for (auto p : parms)
      p->accept(v);
  }
};

/// Creates a subquery with returning only one row
struct atomic_subselect : value_expr {
  table *tab;
  column *col;
  int offset;
  routine *agg;
  atomic_subselect(prod *p, sqltype *type_constraint = 0);
  virtual void out(std::ostream &out);
};

// It gives out just a constant values
struct const_expr: value_expr {
  std::string expr;
  const_expr(prod *p, sqltype *type_constraint = 0);
  virtual void out(std::ostream &out) { out << expr; }
  virtual ~const_expr() { }
};

/// A column reference from the chosen table for this query.
/// it creates out table.column using the refs from the scope
struct column_reference: value_expr {
  column_reference(prod *p, sqltype *type_constraint = 0);
  virtual void out(std::ostream &out) { out << reference; }
  std::string reference;
  virtual ~column_reference() { }
};

/// It creates a COALESCE(a , b) sub expression
/// It returns the first non-NULL element
struct coalesce : value_expr {
  const char *abbrev_;
  vector<shared_ptr<value_expr> > value_exprs;
  virtual ~coalesce() { };
  coalesce(prod *p, sqltype *type_constraint = 0, const char *abbrev = "coalesce");
  virtual void out(std::ostream &out);
  virtual void accept(prod_visitor *v) {
    v->visit(this);
    for (auto p : value_exprs)
      p->accept(v);
  }
};

/// a NULLIF(a, b)
/// NULLIF when a = b it returns null else a
struct nullif : coalesce {
 virtual ~nullif() { };
     nullif(prod *p, sqltype *type_constraint = 0)
	  : coalesce(p, type_constraint, "nullif")
	  { };
};

/// Base class for bool expressions
struct bool_expr : value_expr {
  virtual ~bool_expr() { }
  bool_expr(prod *p) : value_expr(p) { type = scope->schema->booltype; }
  static shared_ptr<bool_expr> factory(prod *p);
};

/// only gives out 'where true | where false'
struct truth_value : bool_expr {
  virtual ~truth_value() { }
  const char *op;
  virtual void out(std::ostream &out) { out << op; }
  truth_value(prod *p) : bool_expr(p) {
    op = ( (d6() < 4) ? scope->schema->true_literal : scope->schema->false_literal);
    
  }
};

/// It outs "<expression> is NULL | <expression> is not null"
struct null_predicate : bool_expr {
  virtual ~null_predicate() { }
  const char *negate;
  shared_ptr<value_expr> expr;
  null_predicate(prod *p) : bool_expr(p) {
    negate = ((d6()<4) ? "not " : "");
    expr = value_expr::factory(this);
  }
  virtual void out(std::ostream &out) {
    out << *expr << " is " << negate << "NULL";
  }
  virtual void accept(prod_visitor *v) {
    v->visit(this);
    expr->accept(v);
  }
};

/// Creates EXISTS (subquery)
struct exists_predicate : bool_expr {
  shared_ptr<struct query_spec> subquery;
  virtual ~exists_predicate() { }
  exists_predicate(prod *p);
  virtual void out(std::ostream &out);
  virtual void accept(prod_visitor *v);
};

/// Base class binary operation 
struct bool_binop : bool_expr {
  shared_ptr<value_expr> lhs, rhs;
  bool_binop(prod *p) : bool_expr(p) { }
  virtual void out(std::ostream &out) = 0;
  virtual void accept(prod_visitor *v) {
    v->visit(this);
    lhs->accept(v);
    rhs->accept(v);
  }
};

/// creates a lhs (or|and) rhs 
/// lhs and rhs are expressions
struct bool_term : bool_binop {
  virtual ~bool_term() { }
  const char *op;
  virtual void out(std::ostream &out) {
    out << "(" << *lhs << ") ";
    indent(out);
    out << op << " (" << *rhs << ")";
  }
  bool_term(prod *p) : bool_binop(p)
  {
    op = ((d6()<4) ? "or" : "and");
    lhs = bool_expr::factory(this);
    rhs = bool_expr::factory(this);
  }
};

struct distinct_pred : bool_binop {
  distinct_pred(prod *p);
  virtual ~distinct_pred() { };
  virtual void out(std::ostream &o) {
    o << *lhs << " is distinct from " << *rhs;
  }
};

/// only two column and comparison
struct comparison_op : bool_binop {
  op *oper;
  comparison_op(prod *p);
  virtual ~comparison_op() { };
  virtual void out(std::ostream &o) {
    o << *lhs << " " << oper->name << " " << *rhs;
  }
};

struct window_function : value_expr {
  virtual void out(std::ostream &out);
  virtual ~window_function() { }
  window_function(prod *p, sqltype *type_constraint);
  vector<shared_ptr<column_reference> > partition_by;
  vector<shared_ptr<column_reference> > order_by;
  shared_ptr<funcall> aggregate;
  static bool allowed(prod *pprod);
  virtual void accept(prod_visitor *v) {
    v->visit(this);
    aggregate->accept(v);
    for (auto p : partition_by)
      p->accept(v);
    for (auto p : order_by)
      p->accept(v);
  }
};

#endif
