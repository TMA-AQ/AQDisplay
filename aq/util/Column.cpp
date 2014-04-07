#include "Column.h"
#include "Utilities.h"
#include "Exceptions.h"
#include <cstring>
#include <sstream>
#include <boost/scoped_array.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/algorithm/string/case_conv.hpp>

namespace aq
{


//------------------------------------------------------------------------------
Column::Column()
	:	
  TableID(0),
	ID(0), 
	Size(0), 
	Type(COL_TYPE_INT), 
	prmFileItemSize(4),
	currentNumPack(-1),
	packOffset(0),
	nBinItemSize(0),
	Invisible(false), 
	GroupBy(false), 
	OrderBy(false),
  Temporary(false)
{
	this->setBinItemSize();
}

//------------------------------------------------------------------------------
Column::Column(	const std::string& name, unsigned int ID, unsigned int size, ColumnType type)
	: 
  TableID(0),
	ID(ID), 
	Size(size), 
	Type(type),
	prmFileItemSize(4),
	currentNumPack(-1),
	packOffset(0),
	nBinItemSize(0),
	Invisible(false), 
	GroupBy(false), 
	OrderBy(false),
  Temporary(false)
{
	this->setBinItemSize();
	this->setName( name );
}

//------------------------------------------------------------------------------
Column::Column( ColumnType type )
	: 
  TableID(0),
	ID(0),
	Size(0),
	Type(type),
	prmFileItemSize(4),
	currentNumPack(-1),
	packOffset(0),
	nBinItemSize(0),
	Invisible(false), 
	GroupBy(false),
	OrderBy(false),
  Temporary(false)
{
	this->setBinItemSize();
}

//------------------------------------------------------------------------------
Column::Column( const Column& source )
	:
  TableID(source.TableID),
	ID(source.ID),
	Size(source.Size),
	Type(source.Type),
	Name(source.Name),
	OriginalName(source.OriginalName),
	DisplayName(source.DisplayName),
	Count(source.Count),
	TableName(source.TableName),
	prmFileItemSize(source.prmFileItemSize),
	currentNumPack(source.currentNumPack),
	packOffset(source.packOffset),
	nBinItemSize(source.nBinItemSize),
	Invisible(false),
	GroupBy(false),
	OrderBy(false),
  Temporary(source.Temporary)
{
	this->setBinItemSize();
}

//----------------------------------------------------------------------------
Column::~Column() 
{
}

//------------------------------------------------------------------------------
Column& Column::operator=(const Column& source)
{
	if (this != &source)
	{
    this->TableID = source.TableID;
		this->ID = source.ID;
		this->Size = source.Size;
		this->Type = source.Type;
		this->Invisible = false;
		this->GroupBy = false;
		this->OrderBy = false;
    this->Temporary = source.Temporary;
		this->Name = source.Name;
		this->OriginalName = source.OriginalName;
		this->DisplayName = source.DisplayName;
		this->Count = source.Count;
		this->TableName = source.TableName;
		this->prmFileItemSize = source.prmFileItemSize;
		this->currentNumPack = source.currentNumPack,
		this->packOffset = source.packOffset;
		this->nBinItemSize = source.nBinItemSize;
		this->setBinItemSize();
	}
	return *this;
}

void Column::setBinItemSize()
{
	switch( this->Type )
	{
	case COL_TYPE_INT:
		nBinItemSize	= 4;
		break;
	case COL_TYPE_BIG_INT:
	case COL_TYPE_DATE:
	case COL_TYPE_DOUBLE:
		nBinItemSize	= 8;
		break;
	case COL_TYPE_VARCHAR:
		nBinItemSize	= this->Size;
		break;
	}
}

//------------------------------------------------------------------------------
void Column::setName( const std::string& name )
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
void Column::setDisplayName( const std::string& name )
{
	this->DisplayName = name;
}

//------------------------------------------------------------------------------
void Column::setOriginalName( const std::string& name )
{
	this->OriginalName = name;
}

//------------------------------------------------------------------------------
std::string& Column::getName()
{
	return this->Name;
}

//------------------------------------------------------------------------------
std::string& Column::getOriginalName()
{
	return this->OriginalName;
}


//------------------------------------------------------------------------------
std::string& Column::getDisplayName()
{
	if( this->DisplayName == "" )
		if( this->OriginalName == "" )
			return this->Name;
		else
			return this->OriginalName;
	else
		return this->DisplayName;
}

//------------------------------------------------------------------------------
void Column::setTableName( const std::string& name )
{
	this->TableName = name;
	boost::to_upper(this->TableName);
	boost::trim(this->TableName);
}

//------------------------------------------------------------------------------
std::string& Column::getTableName()
{
	return this->TableName;
}

//------------------------------------------------------------------------------
void Column::dumpRaw( std::ostream& os )
{
  std::string colName = this->getOriginalName();
  size_t pos = colName.find('.');
  if( pos != std::string::npos )
    colName = colName.substr( pos + 1 );
  os << "\""<< colName << "\" " << this->ID << " " << this->Size << " ";
  switch( this->Type )
  {
  case COL_TYPE_INT: os << "INT"; break;
  case COL_TYPE_BIG_INT: os << "BIG_INT"; break;
  case COL_TYPE_DOUBLE: os << "DOUBLE"; break;
  case COL_TYPE_DATE: os << "DATE"; break;
  case COL_TYPE_VARCHAR: os << "VARCHAR2"; break;
  default:
    throw generic_error(generic_error::NOT_IMPLEMENTED, "");
  }
  os << std::endl;
}

//------------------------------------------------------------------------------
void Column::dumpXml( std::ostream& os )
{
  std::string colName = this->getOriginalName();
  size_t pos = colName.find('.');
  if( pos != std::string::npos )
    colName = colName.substr( pos + 1 );
  os << "<Column Name=\"" << colName << "\" ID=\"" << this->ID << "\" Size=\"" << this->Size << "\" Type=\"";
  switch( this->Type )
  {
  case COL_TYPE_INT: os << "INT"; break;
  case COL_TYPE_BIG_INT: os << "BIG_INT"; break;
  case COL_TYPE_DOUBLE: os << "DOUBLE"; break;
  case COL_TYPE_DATE: os << "DATE"; break;
  case COL_TYPE_VARCHAR: os << "VARCHAR2"; break;
  default:
    throw generic_error(generic_error::NOT_IMPLEMENTED, "");
  }
  os << "\"/>" << std::endl;
}

}
