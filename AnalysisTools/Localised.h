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
#include "AnalysisTools/Logger.h"
#include "AnalysisTools/ILocalised.h"

using namespace std;

namespace AnalysisTools {
  
  /**
   * Wrapper class around ROOT's TDirectory, to allow for easily creating hierarchical folder structures in the output file.
   */
  class Localised : public Logger, virtual public ILocalised {
    
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
    
    Localised (const Localised& other) {
        this->m_name     = other.m_name;
	this->m_children = other.m_children;// {};
	this->m_dir      = other.m_dir; //nullptr;
	this->m_parent   = other.m_parent; //nullptr;
	this->m_debug    = other.m_debug;
	this->m_verbose  = other.m_verbose;
    };
   
    // Destructor(s).
    ~Localised () {};
    
    
  public:
    
    // Set method(s).
    void setName     (const string& name);
    void prependName (const string& prefix);
    void addChild    (ILocalised* other, const string& postfix = "");
    void popChild    (ILocalised* other, const string& postfix = "");
    
    // Get method(s).
    string      name     () const;
    TDirectory* dir      () const;
    bool        locked   () const;
    bool        hasChild (ILocalised* other, const string& postfix = "") const;
    ILocalised* parent   () const;
    vector< pair< ILocalised*, string> > children () const;
    
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
