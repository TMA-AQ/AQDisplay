#ifndef __LOG_H__
#define __LOG_H__

#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <cstdarg>
#include <sstream>
#include <boost/cstdint.hpp>
#include <boost/thread/mutex.hpp>

#if defined(__FreeBSD__)
# include <syslog.h>
# include <pthread.h>
#endif

#define LOG_FILE  -1

#define SYSLOG		0x01
#define LOCALFILE	0x02
#define STDOUT		0x04

#define LOGBUFFER 8192

namespace aq
{

  /// \brief Logging singleton class
  class Logger
  {
  public:

    static Logger& getInstance();
    static Logger& getInstance(const char * ident, int mode);

    ~Logger();

    void redirectToStream(std::ostream& os) { this->out = &os; }
		void setLocalFile(const char *);

    void setLockMode(bool value)      { this->lockMode = value; }
    void setColorMode(bool value)     { this->colorLog = value; }
    void setDateMode(bool value)      { this->dateMode = value; }
    void setPidMode(bool value)       { this->pidMode = value; }
    void setMode(char _mode)          { this->mode = _mode; }
    void setLevel(int _level)         { this->level = _level; }
    void setIdent(const char *_ident) { 
      ::memset(this->ident, 0, sizeof(ident)); 
      ::strncpy(this->ident, _ident, sizeof(this->ident)); 
    }

    char getMode() const { return this->mode; }
    const char * getIdent() const { return this->ident; }
    
    void log(int facility, const char * format, ...) const;
    void log(const char *file, const char *function, unsigned int line, int facility, const char * format, ...) const;

  private:  
    Logger(const char * ident, int mode);
    Logger(const Logger& );
    Logger& operator=(const Logger& source);

    void printStdOut(const char * but, int facility) const;
    void printWithColor(const char * buf, int facility) const;
    int  openFile(const char *);
    void closeFile(void);

    std::ostream * out;
    int level;
    FILE * localFile;
    char mode;
    char ident[64];
    char statsIdent[64];
    char httpIdent[64];

    mutable boost::mutex mutex;

    bool colorLog;
    bool lockMode;
    bool dateMode;
    bool pidMode;
  };

}

enum log_facilities {
  AQ_LOG_CRIT = 2,
  AQ_LOG_ERR,
  AQ_LOG_WARNING,
  AQ_LOG_NOTICE,
  AQ_LOG_INFO,
  AQ_LOG_DEBUG,
};

#ifdef _DEBUG

# define AQ_CRITICAL __FILE__, __FUNCTION__, __LINE__, AQ_LOG_CRIT
# define AQ_ERROR __FILE__, __FUNCTION__, __LINE__, AQ_LOG_ERR
# define AQ_WARNING __FILE__, __FUNCTION__, __LINE__, AQ_LOG_WARNING
# define AQ_NOTICE __FILE__, __FUNCTION__, __LINE__, AQ_LOG_NOTICE
# define AQ_INFO __FILE__, __FUNCTION__, __LINE__, AQ_LOG_INFO
# define AQ_DEBUG __FILE__, __FUNCTION__, __LINE__, AQ_LOG_DEBUG

# define AQ_LOG_DEBUG(args) \
  do {                   \
    aq::Logger::getInstance().log(AQ_DEBUG, args);                \
  } while (false) ;

#else

# define AQ_CRITICAL __FILE__, __FUNCTION__, __LINE__, AQ_LOG_CRIT
# define AQ_ERROR __FILE__, __FUNCTION__, __LINE__, AQ_LOG_ERR
# define AQ_WARNING __FILE__, __FUNCTION__, __LINE__, AQ_LOG_WARNING
# define AQ_NOTICE __FILE__, __FUNCTION__, __LINE__, AQ_LOG_NOTICE
# define AQ_INFO __FILE__, __FUNCTION__, __LINE__, AQ_LOG_INFO
# define AQ_DEBUG __FILE__, __FUNCTION__, __LINE__, AQ_LOG_DEBUG

# define AQ_LOG_DEBUG(args) \
  do {                   \
    aq::Logger::getInstance().log(AQ_DEBUG, args);                \
  } while (false) ;

//# define AQ_CRITICAL AQ_LOG_CRIT
//# define AQ_ERROR AQ_LOG_ERR
//# define AQ_WARNING AQ_LOG_WARNING
//# define AQ_NOTICE AQ_LOG_NOTICE
//# define AQ_INFO AQ_LOG_INFO
//# define AQ_DEBUG AQ_LOG_DEBUG
//
//# define AQ_LOG_DEBUG(inst)

#endif

#endif
