#include "AQMatrix.h"
#include <aq/util/Logger.h>
#include <aq/util/Utilities.h>
#include <aq/util/Exceptions.h>
#include <aq/util/FileMapper.h>
#include <iostream>
#include <set>
#include <list>
#include <cinttypes>
#include <boost/scoped_array.hpp>

using namespace aq::engine;
 
#define STR_BIG_BUF_SIZE 128

namespace
{
  
	struct inner_column_cmp_t
	{
	public:
		inner_column_cmp_t(const std::vector<size_t>& lessThanColumn)
			: m_lessThanColumn(lessThanColumn)
		{
		}
		bool operator()(size_t idx1, size_t idx2)
		{
			return m_lessThanColumn[idx1] < m_lessThanColumn[idx2];
		}
	private:
		const std::vector<size_t>& m_lessThanColumn;
	};

}

uint64_t AQMatrix::uid_generator = 0;

AQMatrix::AQMatrix(const aq::Settings::Ptr _settings, const aq::Base::Ptr _baseDesc)
	: settings(_settings),
    baseDesc(_baseDesc),
		totalCount(0),
    nbRows(0),
    nbRowsParsed(0),
    nbPacket(0),
    packet(0),
    rowCountCheck(0),
		hasCount(false)
{
  uid = ++AQMatrix::uid_generator;
}

AQMatrix::AQMatrix(const AQMatrix& source)
	: settings(source.settings),
    baseDesc(source.baseDesc),
		totalCount(source.totalCount),
		nbRows(source.nbRows),
    nbRowsParsed(0),
    nbPacket(0),
    packet(0),
    rowCountCheck(0),
		hasCount(source.hasCount)
{
}

AQMatrix::~AQMatrix()
{
}

AQMatrix& AQMatrix::operator=(const AQMatrix& source)
{
	if (this != &source)
	{
		totalCount = source.totalCount;
		nbRows = source.nbRows;
		hasCount = source.hasCount;
    nbRowsParsed = source.nbRowsParsed;
    nbPacket = source.nbPacket;
    packet = source.packet;
	}
	return *this;
}

//void AQMatrix::simulate(size_t rows, const std::vector<long long>& tableIDs)
//{
//	this->hasCount = true;
//	this->totalCount = rows;
//	this->nbRows = rows;
//	this->matrix.resize(tableIDs.size());
//
//  for ( size_t idx = 0; idx < tableIDs.size(); ++idx )
//    this->matrix[idx].table_id = static_cast<size_t>( tableIDs[idx] );
//
//	for (std::vector<column_t>::iterator it = this->matrix.begin(); it != this->matrix.end(); ++it)
//    for (size_t idx = 0; idx < rows; ++idx)
//      (*it).indexes.push_back(rand() % this->baseDesc.getTable((*it).table_id)->TotalCount);
//	this->count.resize(rows, 1);
//
//  this->groupByIndex.push_back(std::make_pair(rows, rows));
//}

void AQMatrix::clear()
{
  matrix.clear();
	count.clear();
  groupByIndex.clear();
	totalCount = 0;
	nbRows = 0;
	hasCount = false;
}

void AQMatrix::write(const char * filePath)
{
  FILE * fd = fopen(filePath, "wb");
  uint64_t value = this->matrix.size();
  uint64_t size = count.size();
  fwrite(&value, sizeof(uint64_t), 1, fd);
  for (size_t i = 0; i < this->matrix.size(); ++i)
  {
    assert(size == this->matrix[i].indexes.size());
    value = this->matrix[i].table_id;
    fwrite(&value, sizeof(uint64_t), 1, fd);
  }
  
  fwrite(&size, sizeof(uint64_t), 1, fd);
  for (size_t i = 0; i < size; ++i)
  {
    for (size_t c = 0; c < this->matrix.size(); ++c)
    {
      value = this->matrix[c].indexes[i];
      fwrite(&value, sizeof(uint64_t), 1, fd);
    }
    value = this->count[i];
    fwrite(&value, sizeof(uint64_t), 1, fd);
  }
  fclose(fd);
}

void AQMatrix::load(const char * filePath, std::vector<long long>& tableIDs)
{
  this->loadHeader(filePath, tableIDs);
  if (this->nbRows > 0)
  {
    this->loadData(filePath);
  }
}

