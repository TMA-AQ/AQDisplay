#ifndef __AQ_ENGINE_H__
#define __AQ_ENGINE_H__

#include "AQEngine_Intf.h"
#include "Settings.h"
#include <aq/util/Base.h>
#include <aq/util/Utilities.h>

namespace aq {
namespace engine {

class AQEngine : public AQEngine_Intf
{
public:
	AQEngine(const Base::Ptr _baseDesc, const Settings::Ptr _settings);
	~AQEngine();
  
  void call(const std::string& query, mode_t mode = AQEngine_Intf::mode_t::REGULAR);
  void call(const aq::core::SelectStatement& query, mode_t mode = mode_t::REGULAR);
  
  void renameResult(unsigned int id, std::vector<std::pair<std::string, std::string> >& resultTables);
	AQMatrix::Ptr getAQMatrix() { return aqMatrix; }
	const std::vector<llong>& getTablesIDs() const { return tableIDs; }

  void prepare() const;
  void clean() const;

protected:
  virtual int run(const char * prg, const char * args) const = 0;

private:
	const Base::Ptr baseDesc;
	const Settings::Ptr settings;
  AQMatrix::Ptr aqMatrix;
	std::vector<llong> tableIDs;
};

#ifdef WIN32

class AQEngineWindows : public AQEngine
{
public:
  AQEngineWindows(const Base::Ptr _baseDesc, const Settings::Ptr settings);
protected:
  int run(const char * prg, const char * args) const;
};

#endif

class AQEngineSystem : public AQEngine
{
public:
  AQEngineSystem(const Base::Ptr _baseDesc, const Settings::Ptr settings);
protected:
  int run(const char * prg, const char * args) const;
};

}
}

#endif
