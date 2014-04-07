#include "AQEngine.h"
#include <aq/util/Exceptions.h>
#include <aq/util/Logger.h>
#include <string>
#include <fstream>
#include <boost/filesystem.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

using namespace aq::engine;

AQEngine::AQEngine(const aq::Base::Ptr _baseDesc, const aq::Settings::Ptr _settings)
  : baseDesc(_baseDesc), settings(_settings)
{
}


AQEngine::~AQEngine(void)
{
}

void AQEngine::call(const std::string& query, mode_t mode)
{
  if (query.size() < 2048) // fixme
    aq::Logger::getInstance().log(AQ_DEBUG, "\n%s\n", query.c_str());

  //
  //
  if (this->settings->cmdLine && this->settings->trace)
  {
    std::cout << std::endl;
    std::cout << query << std::endl;
    std::cout << std::endl;
  }

  //
  //
  std::string new_request_file = settings->workingPath + "New_Request.txt";
  aq::util::SaveFile(new_request_file.c_str(), query.c_str());

  if ((mode == 0) || (!settings->skipNestedQuery))
  {
    int rc = 1;
    std::string prg = settings->aqEngine;
    std::string arg = settings->iniFile + " " + settings->queryIdent;
    arg += mode == NESTED_2 ? " NoDpy" : " Dpy";
    if ((rc = this->run(prg.c_str(), arg.c_str())) != 0)
    {
      aq::Logger::getInstance().log(AQ_ERROR, "call to %s %s failed [ExitCode:%d]\n", prg.c_str(), arg.c_str(), rc);
      if (query.size() < 2048) // fixme
        aq::Logger::getInstance().log(AQ_ERROR, "\n\nAQEngine ERROR:\n%s\n", query.c_str());
      throw aq::generic_error(aq::generic_error::AQ_ENGINE, "call to %s %s failed [ExitCode:%d]", prg.c_str(), arg.c_str(), rc);
    }
  }
  else
  {
    aq::Logger::getInstance().log(AQ_DEBUG, "do not call aq engine for nested query\n");
  }

  if ((mode == REGULAR) || (mode == NESTED_1))
  {
    tableIDs.clear();
    aqMatrix.reset(new AQMatrix(settings, baseDesc));

#ifndef __NO_LOAD_FULL_AQ_MATRIX__
    aqMatrix->load(settings->dpyPath.c_str(), this->tableIDs);
    if (mode == REGULAR)
    {
      aq::util::DeleteFolder(settings->dpyPath.c_str());
    }
    else
    {
      aq::util::CleanFolder(settings->tmpPath.c_str());
    }
#else
    aqMatrix->loadHeader(settings.szTempPath2, this->tableIDs);
    aqMatrix->prepareData(settings.szTempPath2);
#endif

  }
  else if (mode == NESTED_2)
  {
  }
  else
  {
    throw aq::generic_error(aq::generic_error::NOT_IMPLEMENTED, "aq engine mode not supported");
  }

}

void AQEngine::call(const aq::core::SelectStatement& query, mode_t mode)
{
  std::string query_str;
  query.setOutput(aq::core::SelectStatement::output_t::AQL);
  query.to_string(query_str);
  this->call(query_str, mode);
}

void AQEngine::renameResult(unsigned int id, std::vector<std::pair<std::string, std::string> >& resultTables)
{
  std::vector<std::string> files;
  if (aq::util::getFileNames(this->settings->tmpPath.c_str(), files) != 0)
    throw aq::generic_error(aq::generic_error::COULD_NOT_OPEN_FILE, "");

  size_t reg = 0;
  size_t packet = 0;
  for (auto& file : files)
  {
    if (((file).length() == 32)
      && ((file)[0] == 'B')
      && ((file)[4] == 'T')
      && ((file)[9] == 'T')
      && ((file)[10] == 'P')
      && ((file)[11] == 'N')
      && ((file)[17] == 'P')
      && ((file)[30] == '.')
      && (((file)[31] == 's') || ((file)[31] == 't'))) // BxxxTxxxxTPNxxxxPxxxxxxxxxxxx.[st]
    {
      reg = boost::lexical_cast<size_t>((file).substr(5, 4));
      packet = boost::lexical_cast<size_t>((file).substr(18, 12));
    }
    else if (((file).length() == 26)
      && ((file)[0] == 'B')
      && ((file)[4] == 'R')
      && ((file)[5] == 'E')
      && ((file)[6] == 'G')
      && ((file)[11] == 'P')
      && ((file)[24] == '.')
      && (((file)[25] == 's') || ((file)[25] == 't'))) // BxxxREGTxxxxPxxxxxxxxxxxx.[st]
    {
      reg = boost::lexical_cast<size_t>((file).substr(7, 4));
      packet = boost::lexical_cast<size_t>((file).substr(12, 12));
    }

    if (reg != 0)
    {
      std::string oldFile = std::string(this->settings->tmpPath.c_str()) + "/" + file;

      try
      {
        char newFile[_MAX_PATH];
        sprintf(newFile, "%s/B001REG%.4luTMP%.4uP%.12lu.TMP", this->settings->tmpPath.c_str(), reg, id, packet);
        ::remove(newFile);
        ::rename(oldFile.c_str(), newFile);

        Table::Ptr table = this->baseDesc->getTable(reg);
        packet = table->getTotalCount() / this->settings->packSize;
        sprintf(newFile, "B001REG%.4luTMP%.4uP%.12lu", reg, id, packet + 1);

        for (auto it = this->baseDesc->getTables().rbegin(); it != this->baseDesc->getTables().rend(); ++it)
        {
          if ((*it)->getReferenceTable() == table->getName())
          {
            table = *it;
            break;
          }
        }

        std::pair<std::string, std::string> p(newFile, table->getName());
        if (std::find(resultTables.begin(), resultTables.end(), p) == resultTables.end())
        {
          resultTables.push_back(p);
        }
      }
      catch (const boost::bad_lexical_cast&)
      {
        throw aq::generic_error(aq::generic_error::COULD_NOT_OPEN_FILE, "invalid result file '%s'", oldFile.c_str());
      }
    }

  }

  if (resultTables.empty())
  {
    throw aq::generic_error(aq::generic_error::AQ_ENGINE, "empty result");
  }

}

