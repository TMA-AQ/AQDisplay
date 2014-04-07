#include "AQLParser.h"

#if defined (ENABLE_AQL_PARSER)

#include <boost/algorithm/string.hpp>
#include <boost/fusion/adapted.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix.hpp>

namespace qi = boost::spirit::qi;
namespace phx = boost::phoenix;

// --------------------------------------------------------
BOOST_FUSION_ADAPT_STRUCT(aq::core::TableReference,
                          (std::string, name)
                          );

BOOST_FUSION_ADAPT_STRUCT(aq::core::ColumnReference,
                          (aq::core::TableReference, table)
                          (std::string, name)
                          );

BOOST_FUSION_ADAPT_STRUCT(aq::core::InCondition,
                          (aq::core::ColumnReference, column)
                          (std::vector<std::string>, values)
                          );

BOOST_FUSION_ADAPT_STRUCT(aq::core::JoinCondition,
                          (std::string, type_right)
                          (std::string, jt_right)
                          (aq::core::ColumnReference, right)
                          (std::string, op)
                          (std::string, type_left)
                          (std::string, jt_left)
                          (aq::core::ColumnReference, left)
                          );

BOOST_FUSION_ADAPT_STRUCT(aq::core::SelectStatement, 
                          (std::vector<aq::core::ColumnReference>, selectedTables)
                          (std::vector<aq::core::TableReference>, fromTables)
                          (std::vector<aq::core::ColumnReference>, groupedColumns)
                          (std::vector<aq::core::ColumnReference>, orderedColumns)
                          );

namespace aq {
  namespace parser {

    // --------------------------------------------------------
    template <typename It, typename Skipper = qi::space_type>
    struct aq_infix_parser : qi::grammar<It, aq::core::SelectStatement(), Skipper>
    {
      aq_infix_parser(aq::core::SelectStatement& _ss) : aq_infix_parser::base_type(start), ss(_ss)
      {
        using namespace qi;

        sql_ident = lexeme [ qi::char_("a-zA-Z_") >> *qi::char_("a-zA-Z_0-9") ];
        table_ref = sql_ident;
        column_ref = table_ref >> '.' >> sql_ident;
        value = lexeme [ *qi::char_("a-zA-Z_0-9") ];
        list_value = '(' >> value % ',' >> ')';
        in_condition = column_ref >> no_case [ "in" ] >> list_value;
        join_type = no_case [ string("k_active") ] | no_case [ string("k_filter") ] | no_case [ string("k_neutral") ];
        join = no_case [ string("k_inner") ] | no_case [ string("k_outer") ];
        op = 
          no_case [ string("k_jeq") ] | 
          no_case [ string("k_jneq") ] | 
          no_case [ string("k_jinf") ] | 
          no_case [ string("k_jieq") ] | 
          no_case [ string("k_jsup") ] | 
          no_case [ string("k_jseq") ];  
        join_condition = join_type >> join >> column_ref >> op >> join_type >> join >> column_ref;
        unary_join = no_case [ string("k_jno") ] >> (-join_type) >> (-join) >> column_ref;
        condition = 
          ( 
          unary_join
          |
          join_condition [ phx::bind(&aq_infix_parser::add_join_condition, this, qi::_1) ] 
          | 
          in_condition [ phx::bind(&aq_infix_parser::add_in_condition, this, qi::_1) ]      
        ) 
          ;

        column_list = column_ref % ',';
        table_list = table_ref % ',' ;
        where_list = condition % no_case [ "and" ];

        select   = no_case [ "select" ] >> column_list;
        from     = no_case [ "from" ]   >> table_list;
        where    = no_case [ "where" ]  >> where_list;
        group    = no_case [ "group" ]  >> column_list;
        order    = no_case [ "order" ]  >> column_list;

        start    = 
          select 
          >> from 
          >> -(where)
          >> -(group)
          >> -(order)
          >> ';'
          ;

        BOOST_SPIRIT_DEBUG_NODE(start);
        BOOST_SPIRIT_DEBUG_NODE(sql_ident);
        BOOST_SPIRIT_DEBUG_NODE(value);
        BOOST_SPIRIT_DEBUG_NODE(table_ref);
        BOOST_SPIRIT_DEBUG_NODE(column_ref);
        BOOST_SPIRIT_DEBUG_NODE(list_value);
        BOOST_SPIRIT_DEBUG_NODE(in_condition);
        BOOST_SPIRIT_DEBUG_NODE(join_condition);
        BOOST_SPIRIT_DEBUG_NODE(unary_join);
        BOOST_SPIRIT_DEBUG_NODE(condition);
        BOOST_SPIRIT_DEBUG_NODE(column_list);
        BOOST_SPIRIT_DEBUG_NODE(table_list);
        BOOST_SPIRIT_DEBUG_NODE(where_list);
        BOOST_SPIRIT_DEBUG_NODE(select);
        BOOST_SPIRIT_DEBUG_NODE(from);
        BOOST_SPIRIT_DEBUG_NODE(where);
        BOOST_SPIRIT_DEBUG_NODE(group);
        BOOST_SPIRIT_DEBUG_NODE(order);
      }

