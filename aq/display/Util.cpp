#include "Util.h"
#include "ColumnItem.h"
#include "ColumnMapper.h"
#include <aq/util/FileMapper.h>
#include <aq/util/Base.h>
#include <aq/util/Exceptions.h>
#include <algorithm>
#include <fstream>
#include <tuple>
#include <boost/filesystem.hpp>
#include <boost/variant.hpp>

namespace aq
{
  
  // ----------------------------------------------------------------------------
  typedef boost::variant<
    aq::ColumnItem<int32_t>::Ptr, 
    aq::ColumnItem<int64_t>::Ptr, 
    aq::ColumnItem<double>::Ptr, 
    aq::ColumnItem<char*>::Ptr 
  > item_t;
  typedef std::vector<std::pair<item_t, aq::ColumnType> > v_item_t;
  typedef boost::variant<
    aq::ColumnMapper_Intf<int32_t>::Ptr, 
    aq::ColumnMapper_Intf<int64_t>::Ptr, 
    aq::ColumnMapper_Intf<double>::Ptr, 
    aq::ColumnMapper_Intf<char>::Ptr
  > column_mapper_t;

  // ----------------------------------------------------------------------------
  template <typename T>
  bool cmp_item(const typename aq::ColumnItem<T>::Ptr& i1, const typename aq::ColumnItem<T>::Ptr& i2)
  {
    if (aq::ColumnItem<T>::lessThan(*i1, *i2))
      return true;
    else if (!aq::ColumnItem<T>::equal(*i1, *i2))
      return false;
    return false;
  }

