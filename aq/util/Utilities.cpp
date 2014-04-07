#include "Utilities.h"
#include "Exceptions.h"
#include "Logger.h"
#include <cstdio>
#include <cstdlib>
#include <climits>
#include <cstring>
#include <boost/filesystem.hpp>

#ifdef WIN32
#include <Windows.h>
#endif

#ifndef _MAX_PATH
#define _MAX_PATH 1024
#endif

namespace aq {
namespace util {
  
//------------------------------------------------------------------------------
#if defined(_MSC_VER)
# define strtoll _strtoi64
#endif
#if defined(__FreeBSD__)
# define strtoll strtol
# define atoll atol
#endif
int StrToInt( const char* psz, llong* pnVal )
{
	char* pszIdx;
	if( pnVal == nullptr || psz == nullptr )
		return -1;
	if( psz[0] <= ' ' )
		return -1;
	errno = 0;
	*pnVal = strtoll(psz, &pszIdx, 10);
	if( errno != 0 )
		return -1;
	if( *pszIdx != '\0' )
		return -1;
	return 0;
}

//------------------------------------------------------------------------------
int StrToDouble( const char* psz, double* pdVal  )
{
	char* pszIdx;
	if( pdVal == nullptr || psz == nullptr )
		return -1;
	if( psz[0] <= ' ' )
		return -1;
	errno = 0;
	*pdVal = strtod(psz, &pszIdx);
	if( errno != 0 )
		return -1;
	if( *pszIdx != '\0' )
		return -1;
	return 0;
}

//------------------------------------------------------------------------------
char* strtoupr( char* pszStr ) 
{
	char *psz;

	if ( pszStr == nullptr )
		return nullptr;

	psz = pszStr;
	while ( *psz != '\0' )
	{
		*psz = toupper( *psz );
		psz += 1;
	}
	return pszStr;
}

//------------------------------------------------------------------------------
std::wstring string2Wstring(const std::string& s)
{
#ifdef WIN32
  int len;
  int slength = (int)s.length() + 1;
  len = MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, 0, 0); 
  wchar_t* buf = new wchar_t[len];
  MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, buf, len);
  std::wstring r(buf);
  delete[] buf;
  return r;
#else
  assert(false);
  (void)s;
#endif
}

//------------------------------------------------------------------------------
void doubleToString( char* strVal, double dVal )
{
	llong iVal = (llong)(dVal * 100 + ((dVal > 0.0) ? 0.5 : -0.5));
	dVal = (double) iVal / 100;
	sprintf( strVal, "%.2lf", dVal );
}

//------------------------------------------------------------------------------
aq::ColumnType symbole_to_column_type(symbole s)
{
  switch (s)
  {
  case t_int: return aq::COL_TYPE_INT; break;
  case t_long_long: return aq::COL_TYPE_BIG_INT; break;
  case t_date1: return aq::COL_TYPE_DATE; break;
  case t_date2: return aq::COL_TYPE_DATE; break;
  case t_date3: return aq::COL_TYPE_DATE; break;
  case t_char: return aq::COL_TYPE_VARCHAR; break;
  case t_double: return aq::COL_TYPE_DOUBLE; break;
  case t_raw: return aq::COL_TYPE_VARCHAR; break;
  default: return aq::COL_TYPE_VARCHAR;
  }
}

