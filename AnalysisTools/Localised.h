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
        Localised () {
            cout << "<Localised::Localised> Entering (default)." << endl;
            cout << "<Localised::Localised> Exiting." << endl;
        };
        Localised (const string& name) {
            cout << "<Localised::Localised> Entering (name)." << endl;
            cout << "<Localised::Localised>   Name: '" << name << "'" << endl;
            setName(name);
            cout << "<Localised::Localised> Exiting." << endl;
        };
        Localised (const string& name, TDirectory* dir) {
            cout << "<Localised::Localised> Entering (name + dir)." << endl;
            cout << "<Localised::Localised>   Name: '" << name << "'" << endl;
            if (dir != nullptr) {
                cout << "<Localised::Localised>   Dir:  '" << dir->GetName() << "'" << endl;
            } else {
                cout << "<Localised::Localised>   Dir:  'null'" << endl;
            }
            setName(name);
            setDir(dir);
            cout << "<Localised::Localised> Exiting." << endl;
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

        /*
    protected:
        
        string      m_name   = "";
        TDirectory* m_dir    = nullptr;
        
        bool        m_locked = false;
        
        vector< pair<ILocalised*, string> > m_children;
*/
        
    };
    
}

#endif