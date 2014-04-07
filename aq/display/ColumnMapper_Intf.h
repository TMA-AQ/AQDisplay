#ifndef __COLUMN_MAPPER_INTF_H__
#define __COLUMN_MAPPER_INTF_H__

#include <cstring>
#include <memory>

namespace aq
{

// helper function
template <typename T> struct type_conversion { static const aq::ColumnType type = aq::ColumnType::COL_TYPE_INT; };
template <> struct type_conversion<int32_t> { static const aq::ColumnType type = aq::ColumnType::COL_TYPE_INT; };
template <> struct type_conversion<int64_t> { static const aq::ColumnType type = aq::ColumnType::COL_TYPE_BIG_INT; };
template <> struct type_conversion<double> { static const aq::ColumnType type = aq::ColumnType::COL_TYPE_DOUBLE; };
template <> struct type_conversion<char> { static const aq::ColumnType type = aq::ColumnType::COL_TYPE_VARCHAR; };

/// \brief Column interface reader to read values stored in database
/// The link to the column is manage by the implemented class.
/// \param T the type of the value
template <typename T>
class ColumnMapper_Intf
{
public:
	typedef std::shared_ptr<ColumnMapper_Intf> Ptr;
  virtual ~ColumnMapper_Intf() {}

  /// \brief load an item
  /// \param index the position in the column
  /// \param value a pointer holding the value to load
  /// \return 0 if succeed, -1 otherwise
	virtual int loadValue(size_t index, T * value) = 0;
  
  /// \brief set an item
  /// \param index the position in the column
  /// \param value a pointer holdind the value to set 
  /// \todo value should be a const reference
  /// \return 0 if succeed, -1 otherwise
  virtual int setValue(size_t index, T * value) = 0;
  
  /// \brief append an item
  /// \param value a pointer holding the value to append
  /// \todo value should be a const reference
  /// \return 0 if succeed, -1 otherwise
  virtual int append(T * value) = 0;
  
  /// \brief load an item value
  /// \return the column type
  const aq::ColumnType getType() const { return aq::type_conversion<T>::type; } ;
};

//template <typename T> struct enum_to_type { typedef int32_t type; };
//template <> struct enum_to_type<aq::ColumnType = aq::ColumnType::COL_TYPE_INT> { static const aq::ColumnType type = aq::ColumnType::COL_TYPE_INT; };
//template <> struct enum_to_type<aq::ColumnType::COL_TYPE_BIG_INT> { static const aq::ColumnType type = aq::ColumnType::COL_TYPE_BIG_INT; };
//template <> struct enum_to_type<aq::ColumnType::COL_TYPE_DOUBLE> { static const aq::ColumnType type = aq::ColumnType::COL_TYPE_DOUBLE; };
//template <> struct enum_to_type<aq::ColumnType::COL_TYPE_VARCHAR> { static const aq::ColumnType type = aq::ColumnType::COL_TYPE_VARCHAR; };

//template <typename T>
//void fill_item(ColumnItem<T>& item, T * value, size_t size)
//{
//  assert(size == 1);
//  item.value = *value;
//}
//
//template <> inline
//void fill_item<char*>(ColumnItem<char*>& item, char * value, size_t size)
//{
//  item.setValue(value); // FIXME
//}
//
//template <typename T>
//void dump_item(T * value, size_t size, ColumnItem<T>& item)
//{
//  assert(size == 1);
//  *value = item.value;
//}
//
//template <> inline 
//void dump_item<char*>(char * value, size_t size, ColumnItem<char*>& item)
//{
//  ::memcpy(value, item.strval.c_str(), size);
//}

}

#endif
