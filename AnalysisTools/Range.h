#ifndef AnalysisTools_Range_h
#define AnalysisTools_Range_h

/**
 * @file Range.h
 * @author Andreas Sogaard
 */

// STL include(s).
#include <vector>
#include <utility> /* std::pair */
#include <cassert> /* assert */

// ROOT include(s).
// ...

// AnalysisTools include(s).
#include "AnalysisTools/Utilities.h"

namespace AnalysisTools {

    class Range {
        
    public:

        // Constructor(s).
        Range () {};
        Range (const std::pair<float, float>& limits);
        Range (const float& down, const float& up);
        
        // Destructor(s).
        ~Range () {};
        
        
    public:
        
        // Set method(s).
        void setLimits (const std::pair<float, float>& limits);
        void setLimits (const float& down, const float& up);

        void setLowerLimit (const float& down);
        void setUpperLimit (const float& up);

        
        // Get method(s).
        const std::pair<float, float>& limits () const;
        
        const float& lowerLimit () const;
        const float& upperLimit () const;

        const float& down () const;
        const float& up   () const;
        
        
        // High-level management method(s).
	bool contains     (const float& val) const;
        bool containsIncl (const float& val) const;
        bool containsExcl (const float& val) const;
        
        
    private:
        
	std::pair<float, float> m_limits = std::pair<float, float>(-inf, inf);
        
    };

    using Ranges = std::vector<Range>;
    
}

#endif