//------------------------------------------------------------------------------
const char * symbole_to_char(aq::symbole sid)
{
  switch (sid)
  {
  case faux: return "faux"; break; 
  case vrai: return "vrai"; break; 
  case vide: return "vide"; break; 
  case unaire: return "unaire"; break; 
  case binaire: return "binaire"; break; 
  case scalaire: return "scalaire"; break; 
  case vecteur: return "vecteur"; break; 
  case liste: return "liste"; break; 
  case r_et: return "r_et"; break; 
  case r_ou: return "r_ou"; break; 
  case r_feuille: return "r_feuille"; break; 
  case r_frere: return "r_frere"; break; 
  case mini_mot: return "mini_mot"; break; 
  case maxi_mot: return "maxi_mot"; break; 
  case liste_mot: return "liste_mot"; break;
  case r_liste: return "r_liste"; break; 
  case inf_egal: return "inf_egal"; break; 
  case egal: return "egal"; break; 
  case sup_egal: return "sup_egal"; break;
  case hp: return "hp"; break;
  case tuples: return "tuples"; break; 
  case r_tag: return "r_tag"; break; 
  case fils_gauche: return "fils_gauche"; break; 
  case fils_droit: return "fils_droit"; break;
  case t_int: return "t_int"; break; 
  case t_double: return "t_double"; break; 
  case t_date1: return "t_date1"; break; 
  case t_date2: return "t_date2"; break; 
  case t_date3: return "t_date3"; break;
  case t_char : return "t_char "; break; 
  case t_long_long: return "t_long_long"; break; 
  case t_raw : return "t_raw "; break;
  case m_up: return "m_up"; break; 
  case m_down: return "m_down"; break;
  case n_contenu: return "n_contenu"; break; 
  case n_table: return "n_table"; break;
  case t_continue : return "t_continue "; break;  
  case t_done : return "t_done "; break; 
  case t_eof: return "t_eof"; break; 
  case t_file_read_error: return "t_file_read_error"; break; 
  case r_jeq: return "r_jeq"; break;
  case r_between: return "r_between"; break; 
  case r_sup: return "r_sup"; break; 
  case r_inf: return "r_inf"; break; 
  case r_leq: return "r_leq"; break; 
  case r_seq: return "r_seq"; break; 
  case r_in: return "r_in"; break; 
  case r_equal : return "r_equal "; break;
  case l_source: return "l_source"; break;  
  case l_pivot: return "l_pivot"; break; 
  case l_cible: return "l_cible"; break;
  case ec_requete: return "ec_requete"; break; 
  case ec_jointure: return "ec_jointure"; break; 
  case ec_etape_1: return "ec_etape_1"; break; 
  case ec_etape_2: return "ec_etape_2"; break; 
  case ec_etape_3 : return "ec_etape_3 "; break;
  case ec_etape_4: return "ec_etape_4"; break; 
  case ec_hp: return "ec_hp"; break; 
  case ec_tuple: return "ec_tuple"; break;
  case c_neutre: return "c_neutre"; break; 
  case c_calcul: return "c_calcul"; break;
  case string: return "string"; break; 
  case integer: return "integer"; break; 
  case d_nulle: return "d_nulle"; break; 
  case comma: return "comma"; break;
  case my_eof: return "my_eof"; break; 
  case une_table: return "une_table"; break; 
  case column: return "column"; break; 
  case copy: return "copy"; break; 
  case vdg: return "vdg"; break; 
  case troncat: return "troncat"; break; 
  case name: return "name"; break;
  case file: return "file"; break; 
  case t_row_id: return "t_row_id"; break; 
  case precision: return "precision"; break; 
  case t_star: return "t_star"; break; 
  case last_symbole: return "last_symbole"; break;
  default: return "unknown"; break;
  }
}

//-------------------------------------------------------------------------------
void removeCharAtEnd(char *my_field, char c)
{
	size_t max_size = strlen(my_field);
  if (max_size == 0) 
    return;
	for (size_t i = max_size - 1; i > 0 ; i--)
	{
		if (my_field[i] == c) 
      my_field[i] = '\0';
		else 
      return;
	}
}

//-------------------------------------------------------------------------------
void ChangeChar(char * string, char old_c, char new_c)
{
  char * p = nullptr;
  do
  {
    p = strchr(string, old_c);
    if (p != nullptr) 
      *p = new_c ;
  } while (p != nullptr);
}

//------------------------------------------------------------------------------
void SaveFile( const char *pszFN, const char* pszToSave )
{
	FILE *pFOut;

	pFOut = fopen( pszFN, "wt" );
	if ( pFOut == nullptr )
		throw generic_error(generic_error::GENERIC, "");
	if ( fputs( pszToSave, pFOut ) < 0 ) {
		fclose( pFOut );
		throw generic_error(generic_error::GENERIC, "");
	}
	fclose( pFOut );
}

//------------------------------------------------------------------------------
int FileCopy( char* pszSrcPath, char* pszDstPath )
{
	if( strcmp(pszSrcPath, pszDstPath) == 0 )
		return 0;

	FILE *fsrc = nullptr;
	FILE *fdst = nullptr;
	char c;

	fsrc = fopen( pszSrcPath, "rb" );
	if( fsrc == nullptr )
		return -1;

	fdst = fopen( pszDstPath, "wb" );
	if( fdst == nullptr )
		return -1;

	while( fread(&c, 1, 1, fsrc ) == 1 )
		if( fwrite(&c, 1, 1, fdst) != 1 ) 
		{
			fclose( fsrc );
			fclose( fdst );
			return -1;
		}

	fclose( fsrc );
	fclose( fdst );
	return 0;
}

