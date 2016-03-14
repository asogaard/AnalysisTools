#ifndef AnalysisTools_ILocalised_h
#define AnalysisTools_ILocalised_h

/**
 * @file Localised.h
 * @author Andreas Sogaard
 **/

// STL include(s).
#include <vector>
#include <algorithm> /* std::find_if, std::remove_if */
#include <utility> /* pair */
#include <assert.h> /* assert */

// ROOT include(s).
#include "TDirectory.h"

// AnalysisTools include(s).
#include "AnalysisTools/Utilities.h"

using namespace std;

namespace AnalysisTools {

    class ILocalised {
        
        /**
         * Interface for wrapper class around ROOT's TDirectory, to allow for easily creating hierarchical folder structures in the output file.
         */
        
        friend class Localised;
        
    public:

        // Constructor(s).
        //virtual ILocalised () {};
        
        // Destructor(s).
        virtual ~ILocalised () {};
        
        
    public:

        // Set method(s).
        virtual void setName  (const string& name) = 0;
        virtual void addChild (ILocalised* other, const string& postfix = "") = 0;
        virtual void popChild (ILocalised* other, const string& postfix = "") = 0;
                
        // Get method(s).
        virtual string      name     () const = 0;
        virtual bool        locked   () const = 0;
        virtual bool        hasChild (ILocalised* other, const string& postfix = "") const = 0;
        virtual ILocalised* parent () const = 0;
        
        // High-level management method(s).
        virtual void grab (ILocalised* other, const string& postfix = "") = 0;
        virtual void put  (ILocalised* other, const string& postfix = "") = 0;

        
    protected:

        // Low-level management method(s).
        virtual void        setDir  (TDirectory* dir) = 0;
        virtual void        lock    () = 0;
        virtual void        lockAll () = 0;
        virtual TDirectory* parentDir () = 0;

       
    protected:

        string      m_name   = "";
        TDirectory* m_dir    = nullptr;
        
        bool        m_locked = false;
        
        ILocalised* m_parent = nullptr;
        vector< pair<ILocalised*, string> > m_children;

    };
    
}

#endif