  // ----------------------------------------------------------------------------
  struct grp_cmp
  {
    bool operator()(const v_item_t& v1, const v_item_t& v2)
    {
      assert(v1.size() == v2.size());
      for (size_t i = 0; i < v1.size(); ++i)
      {
        assert(v1[i].second == v2[i].second);
        switch (v1[i].second)
        {
        case aq::ColumnType::COL_TYPE_BIG_INT:
        case aq::ColumnType::COL_TYPE_DATE:
          return cmp_item<int64_t>(boost::get<aq::ColumnItem<int64_t>::Ptr>(v1[i].first), boost::get<aq::ColumnItem<int64_t>::Ptr>(v2[i].first));
        break;
        case aq::ColumnType::COL_TYPE_DOUBLE:
          return cmp_item<double>(boost::get<aq::ColumnItem<double>::Ptr>(v1[i].first), boost::get<aq::ColumnItem<double>::Ptr>(v2[i].first));
        break;
        case aq::ColumnType::COL_TYPE_INT:
          return cmp_item<int32_t>(boost::get<aq::ColumnItem<int32_t>::Ptr>(v1[i].first), boost::get<aq::ColumnItem<int32_t>::Ptr>(v2[i].first));
        break;
        case aq::ColumnType::COL_TYPE_VARCHAR:
          return cmp_item<char*>(boost::get<aq::ColumnItem<char*>::Ptr>(v1[i].first), boost::get<aq::ColumnItem<char*>::Ptr>(v2[i].first));
        break;
        }
      }
      return false;
    }
  };

// ------------------------------------------------------------------------------
int generate_working_directories(const struct opt& o, std::string& iniFilename)
{
  boost::filesystem::path p;
  p = boost::filesystem::path(o.dbPath + "calculus/" + o.queryIdent);
  boost::filesystem::create_directory(p);
  p = boost::filesystem::path(o.workingPath + "data_orga/tmp/" + o.queryIdent);
  if (boost::filesystem::exists(p))
  {
    if (o.force)
    {
      boost::filesystem::remove_all(p);
    }
    else
    {
      throw aq::generic_error(aq::generic_error::INVALID_FILE, std::string(p.string() + " already exist").c_str());
    }
  }
  boost::filesystem::create_directory(p);
  p = boost::filesystem::path(o.workingPath + "data_orga/tmp/" + o.queryIdent + "/dpy");
  boost::filesystem::create_directory(p);
  p = boost::filesystem::path(o.dbPath + "calculus/" + o.queryIdent);
  boost::filesystem::create_directory(p);
  
  iniFilename = o.dbPath + "calculus/" + o.queryIdent + "/aq_engine.ini";
  std::ofstream ini(iniFilename.c_str());
  ini << "export.filename.final=" << o.dbPath << "base_struct/base.aqb" << std::endl;
  ini << "step1.field.separator=;" << std::endl;
  ini << "k_rep_racine=" << o.dbPath << std::endl;
  ini << "k_rep_racine_tmp=" << o.workingPath << std::endl;
  ini.close();

  return 0;
}

// ------------------------------------------------------------------------------
int run_aq_engine(const std::string& aq_engine, const std::string& iniFilename, const std::string& ident)
{
  std::string args = iniFilename + " " + ident + " Dpy";
  int rc = system((aq_engine + " " + args).c_str());
  if (rc != 0)
  {
    aq::Logger::getInstance().log(AQ_ERROR, "error running: '%s %s'\n", aq_engine.c_str(), args.c_str());
  }
  return rc;
}

// ------------------------------------------------------------------------------
template <typename T>
void push_to_cb(char * buf, display_cb * cb, size_t index, column_mapper_t& cm)
{
  typename aq::ColumnItem<T>::Ptr item(new aq::ColumnItem<T>);
  auto m = boost::get<typename aq::ColumnMapper_Intf<T>::Ptr>(cm);
  T item_value;
  m->loadValue(index - 1, &item_value);
  item->setValue(item_value);
  item->toString(buf);
  cb->push(buf);
}

template <> inline
void push_to_cb<char*>(char * buf, display_cb * cb, size_t index, column_mapper_t& cm)
{
  aq::ColumnItem<char*>::Ptr item(new aq::ColumnItem<char*>);
  auto m = boost::get<aq::ColumnMapper_Intf<char>::Ptr>(cm);
  char * item_value = new char[128];
  m->loadValue(index - 1, item_value);
  item->setValue(item_value);
  item->toString(buf);
  cb->push(buf);
}

// ------------------------------------------------------------------------------
class print_data
{
public:
  typedef std::vector<std::tuple<size_t, aq::ColumnType, column_mapper_t> > display_order_t;

public:
  print_data(
    const struct opt& _o,
    display_cb * _cb, 
    display_order_t& _display_order) 
    : o(_o), cb(_cb), display_order(_display_order)
  {
  }
  void handle(std::vector<size_t>& rows)
  {
    cb->next();

    if (o.withIndex)
    {
      for (size_t i = 0; i < rows.size() - 1; i++)
      {
        ss.str("");
        ss << rows[i];
        cb->push(ss.str());
      }
    }

    size_t i = o.withCount ? 1 : *(rows.rbegin());
    do
    {
      for (auto& c : display_order)
      {
        auto& tindex = std::get<0>(c);
        auto& type = std::get<1>(c);
        auto& cm = std::get<2>(c);
        auto index = rows[tindex];
        if (index == 0)
        {
          cb->push("NULL");
        }
        else
        {
          switch (type)
          {
          case aq::ColumnType::COL_TYPE_BIG_INT:
          case aq::ColumnType::COL_TYPE_DATE:
            push_to_cb<int64_t>(buf, cb, index, cm);
            break;
          case aq::ColumnType::COL_TYPE_DOUBLE:
            push_to_cb<double>(buf, cb, index, cm);
            break;
          case aq::ColumnType::COL_TYPE_INT:
            push_to_cb<int32_t>(buf, cb, index, cm);
            break;
          case aq::ColumnType::COL_TYPE_VARCHAR:
            push_to_cb<char*>(buf, cb, index, cm);
            break;
          }
        }
      }
    } while (--i > 0);

    if (o.withCount)
    {
      ss.str("");
      ss << *(rows.rbegin());
      cb->push(ss.str());
    }

  }
private:
  char buf[128];
  std::stringstream ss;
  const struct opt& o;
  display_cb * cb;
  std::vector<std::tuple<size_t, aq::ColumnType, column_mapper_t> >& display_order;
};

// ------------------------------------------------------------------------------
int display(display_cb * cb,
            const aq::engine::AQMatrix::Ptr aqMatrix,
            const aq::Base::Ptr baseDesc,
            const aq::Settings::Ptr settings,
            const struct opt& o,
            const std::vector<std::string>& selectedColumns)
{
  std::stringstream ss;
  const auto& matrix = aqMatrix->getMatrix();

  // check size, print column name and prepare column mapping
  size_t size = 0;
  std::vector<std::tuple<size_t, aq::ColumnType, column_mapper_t> > display_order(selectedColumns.size());
  for (size_t tindex = 0; tindex < matrix.size(); tindex++)
  {
    auto& t = matrix[tindex];
    if (size == 0)
    {
      size = t.indexes.size();
    }
    else if (size != t.indexes.size())
    {
      std::cerr << "FATAL ERROR: indexes size of table differs" << std::endl;
      exit(-1);
    }
    
    aq::Table::Ptr table = baseDesc->getTable(t.table_id);
    for (auto& col : table->Columns)
    {
      auto it = std::find(selectedColumns.begin(), selectedColumns.end(), std::string(table->getName() + "." + col->getName()));
      if (it != selectedColumns.end())
      {
        column_mapper_t cm;
        switch(col->getType())
        {
        case aq::ColumnType::COL_TYPE_INT:
          {
            aq::ColumnMapper_Intf<int32_t>::Ptr m(new aq::ColumnMapper<int32_t, FileMapper>(settings->dataPath.c_str(), t.table_id, col->getID(), 1/*(*itCol)->Size*/, o.packetSize));
            cm = m;
          }
          break;
        case aq::ColumnType::COL_TYPE_BIG_INT:
        case aq::ColumnType::COL_TYPE_DATE:
          {
            aq::ColumnMapper_Intf<int64_t>::Ptr m(new aq::ColumnMapper<int64_t, FileMapper>(settings->dataPath.c_str(), t.table_id, col->getID(), 1/*(*itCol)->Size*/, o.packetSize));
            cm = m;
          }
          break;
        case aq::ColumnType::COL_TYPE_DOUBLE:
          {
            aq::ColumnMapper_Intf<double>::Ptr m(new aq::ColumnMapper<double, FileMapper>(settings->dataPath.c_str(), t.table_id, col->getID(), 1/*(*itCol)->Size*/, o.packetSize));
            cm = m;
          }
          break;
        case aq::ColumnType::COL_TYPE_VARCHAR:
          {
            aq::ColumnMapper_Intf<char>::Ptr m(new aq::ColumnMapper<char, FileMapper>(settings->dataPath.c_str(), t.table_id, col->getID(), col->getSize(), o.packetSize));
            cm = m;
          }
          break;
        }
        display_order[std::distance(selectedColumns.begin(), it)] = std::make_tuple(tindex, col->getType(), cm);
      }
    }
  }

  if (o.withIndex)
  {
    for (auto& t : matrix)
    {
      ss.str("");
      ss << "TABLE " << t.table_id; // FIXME : put table's name
      cb->push(ss.str());
    }
  }

  for (auto& sc : selectedColumns)
    cb->push(sc);
  if (o.withCount)
    cb->push("COUNT");
  
  // print data
  if (size == 0) // FIXME : size can be 0 if there is no result
  {
    print_data pd_cb(o, cb, display_order);
    aqMatrix->readData<print_data>(pd_cb);
    cb->next();
  }
  else
  {
    char buf[128];
    for (size_t i = 0; i < size && ((o.limit == 0) || (i < o.limit)); ++i)
    {

      cb->next();

      if (o.withIndex)
      {
        for (auto& t : matrix)
        {
          ss.str("");
          ss << t.indexes[i];
          cb->push(ss.str());
        }
      }

      for (auto& c : display_order)
      {
        auto& tindex = std::get<0>(c);
        auto& type = std::get<1>(c);
        auto& cm = std::get<2>(c);
        auto index = matrix[tindex].indexes[i];
        if (index == 0)
        {
          cb->push("NULL");
        }
        else
        {
          switch (type)
          {
          case aq::ColumnType::COL_TYPE_BIG_INT:
          case aq::ColumnType::COL_TYPE_DATE:
            push_to_cb<int64_t>(buf, cb, index, cm);
            break;
          case aq::ColumnType::COL_TYPE_DOUBLE:
            push_to_cb<double>(buf, cb, index, cm);
            break;
          case aq::ColumnType::COL_TYPE_INT:
            push_to_cb<int32_t>(buf, cb, index, cm);
            break;
          case aq::ColumnType::COL_TYPE_VARCHAR:
            push_to_cb<char*>(buf, cb, index, cm);
            break;
          }
        }
      }

      if (o.withCount)
      {
        ss.str("");
        ss << aqMatrix->getCount()[i];
        cb->push(ss.str());
      }
    }
    
    // cb->next();
  }
  return 0;
}

}
