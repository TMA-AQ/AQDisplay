#ifndef __GENERIC_FILE_MAPPER_H__
#define __GENERIC_FILE_MAPPER_H__

#include <cstdio>
#include <string>

namespace aq
{

/// \brief A generic file mapper used the standard C file library
class GenericFileMapper
{
public:
  enum mode_t
  {
    READ,
    WRITE,
    READ_WRITE,
  };
public:
	GenericFileMapper(const char * _filename, const mode_t _mode = mode_t::READ);
	~GenericFileMapper();

	int read(void * buffer, size_t offset, size_t len);
  int write(void * buffer, size_t offset, size_t len);
  int move(size_t new_offset, size_t old_offset, size_t len); ///< \todo not implemented
  int erase(size_t offset, size_t len);
  size_t size() { return this->m_size; }

private:
	GenericFileMapper(const GenericFileMapper&);
	GenericFileMapper& operator=(const GenericFileMapper&);
	const std::string m_filename;
  size_t m_size;
  FILE * m_fd;
  mode_t mode;
};

}

#endif
