#ifndef __AQ_CORE_QUERY_HPP__
#define __AQ_CORE_QUERY_HPP__

#include <string>
#include <vector>

namespace aq {
namespace core {

struct TableReference
{
  std::string name;
};

struct ColumnReference
{
  TableReference table;
  std::string name;
};

struct InCondition
{
  ColumnReference column;
  std::vector<std::string> values;

  InCondition() {}
  InCondition(const InCondition& o) : column(o.column), values(o.values) {}
  InCondition& operator=(const InCondition& o)
  {
    if (this != &o)
    {
      this->column = o.column;
      this->values = o.values;
    }
    return *this;
  }

};

struct JoinCondition
{
  enum class type_t
  {
    K_ACTIVE,
    K_FILTER,
    K_NEUTRAL,
  };

  enum class join_t
  {
    INNER,
    LEFT_OUTER,
    RIGHT_OUTER,
    FULL_OUTER,
  };

  enum class op_t
  {
    AUTO,
    EQ,
    NEQ,
    INF,
    IEQ,
    SEQ,
    SUP,
  };
  
  std::string op;
  std::string type_left, type_right;
  std::string jt_left, jt_right;
  ColumnReference left, right;

  JoinCondition() {}
  JoinCondition(const JoinCondition& o) : op(o.op), type_left(o.type_left), type_right(o.type_right), jt_left(o.jt_left), jt_right(o.jt_right), left(o.left), right(o.right) 
  {
  }
  JoinCondition& operator=(const JoinCondition& o)
  {
    if (this != &o)
    {
      this->op = o.op;
      this->type_left = o.type_left;
      this->type_right = o.type_right;
      this->jt_left = o.jt_left;
      this->jt_right = o.jt_right;
      this->left = o.left;
      this->right = o.right;
    }
    return *this;
  }

};

struct SelectStatement
{
  enum output_t
  {
    AQL,
    AQL_INFIX,
    SQL,
  };
  mutable output_t output;
  inline void setOutput(output_t o) const { this->output = o; }
  std::string to_string(output_t o = output_t::SQL) const;
  void to_string(std::string& str) const;

  std::vector<ColumnReference> selectedTables;
  std::vector<TableReference> fromTables;
  std::vector<JoinCondition> joinConditions;
  std::vector<InCondition> inConditions;
  std::vector<ColumnReference> groupedColumns, orderedColumns;

};

}
}

std::ostream& operator<<(std::ostream& os, aq::core::TableReference const& ss);
std::ostream& operator<<(std::ostream& os, aq::core::ColumnReference const& ss);
std::ostream& operator<<(std::ostream& os, aq::core::SelectStatement const& ss);

bool operator==(const aq::core::TableReference& tr1, const aq::core::TableReference& tr2);
bool operator==(const aq::core::ColumnReference& cr1, const aq::core::ColumnReference& cr2);
bool operator==(const aq::core::InCondition& ic1, const aq::core::InCondition& ic2);
bool operator==(const aq::core::JoinCondition& jc1, const aq::core::JoinCondition& jc2);

#endif
