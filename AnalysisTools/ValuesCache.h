#ifndef AnalysisTools_ValuesCache_h
#define AnalysisTools_ValuesCache_h

/**
 * @file ValuesCache.h
 * @author Andreas Sogaard
 */

// STL include(s).
#include <string>
#include <vector>
#include <map>
#include <cassert>
#include <set>
#include <functional> /* std::function */

// ROOT include(s).
#include "TLorentzVector.h"

// AnalysisTools include(s).
#include "AnalysisTools/Logger.h"
#include "AnalysisTools/Event.h"
#include "AnalysisTools/PhysicsObject.h"

namespace AnalysisTools {

  /**
   * Small utility class for caching function return-values, mainly for use with Cut::apply and Operation::apply.
  */
  class ValuesCache : public Logger {
    
  public:
    
    // Set method(s).
    template<class T>
      void add (const std::string& key, const T& obj,
		const std::function< float(const T&)>& f = [](const float& var){ return (float) var; });    
    
    // Set method(s).
    inline const float& get (const std::string& key) const {
      return m_cache.at(key);
    }
    
    // High-level method(s).
    inline void clear () { 
      m_keys .clear();
      m_cache.clear();
    }
    
  private:
    
    std::set<std::string>        m_keys;
    std::map<std::string, float> m_cache;
    
  };
  
} // namespace

#endif
