#ifndef AnalysisTools_Localised_h
#define AnalysisTools_Localised_h

/**
 * @file Localised.h
 * @author Andreas Sogaard
 **/

// STL include(s).
#include <vector>
#include <iostream> /* std::cout */
#include <algorithm> /* std::find_if, std::remove_if */
#include <utility> /* pair */
#include <assert.h> /* assert */

// ROOT include(s).
#include "TDirectory.h"

// AnalysisTools include(s).
#include "AnalysisTools/Utilities.h"
#include "AnalysisTools/ILocalised.h"

using namespace std;

namespace AnalysisTools {

    class Localised : public virtual ILocalised {
        
        /**
         * Wrapper class around ROOT's TDirectory, to allow for easily creating hierarchical folder structures in the output file.
         */
        
    public:

        // Constructor(s).
        Localised () {};
        Localised (const string& name) {
            setName(name);
        };
        Localised (const string& name, TDirectory* dir) {
            setName(name);
            setDir(dir);
        };

        
        // Destructor(s).
        ~Localised () {};
        
        
    public:
        
        // Set method(s).
        void setName  (const string& name);
        void addChild (ILocalised* other, const string& postfix = "");
        void popChild (ILocalised* other, const string& postfix = "");
                
        // Get method(s).
        string      name     () const;
        TDirectory* dir      () const;
        bool        locked   () const;
        bool        hasChild (ILocalised* other, const string& postfix = "") const;
        ILocalised* parent   () const;
        
        // High-level management method(s).
        void grab (ILocalised* other, const string& postfix = "");
        void put  (ILocalised* other, const string& postfix = "");
        
        
    protected:
        
        // Low-level management method(s).
        void        setDir  (TDirectory* dir);
        void        lock    ();
        void        lockAll ();
        TDirectory* parentDir ();

    };
    
}

#endif