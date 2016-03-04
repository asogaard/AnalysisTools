#include "AnalysisTools/Range.h"

namespace AnalysisTools {

    // Constructor(s).
    Range::Range (const pair<double, double>& limits) {
        setLimits(limits);
    }

    Range::Range (const double& down, const double& up) {
        setLimits(down, up);
    }

    
    // Set method(s).
    void Range::setLimits (const pair<double, double>& limits) {
        assert(limits.first <= limits.second);
        m_limits = limits;
        return;
    }
    
    void Range::setLimits (const double& down, const double& up) {
        assert(down <= up);
        m_limits.first  = down;
        m_limits.second = up;
        return;
    }
    
    void Range::setLowerLimit (const double& down) {
        assert(down <= m_limits.second);
        m_limits.first  = down;
        return;
    }
    
    void Range::setUpperLimit (const double& up) {
        assert(m_limits.first <= up);
        m_limits.second = up;
        return;
    }
    
    
    // Get method(s).
    pair<double, double> Range::limits () const {
        return m_limits;
    }

    double Range::lowerLimit () const {
        return m_limits.first;
    }

    double Range::upperLimit () const {
        return m_limits.second;
    }
    
    double Range::down () const {
        return lowerLimit();
    }

    double Range::up   () const {
        return upperLimit();
    }
    
    
    // High-level management method(s).
    bool Range::contains     (const double& val) const {
        return containsIncl(val);
    }

    bool Range::containsIncl (const double& val) const {
        return val >= lowerLimit() && val <= upperLimit();
    }
    
    bool Range::containsExcl (const double& val) const {
        return val >  lowerLimit() && val <  upperLimit();
    }
    
}