#include "AQLQuery.h"
#include "Algo.h"
#include <sstream>
#include <boost/algorithm/string.hpp>

bool operator==(const aq::core::TableReference& tr1, const aq::core::TableReference& tr2)
{
  return tr1.name == tr2.name;
}

bool operator==(const aq::core::ColumnReference& cr1, const aq::core::ColumnReference& cr2)
{
  return (cr1.table == cr2.table) && (cr1.name == cr2.name);
}

bool operator==(const aq::core::InCondition& ic1, const aq::core::InCondition& ic2)
{
  bool rc = (ic1.column == ic2.column) && std::equal(ic1.values.begin(), ic1.values.end(), ic2.values.begin());
  return rc;
}

bool operator==(const aq::core::JoinCondition& jc1, const aq::core::JoinCondition& jc2)
{
  bool rc =  
    (jc1.op == jc2.op) &&
    (jc1.left == jc2.left) &&
    (jc1.right == jc2.right) &&
    (jc1.jt_left == jc2.jt_left) &&
    (jc2.jt_right == jc2.jt_right);
  return rc;
}

aq::core::JoinCondition::join_t join_str_2_join_t(std::string jt_left_str, std::string jt_right_str)
{
  aq::core::JoinCondition::join_t jt = aq::core::JoinCondition::join_t::INNER;
  boost::to_lower(jt_left_str);
  boost::to_lower(jt_right_str);
  if ((jt_left_str == "k_inner") && (jt_right_str == "k_inner")) jt = aq::core::JoinCondition::join_t::INNER;
  else if ((jt_left_str == "k_inner") && (jt_right_str == "k_outer")) jt = aq::core::JoinCondition::join_t::LEFT_OUTER;
  else if ((jt_left_str == "k_outer") && (jt_right_str == "k_inner")) jt = aq::core::JoinCondition::join_t::RIGHT_OUTER;
  else if ((jt_left_str == "k_outer") && (jt_right_str == "k_outer")) jt = aq::core::JoinCondition::join_t::FULL_OUTER;
  else assert(false);
  return jt;
}

std::ostream& operator<<(std::ostream& os, const aq::core::JoinCondition::join_t& jt)
{
  switch (jt)
  {
  case aq::core::JoinCondition::join_t::INNER: os << "inner join"; break;
  case aq::core::JoinCondition::join_t::LEFT_OUTER: os << "left outer join"; break;
  case aq::core::JoinCondition::join_t::RIGHT_OUTER: os << "right outer join"; break;
  case aq::core::JoinCondition::join_t::FULL_OUTER: os << "full outer join"; break;
  }
  return os;
}

aq::core::JoinCondition::op_t op_str_2_op_t(std::string op_str)
{
  boost::to_lower(op_str);
  aq::core::JoinCondition::op_t op = aq::core::JoinCondition::op_t::AUTO;
  if (op_str == "k_jeq") op = aq::core::JoinCondition::op_t::EQ;
  else if (op_str == "k_auto") op = aq::core::JoinCondition::op_t::AUTO;
  else if (op_str == "k_jneq") op = aq::core::JoinCondition::op_t::NEQ;
  else if (op_str == "k_jinf") op = aq::core::JoinCondition::op_t::INF;
  else if (op_str == "k_jieq") op = aq::core::JoinCondition::op_t::IEQ;
  else if (op_str == "k_jsup") op = aq::core::JoinCondition::op_t::SUP;
  else if (op_str == "k_jseq") op = aq::core::JoinCondition::op_t::SEQ;
  else assert(false);
  return op;
}

std::ostream& operator<<(std::ostream& os, const aq::core::JoinCondition::op_t& op)
{
  switch (op)
  {
  case aq::core::JoinCondition::op_t::EQ:
  case aq::core::JoinCondition::op_t::AUTO: os << "="; break;
  case aq::core::JoinCondition::op_t::NEQ: os << "<>"; break;
  case aq::core::JoinCondition::op_t::INF: os << "<"; break;
  case aq::core::JoinCondition::op_t::IEQ: os << "<="; break;
  case aq::core::JoinCondition::op_t::SUP: os << ">"; break;
  case aq::core::JoinCondition::op_t::SEQ: os << ">="; break;
  }
  return os;
}

std::ostream& operator<<(std::ostream& os, const aq::core::TableReference& table)
{
  return os << table.name;
}

std::ostream& operator<<(std::ostream& os, const aq::core::ColumnReference& col)
{
  return os << col.table << "." << col.name;
}

