#ifndef __FILE_MAPPER_H__
#define __FILE_MAPPER_H__

#if defined (WIN32)

#include "WIN32FileMapper.h"
namespace aq
{
  typedef WIN32FileMapper FileMapper;
}

#else

#include "GenericFileMapper.h"
namespace aq
{
  typedef aq::GenericFileMapper FileMapper;
}

#endif

#endif
