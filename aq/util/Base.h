#ifndef __AQ_BASE_H__
#define __AQ_BASE_H__

#include "Table.h"

#include <vector>
#include <string>
#include <deque>

namespace aq
{

/// \brief base description for query
/// This contains also the temporaries tables representation of nested queries
class Base
{
public:
  typedef boost::shared_ptr<Base> Ptr;
	typedef std::vector<Table::Ptr> tables_t;
  
  Base();
  Base(const Base& source);
  Base(const std::string& file);
  ~Base();
  Base& operator=(const Base& source);
  

	void loadFromBaseDesc( const aq::base_t& base );
  void dumpRaw(std::ostream& os) const;
	void dumpXml(std::ostream& os) const;

  void clear();

  tables_t& getTables() { return this->Tables; }
  const tables_t& getTables() const { return this->Tables; }
  const std::string& getName() const { return this->Name; }
  
  Table::Ptr getTable(size_t id);
	Table::Ptr getTable(const std::string& name) ;
  const Table::Ptr getTable(size_t id) const;
	const Table::Ptr getTable(const std::string& name) const;
  
private:
	tables_t    Tables;
	std::string Name;
};

}

#endif