template <typename T>
void print_list(std::ostream& os, T& l, const char * delim = "")
{
  typename T::const_iterator it = l.begin();
  while (it != l.end())
  {
    os << delim << *it << delim;
    ++it;
    if (it != l.end())
    {
      os << ", ";
    }
  }
}

void print_aql(std::ostream& os, const aq::core::SelectStatement& query)
{
  os << "SELECT ";
  for (size_t i = 1; i < query.selectedTables.size(); ++i) os << ", ";
  for (auto& c : query.selectedTables) os << ". " << c.table.name << " " << c.name << " ";
  os << std::endl;

  os << "FROM ";
  for (size_t i = 1; i < query.fromTables.size(); ++i) os << ", ";
  for (auto& t : query.fromTables) os << t.name << " ";
  os << std::endl;

  if (!query.joinConditions.empty())
  {
    os << "WHERE ";
    for (size_t i = 1; i < query.joinConditions.size() + query.inConditions.size(); ++i) os << "AND ";
    os << std::endl;
    for (auto& w : query.joinConditions) 
    {
      os << "  " << w.op << " ";
      os << w.type_left << " ";
      os << w.jt_left << " . " << w.left.table.name << " " << w.left.name << " ";
      os << w.type_right << " ";
      os << w.jt_right << " . " << w.right.table.name << " " << w.right.name << " ";
      os << std::endl;
    }
    for (auto& w : query.inConditions)
    {
      os << "  " << "IN . " << w.column.table.name << " " << w.column.name;
      for (auto it = w.values.begin(); it != w.values.end(); ++it)
      {
        auto it_tmp = it;
        ++it_tmp;
        if (it_tmp != w.values.end())
        {
          os << " ,";
        }
        os << " K_VALUE " << *it;
      }
      os << std::endl;
    }
  }

  if (!query.groupedColumns.empty())
  {
    os << "GROUP ";
    for (size_t i = 1; i < query.groupedColumns.size(); ++i) os << ", ";
    for (auto& c : query.groupedColumns) os << ". " << c.table.name << " " << c.name << " ";
    os << std::endl;
  }

  if (!query.orderedColumns.empty())
  {
    os << "ORDER ";
    for (size_t i = 1; i < query.orderedColumns.size(); ++i) os << ", ";
    for (auto& c : query.orderedColumns) os << ". " << c.table.name << " " << c.name << " ";
    os << std::endl;
  }
}

void print_aql_infix(std::ostream& os, const aq::core::SelectStatement& query)
{
  os << "SELECT ";
  print_list(os, query.selectedTables);
  os << std::endl;

  os << "FROM ";
  print_list(os, query.fromTables);
  os << std::endl;

  if (!query.joinConditions.empty())
  {
    os << "WHERE ";
    size_t i = 0;
    size_t n = query.joinConditions.size() + query.inConditions.size();
    os << std::endl;
    for (auto& w : query.joinConditions) 
    {
      os << " ";

      os << w.type_left << " ";
      os << w.jt_left << " " << w.left.table.name << "." << w.left.name;
      
      os << " " << w.op << " ";

      os << w.type_right << " ";
      os << w.jt_right << " " << w.right.table.name << "." << w.right.name;
      
      if (++i < n)
        os << " AND";
      
      os << std::endl;
    }
    for (auto& w : query.inConditions)
    {
      os << " " << w.column.table.name << "." << w.column.name << " IN (";
      for (auto it = w.values.begin(); it != w.values.end();)
      {
        os << *it;
        ++it;
        if (it != w.values.end())
        {
          os << ", ";
        }
      }
      os << ")";
      if (++i < n)
        os << " AND";
      os << std::endl;
    }
  }

  if (!query.groupedColumns.empty())
  {
    os << "GROUP ";
    print_list(os, query.groupedColumns);
    os << std::endl;
  }

  if (!query.orderedColumns.empty())
  {
    os << "ORDER ";
    print_list(os, query.orderedColumns);
    os << std::endl;
  }
}

