#include "GenericFileMapper.h"
#include "Logger.h"
#include "Exceptions.h"
#include <iostream>
#include <stdint.h>

using namespace aq;

GenericFileMapper::GenericFileMapper(const char * _filename, const mode_t _mode)
	: m_filename(_filename), mode(_mode)
{
  if (_mode == mode_t::READ)
  {
    this->m_fd = fopen(_filename, "rb");
  }
  else
  {
    this->m_fd = fopen(_filename, "a+");
  }
  if (this->m_fd != nullptr)
  {
    fseek(this->m_fd, 0, SEEK_END);
    this->m_size = ftell(this->m_fd);
    fseek(this->m_fd, 0, SEEK_SET);
  }
}

GenericFileMapper::~GenericFileMapper()
{
  fclose(this->m_fd);
}

int GenericFileMapper::read(void * buffer, size_t offset, size_t len) 
{
  assert(this->m_fd);
  assert(offset < std::numeric_limits<size_t>::max());
  if (this->m_fd == nullptr)
  {
    throw aq::generic_error(aq::generic_error::COULD_NOT_OPEN_FILE, m_filename);
  }
  if (offset >= std::numeric_limits<size_t>::max())
  {
    throw aq::generic_error(aq::generic_error::GENERIC, "bad offset of mapped file " + m_filename);
  }
  
  fseek(this->m_fd, static_cast<long>(offset), SEEK_SET);
  fread(buffer, 1, len, this->m_fd);
  if (feof(this->m_fd))
    return -1;
  return 0;
}

int GenericFileMapper::write(void * buffer, size_t offset, size_t len)
{
  assert(this->m_fd);
  assert(offset < std::numeric_limits<size_t>::max());
  if (this->m_fd == nullptr)
  {
    throw aq::generic_error(aq::generic_error::COULD_NOT_OPEN_FILE, m_filename);
  }
  if (offset >= std::numeric_limits<size_t>::max())
  {
    throw aq::generic_error(aq::generic_error::GENERIC, "bad offset of mapped file " + m_filename);
  }
  
  fseek(this->m_fd, static_cast<long>(offset), SEEK_SET);
  fwrite(buffer, 1, len, this->m_fd);
  if (feof(this->m_fd))
    return -1;
  return 0;
}

int GenericFileMapper::move(size_t new_offset, size_t old_offset, size_t len)
{
  // TODO
  return 0;
}

int GenericFileMapper::erase(size_t offset, size_t len)
{
  return this->move(offset, offset + len, this->size() - len);
}