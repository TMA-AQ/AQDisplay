#ifndef __AQ_PARSER_H__
#define __AQ_PARSER_H__

#include "AQLQuery.h"

namespace aq {

/// parser aql
namespace parser {

/// \brief parse aql prefixed query
/// \param queryStr the aql string represention
/// \query the query struct parsed
/// \return true if succeed, false otherwise
///
/// the aql query is formed as described below: TODO
///
/// SELECT ...
/// FROM ...
/// WHERE ...
/// GROUP ...
/// ORDER ...
bool parse(const std::string& queryStr, aq::core::SelectStatement& query);

/// \brief parse aql infixed quer
bool parse_infix(const std::string& queryStr, aq::core::SelectStatement& query);
  
}
}

#endif
