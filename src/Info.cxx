#include "AnalysisTools/Info.h"

namespace AnalysisTools {
    
  /// Explicit implementations.
  template<> std::map< std::string, const double* >& BasicInfo::container_() { return m_doubles; }
  template<> std::map< std::string, const float* >&  BasicInfo::container_() { return m_floats; }
  template<> std::map< std::string, const bool* >&   BasicInfo::container_() { return m_bools; }
  template<> std::map< std::string, const int* >&    BasicInfo::container_() { return m_ints; }

  template<> std::map< std::string, const std::vector<double>* >& VectorInfo::container_() { return m_doubles; }
  template<> std::map< std::string, const std::vector<float>* >&  VectorInfo::container_() { return m_floats; }
  template<> std::map< std::string, const std::vector<bool>* >&   VectorInfo::container_() { return m_bools; }
  template<> std::map< std::string, const std::vector<int>* >&    VectorInfo::container_() { return m_ints; }


} // namespace
