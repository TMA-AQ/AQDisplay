#include "QueryReader.h"

#include <iostream>
#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>
#include <boost/algorithm/string.hpp>

using namespace aq;

std::string QueryReader::next()
{
  // reset ident
  ident = "";
  expected = "";

  //
  // load sql queries
  std::string line;
  std::string query;
  bool fullQuery = false;
  do
  {

    if (this->prompt != "")
    {
      if (query == "")
      {
        std::cout << this->prompt << "> ";
      }
      else
      {
        for (size_t i = 0; i < this->prompt.size() - 1; i++)
          std::cout << " ";
        std::cout << "-> ";
      }
    }

    if (!std::getline(this->queriesStream, line))
    {
      break;
    }
    boost::algorithm::trim(line);
    std::string::size_type pos = line.find("--");
    if (pos != std::string::npos)
    {
      QueryReader::extract(line, "ident", ident);
      QueryReader::extract(line, "suite", suite);
      QueryReader::extract(line, "expected", expected);
      line.erase(pos);
    }
    if ((line == ""))
      continue;
    pos = line.find(";");
    if (pos != std::string::npos)
    {
      query += " " + line.substr(0, pos + 1) + "\n";
      fullQuery = true;
    }
    else
    {
      query += " " + line + "\n";
    }
  }
  while (!fullQuery);

  if (ident == "")
  {
    std::ostringstream identSS;
    identSS << "query " << n;
    ident = identSS.str();
  }
  
  boost::trim(query);
  std::string s = query.substr(0, 4);
  boost::to_upper(s);
  if (s == "QUIT")
    query = "";
  else
    n++;
  return query;
}

void QueryReader::extract(const std::string& line, const std::string& key, std::string& result)
{
  std::string::size_type b = line.find("<" + key + ">");
  std::string::size_type e = line.find("</" + key + ">");
  if ((b != std::string::npos) && (e != std::string::npos) && (b < e))
  {
    result = line.substr(b + key.size() + 2, e - (b + key.size() + 2));
  }
}
