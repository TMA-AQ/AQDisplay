#ifndef __AQ_COLUMN_H__
#define __AQ_COLUMN_H__

#include "DBTypes.h"
#include "FileMapper.h"

#include <vector>
#include <string>
#include <boost/shared_ptr.hpp>

namespace aq
{

/// \brief column of query
class Column
{
public:
  
  typedef boost::shared_ptr<Column> Ptr;

	Column();
	Column(aq::ColumnType type);
	Column(const std::string& name, unsigned int ID, unsigned int size, aq::ColumnType type);
	Column(const Column& source);
  ~Column();
	Column& operator=(const Column& source);

	void setName( const std::string& name );
	void setDisplayName(const std::string& name);
  void setOriginalName(const std::string& name);
	std::string& getName();
	std::string& getDisplayName();
	std::string& getOriginalName();

	void setTableName(const std::string& name);
	std::string& getTableName();

	void dumpRaw(std::ostream& os);
	void dumpXml(std::ostream& os);

  size_t getTableID() const { return TableID; }
  size_t getID() const { return ID; }
  size_t getSize() const { return Size; }
  aq::ColumnType getType() const { return Type; }

  void setTableID(size_t _tableId) { TableID = _tableId; }
  void setID(size_t _id) { ID = _id; }
  void setSize(size_t _size) { Size = _size; }
  void setType(aq::ColumnType _type) { Type = _type; } 

private:
	void setBinItemSize();

  size_t	                     TableID;
	size_t	                     ID;
	size_t	                     Size;	///< maximum size of the text, not number of items
	aq::ColumnType		           Type;

	std::string	Name;
	std::string	OriginalName;
	std::string	DisplayName;
	Column::Ptr	Count;	//reference to count column
	std::string	TableName;

	boost::shared_ptr<aq::FileMapper> prmMapper;
	boost::shared_ptr<aq::FileMapper> thesaurusMapper;

	size_t prmFileItemSize;
	size_t currentNumPack;
	size_t packOffset;
	size_t nBinItemSize;

public:
	bool			Invisible;
	bool			GroupBy;
	bool			OrderBy;
  bool      Temporary;

};

}

#endif
