#ifndef __DATABASE_HELPER_H__
#define __DATABASE_HELPER_H__

#include <string>
#include <memory>
#include <list>

#include <aq/util/Base.h>
#include <aq/util/Database.h>
#include <aq/util/AQLQuery.h>
#include <aq/engine/Settings.h>

namespace aq {

  class DatabaseIntf
  {
  public:
    typedef std::list<std::list<std::string> > result_t;

    virtual ~DatabaseIntf() {}
    virtual bool execute(const aq::core::SelectStatement& ss) = 0;
  };

  class AlgoQuestDatabase : public DatabaseIntf
  {
  public:
    AlgoQuestDatabase(aq::Settings::Ptr _settings);
    bool execute(const aq::core::SelectStatement& ss);
  private:
    aq::base_t        base;
    aq::Database      db;
    aq::Settings::Ptr settings;
  };

}

#endif
