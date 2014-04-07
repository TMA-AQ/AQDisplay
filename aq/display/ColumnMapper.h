#ifndef __AQ_COLUMN_MAPPER_H__
#define __AQ_COLUMN_MAPPER_H__

#include "ColumnMapper_Intf.h"
#include <aq/util/Utilities.h>
#include <aq/util/Logger.h>
#include <aq/util/DBTypes.h>
#include <aq/util/Exceptions.h>
#include <aq/util/Database.h>
#include <map>
#include <vector>
#include <boost/shared_ptr.hpp>
#include <boost/filesystem.hpp>

namespace aq
{
  
// ---------------------------------------------------------------------------------
template <typename T, class M>
class ColumnMapper : public ColumnMapper_Intf<T>
{
public:
	ColumnMapper(const char * path, size_t tableId, size_t columnId, size_t _size, size_t _packetSize, bool _cache = true, typename M::mode_t mode = M::mode_t::READ);
	~ColumnMapper();
	int loadValue(size_t index, T * value);
  int setValue(size_t index, T * value);
  int append(T * value);
	const std::vector<size_t>& getSimilarIndex(size_t index) const;
	const aq::ColumnType getType() const { return type_conversion<T>::type; }
private:
  size_t setPrmThe(size_t index);
  uint32_t updateThesaurus(T * value, size_t size);
  void updatePrm(size_t offset, int gap);
	static const aq::ColumnType type;
	size_t nbRemap;
	size_t tableId;
	size_t columnId;
  size_t size;
	size_t currentPart;
	size_t packetSize;
	const std::string path;
  typename M::mode_t mode;
  bool cache;
	boost::shared_ptr<M> prmMapper;
	boost::shared_ptr<M> thesaurusMapper;
	std::map<size_t, boost::shared_ptr<M> > prmMappers;
	std::map<size_t, boost::shared_ptr<M> > thesaurusMappers;
  T * val;
};

template <typename T, class M>
ColumnMapper<T, M>::ColumnMapper(const char * _path, size_t _tableId, size_t _columnId, size_t _size, size_t _packetSize, bool _cache, typename M::mode_t _mode)
	: nbRemap(0),
		tableId(_tableId),
		columnId(_columnId),
    size(_size),
		currentPart(0),
		packetSize(_packetSize),
		path(_path),
    mode(_mode),
    cache(_cache)
{
	std::string prmFilename = aq::Database::getPrmFileName(path.c_str(), tableId, columnId, currentPart);
	std::string thesaurusFilename = aq::Database::getThesaurusFileName(path.c_str(), tableId, columnId, currentPart);
	this->prmMapper.reset(new M(prmFilename.c_str(), mode));
	this->thesaurusMapper.reset(new M(thesaurusFilename.c_str(), mode));
  this->val = new T[size];
}

template <typename T, class M>
ColumnMapper<T, M>::~ColumnMapper()
{
	aq::Logger::getInstance().log(AQ_DEBUG, "%u remaping\n", nbRemap);
  delete[] this->val;
}

template <typename T, class M>
int ColumnMapper<T, M>::loadValue(size_t index, T * value)
{
  int rc = 0;
  size_t i = this->setPrmThe(index);
	uint32_t offset = 0;
	if ((rc = this->prmMapper->read(&offset, i * sizeof(uint32_t), sizeof(uint32_t))) != -1)
  {
    if ((rc = this->thesaurusMapper->read(val, offset * size * sizeof(T), size * sizeof(T))) == 0)
    {
      memcpy(value, this->val, this->size * sizeof(T));
    }
  }
  return rc;
}

template <typename T, class M>
int ColumnMapper<T, M>::setValue(size_t index, T * value)
{
  int rc = 0;
  size_t i = this->setPrmThe(index);

	uint32_t old_offset = 0;
	uint32_t new_offset = 0;

  if ((rc = this->prmMapper->read(&old_offset, i * sizeof(uint32_t), sizeof(uint32_t))) != 0)
  {
    throw aq::generic_error(aq::generic_error::INVALID_BASE_FILE, "cannot find prm index [%u] [error:%d]", index, rc);
  }
  
  new_offset = this->updateThesaurus(value, size);

  if ((rc = this->prmMapper->write(&new_offset, i * sizeof(uint32_t), sizeof(uint32_t))) != 0)
  {
    throw aq::generic_error(aq::generic_error::INVALID_BASE_FILE, "cannot update offset [%u] in prm file [error:%d]", new_offset, rc);
  }

  bool exist = false;
  i = 0;
  uint32_t offset = 0;
  while (!exist)
  {
    if (this->prmMapper->read(&offset, i * sizeof(uint32_t), sizeof(uint32_t)) != 0)
    {
      break;
    }
    if (offset == old_offset)
    {
      exist = true;
    }
    i += 1;
  }

  if (!exist)
  {

    // remove value thesaurus
    if (this->thesaurusMapper->erase(old_offset * sizeof(T), size * sizeof(T)) != 0)
    {
      throw aq::generic_error(aq::generic_error::GENERIC, "error updating thesaurus");
    }

    // update prm
    this->updatePrm(old_offset, -1);
  }

  return rc;
}

template <typename T, class M>
int ColumnMapper<T, M>::append(T * value)
{
  uint32_t index = this->updateThesaurus(value, this->size);
  return this->prmMapper->write(&index, this->prmMapper->size(), sizeof(uint32_t));
}

template <typename T, class M>
size_t ColumnMapper<T, M>::setPrmThe(size_t index)
{	
  size_t part = index / packetSize;
	size_t i = index % packetSize;
	if (currentPart != part)
	{
		++nbRemap;
		currentPart = part;

		if (this->prmMappers.find(currentPart) == this->prmMappers.end())
		{
			std::string prmFilename = aq::Database::getPrmFileName(path.c_str(), tableId, columnId, currentPart);
			std::string thesaurusFilename = aq::Database::getThesaurusFileName(path.c_str(), tableId, columnId, currentPart);

			aq::Logger::getInstance().log(AQ_DEBUG, "open %s\n", prmFilename.c_str());
			aq::Logger::getInstance().log(AQ_DEBUG, "open %s\n", thesaurusFilename.c_str());

			boost::filesystem::path p1(prmFilename);
			if (!boost::filesystem::exists(p1))
			{
        throw aq::generic_error(aq::generic_error::COULD_NOT_OPEN_FILE, prmFilename.c_str());
			}

			boost::filesystem::path p2(thesaurusFilename);
      if (!boost::filesystem::exists(p2))
      {
        throw aq::generic_error(aq::generic_error::COULD_NOT_OPEN_FILE, thesaurusFilename.c_str());
			}

			this->prmMapper.reset(new M(prmFilename.c_str(), mode));
			this->thesaurusMapper.reset(new M(thesaurusFilename.c_str(), mode));

      if (this->cache)
      {
        this->prmMappers.insert(std::make_pair(currentPart, this->prmMapper));
        this->thesaurusMappers.insert(std::make_pair(currentPart, this->thesaurusMapper));
      }
    }
		else
		{
			this->prmMapper = this->prmMappers[currentPart];
			this->thesaurusMapper = this->thesaurusMappers[currentPart];
		}
	}
  return i;
}

template <typename T, class M>
uint32_t ColumnMapper<T, M>::updateThesaurus(T * value, size_t size)
{
  // find value in thesaurus
  int cmp = 0;
  uint32_t index = 0;
  while (this->thesaurusMapper->read(val, index * size * sizeof(T), size * sizeof(T)) == 0)
  {
    cmp = memcmp(val, value, size * sizeof(T));
    if (cmp == 0)
    {
      break;
    }
    else if (cmp > 0)
    {
      // the value doesn't already exist in thesaurus : insert the value
      size_t new_position = (index + 1) * size * sizeof(T);
      size_t old_position = index * size * sizeof(T);
      size_t len = this->thesaurusMapper->size() - (index * size * sizeof(T));
      if (this->thesaurusMapper->move(new_position, old_position, len))
      {
        throw aq::generic_error(aq::generic_error::GENERIC, "error updating thesaurus");
      }
      if (this->thesaurusMapper->write(value, index * size * sizeof(T), size * sizeof(T)) != 0)
      {
        throw aq::generic_error(aq::generic_error::GENERIC, "error updating thesaurus");
      }
      // prm must be updated
      this->updatePrm(index, 1);
      break;
    }
    index += 1;
  }
  if (cmp < 0)
  {
    // insert at end
    if (this->thesaurusMapper->write(value, index * size * sizeof(T), size * sizeof(T)) != 0)
    {
      throw aq::generic_error(aq::generic_error::GENERIC, "error updating thesaurus");
    }
  }
  return index;
}

template <typename T, class M>
void ColumnMapper<T, M>::updatePrm(size_t offset, int gap)
{
  size_t index = 0;
  uint32_t tmp_offset = 0;
  while (this->prmMapper->read(&tmp_offset, index * sizeof(uint32_t), sizeof(uint32_t)) == 0)
  {
    if (((gap > 0) && (tmp_offset >= offset)) || ((gap < 0) && (tmp_offset > offset)))
    {
      tmp_offset += gap;
      if (this->prmMapper->write(&tmp_offset, index * sizeof(uint32_t), sizeof(uint32_t)) != 0)
      {
        throw aq::generic_error(aq::generic_error::GENERIC, "error updating prm");
      }
    }
    index += 1;
  }
}

//// factory
//template <class M>
//boost::shared_ptr<aq::ColumnMapper_Intf> build_column_mapper(const aq::ColumnType       type, 
//                                                             const char               * path, 
//                                                             const size_t               tableId, 
//                                                             const size_t               columnId, 
//                                                             const size_t               size, 
//                                                             const size_t               packetSize,
//                                                             const bool                 cache = false,
//                                                             const typename M::mode_t   mode = M::mode_t::READ)
//{      
//  boost::shared_ptr<aq::ColumnMapper_Intf> cm;
//  switch(type)
//  {
//  case aq::ColumnType::COL_TYPE_BIG_INT:
//  case aq::ColumnType::COL_TYPE_DATE:
//    cm.reset(new aq::ColumnMapper<int64_t, M>(path, tableId, columnId, size, packetSize, cache, mode));
//    break;
//  case aq::ColumnType::COL_TYPE_INT:
//    cm.reset(new aq::ColumnMapper<int32_t, M>(path, tableId, columnId, size, packetSize, cache, mode));
//    break;
//  case aq::ColumnType::COL_TYPE_DOUBLE:
//    cm.reset(new aq::ColumnMapper<double, M>(path, tableId, columnId, size, packetSize, cache, mode));
//    break;
//  case aq::ColumnType::COL_TYPE_VARCHAR:
//    cm.reset(new aq::ColumnMapper<char, M>(path, tableId, columnId, size, packetSize, cache, mode));
//    break;
//  }
//  return cm;
//}

}

#endif
