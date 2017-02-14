#include "AnalysisTools/ValuesCache.h"

namespace AnalysisTools {

  template<class T>
  void ValuesCache::add (const std::string& key, const T& obj, const std::function< float(const T&)>& f) {
    
    // Cache the 'CutVariable', but make it overwrite on each 'add'
    if (key != "CutVariable") {
      // Skip if variable is already cached.
      auto ret = m_keys.emplace(key);
      if (!ret.second) { return; }
    }
    
    // Store variable in cache.
    m_cache.emplace(key, f(obj));
    
    return;
  }

  // Explicit instantiations.
  template void ValuesCache::add (const std::string& key, const float& obj, 
				  const std::function< float(const float&)>& f);
  template void ValuesCache::add (const std::string& key, const TLorentzVector& obj, 
				  const std::function< float(const TLorentzVector&)>& f);
  template void ValuesCache::add (const std::string& key, const PhysicsObject& obj, 
				  const std::function< float(const PhysicsObject&)>& f);
  template void ValuesCache::add (const std::string& key, const Event& obj, 
				  const std::function< float(const Event&)>& f);
  
} // namespace
