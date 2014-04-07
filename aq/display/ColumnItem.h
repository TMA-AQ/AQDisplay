#ifndef __AQ_COLUMN_ITEM_H__
#define __AQ_COLUMN_ITEM_H__

#include <aq/util/DBTypes.h>
#include <aq/util/Utilities.h>
#include <aq/util/Exceptions.h>
#include <string>
#include <memory>
#include <stdint.h>

namespace aq
{
  
  //------------------------------------------------------------------------------
  template <typename T>
  class ColumnItem
  {
  public:
    typedef std::shared_ptr<ColumnItem<T>> Ptr;
    
  public:
    ColumnItem();
    ColumnItem(T _value);
    ColumnItem(const ColumnItem<T>& source);
    ~ColumnItem();
    ColumnItem<T>& operator=(const ColumnItem<T>& source); 

    void setValue(const T& _value);
    const T& getValue() const { return value; }
    uint64_t getCount() const { return count; }
    void toString(char * buffer) const;
    std::string toString() const;
    void applyAggregate(aggregate_function_t aggFunc, const ColumnItem<T>& item);

    // static typename ColumnItem<T>::Ptr build(aq::ColumnType type);
    
    static bool lessThan( const ColumnItem<T>& first, const ColumnItem<T>& second )
    {
      return first.getValue() < second.getValue();
    }

    static bool equal( const ColumnItem<T>& first, const ColumnItem<T>& second )
    {
      return first.getValue() == second.getValue();
    }

  private:
    T value;
    uint64_t count;
  };
  
  //------------------------------------------------------------------------------
  template <typename T>
  ColumnItem<T>::ColumnItem()
    : count(0)
  {
  }
  
  //------------------------------------------------------------------------------
  template <typename T>
  ColumnItem<T>::ColumnItem(T _value)
    : value(_value), count(1)
  {
  }
  
  //------------------------------------------------------------------------------
  template <typename T>
  ColumnItem<T>::ColumnItem(const ColumnItem<T>& source)
  {
    this->value = source.value;
    this->count = source.count;
  }

  //------------------------------------------------------------------------------
  template <typename T>
  ColumnItem<T>::~ColumnItem()
  {
  }
  
  //------------------------------------------------------------------------------
  template <typename T>
  ColumnItem<T>& ColumnItem<T>::operator=(const ColumnItem<T>& source)
  {
    if (this != &source)
    {
      this->value = source.value;
      this->count = source.count;
    }
    return *this;
  }
  
  //------------------------------------------------------------------------------
  template <typename T>
  void ColumnItem<T>::setValue(const T& _value)
  {
    this->value = _value;
    this->count = 1;
  }

  //------------------------------------------------------------------------------
  template <typename T>
  void ColumnItem<T>::toString(char * buffer) const
  {
    int res = 0;
    res = sprintf( buffer, "%lld", (llong) this->value );
    if (res < 0)
    {
      throw generic_error(generic_error::GENERIC, "error while converting generic item to string");
    }
  }  
  
  //------------------------------------------------------------------------------
  template <> inline
  void ColumnItem<double>::toString(char * buffer) const
  {
    util::doubleToString(buffer, this->value);
  }
  
  //template <>
  //void ColumnItem<aq::Date>::toString(char * buffer) const
  //{
  //}
  
  //------------------------------------------------------------------------------
  template <typename T>
  std::string ColumnItem<T>::toString() const
  {
    std::stringstream ss;
    ss << static_cast<T>(this->value);
    return ss.str();
  }
  
  //template <>
  //std::string ColumnItem<aq::Date>::toString() const
  //{
  //}
  
  //------------------------------------------------------------------------------
  template <typename T>
  void ColumnItem<T>::applyAggregate(aggregate_function_t aggFunc, const ColumnItem<T>& item)
  {
    switch (aggFunc)
    {
    case MIN:
      this->value = std::min(this->value, item.getValue());
      break;
    case MAX:
      this->value = std::max(this->value, item.getValue());
      break;
    case SUM:
      this->value += (static_cast<T>(item.getCount()) * item.getValue());
      break;
    case AVG:
      this->value = ((static_cast<T>(this->count) * this->value) + (static_cast<T>(item.getCount()) * item.getValue())) / static_cast<T>(this->count + item.getCount());
      break;
    case COUNT:
    case NONE:
      break;
    }
    this->count += item.getCount();
  }
  
  //------------------------------------------------------------------------------
  //------------------------------------------------------------------------------
  
  //------------------------------------------------------------------------------
  template <>
  class ColumnItem<char*> 
  {
  public:
    typedef std::shared_ptr<ColumnItem<char*>> Ptr;
    
  public:
    ColumnItem()
      : value(nullptr), count(0)
    {
    }

    ColumnItem(const char * _value)
      : value(nullptr), count(1)
    {
      this->setValue(_value);
    }

    ColumnItem(const ColumnItem<char*>& source)
      : value(nullptr)
    {
      this->setValue(source.getValue());
      this->count = source.getCount();
    }

    ~ColumnItem()
    {
      if (this->value)
      {
        delete [] this->value;
      }
    }

    ColumnItem<char*>& operator=(const ColumnItem<char*>& source)
    {
      if (this != &source)
      {
        this->setValue(source.getValue());
        this->count = source.getCount();
      }
      return *this;
    }

    void setValue(const char* _value)
    {
      if (this->value)
      {
        delete this->value;
        this->value = nullptr;
      }
      if (_value)
      {
        this->value = new char[strlen(_value) + 1];
        strcpy(this->value, _value);
      }
    }

    const char * getValue() const 
    { 
      return value; 
    }
    
    uint64_t getCount() const 
    { 
      return count; 
    }

    void toString(char * buffer) const
    {
      strcpy(buffer, this->value);
    }

    std::string toString() const
    {
      return std::string(this->value);
    }

    void applyAggregate(aggregate_function_t aggFunc, const ColumnItem<char*>& item)
    {
      switch (aggFunc)
      {
      case MIN:
        if (strcmp(item.getValue(), this->value) < 0)
        {
          strcpy(this->value, item.getValue());
        }
        break;
      case MAX:
        if (strcmp(item.getValue(), this->value) > 0)
        {
          strcpy(this->value, item.getValue());
        }
        break;
      case SUM:
        throw aq::generic_error(aq::generic_error::INVALID_QUERY, "cannot apply sum on char type");
        break;
      case AVG:
        throw aq::generic_error(aq::generic_error::INVALID_QUERY, "cannot apply avg on char type");
        break;
      case COUNT:
      case NONE:
        break;
      }
      this->count += item.getCount();
    }

    static bool lessThan(const ColumnItem<char*>& first, const ColumnItem<char*>& second)
    {
      return strcmp(first.getValue(), second.getValue()) < 0;
    }

    static bool equal(const ColumnItem<char*>& first, const ColumnItem<char*>& second)
    {
      return strcmp(first.getValue(), second.getValue()) == 0;
    }

  private:
    char * value;
    uint64_t count;
  };
  
}

#endif
