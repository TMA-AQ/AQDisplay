#ifndef __AQ_DATABASE_H__
#define __AQ_DATABASE_H__

#include <string>
#include <sstream>
#include "BaseDesc.h"

namespace aq
{

/// \brief Database management
/// all database are stored on disk
/// \todo add a handle to manage storage
class Database
{
public:
  
  explicit Database(const std::string& _path);
  void dump(std::ostream& os) const;
  
  /// \brief check if database is valid (ready to use)
  bool isValid() const;

  /// \brief create database on disk from a database description
  /// \param base the base description of the database to create
  /// \todo handle errors
  void create(aq::base_t& base);

  /// \brief remove database from disk
  /// \todo handle errors
  void erase();

  /// \brief get database name 
  /// \return database name
  std::string getName() const;

  /// \brief get database description 
  /// \return database description
  aq::base_t getBaseDesc() const;
  
  /// \brief get database root path 
  /// \return database root path
  std::string getRootPath() const; 

  /// \brief get database working path 
  /// \return database working path
  std::string getWorkingPath() const;

  /// \brief get database data path 
  /// \return database data path
  std::string getDataPath() const;

  /// \deprecated 
  /// \brief get database base description file 
  /// \return database description file
  std::string getBaseDescFile() const;

  /// \brief get database temporary working path 
  /// \return database temporary working path
  std::string getTemporaryWorkingPath() const;

  /// \brief get database temporary table load path 
  /// \return database temporary table load path
  std::string getTemporaryTableLoadPath() const;

  /// \brief get database temporary column load path 
  /// \return database temporary column load path
  std::string getTemporaryColumnLoadPath() const;

  /// \brief get prm filename
  /// \param tableIdx table's index
  /// \param columnIdx column's index
  /// \param partIdx paquet's index
  /// \return prm filename
  std::string getPrmFileName(size_t tableIdx, size_t columnIdx, size_t partIdx);
  
  /// \brief get thesaurus filename
  /// \param tableIdx table's index
  /// \param columnIdx column's index
  /// \param partIdx paquet's index
  /// \return thesaurus filename
  std::string getThesaurusFileName(size_t tableIdx, size_t columnIdx, size_t partIdx );
  
  static std::string getPrmFileName(const char* path, size_t tableIdx, size_t columnIdx, size_t partIdx);
  static std::string getThesaurusFileName(const char* path, size_t tableIdx, size_t columnIdx, size_t partIdx);
  static std::string getTemporaryFileName(size_t tableIdx, size_t columnIdx, size_t partIdx, const char * type, size_t size);
  
private:
  int load();
  static std::string getDataFileName(const char * path, size_t tIdx, size_t cIdx, size_t pIdx, const char * ext);

  std::string path;
  aq::base_t baseDesc;
};

}

#endif