//------------------------------------------------------------------------------
int FileRename(const char* pszSrcPath, const char* pszDstPath)
{
	boost::system::error_code ec;
	boost::filesystem::path oldFile(pszSrcPath); 
	boost::filesystem::path newFile(pszDstPath);
	boost::filesystem::rename(oldFile, newFile, ec);
	if (ec)
	{
		aq::Logger::getInstance().log(AQ_ERROR, "Cannot rename file %s to %s\n", pszSrcPath, pszDstPath);
		return -1;
	}
	aq::Logger::getInstance().log(AQ_DEBUG, "rename file %s to %s\n", pszSrcPath, pszDstPath);
	return 0;
}

//------------------------------------------------------------------------------
void CleanFolder( const char * pszPath )
{
	boost::filesystem::path p(pszPath);
	if (!boost::filesystem::exists(p))
	{
		aq::Logger::getInstance().log(AQ_INFO, "path %s doesn't exists\n", p.string().c_str());
	}
	else
	{
		boost::system::error_code ec;
    for (boost::filesystem::directory_iterator file(p); file != boost::filesystem::directory_iterator(); ++file)
    {
      const std::string& filename = (*file).path().string();
      if ((filename.size() > 4) && (filename.substr(filename.size() - 4) == ".TMP")) continue; // FIXME
      if (!boost::filesystem::remove_all(*file, ec))
      {
        aq::Logger::getInstance().log(AQ_ERROR, "cannot delete path %s\n", (*file).path().string().c_str());
      }
      else
      {
        aq::Logger::getInstance().log(AQ_DEBUG, "delete path %s\n", (*file).path().string().c_str());
      }
    }
	}
}

//------------------------------------------------------------------------------
void DeleteFolder( const char * pszPath )
{
	boost::filesystem::path p(pszPath);
	if (!boost::filesystem::exists(p))
	{
		aq::Logger::getInstance().log(AQ_INFO, "path %s doesn't exists\n", p.string().c_str());
	}
	else
	{
		boost::system::error_code ec;
		if (!boost::filesystem::remove_all(p, ec))
		{
			aq::Logger::getInstance().log(AQ_ERROR, "cannot delete path %s\n", p.string().c_str());
		}
		else
		{
			aq::Logger::getInstance().log(AQ_DEBUG, "delete path %s\n", p.string().c_str());
		}
	}
}

//------------------------------------------------------------------------------
FILE* fopenUTF8( const char* pszFlename, const char* pszMode )
{
	FILE	*fp = nullptr;
	unsigned char bom[3];
	fp = fopen(pszFlename, pszMode);
	if( fp == nullptr )
		return nullptr;

	/* Skip UTF-8 BOM if present */
	if( fread( bom, 3, sizeof(char), fp ) != 1 )
	{
		fclose( fp );
		return nullptr;
	}
	if( !((bom[0] == 0xEF) && (bom[1] == 0xBB) && (bom[2] == 0xBF)) )
	{
		/* BOM not present, return to the beginning */
		if ( fseek( fp, 0, SEEK_SET ) != 0 ) {
			fclose( fp );
			return nullptr;
		}
	}
	return fp;
}

//------------------------------------------------------------------------------
int GetFiles(const char* pszSrcPath, std::vector<std::string>& files)
{
#ifdef WIN32
	WIN32_FIND_DATA ffd;
	HANDLE hFind = INVALID_HANDLE_VALUE;
	std::string path( pszSrcPath );
	path += "\\*";
	hFind = FindFirstFile( path.c_str(), &ffd);

	if (INVALID_HANDLE_VALUE == hFind)
		return -1;

	do
	{
		if( !(ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) )
			files.push_back( ffd.cFileName );
	}
	while (FindNextFile(hFind, &ffd) != 0);

	DWORD dwError = GetLastError();
	if (dwError != ERROR_NO_MORE_FILES) 
		return -1;

	FindClose(hFind);
#else
  throw aq::generic_error(aq::generic_error::NOT_IMPLEMENTED, "");
	//not implemented yet
#endif
	return 0;
}

//------------------------------------------------------------------------------
int getFileNames(const char * path, std::vector<std::string>& filenames, const char * prefix)
{
  boost::filesystem::path p(path);
	if (!boost::filesystem::exists(p))
	{
		aq::Logger::getInstance().log(AQ_INFO, "path %s doesn't exists\n", p.string().c_str());
    return -1;
	}
	else
	{
		boost::system::error_code ec;
    for (boost::filesystem::directory_iterator file(p); file != boost::filesystem::directory_iterator(); ++file)
    {
      if ((prefix != nullptr) && ((*file).path().string().find(prefix) != std::string::npos))
      {
        filenames.push_back((*file).path().string());
      }
    }
	}
  return 0;
}

}
}
