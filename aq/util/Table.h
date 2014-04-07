#ifndef __AQ_TABLE_H__
#define __AQ_TABLE_H__

#include "Column.h"
#include "BaseDesc.h"
#include "Utilities.h"

#include <vector>
#include <string>
#include <deque>

namespace aq 
{

/// \brief Table of query
class Table
{
public:
  typedef boost::shared_ptr<Table> Ptr;
	typedef std::vector<Column::Ptr> columns_t;

	Table(const std::string& name, unsigned int ID, uint64_t _totalCount);
	Table(const std::string& name, unsigned int ID, uint64_t _totalCount, bool temporary);
  Table(const Table& source);
  ~Table();
  Table& operator=(const Table& source);

  size_t getID() const { return ID; }
  uint64_t getTotalCount() const { return TotalCount; }
  bool hasCount() const { return HasCount; }
  bool isGroupByApplied() const { return GroupByApplied; }
  bool isOrderByApplied() const { return OrderByApplied; }
  bool hasNoAnswer() const { return NoAnswer; }

  //void setID(size_t _id) { this->ID = _id; }
  //void setTotalCount(uint64_t _totalCount) { this->TotalCount = _totalCount; }

	int getColumnIdx(const std::string& name)  const;
  Column::Ptr getColumn(const std::string& columnName) const;
	std::vector<Column::Ptr> getColumnsByName(std::vector<Column::Ptr>& columns) const;

	void setName(const std::string& name);
	const std::string& getName() const;

  void setOriginalName(const std::string& _originalName) { this->OriginalName = _originalName; }
	const std::string& getOriginalName() const;
  
  bool isTemporary() const { return temporary; }
  const char * getTemporaryName() const { return temporaryName.c_str(); }

  void setReferenceTable(const std::string& _referenceTable) { this->referenceTable = _referenceTable; }
  std::string getReferenceTable() const { return this->referenceTable; }

	void dumpRaw(std::ostream& os);
	void dumpXml(std::ostream& os);
  
	columns_t Columns;

private:
	size_t  	ID;
	uint64_t  TotalCount;
	bool			HasCount; ///< last column is "Count"
	bool			GroupByApplied; ///< used by aggregate functions to know when there is a GROUP BY in the query
	bool			OrderByApplied;
	bool			NoAnswer;

	std::string		Name;
	std::string		OriginalName;
  std::string   temporaryName;
  std::string   referenceTable;
	std::vector<size_t> Index;
	char szBuffer[STR_BUF_SIZE];
  bool temporary;
};

}

#endif
