#include "Table.h"
#include "Exceptions.h"
#include "Logger.h"

#include <cassert>
#include <memory>
#include <algorithm>
#include <set>
#include <boost/scoped_array.hpp>
#include <boost/bind.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/thread.hpp>

//------------------------------------------------------------------------------
#define STR_BIG_BUF_SIZE 1048576

namespace aq
{

//------------------------------------------------------------------------------
Table::Table(const std::string& _name, unsigned int _id, uint64_t _totalCount)
  : 
  ID(_id), 
  TotalCount(_totalCount),
  HasCount(false),
  GroupByApplied(false), 
  OrderByApplied(false),
  NoAnswer(false),
  temporary(false)
{
	this->setName(_name);
	memset(szBuffer, 0, STR_BUF_SIZE);
}

//------------------------------------------------------------------------------
Table::Table(const std::string& _name, unsigned int _id, uint64_t _totalCount, bool _temporary)
  : 
	ID(_id),
	TotalCount(0),
  HasCount(false),
	GroupByApplied(false), 
	OrderByApplied(false),
	NoAnswer(false),
  temporary(_temporary)
{
	this->setName(_name);
	memset(szBuffer, 0, STR_BUF_SIZE);
}

//------------------------------------------------------------------------------
Table::Table(const Table& source)
  : 
	ID(source.ID), 
	TotalCount(source.TotalCount),
	HasCount(source.HasCount),
	GroupByApplied(source.GroupByApplied), 
	OrderByApplied(source.OrderByApplied),
	NoAnswer(source.NoAnswer),
  temporary(source.temporary)
{
  this->setName(source.Name);
	memset(szBuffer, 0, STR_BUF_SIZE);
  for (auto& c : source.Columns)
  {
    Column::Ptr column(new Column(*c));
    this->Columns.push_back(column);
  }
}

//------------------------------------------------------------------------------
Table::~Table()
{
}

//------------------------------------------------------------------------------
Table& Table::operator=(const Table& source)
{
  if (this != &source)
  {
    ID = source.ID; 
    HasCount = source.HasCount;
    TotalCount = source.TotalCount;
    GroupByApplied = source.GroupByApplied;
    OrderByApplied = source.OrderByApplied;
    NoAnswer = source.NoAnswer;
    temporary = source.temporary;
    this->setName(source.Name);
    memset(szBuffer, 0, STR_BUF_SIZE);
    for (auto& c : source.Columns)
    {
      Column::Ptr column(new Column(*c));
      this->Columns.push_back(column);
    }
  }
  return *this;
}

//------------------------------------------------------------------------------
Column::Ptr Table::getColumn(const std::string& columnName) const
{
  std::string aux = columnName;
  boost::trim(aux);
  boost::to_upper(aux);
  for (auto& c : this->Columns)
  {
    if ((c->getName() == aux) || (c->getOriginalName() == aux))
    {
      return c;
    }
  }
  throw aq::generic_error(aq::generic_error::INVALID_QUERY, "cannot find column [%s]", columnName.c_str());
}

//------------------------------------------------------------------------------
int Table::getColumnIdx( const std::string& name ) const
{
	std::string auxName = name;
	boost::to_upper(auxName);
	boost::trim(auxName);
	for( size_t idx = 0; idx < this->Columns.size(); ++idx )
		if( auxName == this->Columns[idx]->getName() )
			return static_cast<int>(idx);
	return -1;
}

//------------------------------------------------------------------------------
std::vector<Column::Ptr> Table::getColumnsByName( std::vector<Column::Ptr>& columns ) const
{
	std::vector<Column::Ptr> correspondingColumns;
	for( size_t idx = 0; idx < columns.size(); ++idx )
	{
		bool found = false;
		for( size_t idx2 = 0; idx2 < this->Columns.size(); ++idx2 )
    {
			if( columns[idx]->getName() == this->Columns[idx2]->getName() )
			{
				correspondingColumns.push_back( this->Columns[idx2] );
				found = true;
				break;
			}
    }
		if( !found )
		{
      throw generic_error(generic_error::INVALID_QUERY, "");
		}
	}
	return correspondingColumns;
}

//------------------------------------------------------------------------------
void Table::setName( const std::string& name )
{
  if (this->OriginalName == "")
  {
    this->OriginalName = name;
    boost::to_upper(this->OriginalName);
    boost::trim(this->OriginalName);
  }
	this->Name = name;
	boost::to_upper(this->Name);
	boost::trim(this->Name);
}

//------------------------------------------------------------------------------
const std::string& Table::getName() const
{
	return this->Name;
}

//------------------------------------------------------------------------------
const std::string& Table::getOriginalName() const
{
	return this->OriginalName;
}

//------------------------------------------------------------------------------
void Table::dumpRaw( std::ostream& os )
{
  if( this->Columns.size() == 0 )
    throw generic_error(generic_error::INVALID_TABLE, "");
  size_t nrColumns = this->Columns.size();
  if( this->HasCount )
    --nrColumns;
  os << "\"" << this->getOriginalName() << "\" " << this->ID << " " << this->TotalCount << " " << nrColumns << std::endl;
  for (auto& col : Columns)
  {
    col->dumpRaw(os);
  }
  os << std::endl;
}

//------------------------------------------------------------------------------
void Table::dumpXml( std::ostream& os )
{
  if( this->Columns.size() == 0 )
    throw generic_error(generic_error::INVALID_TABLE, "");
  os << "<Table Name=\"" << this->getOriginalName() << "\" ID=\"" << this->ID << "\" NbRows=\"" << this->TotalCount << "\">" << std::endl;
  os << "<Columns>" << std::endl;
  for (auto& col : Columns)
  {
    col->dumpXml(os);
  }
  os << "</Columns>" << std::endl;
  os << "</Table>" << std::endl;
}

}
