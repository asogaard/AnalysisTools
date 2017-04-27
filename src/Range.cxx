#include "AnalysisTools/Range.h"

namespace AnalysisTools {
  
  // Constructor(s).
  Range::Range (const std::pair<float, float>& limits) {
    setLimits(limits);
  }
  
  Range::Range (const float& down, const float& up) {
    setLimits(down, up);
  }
  
  Range::Range (const std::vector<float>& limits) {
    setLimits(limits);
  }
  
  
  // Set method(s).
  void Range::setLimits (const std::pair<float, float>& limits) {
    assert(limits.first <= limits.second);
    m_limits = limits;
    return;
  }
  
  void Range::setLimits (const float& down, const float& up) {
    assert(down <= up);
    m_limits.first  = down;
    m_limits.second = up;
    return;
  }
  
  void Range::setLimits (const std::vector<float>& limits) {
    assert(limits.size() == 2);
    assert(limits[0] <= limits[1]);
    m_limits.first  = limits[0];
    m_limits.second = limits[1];
    return;
  }
  
  void Range::setLowerLimit (const float& down) {
    assert(down <= m_limits.second);
    m_limits.first  = down;
    return;
  }
  
  void Range::setUpperLimit (const float& up) {
    assert(m_limits.first <= up);
    m_limits.second = up;
    return;
  }
  
  
  // Get method(s).
  const std::pair<float, float>& Range::limits () const {
    return m_limits;
  }
  
  const float& Range::lowerLimit () const {
    return m_limits.first;
  }
  
  const float& Range::upperLimit () const {
    return m_limits.second;
  }
  
  const float& Range::down () const {
    return lowerLimit();
  }
  
  const float& Range::up   () const {
    return upperLimit();
  }
  
  
  // High-level management method(s).
  bool Range::contains     (const float& val) const {
    return containsIncl(val);
  }
  
  bool Range::containsIncl (const float& val) const {
    return val >= lowerLimit() && val <= upperLimit();
  }
  
  bool Range::containsExcl (const float& val) const {
    return val >  lowerLimit() && val <  upperLimit();
  }

  bool Range::contains (const Range& other) const {
    return contains(other.down()) && contains(other.up());
  }

  bool Range::overlaps (const Range& other) const {
    return contains(other) || other.contains(*this) || contains(other.down()) || contains(other.up());
  }

  
}