void print_sql(std::ostream& os, const aq::core::SelectStatement& ss)
{
  // copy inConditions into inCondtionsTmp
  std::vector<aq::core::InCondition> joinConditionsTmp;
  std::vector<aq::core::InCondition> inConditionsTmp;
  std::copy(ss.inConditions.begin(), ss.inConditions.end(), std::back_inserter(joinConditionsTmp));
  std::copy(ss.inConditions.begin(), ss.inConditions.end(), std::back_inserter(inConditionsTmp));

  // Select
  os << "SELECT " ;
  print_list(os, ss.selectedTables);
  os << std::endl;

  // From
  std::vector<aq::core::InCondition> condToAddToWhere;
  std::vector<aq::core::InCondition> condToRemoveFromWhere;
  if (ss.joinConditions.empty())
  {
    os << "FROM ";
    print_list(os, ss.fromTables);
  }
  else
  {
    os << "FROM " << *ss.fromTables.begin() << std::endl;
    auto p = (*ss.fromTables.begin());
    for (auto it = ss.fromTables.begin() + 1; it != ss.fromTables.end(); ++it)
    {
      auto t = *it;
      for (auto& j : ss.joinConditions)
      {
        if ((j.right.table == p) && (j.left.table == t))
        {
          os << "  " << join_str_2_join_t(j.jt_left, j.jt_right) << " ";
          os << j.left.table << " ON (";
          os << j.right << " " << op_str_2_op_t(j.op) << " " << j.left;
          // os << j.right << " " << j.op << " " << j.left;

          // add conditions from inConditionsTmp if needed and remove it from inConditionsTmp
          auto it = inConditionsTmp.begin();
          std::vector<aq::core::InCondition> addConditions;
          while (it != inConditionsTmp.end())
          {
            auto cond = *it;
            if ((cond.column == j.left) || (cond.column == j.right))
            {
              addConditions.push_back(cond);
              condToRemoveFromWhere.push_back(cond);
              condToAddToWhere.push_back(cond);
            }
            ++it;
          }

          if (!addConditions.empty())
          {
            for (auto& cond : addConditions)
            {
              os << " AND " << cond.column << " IN (";
              print_list(os, cond.values, "'");
              os << ")";
            }
          }

          os << ")" << std::endl;
          p = t;
        }
      }
    }
  }

  // Make Conditions Unique
  aq::core::unique(condToAddToWhere);
  aq::core::unique(condToRemoveFromWhere);

  // Remove Conditions
  for (auto& cond1 : condToRemoveFromWhere)
  {
    auto it = std::remove_if(inConditionsTmp.begin(), inConditionsTmp.end(), [&] (const aq::core::InCondition& cond2) { return (cond1 == cond2); });
    inConditionsTmp.erase(it, inConditionsTmp.end());
  }

  // Where
  if (!joinConditionsTmp.empty() || !inConditionsTmp.empty())
  {
    os << "WHERE " << std::endl;
  }
  if (!condToAddToWhere.empty())
  {
    os << "  (" << std::endl; 
    for (auto it = condToAddToWhere.begin(); it != condToAddToWhere.end();)
    {
      auto& cond = *it;
      os << "  " << "  " << cond.column << " IN (";
      print_list(os, cond.values, "'");
      os << ")";
      ++it;
      if (it != condToAddToWhere.end())
      {
        os << " OR";
      }
      os << std::endl;
    }
    os << "  )"; 
    if (!inConditionsTmp.empty())
    {
      os << " AND";
    } 
    os << std::endl;
  }
  if (!inConditionsTmp.empty())
  {
    for (auto it = inConditionsTmp.begin(); it != inConditionsTmp.end();)
    {
      auto& cond = *it;
      os << "  " << cond.column << " IN (";
      print_list(os, cond.values, "'");
      os << ")";
      ++it;
      if (it != inConditionsTmp.end())
      {
        os << " AND";
      }
      os << std::endl;
    }
  }

  // Group
  if (!ss.groupedColumns.empty())
  {
    os << "GROUP BY ";
    print_list(os, ss.groupedColumns);
    os << std::endl;
  }

  // Order
  if (!ss.orderedColumns.empty())
  {
    os << "ORDER BY ";
    print_list(os, ss.orderedColumns);
    os << std::endl;
  }

  os << ";" << std::endl;
}

std::string aq::core::SelectStatement::to_string(output_t o) const
{
  output_t obak = this->output;
  this->output = o;
  std::stringstream ss;
  ss << *this;
  this->output = obak;
  return ss.str();
}

void aq::core::SelectStatement::to_string(std::string& str) const
{
  std::stringstream ss;
  ss << *this;
  str = ss.str();
}

std::ostream& operator<<(std::ostream& os, const aq::core::SelectStatement& ss)
{
  switch (ss.output)
  {
  case aq::core::SelectStatement::SQL:
    print_sql(os, ss);
    break;
  case aq::core::SelectStatement::AQL:
    print_aql(os, ss);
    break;
  case aq::core::SelectStatement::AQL_INFIX:
    print_aql_infix(os, ss);
    break;
  }
  return os;
}
