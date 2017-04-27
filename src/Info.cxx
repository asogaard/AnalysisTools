#include "AnalysisTools/Info.h"

namespace AnalysisTools {
    
  /// Explicit implementations.
  template<> basicContainer_t<std::string>& BasicInfo::infoContainer_() { return m_strings; }
  template<> basicContainer_t<unsigned>&    BasicInfo::infoContainer_() { return m_unsigneds; }
  template<> basicContainer_t<double>&      BasicInfo::infoContainer_() { return m_doubles; }
  template<> basicContainer_t<float>&       BasicInfo::infoContainer_() { return m_floats; }
  template<> basicContainer_t<bool>&        BasicInfo::infoContainer_() { return m_bools;  }
  template<> basicContainer_t<int>&         BasicInfo::infoContainer_() { return m_ints; }

  template<> vectorContainer_t<std::string>& VectorInfo::infoContainer_() { return m_strings; }
  template<> vectorContainer_t<unsigned>&    VectorInfo::infoContainer_() { return m_unsigneds; }
  template<> vectorContainer_t<double>&      VectorInfo::infoContainer_() { return m_doubles; }
  template<> vectorContainer_t<float>&       VectorInfo::infoContainer_() { return m_floats; }
  template<> vectorContainer_t<bool>&        VectorInfo::infoContainer_() { return m_bools; }
  template<> vectorContainer_t<int>&         VectorInfo::infoContainer_() { return m_ints; }


} // namespace
