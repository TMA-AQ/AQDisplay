#include "DatabaseHelper.h"
#include "Util.h"

#include <aq/util/Logger.h>
#include <aq/util/Database.h>
#include <aq/util/Base.h>
#include <aq/engine/AQEngine_Intf.h>

#include <fstream>

#include <boost/algorithm/string.hpp>

using namespace aq;

AlgoQuestDatabase::AlgoQuestDatabase(aq::Settings::Ptr _settings)
  : db(_settings->rootPath), settings(_settings)
{
  base.id = 1; // fixme
  base.name = settings->rootPath;
  boost::replace_all(base.name, "\\", "/");
  while (!base.name.empty() && (*base.name.rbegin() == '/'))
    base.name.erase(base.name.size() - 1);
  std::string::size_type pos = base.name.find_last_of("/");
  if ((pos != std::string::npos) && (pos < base.name.size() - 1))
    base.name = base.name.substr(pos + 1);
}

struct result_handler_t : public aq::display_cb
{
  result_handler_t(DatabaseIntf::result_t& _result)
    : result(_result)
  {
    it = result.rend();
  }
  ~result_handler_t()
  {
    it = result.rend();
  }
  void push(const std::string& value)
  {
    if (it != result.rend())
      (*it).push_back(value);
  }
  void next()
  {
    result.push_back(DatabaseIntf::result_t::value_type());
    it = result.rbegin();
  }
  DatabaseIntf::result_t& result;
  DatabaseIntf::result_t::reverse_iterator it;
};

bool AlgoQuestDatabase::execute(const aq::core::SelectStatement& ss)
{
  try
  {
    aq::Base::Ptr base(new aq::Base(settings->dbDesc));
    aq::engine::AQEngine_Intf::Ptr engine(aq::engine::getAQEngineSystem(base, settings));
    settings->changeIdent("test_aq_engine");
    engine->prepare();
    engine->call(ss);
    // engine->clean();
    auto matrix = engine->getAQMatrix();
    if (matrix != nullptr)
    {
      DatabaseIntf::result_t result;
      boost::shared_ptr<aq::display_cb> cb(new result_handler_t(result)); 
      std::vector<std::string> columns;
      aq::opt o;
      o.withCount = o.withIndex = false;
      for (const auto& c : ss.selectedTables)
        columns.push_back(c.table.name + "." + c.name);
      aq::display(cb.get(), matrix, base, settings, o, columns);
    }
  }
  catch (const aq::generic_error& ge)
  {
    aq::Logger::getInstance().log(AQ_NOTICE, "%s\n", ge.what());
  }
  return true;
}