void AQMatrix::loadHeader(const char * filePath, std::vector<long long>& tableIDs)
{
  std::string answertHeader(filePath);
  answertHeader += "/AnswerHeader.a";
  FILE * fd = fopen(answertHeader.c_str(), "rb");
  if (fd == nullptr)
  {
    // try old name
    std::string answertHeader(filePath);
    answertHeader += "/AnswerHeader00000.a";
    fd = fopen(answertHeader.c_str(), "rb");
    if (fd == nullptr)
    {
      throw aq::generic_error(aq::generic_error::AQ_ENGINE, "cannot find aq matrix header file");
    }
  }

  uint64_t nbTable, tableId, nbGroups;
  fread(&nbTable, sizeof(uint64_t), 1, fd);
  for (uint32_t i = 0; i < nbTable; ++i)
  {
    this->matrix.push_back(column_t());
    fread(&tableId, sizeof(uint64_t), 1, fd);
    this->matrix[this->matrix.size() - 1].table_id = tableId;
    tableIDs.push_back(tableId);
  }
  
  fread(&this->totalCount, sizeof(uint64_t), 1, fd);
  fread(&this->nbRows, sizeof(uint64_t), 1, fd);
  fread(&nbGroups, sizeof(uint64_t), 1, fd);

  aq::Logger::getInstance().log(AQ_NOTICE, "aq matrix: [table:%u;count:%u;rows:%u;groups:%u]\n", nbTable, this->totalCount, nbRows, nbGroups);

  uint64_t countCheck = 0;
  uint64_t rowCheck = 0;
  uint64_t grpCount, grpRows;
  for (uint64_t i = 0; i < nbGroups; ++i)
  {
    fread(&grpCount, sizeof(uint64_t), 1, fd);
    fread(&grpRows, sizeof(uint64_t), 1, fd);
    
    if ((grpCount == 0) || (grpRows == 0))
      throw aq::generic_error(aq::generic_error::AQ_ENGINE, "bad value in result");

    this->groupByIndex.push_back(std::make_pair(grpCount, grpRows));
    countCheck += grpCount;
    rowCheck += grpRows;
    assert(grpCount);
    assert(grpRows);
  }
  if ((this->totalCount != countCheck) || (this->nbRows != rowCheck))
    throw aq::generic_error(aq::generic_error::AQ_ENGINE, "mismatch values in result");

  fclose(fd);
  
  this->hasCount = true;
}

void AQMatrix::loadData(const char * filePath)
{
  this->prepareData(filePath);
  for (uint64_t packet = 0; packet < this->nbPacket; ++packet)
  {
    this->loadNextPacket();
  }
  if (this->nbRows != this->count.size())
  {
    throw aq::generic_error(aq::generic_error::AQ_ENGINE, "bad matrix data file");
  }
}

void AQMatrix::prepareData(const char * filePath)
{
  this->nbRowsParsed = 0;
  this->packet = 0;
  this->nbPacket = (this->nbRows / aq::packet_size) + 1; 
  this->answerFormat = std::string(filePath);
  this->answerFormat += "/AnswerData%.5u.a";
}

void AQMatrix::loadNextPacket()
{
  boost::scoped_array<char> answerData(new char[this->answerFormat.size() + 128]); // FIXME
  sprintf(answerData.get(), this->answerFormat.c_str(), this->packet);
  FILE * fd = fopen(answerData.get(), "rb");
  if (fd == nullptr)
  {
    throw aq::generic_error(aq::generic_error::AQ_ENGINE, "cannot find aq matrix data file %s", answerData.get());
  }
  uint64_t value;
  for (size_t i = 0; (i < aq::packet_size) && this->count.size() < nbRows; ++i)
  {
    for (size_t c = 0; c < this->matrix.size(); ++c)
    {
      fread(&value, sizeof(uint64_t), 1, fd);
      this->matrix[c].indexes.push_back(value);
    }
    fread(&value, sizeof(uint64_t), 1, fd);
    this->count.push_back(value);
    this->rowCountCheck += value;
  }
  fclose(fd);
  this->packet += 1;

  if (this->packet == nbPacket)
  {
    assert(this->totalCount == this->rowCountCheck);
    assert(this->nbRows == this->count.size());
    if (this->totalCount != this->rowCountCheck)
    {
      throw aq::generic_error(aq::generic_error::AQ_ENGINE, "bad matrix data file [count_expected:%u] [count_get:%u]", this->totalCount, this->rowCountCheck);
    }
    if (this->nbRows != this->count.size())
    {
      throw aq::generic_error(aq::generic_error::AQ_ENGINE, "bad matrix data file [nb_rows:%u] [nb_count:%u]", this->nbRows, this->count.size());
    }
  }
}

