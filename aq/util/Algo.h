#ifndef __AQ_CORE_ALGO_H__
#define __AQ_CORE_ALGO_H__

#include <algorithm>

namespace aq {
  namespace core {

    template <class C>
    void unique(C& c)
    {
      if (c.empty()) return;
      for (auto it = c.begin(); (it != c.end()) && (it + 1 != c.end()); ++it)
      {
        bool find = false;
        auto& cond1 = *it;
        typename std::vector<typename C::value_type>::iterator it_to_remove;
        do
        {
          find = false;
          it_to_remove = std::find_if(it + 1, c.end(), [&] (const typename C::value_type& cond2) { 
            return (cond1 == cond2);
          });
          if (it_to_remove != c.end())
          {
            c.erase(it_to_remove);
            find = true;
          }
        } while (find);
      }
    }

  }
}

#endif
