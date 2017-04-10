#ifndef AnalysisTools_CollectionRetriever_h
#define AnalysisTools_CollectionRetriever_h

/**
 * @file CollectionRetriever.h
 * @author Andreas Sogaard
 */

// STL include(s).
#include <string>
#include <vector>
#include <map>
#include <memory> /* std::unique_ptr */
#include <functional> /* std::function */

// ROOT include(s).
#include "TLorentzVector.h"
#include "TTreeFormula.h"

// AnalysisTools include(s).
#include "AnalysisTools/Logger.h"
#include "AnalysisTools/Retriever.h"
#include "AnalysisTools/PhysicsObject.h"

namespace AnalysisTools {

  /**
   * Utility structs for construction the collection from various types of input.
   */
  enum class RetrieverMode { PxPyPzE, PtEtaPhiE, PtEtaPhiM, TLorentzVector, NA };

  struct FromPxPyPzE {
    inline FromPxPyPzE(const std::string& px = "px",
		       const std::string& py = "py", 
		       const std::string& pz = "pz", 
		       const std::string& e  = "e") :
      branches({px, py, pz, e}) {}
    std::vector<std::string> branches;
  };

  struct FromPtEtaPhiE {
    inline FromPtEtaPhiE(const std::string& pt  = "pt",   
			 const std::string& eta = "eta", 
			 const std::string& phi = "phi", 
			 const std::string& e   = "e") :
      branches({pt, eta, phi, e}) {}
    std::vector<std::string> branches;
  };

  struct FromPtEtaPhiM {
    inline FromPtEtaPhiM(const std::string& pt  = "pt",   
			 const std::string& eta = "eta", 
			 const std::string& phi = "phi", 
			 const std::string& m   = "m") :
      branches({pt, eta, phi, m}) {}
    std::vector<std::string> branches;
  };

  struct FromTLorentzVector {
    inline FromTLorentzVector(const std::string& tlv) : 
      branches({tlv}) {}
    std::vector<std::string> branches;
  };


  /**
   * CollectionRetriever of PhysicsObjects, constructuted from TTree branches.
   */
  class CollectionRetriever : public Retriever<PhysicsObject> {

  public:

    /// Constructor(s)
    CollectionRetriever (const FromPxPyPzE& st, const std::string& prefix = "") :
    CollectionRetriever(RetrieverMode::PxPyPzE, st.branches, prefix) {};

    CollectionRetriever (const FromPtEtaPhiE& st, const std::string& prefix = "") :
    CollectionRetriever(RetrieverMode::PtEtaPhiE, st.branches, prefix) {};

    CollectionRetriever (const FromPtEtaPhiM& st, const std::string& prefix = "") :
    CollectionRetriever(RetrieverMode::PtEtaPhiM, st.branches, prefix) {};
    
    CollectionRetriever (const FromTLorentzVector& st, const std::string& prefix = "") :
    CollectionRetriever(RetrieverMode::TLorentzVector, st.branches, prefix) {};
    
    CollectionRetriever (const RetrieverMode& mode, const std::vector<std::string>& branches, const std::string& prefix) :
    m_mode(mode)
    {
      addBranches_(branches, prefix);
    };
    
 
  public:
    
    /// High-level method(s).
    // Return (masked?) PhysicsObejct content of this collection
    std::vector<PhysicsObject>* result ();


  private:

    /// Low-level method(s)
    virtual void clearCache_ ();
    virtual void fillCache_  ();


  private:
    
    /// Data member(s)
    // RetrieverMode
    RetrieverMode m_mode = RetrieverMode::NA;

    // Stored cache collection of PhysicsObjects
    std::vector<PhysicsObject> m_collection;
        
  };
  
} // namespace

#endif
