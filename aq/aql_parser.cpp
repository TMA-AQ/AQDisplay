#include <aq/util/Logger.h>
#include <aq/util/Exceptions.h>
#include <aq/util/AQLParser.h>
#include <fstream>
#include <iostream>
#include <boost/program_options.hpp>
#include <boost/algorithm/string.hpp>

namespace po = boost::program_options;

namespace helper
{
  
  enum class type_t { SQL, AQL_PREFIXED, AQL_INFIXED };

  int transform_query(std::string& query, type_t type, bool verbose)
  {
    int rc = 0;
    aq::core::SelectStatement ss;

    if (verbose)
      std::cout << query << std::endl << std::endl;

    bool ok = false;
    switch (type)
    {
    case type_t::AQL_INFIXED:
      ok = aq::parser::parse_infix(query, ss);
      break;
    case type_t::AQL_PREFIXED:
      ok = aq::parser::parse(query, ss);
      break;
    case type_t::SQL:
      // todo
      break;
    }

    if (ok)
    {
      if (verbose)
      {
        std::cout << ss.to_string(aq::core::SelectStatement::output_t::AQL) << std::endl;
        std::cout << ss.to_string(aq::core::SelectStatement::output_t::AQL_INFIX) << std::endl;
      }
      std::cout << ss.to_string(aq::core::SelectStatement::output_t::SQL) << std::endl;
    }
    else
    {
      rc = -1;
    }

    return rc;
  }

  template <class RQ>
  void transform_queries(RQ& rq, helper::type_t type, bool verbose)
  {
    std::string query;
    while (rq.next(query) != -1)
    {
      std::cout << query << std::endl;
      transform_query(query, type, verbose);
      query = "";
    }
  }

  class StringIterator
  {
  public:
    StringIterator(const std::string& _queries) : queries(_queries)
    {
    }
    int next(std::string& query)
    {
      // todo
      query = queries;
      return 0;
    }
  private:
    std::string queries;
  };

  class StreamIterator
  {
  public:
    StreamIterator(std::istream& _input) : input(_input)
    {
    }

    int next(std::string& query)
    {
      std::string line;
      do
      {
        std::getline(input, line);
        boost::trim(line);
        std::string::size_type pos = line.find("--");
        if (pos != 0)
        {
          query += line.substr(0, pos) + " \n";
        }
      } while (!input.eof() && (line.find(";") == std::string::npos));
      return input.eof() ? -1 : 0;
    }
  private:
    std::istream& input;
  };

}

int main(int argc, char ** argv)
{
  try 
  {
    std::string logMode;
    std::string query;
    std::string file;
    std::string type_str;
    unsigned int logLevel;
    bool verbose;

    //
    // command line arguments are prior to settings file
    po::options_description all("Allowed options");
    all.add_options()
      ("help,h", "produce help message")
      ("log-output", po::value<std::string>(&logMode)->default_value("STDOUT"), "[STDOUT|LOCALFILE|SYSLOG]")
      ("log-level", po::value<unsigned int>(&logLevel)->default_value(AQ_LOG_WARNING), "CRITICAL(2), ERROR(3), WARNING(4), NOTICE(5), INFO(6), DEBUG(7)")
      ("verbose,v", po::bool_switch(&verbose))
      ("query,q", po::value<std::string>(&query))
      ("file,f", po::value<std::string>(&file))
      ("type,t", po::value<std::string>(&type_str)->default_value("AQL_PREFIX"), "[SQL|AQL_INFIX|AQL_PREFIX]")
      ;

		aq::Logger::getInstance(argv[0], logMode == "STDOUT" ? STDOUT : logMode == "LOCALFILE" ? LOCALFILE : logMode == "SYSLOG" ? SYSLOG : STDOUT);
		aq::Logger::getInstance().setLevel(logLevel);
    
		po::variables_map vm;
		po::store(po::command_line_parser(argc, argv).options(all).run(), vm);
		po::notify(vm);    

		if (vm.count("help"))
		{
			std::cout << all << "\n";
			return 1;
    }

    helper::type_t type;
    boost::to_upper(type_str);
    if (type_str == "SQL")
    {
      type = helper::type_t::SQL;
    }
    else if (type_str == "AQL_INFIX")
    {
      type = helper::type_t::AQL_INFIXED;
    }
    else if (type_str == "AQL_PREFIX")
    {
      type = helper::type_t::AQL_PREFIXED;
    }
    else
    {
      throw aq::generic_error(aq::generic_error::GENERIC, "%s is not a supported language", type_str.c_str());
    }

    if (file != "")
    {
      std::ifstream ifile;
      ifile.open(file);
      if (!ifile.is_open())
      {
        std::cerr << "cannot find file " << file << std::endl;
        exit(EXIT_FAILURE);
      }
      auto iterator = helper::StreamIterator(ifile);
      helper::transform_queries<helper::StreamIterator>(iterator, type, verbose);
    }
    else if (query != "")
    {
      auto iterator = helper::StringIterator(query);
      helper::transform_queries<helper::StringIterator>(iterator, type, verbose);
    }
    else
    {
      auto iterator = helper::StreamIterator(std::cin);
      helper::transform_queries<helper::StreamIterator>(iterator, type, verbose);
    }

  }
  catch (const boost::program_options::error& er)
  {
    std::cerr << er.what() << std::endl;
  }
  catch (const aq::generic_error& er)
  {
    std::cerr << er.what() << std::endl;
  }

	return 0;
}

