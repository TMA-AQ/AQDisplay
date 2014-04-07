#include "Logger.h"
#include <boost/thread.hpp>
#include <boost/shared_ptr.hpp>

#if defined(__FreeBSD__)
# include <fcntl.h>
# include <sys/types.h>
# include <sys/uio.h>
# include <unistd.h>
#elif defined(WIN32)
# define snprintf sprintf_s
#endif

#define BRIGHT 1

// Foreground colors
#define FG_BLACK           30    
#define FG_RED             31    
#define FG_GREEN           32    
#define FG_YELLOW          33    
#define FG_BLUE            34    
#define FG_MAGENTA         35    
#define FG_CYAN            36    
#define FG_WHITE           37    

//    Background colors
#define BG_BLACK           40    
#define BG_RED             41    
#define BG_GREEN           42    
#define BG_YELLOW          43    
#define BG_BLUE            44    
#define BG_MAGENTA         45    
#define BG_CYAN            46    
#define BG_WHITE           47    

using namespace aq;

static boost::shared_ptr<Logger> systemLog;

Logger& Logger::getInstance()
{
  if (systemLog == 0)
  {
    systemLog.reset(new Logger("UserLog", STDOUT));
  }
  return *systemLog;
}

Logger& Logger::getInstance(const char * ident, int mode)
{
  if (systemLog == 0)
  {
    systemLog.reset(new Logger(ident, mode));
  }
  return *systemLog;
}

Logger::Logger(const char *_ident, int _mode) 
  : level(-1),
  localFile(0),
  mode(_mode),
  colorLog(false),
  lockMode(false),
  dateMode(false),
  pidMode(false)
{
  ::strncpy(this->ident, _ident, sizeof(this->ident)-1);
  ident[sizeof(this->ident)-1] = '\0';
  if (this->mode & SYSLOG) 
  {
#if defined (__SYSLOG__)
    ::openlog(this->ident, LOG_NDELAY, LOG_USER);
#endif
  }
  else if (this->mode & LOCALFILE)
  {
    if (this->openFile(_ident) != 0)
		{
			this->mode = STDOUT;
		}
  }
}

Logger::~Logger()
{
  ::memset(this->ident, 0, sizeof(this->ident));
  if (this->mode & SYSLOG)
  {
#if defined (__SYSLOG__FreeBSD)
    ::closelog();
#endif
  }
  if (this->mode & LOCALFILE)
  {
    this->closeFile();
  }
}

void Logger::setLocalFile(const char * filename)
{
	if (this->localFile)
	{
		this->closeFile();
	}
	this->mode = LOCALFILE;
	if (this->openFile(filename) != 0)
	{
		this->mode = STDOUT;
	}
}

void Logger::log(const char *file, const char *function, unsigned int line, int facility, const char *format, ...) const
{
  int pos = 0;
  va_list ap;
  char buf1[LOGBUFFER];
  ::memset(buf1, 0, LOGBUFFER);
  if ((pos = ::snprintf(buf1, sizeof(buf1), "%s - %s:%d: ", file, function, line)) < 0)
  {
    std::cerr << "ERROR: cannot log" << std::endl;
  }
  va_start(ap, format);
  vsnprintf(buf1 + pos, sizeof(buf1) - pos, format, ap);
  va_end(ap);
  this->log(facility, "%s", buf1);
}

void Logger::log(int facility, const char * format, ...) const
{
  int pos = 0;
  va_list ap;
  time_t currentTime;
  struct tm * localTime = 0;
  char date[256];
  char buf1[LOGBUFFER];
  ::memset(date, 0, 256);
  ::memset(buf1, 0, LOGBUFFER);

  if (this->level != -1 && (this->level < facility))
  {
    // do not log
    return ;
  }

  if (this->pidMode)
  {
    std::ostringstream id;
    id << boost::this_thread::get_id();
    pos = ::snprintf(buf1, LOGBUFFER, "[%s] - ", id.str().c_str());
    if (pos < 0)
    {
      std::cerr << "ERROR: cannot log" << std::endl;
    }
  }

  if (this->dateMode && ((this->mode & STDOUT) || (this->mode & LOCALFILE)))
  {
    currentTime = ::time(nullptr);
    if ((localTime = ::localtime(&currentTime)) != 0)
    {
      ::strftime(date, sizeof(date), "%d/%b/%Y:%H-%M", localTime);
    }
    pos += ::snprintf(buf1 + pos, sizeof(buf1) - pos, "[%s] - ", date);
    if (pos < 0)
    {
      std::cerr << "ERROR: cannot log" << std::endl;
    }
  }

  va_start(ap, format);
  vsnprintf(buf1 + pos, sizeof(buf1) - pos, format, ap);
  va_end(ap);

  //
  //

#if defined (__SYSLOG__)
  if (this->mode & SYSLOG) 
  {
    boost::mutex::scoped_lock lock(this->mutex);
    ::syslog(facility, "%s", buf1);
  }
#endif

  if (this->mode & LOCALFILE) 
  {
    boost::mutex::scoped_lock lock(this->mutex);
    ::fwrite(buf1, sizeof(char), strlen(buf1), this->localFile);
  }

  if (this->mode & STDOUT)
  {
    if (this->lockMode) 
    {
      boost::mutex::scoped_lock lock(this->mutex);
      this->printStdOut(buf1, facility);
    }
    else
    {
      this->printStdOut(buf1, facility);
    }
  }

}

void Logger::printStdOut(const char * buf, int facility) const
{
  if (this->out != nullptr)
  {
    (*this->out) << buf;
  }
  else if (this->colorLog)
  {
    this->printWithColor(buf, facility);
  }
  else
  {
    ::fprintf(stderr, "%s", buf);
  }
  ::fflush(stderr);
}

void Logger::printWithColor(const char * buf, int facility) const
{
  switch (facility)
  {
  case AQ_LOG_CRIT:
    ::fprintf(stderr, "%c[%d;%d;%dm%s\033[0m", 0x1B, BRIGHT, FG_YELLOW, BG_RED, buf);
    break;
  case AQ_LOG_ERR:
    ::fprintf(stderr, "%c[%d;%dm%s\033[0m", 0x1B, BRIGHT, FG_RED, buf);
    break;
  case AQ_LOG_WARNING:
    ::fprintf(stderr, "%c[%d;%dm%s\033[0m", 0x1B, BRIGHT, FG_YELLOW, buf);
    break;
  case AQ_LOG_NOTICE:
    ::fprintf(stderr, "%c[%d;%dm%s\033[0m", 0x1B, BRIGHT, FG_GREEN, buf);
    break;
  case AQ_LOG_INFO:
    ::fprintf(stderr, "%c[%d;%dm%s\033[0m", 0x1B, BRIGHT, FG_CYAN, buf);
    break;
  case AQ_LOG_DEBUG:
    ::fprintf(stderr, "%c[%d;%dm%s\033[0m", 0x1B, BRIGHT, FG_MAGENTA, buf);
    break;
  default:
    ::fprintf(stderr, "%c[%d;%dm%s\033[0m", 0x1B, BRIGHT, FG_BLACK, buf);
  }
}

int Logger::openFile(const char * name)
{
  this->localFile = fopen(name, "a");
	if (localFile == nullptr)
		return -1;
	return 0;
}

void Logger::closeFile(void)
{	
  boost::mutex::scoped_lock lock(this->mutex);
  fclose(this->localFile);
}
