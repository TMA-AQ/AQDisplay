/// \file Utilities.h

#ifndef __AQ_UTILITIES_H__
#define __AQ_UTILITIES_H__

#include "Symbole.h"
#include "DBTypes.h"
#include "Exceptions.h"
#include <cstdio>
#include <string>
#include <vector>

//------------------------------------------------------------------------------
typedef long long llong;

//------------------------------------------------------------------------------
#define STR_BUF_SIZE 4096

#ifndef _MAX_PATH
#define _MAX_PATH 1024
#endif

namespace aq {

/// utils functions
namespace util {

/// \defgroup conversion_function miscellaenous conversions functions
/// \{

int StrToInt(const char* psz, llong* pnVal);
int StrToDouble(const char* psz, double* pdVal);
std::wstring string2Wstring(const std::string& s);
char* strtoupr(char* pszStr);
void doubleToString(char* strVal, double dVal);
aq::ColumnType symbole_to_column_type(aq::symbole s);
const char * symbole_to_char(aq::symbole sid);

/// \}

/// \brief remove all char at end of an input string
/// \param input the string to modify
/// \param c the char(s) to remove at end of the input string (space by default if not specify)
void removeCharAtEnd(char * input, char c = ' ');

/// \brief Change all char that match an input char in a string
/// \param input string to modify
/// \param old_c the char to change
/// \param new_c the new char 
void ChangeChar(char * input, char old_c, char new_c);

/// \defgroup file_function miscellaenous files functions
/// \{

/// \brief save a file with data to write in
/// \throw generic_error
void SaveFile(const char * pszFN, const char * pszToSave);

/// \brief copy a file
/// \param srcPath the file to copy
/// \param dstPath the new file
/// \return 0 on success, -1 on error
int FileCopy(char * srcPath, char * dstPath);

/// \brief rename a file
/// \param srcPath the file to rename
/// \param dstPath the new file name
/// \return 0 on success, -1 on error
int FileRename(const char * srcPath, const char * dstPath);

/// \brief clean a directory
/// \param path the directory to clean
void CleanFolder(const char * path);

/// \brief clean a directory
/// \param path the directory to remove
void DeleteFolder(const char * path);

/// \brief open an UTF-8 file
/// Should be used when reading UTF8 files
/// \param filename the name of file to open
/// \param mode refer to mode used by c fopen function
/// \return a FILE stream if succeed, nullptr if failed
FILE* fopenUTF8(const char * filename, const char * mode);

/// \brief get list of files in directory
/// \deprecated
/// \param path the directory
/// \param files the list of files
/// \return 0 on success, -1 on error
int GetFiles(const char * path, std::vector<std::string>& files);

/// \brief get filename in directory
/// \param path the directory
/// \param filenames the list of files
/// \param prefix a prefix to add for each file
/// \return 0 on success, -1 on error
int getFileNames(const char * path, std::vector<std::string>& filenames, const char * prefix = nullptr);

/// \}

}
}

#endif /* __AQ_UTILITIES_H__ */
