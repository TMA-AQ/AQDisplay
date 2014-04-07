#ifndef __AQ_SETTINGS_H__
#define __AQ_SETTINGS_H__

#if defined (WIN32)
# ifdef AQENGINE_EXPORTS
#  define AQENGINE_API __declspec(dllexport)
# else
#  define AQENGINE_API __declspec(dllimport)
# endif
#else
// # define AQENGINE_API __stdcall
# define AQENGINE_API
#endif

#include <string>
#include <sstream>
#include <boost/shared_ptr.hpp>
#include <boost/optional.hpp>

#define STR_BUF_SIZE 4096

#ifndef _MAX_PATH
#define _MAX_PATH 1024
#endif

namespace aq
{

/// \brief AlgoQuest Settings
struct Settings 
{
  typedef boost::shared_ptr<Settings> Ptr;
  
  mutable std::stringstream output;

	std::string iniFile; ///< settings ini absolute file name
	std::string queryIdent; ///< query ident. each query should have an unique identification (as uuid)
	std::string outputFile; ///< the output file where result are written
	std::string answerFile; ///< the answer file where result are written \deprecated
	std::string dbDesc; ///< the database description file
	std::string aqEngine; ///< aq engine file name executable
	std::string aqLoader; ///< aq loader file name executable
  std::string aqHome; ///< algoquest databases root directory (path containing the directory database)
  std::string aqName; ///< algoquest database name
  std::string rootPath; ///< algoquest dabtabase root directory (aqHome/aqName/)
  std::string workingPath; ///< working directory for aq engine
	std::string tmpRootPath; ///< temporary root working directory for aq engine
  std::string dataPath; ///< data files directory (PRM, THESAURUSE, VDG, NMO and PRD files)
	std::string tmpPath; ///< temporary directory for aq engine (containing temporary table from nested queries)
	std::string dpyPath; ///< tempoaray display directory for aq engine (containing dpy/aq-matrix files)

	char fieldSeparator; ///< field separator of table loading files
	static const int MAX_COLUMN_NAME_SIZE = 255;
	
	size_t worker; ///< number of pool thread to resolve several queries
	size_t group_by_process_size; ///< number of thread to resolve aq matrix when a group by occur
  size_t process_thread; ///< number of thread to resolve a query
	
  int packSize; ///< packet size of the database (see aq-engine specification for more explanation)
	// int maxRecordSize; ///< \deprecated

  /// \name settings_flags settings flags
  /// \todo use mask
  /// \{
	bool computeAnswer;
	bool csvFormat;
	bool skipNestedQuery;
  bool useBinAQMatrix;
  bool displayCount;
  bool cmdLine;
  bool trace;
  /// \}

	AQENGINE_API Settings();
	AQENGINE_API Settings(const Settings&);
	AQENGINE_API ~Settings();
	AQENGINE_API Settings& operator=(const Settings&);

  /// \brief initialize path
  /// \param root
  AQENGINE_API void initPath(const std::string& root);

  /// \brief load ini file and set query ident
  /// \param iniFile
  /// \param queryIdent
  AQENGINE_API void load(const std::string& iniFile, const std::string& queryIdent);
	
  /// \brief load ini file
  /// \param iniFile
  AQENGINE_API void load(const std::string& iniFile);

  /// \brief load ini stream
  /// \param is
	AQENGINE_API void load(std::istream& is);

  /// \brief change query ident
  /// \param queryIdent
	AQENGINE_API void changeIdent(const std::string& queryIdent);

  /// \brief dump settings
  /// \param os
  AQENGINE_API void dump(std::ostream& os) const;

  /// \brief dump settings
  /// \return a string representing settings
  AQENGINE_API std::string to_string() const;

  /// \brief write the aq engine ini file
  /// \param os
  AQENGINE_API void writeAQEngineIni(std::ostream& os) const;
};

}

#endif
