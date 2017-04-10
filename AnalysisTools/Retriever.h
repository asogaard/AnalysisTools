#ifndef AnalysisTools_Retriever_h
#define AnalysisTools_Retriever_h

/**
 * @file Retriever.h
 * @author Andreas Sogaard
 */

// STL include(s).
#include <string>
#include <vector>
#include <map>
#include <memory> /* std::unique_ptr */
#include <functional> /* std::function */
#include <unordered_map> 

// ROOT include(s).
#include "TTreeFormula.h"

// AnalysisTools include(s).
#include "AnalysisTools/Utilities.h"
#include "AnalysisTools/Logger.h"

namespace AnalysisTools {

  /**
   * Base class for retrieving AnalysisTools objects, constructuted from ROOT TTree branches.
   */
  template<class T>
  class Retriever : public Logger {

  public:

    /// Constructor(s)
    ~Retriever () {
      if (!m_retrieved) {
	WARNING("Method 'retrieve' was never called.");
      }
    };

 
  public:
    
    /// Set method(s).
    // Add auxiliary information from TTree branches.
    void addInfo (const std::vector<std::string>& branches, const std::string& prefix = "");

    // Add auxiliary information using function on object itself.
    void addInfo (const std::string& name, const std::function< float(const T&) >& f);

    // Set address of ROOT TTree from which to read information.
    void setTree (TTree* tree);
    
    /// Get method(s).
    // ...
    
    /// High-level method(s).
    // Print (@TEMP)
    void print ();

    // Clear stored object(s).
    void clear ();

    // Rename auxiliary information.
    void rename (const std::string& name1, const std::string& name2);

    // Return retrieved content;
    //virtual T* result () = 0; // Can't do this, due to possible std::vector< ... >

    // Retrieve content from TTree. Must be called each event.
    void retrieve ();


  protected:

    /// Low-level method(s)
    // ...
    void addBranches_ (const std::vector<std::string>& branches, const std::string& prefix = "");

    // Initialise retrieved but setting up connections to target TTree.
    void initialise_ ();

    // Template-parameter specific method for clearing the container for the data being retrieved. E.g.
    //   T = Event -> m_event.clear
    //   T = PhysicsObject -> m_collection.clear()
    // Must be specified in the derived class.
    virtual void clearCache_ () = 0;

    // Template-parameter specific method for filling the container for the data being retrieved.
    virtual void fillCache_ () = 0;


  protected:
    
    /// Data member(s)
    // Whether object is properly initialised.
    bool m_initialised = false;

    // Whether any content was retrieved.
    bool m_retrieved = false;

    // ROOT TTree from which to read data.
    TTree* m_tree = nullptr;

    // ROOT TTree branches from which to read data.
    std::vector<std::string> m_branches;

    // Functions from which to construct additional auxiliary information.
    std::map<std::string, std::function< float(const T&) > > m_infoFunctions;

    // TTreeFormulas for reading heterogenous data from TTrees 
    std::vector< std::unique_ptr<TTreeFormula> > m_formulas;

    // Map for renaming branches.
    std::map<std::string, std::string> m_rename;

    // Fast lookup without having to do renaming for each retrieval.
    std::unordered_map<std::string, std::string> m_branch_to_name;

  };
  
} // namespace

#endif
