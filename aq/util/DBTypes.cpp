#include "DBTypes.h"
#include <cassert>

namespace aq
{

//------------------------------------------------------------------------------
bool compatibleTypes( ColumnType type1, ColumnType type2 )
{
	switch( type1 )
	{
	case COL_TYPE_DOUBLE: 
	case COL_TYPE_INT: 
	case COL_TYPE_BIG_INT: 
		return type2 == COL_TYPE_DOUBLE || type2 == COL_TYPE_INT || type2 == COL_TYPE_BIG_INT; 
		break;
	case COL_TYPE_VARCHAR: 
	case COL_TYPE_DATE:
    return type1 == type2;
		break;
	}
	return false;
}

//------------------------------------------------------------------------------
const char * columnTypeToStr(ColumnType type)
{
  switch (type)
  {	
    case COL_TYPE_VARCHAR: return "CHA"; break;
    case COL_TYPE_INT: return "INT"; break;
    case COL_TYPE_DOUBLE: return "DOU"; break;
    case COL_TYPE_DATE:
    case COL_TYPE_BIG_INT: return "LON"; break;
  }
  assert(false);
  return "";
}

}
