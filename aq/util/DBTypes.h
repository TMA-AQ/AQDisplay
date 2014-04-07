#ifndef __AQ_DB_TYPES_H__
#define __AQ_DB_TYPES_H__

namespace aq
{
  
//------------------------------------------------------------------------------
enum aggregate_function_t // FIXME : not at the right place
{
  NONE,
  MIN,
  MAX,
  SUM,
  AVG,
  COUNT
};

//------------------------------------------------------------------------------
enum ColumnType
{
	COL_TYPE_VARCHAR	= 0,
	COL_TYPE_INT,
	COL_TYPE_BIG_INT,
	COL_TYPE_DOUBLE,
	COL_TYPE_DATE,
};
	
//------------------------------------------------------------------------------
union data_holder_t 
{
	char      *val_str;    // chaine
	long long	 val_int;    // entier
	double     val_number; // réel
};

//------------------------------------------------------------------------------
bool compatibleTypes( ColumnType type1, ColumnType type2 );

//------------------------------------------------------------------------------
const char * columnTypeToStr(ColumnType type);

}

#endif