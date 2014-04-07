#ifndef __AQ_MATRIX_H__
#define __AQ_MATRIX_H__

#if defined (WIN32)
# ifdef AQENGINE_EXPORTS
#  define AQENGINE_API __declspec(dllexport)
# else
#  define AQENGINE_API __declspec(dllimport)
# endif
#else
# define AQLIB_API __stdcall
#endif

#include "Settings.h"

#include <aq/util/Base.h>
#include <aq/util/Exceptions.h>
#include <aq/util/BaseDesc.h>
#include <aq/util/DBTypes.h>

#include <string.h>
#include <map>
#include <vector>

#include <boost/scoped_array.hpp>

namespace aq
{

class Base;

namespace engine 
{

/// \brief representation of aq engine result. See aq engine specification for more explanations.
class AQMatrix
{
public:
  typedef boost::shared_ptr<AQMatrix> Ptr;
  typedef std::vector<size_t> v_size_t;

	struct column_t
	{
		size_t table_id;
		v_size_t indexes;
		v_size_t grpKey;
		v_size_t orderKey;
    std::string tableName;
    std::string baseTableName;
	};

	typedef std::vector<column_t> matrix_t;
  typedef std::vector<std::pair<uint64_t, uint64_t> > group_by_t;

	AQENGINE_API AQMatrix(const Settings::Ptr settings, const Base::Ptr baseDesc);
	AQENGINE_API AQMatrix(const AQMatrix& source);
	AQENGINE_API ~AQMatrix();
	AQENGINE_API AQMatrix& operator=(const AQMatrix& source);

  /// \brief clean directories
  AQENGINE_API void clear();

  /// \brief set join path
  /// \param jp
  AQENGINE_API void setJoinPath(const std::vector<std::string>& jp) { this->joinPath = jp; }
  
  /// \brief write
  /// \param filePath
  AQENGINE_API void write(const char * filePath);

  /// \brief load full aq matrix
  /// \param filePath
  /// \param tableIds
	AQENGINE_API void load(const char * filePath, std::vector<long long>& tableIDs);

  /// \brief load only head aq matrix
  /// \param filePath
  /// \param tableIDs
  AQENGINE_API void loadHeader(const char * filePath, std::vector<long long>& tableIDs);

  /// \brief load only full data aq matrix
  /// \param filePath
  AQENGINE_API void loadData(const char * filePath);

  /// \brief prepare data file to be read by row
  /// \param filePath
  AQENGINE_API void prepareData(const char * filePath);

  /// \brief load next packet
  AQENGINE_API void loadNextPacket();

  /// \brief read data by row and call a callback for each row
  /// \param CB the type of callback to call
  /// \param cb the callback
  template <class CB> void readData(CB& cb);

  /// \brief use for compress matrix when a group by with an aggregate function appears
  AQENGINE_API void compress();

  /// \brief write tempoaray table
  /// \todo
  AQENGINE_API void writeTemporaryTable();

  /// \name aq_matrix_getter_setter AQ Matrix getter
  /// \{
  AQENGINE_API matrix_t& getMatrix() { return this->matrix; }
	AQENGINE_API const group_by_t& getGroupBy() const { return this->groupByIndex; }
  AQENGINE_API const std::vector<std::string>& getJoinPath() { return this->joinPath; }
  AQENGINE_API const matrix_t& getMatrix() const { return this->matrix; }
  AQENGINE_API const size_t getTableId(size_t c) const { return this->matrix[c].table_id; }
	AQENGINE_API const v_size_t& getColumn(size_t c) const { return this->matrix[c].indexes; }
	AQENGINE_API const v_size_t& getCount() const { return this->count; }
	AQENGINE_API size_t getNbColumn() const { return this->matrix.size(); }
	AQENGINE_API uint64_t getTotalCount() const { return this->totalCount; }
	AQENGINE_API uint64_t getNbRows() const { return this->nbRows; }
	AQENGINE_API bool hasCountColumn() const { return this->hasCount; }
  /// \}

  /// \brief Debug purpose
  AQENGINE_API void dump(std::ostream& os) const;

private:

  static uint64_t uid_generator;
  uint64_t uid;
	const Settings::Ptr settings;
  const Base::Ptr baseDesc;
	matrix_t matrix;
	v_size_t count;
  group_by_t groupByIndex;
  std::vector<std::string> joinPath;
  uint64_t totalCount;
	uint64_t nbRows;
  std::string answerFormat;
  size_t nbRowsParsed;
  size_t nbPacket;
  size_t packet;
  size_t rowCountCheck;
	bool hasCount;
};

template <class CB>
void AQMatrix::readData(CB& cb)
{
  boost::scoped_array<char> answerData(new char[this->answerFormat.size() + 128]); // FIXME
  sprintf(answerData.get(), this->answerFormat.c_str(), this->packet);
  FILE * fd = fopen(answerData.get(), "rb");
  if (fd == nullptr)
  {
    throw aq::generic_error(aq::generic_error::AQ_ENGINE, "cannot find aq matrix data file %s", answerData.get());
  }
  uint64_t value;
  std::vector<size_t> rows(this->matrix.size() + 1, 0);
  for (size_t i = 0; (i < aq::packet_size) && (i < nbRows); ++i)
  {
    for (size_t c = 0; c < this->matrix.size(); ++c)
    {
      fread(&value, sizeof(uint64_t), 1, fd);
      rows[c] = value;
    }
    fread(&value, sizeof(uint64_t), 1, fd);
    rows[this->matrix.size()] = value;
    cb.handle(rows);
  }
  fclose(fd);
  this->packet += 1;
}

}
}

#endif
