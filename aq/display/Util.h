#ifndef __UTIL_H__
#define __UTIL_H__

#include <aq/util/Base.h>
#include <aq/engine/AQMatrix.h>
#include <string>
#include <vector>
#include <sstream>

namespace aq
{

struct opt
{
  opt() : 
    packetSize(aq::packet_size), limit(0), execute(false), stopOnError(false), checkResult(false), checkCondition(false),
    jeqParserActivated(false), aql2sql(false), display(true), withCount(false), withIndex(false), force(false)
  {
  }
  std::string dbPath;
  std::string workingPath;
  std::string queryIdent;
  std::string aqEngine;
  std::string queriesFilename;
  std::string filter;
  std::string logFilename;
  uint64_t packetSize;
  uint64_t limit;
  bool execute;
  bool stopOnError;
  bool checkResult;
  bool checkCondition;
  bool jeqParserActivated;
  bool aql2sql;
  bool display;
  bool withCount;
  bool withIndex;
  bool force;
};

struct display_cb
{
  virtual ~display_cb() {}
  virtual void push(const std::string& value) = 0;
  virtual void next() = 0;
};

int generate_working_directories(const struct opt& o, std::string& iniFilename);
int run_aq_engine(const std::string& aq_engine, const std::string& iniFilename, const std::string& ident);

void get_columns(std::vector<std::string>& columns, const std::string& query, const std::string& key);
int check_answer_validity(const struct opt& o, aq::engine::AQMatrix& matrix, 
                          const uint64_t count, const uint64_t nbRows, const uint64_t nbGroups);

int display(display_cb *,
            const aq::engine::AQMatrix::Ptr aqMatrix,
            const aq::Base::Ptr baseDesc,
            const aq::Settings::Ptr settings,
            const struct opt& o,
            const std::vector<std::string>& selectedColumns);

}

#endif