    private:
      qi::rule<It, std::string()                            , Skipper> sql_ident, op, join_type, join, value;
      qi::rule<It, aq::core::TableReference()               , Skipper> table_ref;
      qi::rule<It, aq::core::ColumnReference()              , Skipper> column_ref;
      qi::rule<It, aq::core::JoinCondition()                , Skipper> join_condition;
      qi::rule<It                                           , Skipper> unary_join;
      qi::rule<It, std::vector<std::string>()               , Skipper> list_value;
      qi::rule<It, aq::core::InCondition()                  , Skipper> in_condition;
      qi::rule<It, std::vector<aq::core::ColumnReference>() , Skipper> column_list;
      qi::rule<It, std::vector<aq::core::TableReference>()  , Skipper> table_list;
      qi::rule<It, std::vector<aq::core::ColumnReference>() , Skipper> select;
      qi::rule<It, std::vector<aq::core::TableReference>()  , Skipper> from;
      qi::rule<It                                           , Skipper> condition;
      qi::rule<It                                           , Skipper> where_list;
      qi::rule<It                                           , Skipper> where;
      qi::rule<It, std::vector<aq::core::ColumnReference>() , Skipper> group, order;
      qi::rule<It, aq::core::SelectStatement()              , Skipper> start;
      aq::core::SelectStatement& ss;

      void add_join_condition(const aq::core::JoinCondition& jc)
      {
        ss.joinConditions.push_back(jc);
      }

      void add_in_condition(const aq::core::InCondition& ic)
      {
        ss.inConditions.push_back(ic);
      }

    };

    // --------------------------------------------------------
    template <typename C, typename Skipper>
    bool aq_infix_parse(aq::core::SelectStatement& query, const C& input, const Skipper& skipper)
    {
      auto f(std::begin(input)), l(std::end(input));

      aq_infix_parser<decltype(f), Skipper> p(query);

      try
      {
        bool ok = qi::phrase_parse(f, l, p, skipper, query);
        if (f!=l) std::cerr << "trailing unparsed: '" << std::string(f,l) << "'\n";
        return ok;
      } 
      catch(const qi::expectation_failure<decltype(f)>& e)
      {
        std::string frag(e.first, e.last);
        std::cerr << e.what() << "'" << frag << "'\n";
      }

      return false;
    }
    
    // --------------------------------------------------------
    bool parse_infix(const std::string& queryStr, aq::core::SelectStatement& query)
    {
      return aq::parser::aq_infix_parse(query, queryStr, qi::space);
    }

  }
}

#else

namespace aq {
  namespace parser {

    // --------------------------------------------------------
    bool parse_infix(const std::string& queryStr, aq::core::SelectStatement& query)
    {
      return true;
    }

  }
}

#endif