void AQEngine::prepare() const
{
  boost::filesystem::path p;
  p = boost::filesystem::path(settings->workingPath);
  boost::filesystem::create_directory(p);
  p = boost::filesystem::path(settings->tmpPath + "/dpy");
  boost::filesystem::create_directories(p);

  std::ofstream ini(settings->iniFile.c_str());
  ini << "export.filename.final=" << settings->dbDesc << std::endl;
  ini << "step1.field.separator=;" << std::endl;
  ini << "k_rep_racine=" << settings->rootPath << std::endl;
  ini << "k_rep_racine_tmp=" << settings->rootPath << std::endl;
  ini.close();
}

void AQEngine::clean() const
{
  aq::util::DeleteFolder(settings->workingPath.c_str());
  aq::util::DeleteFolder(settings->tmpPath.c_str());
}

#ifdef WIN32

#include <windows.h>

// ------------------------------------------------------------------------------------------------
AQEngineWindows::AQEngineWindows(const aq::Base::Ptr _baseDesc, const aq::Settings::Ptr _settings)
  : AQEngine(_baseDesc, _settings)
{
}

int AQEngineWindows::run(const char * prg, const char * args) const
{    
  aq::Logger::getInstance().log(AQ_NOTICE, "call: '%s %s'\n", prg, args);
  int rc = 1;
  STARTUPINFOW si;
  PROCESS_INFORMATION pi;
  ZeroMemory(&si, sizeof(si));
  si.cb = sizeof(si);
  ZeroMemory(&pi, sizeof(pi));

  std::string prg_s("E:/AQ_Bin/bin/aq-engine.exe");

  std::wstring wprg = aq::util::string2Wstring(prg_s);
  std::wstring warg = aq::util::string2Wstring(args);
  LPCWSTR prg_wstr = wprg.c_str();
  LPCWSTR arg_wstr = warg.c_str();
  if (CreateProcessW(prg_wstr, (LPWSTR)arg_wstr, nullptr, nullptr, FALSE, CREATE_NO_WINDOW, nullptr, nullptr, &si, &pi))
  {
    rc = WaitForSingleObject(pi.hProcess, INFINITE);
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
  }
  return rc;
}

namespace aq {
namespace engine {

AQEngine_Intf * getAQEngineWindow(const aq::Base::Ptr base, const aq::Settings::Ptr settings)
{
  return new AQEngineWindows(base, settings);
}

}
}

#endif

// ------------------------------------------------------------------------------------------------
AQEngineSystem::AQEngineSystem(const aq::Base::Ptr _baseDesc, const aq::Settings::Ptr _settings)
  : AQEngine(_baseDesc, _settings)
{
}

int AQEngineSystem::run(const char * prg, const char * args) const
{
  aq::Logger::getInstance().log(AQ_NOTICE, "call: '%s %s'\n", prg, args);
  // freopen("tmp.log", "w", stdout);
  int rc = system((std::string(prg) + " " + std::string(args) + " > log.txt").c_str());
  // freopen("CON", "w", stdout);
  return rc;
}

namespace aq {
namespace engine {

AQEngine_Intf * getAQEngineSystem(const aq::Base::Ptr base, const aq::Settings::Ptr settings)
{
  return new AQEngineSystem(base, settings);
}

}
}