void AQMatrix::compress()
{
  size_t p = 0;
  size_t index = 0;
  assert(this->count.size() >= this->groupByIndex.size());
  for (auto it = this->groupByIndex.begin(); it != this->groupByIndex.end(); ++it, ++index)
  {
    this->count[index] = 1;
    for (size_t c = 0; c < this->matrix.size(); ++c)
    {
      assert(p < this->matrix[c].indexes.size());
      this->matrix[c].indexes[index] = this->matrix[c].indexes[p];
      p += (*it).second;
    }
  }

  for (size_t c = 0; c < this->matrix.size(); ++c)
  {
    this->matrix[c].indexes.resize(index);
  }
  this->count.resize(index);
}

void AQMatrix::writeTemporaryTable()
{
  if (this->matrix.empty())
    return;
  uint64_t packet = 0;
  char filename[1024];
  FILE * fd;
  std::vector<std::map<uint64_t, FILE*> > fds(this->matrix.size());
  uint32_t status = 11;
  uint32_t invalid = 0;
  uint32_t pos = 0;
  uint64_t grpIndex = 1;
  uint64_t size = (*this->matrix.begin()).indexes.size();
  for (uint64_t i = 0; i < size; ++i, ++grpIndex)
  {
    for (size_t c = 0; c < this->matrix.size(); ++c)
    {
      pos = static_cast<uint32_t>(this->matrix[c].indexes[i]);
      packet = pos / settings->packSize;
      pos = (pos - 1) % settings->packSize;
      auto it = fds[c].find(packet);
      if (it == fds[c].end())
      {
        sprintf(filename, "%s/B001REG%.4luTMP%.4"PRIu64"P%.12"PRIu64".TMP", this->settings->tmpPath.c_str(), this->matrix[c].table_id, this->uid, packet);
        fd = fopen(filename, "wb");
        if (fd == nullptr)
        {
          throw aq::generic_error(aq::generic_error::COULD_NOT_OPEN_FILE, "cannot create temporary table file '%s'", filename);
        }
        if (!fds[c].insert(std::make_pair(packet, fd)).second)
        {
          throw aq::generic_error(aq::generic_error::GENERIC, "MEMORY ERROR");
        }
        fwrite(&status, sizeof(uint32_t), 1, fd);
      }
      else
      {
        fd = it->second;
      }
      
      fwrite(&invalid, sizeof(uint32_t), 1, fd);
      fwrite(&pos, sizeof(uint32_t), 1, fd);
    }
  }
  
  for (size_t c = 0; c < this->matrix.size(); ++c)
  {
    for (auto& v : fds[c])
    {
      fclose(v.second);
    }
  }
  
  // FIXME : generate empty file => this is temporary
  char tableName[128];
  for (auto it = this->matrix.begin(); it != this->matrix.end(); ++it)
  {
    Table::Ptr table = this->baseDesc->getTable((*it).table_id);
    uint64_t n = table->getTotalCount() / this->settings->packSize;
    for (uint64_t i = 0; i <= n; ++i)
    {
      sprintf(filename, "%s/B001REG%.4luTMP%.4"PRIu64"P%.12"PRIu64".TMP", this->settings->tmpPath.c_str(), (*it).table_id, this->uid, i);
      FILE * fd = fopen(filename, "ab");
      fclose(fd);
    }
    sprintf(tableName, "B001REG%.4luTMP%.4"PRIu64"P%.12"PRIu64, (*it).table_id, this->uid, n + 1);
    (*it).tableName = tableName;
    (*it).baseTableName = table->getName();
  }
}

void AQMatrix::dump(std::ostream& os) const
{
  size_t size = (*this->matrix.begin()).indexes.size();
  
  for (size_t c = 0; c < this->matrix.size(); ++c)
  {
    os << this->matrix[c].table_id << " ";
  }
  os << std::endl;

  for (size_t i = 0; i < size; ++i)
  {
    for (size_t c = 0; c < this->matrix.size(); ++c)
    {
      os << this->matrix[c].indexes[i] << " ";
    }
    os << std::endl;
  }
}
