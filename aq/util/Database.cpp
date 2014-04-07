#include "Database.h"
#include "Exceptions.h"
#include "Utilities.h"
#include <fstream>
#include <boost/filesystem.hpp>
#include <boost/algorithm/string/replace.hpp>

namespace aq
{

Database::Database(const std::string& _path) 
  : path(_path)
{
  boost::replace_all(path, "\\", "/");
  if (!path.empty() && (*path.rbegin() != '/'))
  {
    path += "/";
  }
  this->load();
}

bool Database::isValid() const
{
  if (baseDesc.name == "")
    return false;

  std::string bdFname = this->getBaseDescFile();
  boost::filesystem::path bdFile(bdFname);
  if (!boost::filesystem::exists(bdFile))
    return false;

  boost::filesystem::path workingPath(this->getWorkingPath());
  if (!boost::filesystem::exists(workingPath))
    return false;

  boost::filesystem::path dataPath(this->getDataPath());
  if (!boost::filesystem::exists(dataPath))
    return false;

  return true;
}

void Database::create(aq::base_t& base)
{
  boost::filesystem::path paths[] = 
  {
    boost::filesystem::path(this->path),
    boost::filesystem::path(this->path + "base_struct"),
    boost::filesystem::path(this->getWorkingPath()),
    boost::filesystem::path(this->getDataPath()),
    boost::filesystem::path(this->getTemporaryColumnLoadPath()),
    boost::filesystem::path(this->getTemporaryTableLoadPath()),
    boost::filesystem::path(this->getTemporaryWorkingPath()),
  };

  for (auto& p : paths)
  {
    if (!boost::filesystem::exists(p))
    {
      boost::filesystem::create_directories(p);
    }
  }

  boost::filesystem::path bdFile(this->getBaseDescFile());
  std::string fname = this->getBaseDescFile();
  std::ofstream f(fname.c_str(), std::ios::trunc);
  aq::base_t::dump_raw_base(f, base);
  f.close();

}

void Database::erase()
{
  boost::filesystem::path root(this->path);
  boost::filesystem::remove_all(root);
}

std::string Database::getName() const
{
  std::string name;
  std::ifstream f(this->getBaseDescFile().c_str(), std::ios::in);
  if (f.is_open())
  {
    std::getline(f, name);
  }
  return name;
}

aq::base_t Database::getBaseDesc() const
{
  return baseDesc;
}

int Database::load()
{
  int rc = 0;
  std::string bdFname = this->getBaseDescFile();
  std::ifstream fin(bdFname.c_str(), std::ios::in);
  if (!fin.is_open()) 
    return -1;
  if (bdFname.find(".aqb") != std::string::npos)
    aq::base_t::build_base_from_raw(fin, baseDesc);
  else if (bdFname.find(".xml") != std::string::npos)
    aq::base_t::build_base_from_xml(fin, baseDesc);
  else
    rc = -1;
  return rc;
}

std::string Database::getRootPath() const
{
  return this->path;
}

std::string Database::getWorkingPath() const
{
  return this->path + "calculus/";
}

std::string Database::getDataPath() const
{
  return this->path + "data_orga/vdg/data/";
}

std::string Database::getBaseDescFile() const
{
  return this->path + "base_struct/base.aqb";
}

std::string Database::getTemporaryWorkingPath() const
{
  return this->path + "data_orga/tmp/";
}

std::string Database::getTemporaryTableLoadPath() const
{
  return this->path + "data_orga/tables/";
}

std::string Database::getTemporaryColumnLoadPath() const
{
  return this->path + "data_orga/columns/";
}

void Database::dump(std::ostream& os) const
{
  os << this->getName() << std::endl;
  os << this->path << std::endl;
  os << this->getBaseDescFile() << std::endl;
  os << this->getWorkingPath() << std::endl;
  os << this->getDataPath() << std::endl;
  aq::base_t::dump_raw_base(os, this->baseDesc);
}

std::string Database::getPrmFileName(size_t tableIdx, size_t columnIdx, size_t partIdx)
{
  return Database::getPrmFileName(this->getDataPath().c_str(), tableIdx, columnIdx, partIdx);
}

std::string Database::getThesaurusFileName(size_t tableIdx, size_t columnIdx, size_t partIdx )
{
  return Database::getThesaurusFileName(this->getDataPath().c_str(), tableIdx, columnIdx, partIdx);
}

// static
std::string Database::getPrmFileName(const char* path, size_t tableIdx, size_t columnIdx, size_t partIdx)
{
	return Database::getDataFileName(path, tableIdx, columnIdx, partIdx, "prm");
}

// static
std::string Database::getThesaurusFileName(const char* path, size_t tableIdx, size_t columnIdx, size_t partIdx)
{
	return Database::getDataFileName(path, tableIdx, columnIdx, partIdx, "the");
}

// static
std::string Database::getDataFileName(const char * path, size_t tIdx, size_t cIdx, size_t pIdx, const char * ext)
{
	char szFN[ _MAX_PATH ];
	if (path)
		sprintf(szFN, "%sB001T%.4luC%.4luV01P%.12lu.%s", path, tIdx, cIdx, pIdx, ext);
	else
		sprintf(szFN, "B001T%.4luC%.4luV01P%.12lu.%s", tIdx, cIdx, pIdx, ext);
	return szFN;
}
 
// static
std::string Database::getTemporaryFileName(size_t tableIdx, size_t columnIdx, size_t partIdx, const char * type, size_t size)
{
	char szFN[ _MAX_PATH ];
  sprintf(szFN, "B001TMP%.4luC%.4lu%s%.4luP%.12lu.TMP", tableIdx, columnIdx, type, size, partIdx);
	return szFN;
}

}
