#include "Exceptions.h"
#include <cstdio>
#include <cstdarg>
#include <cassert>

using namespace aq;

//------------------------------------------------------------------------------
generic_error::generic_error( EType type, const std::string& msg )
{
	this->Message = "[" + typeToString(type) + "] " + msg;
}

//------------------------------------------------------------------------------
generic_error::generic_error( EType type, const char * format, ... )
{
	char buf[1024];
  va_list ap;
  va_start(ap, format);
  vsnprintf(buf, sizeof(buf), format, ap);
	this->Message = "[" + typeToString(type) + "] " + std::string(buf);
}

//------------------------------------------------------------------------------
std::string generic_error::typeToString(generic_error::EType type)
{
  switch(type)
  {
    case aq::generic_error::GENERIC: return "GENERIC"; break;
    case aq::generic_error::VERB_TYPE_MISMATCH: return "VERB_TYPE_MISMATCH"; break;
    case aq::generic_error::VERB_BAD_SYNTAX: return "VERB_BAD_SYNTAX"; break;
    case aq::generic_error::THESAURUS_NOT_FOUND: return "THESAURUS_NOT_FOUND"; break;
    case aq::generic_error::SELECT_NOT_FIRST: return "SELECT_NOT_FIRST"; break;
    case aq::generic_error::TYPE_MISMATCH: return "TYPE_MISMATCH"; break;
    case aq::generic_error::AGGREGATE_NOT_IN_SELECT_OR_HAVING: return "AGGREGATE_NOT_IN_SELECT_OR_HAVING"; break;
    case aq::generic_error::NOT_IMPLEMENTED: return "NOT_IMPLEMENTED"; break;
    case aq::generic_error::INVALID_BASE_FILE: return "INVALID_BASE_FILE"; break;
    case aq::generic_error::INVALID_QUERY: return "INVALID_QUERY"; break;
    case aq::generic_error::COULD_NOT_OPEN_FILE: return "COULD_NOT_OPEN_FILE"; break;
    case aq::generic_error::INVALID_TABLE: return "INVALID_TABLE"; break;
    case aq::generic_error::TABLE_ALREADY_EXISTS: return "TABLE_ALREADY_EXISTS"; break;
    case aq::generic_error::AQ_ENGINE: return "AQ_ENGINE"; break;
    case aq::generic_error::INVALID_FILE: return "INVALID_FILE"; break;
    case aq::generic_error::INVALID_DATE_FORMAT: return "INVALID_DATE_FORMAT"; break;
  }
	return "UNKNOWN_EXCEPTION";
}
