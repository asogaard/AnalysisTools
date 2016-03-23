#ifndef AnalysisTools_Range_h
#define AnalysisTools_Range_h

/**
 * @file Range.h
 * @author Andreas Sogaard
 **/

// STL include(s).
#include <vector>
#include <limits> /* std::numeric_limits<double>::infinity */
#include <utility> /* std::pair */
#include <assert.h> /* assert */

// ROOT include(s).
// ...

// AnalysisTools include(s).
#include "AnalysisTools/Utilities.h"

using namespace std;

namespace AnalysisTools {

    class Range {
        
    public:

        // Constructor(s).
        Range () {};
        Range (const pair<double, double>& limits);
        Range (const double& down, const double& up);
        /*
        Range (const AnalysisTools::Range& other) :
            m_limits(other.m_limits)
        {};
         */
        
        // Destructor(s).
        ~Range () {};
        
        
    public:
        
        // Set method(s).
        void setLimits (const pair<double, double>& limits);
        void setLimits (const double& down, const double& up);

        void setLowerLimit (const double& down);
        void setUpperLimit (const double& up);

        
        // Get method(s).
        pair<double, double> limits () const;
        
        double lowerLimit () const;
        double upperLimit () const;

        double down () const;
        double up   () const;
        
        
        // High-level management method(s).
        bool contains     (const double& val) const;
        bool containsIncl (const double& val) const;
        bool containsExcl (const double& val) const;
        
        
    private:
        
        pair<double, double> m_limits = pair<double, double>(-inf, inf);
        
    };

    using Ranges = vector<Range>;
    
}

#endif