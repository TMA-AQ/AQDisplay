#ifndef __AQ_BASE_DESC_H__
#define __AQ_BASE_DESC_H__

#include "Symbole.h"
#include <cstdio>
#include <vector>
#include <sstream>

/// algoquest namespace
namespace aq
{

/// \brief the default size of AlgoQuest packet
static const unsigned int packet_size = 1048576;

/// \brief base description
struct base_t
{
  /// \brief table description
  struct table_t
  {
    /// \brief column description
    struct col_t
    {
      std::string name; ///< column's name
      int id; ///< column's id
      symbole type; /// column's type
      int size; ///< number of elements
      int getSize() const; ///< get size of one element in byte
    };
    typedef std::vector<col_t> cols_t;

    std::string name; ///< table's name
    int id; ///< table's id
    int nb_record; ///< number of record in table
    cols_t colonne; ///< table's columns
  };
  typedef std::vector<table_t> tables_t;

	std::string name; ///< base's name
	int id; ///< base's id
	tables_t table; ///< base's tables

  /// \defgroup base structure input/output operations 
	/// The standard content of a base struct file is defined as :
	/// 
	/// \<Name_Base\> \n
	/// \<Tbl_count\> \n
	/// \n
	/// "\<Tbl_Name\>" \<tbl_id\> \<tbl_records_nb\> \<tbl_col_nb\> \n
	/// "\<Col_Name\>" \<col_id\> \<col_size\> \<col_type\> \n
	/// 
	/// Example : \n
	/// my_base_name \n
	/// 4 \n
	/// \n
	/// "BONUS" 1 1 4 \n
	/// "ENAME" 1 10 VARCHAR2 \n
	/// "JOB" 2 9 VARCHAR2 \n
	/// "SAL" 3 22 NUMBER \n
	/// "COMM" 4 22 NUMBER \n
	/// \n
	/// ...
  /// \{
  
  /// \brief build base from file
  /// \param fname the filename to parse
  /// \param base the base to load
  /// \return 0 if succeed, -1 if failed
  static int build_base_from_raw(const char * fname, base_t& base);
  
  /// \brief build base from stream
  /// \param fp the C stream to parse
  /// \param base the base to load
  /// \todo handle errors
  static void build_base_from_raw(FILE* fp, base_t& base);
  
  /// \brief build base from stream with raw representation
  /// \param is the C++ stream to parse
  /// \param base the base to load
  /// \throw boost::property_tree exception if errors occurs
  static void build_base_from_raw(std::istream& is, base_t& base);
  
  /// \brief build base from stream with xml representation
  /// \param is the C++ stream to parse
  /// \param base the base to load
  /// \throw boost::property_tree exception if errors occurs
  static void build_base_from_xml(std::istream& is, base_t& base);

  /// \brief dump raw representation of base
  /// \param oss the C++ stream to write
  /// \param base the base to write
  static void dump_raw_base(std::ostream& oss, const base_t& base);

  /// \brief build xml representation of base
  /// \param oss the C++ stream to write
  /// \param base the base to write
  static void dump_xml_base(std::ostream& oss, const base_t& base);

  /// \}
};

}

#endif
