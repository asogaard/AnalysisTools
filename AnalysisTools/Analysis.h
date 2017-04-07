#ifndef AnalysisTools_Analysis_h
#define AnalysisTools_Analysis_h

/**
 * @file Analysis.h
 * @author Andreas Sogaard
 */

// STL include(s).
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <memory> /* std::unique_ptr, std::shared_ptr, std::make_shared */
/**
 * Seems like there might be a bug in std::make_shared<T>, wher the copy constructor of T is called.
 * This means that we need to use
 *   @c std::shared_ptr<T>(new T)
 * instead of
 *   @c std::make_shared<T>(other)
 * @see: http://stackoverflow.com/questions/9302296/is-make-shared-really-more-efficient-than-new
 */
#include <ctime> /* std::clock_t */
#include <iomanip> /* std::setprecision */
#include <cstdio> /* printf */

// ROOT include(s).
#include "TDirectory.h"
#include "TFile.h"

// Forward declaration(s).
namespace AnalysisTools {
    class ISelection;

    using SelectionPtr = std::unique_ptr<ISelection>;
    using SelectionPtrs = std::vector< SelectionPtr >;

}

// AnalysisTools include(s).
#include "AnalysisTools/ISelection.h"
#include "AnalysisTools/Categorised.h"

#include "AnalysisTools/PlotMacro1D.h"
#include "AnalysisTools/Range.h"

using namespace std;

namespace AnalysisTools {
    
  class Analysis : public Categorised {

  public:

    // Constructor(s).
    Analysis (const string &name) :
      Categorised(name) //Localised(name)
    {};

    // Destructor(s).
    ~Analysis () {};


  public:

    // Set method(s).
    template<typename T >
    void addSelection    (T* selection, const std::string& pattern = "");
    void addTree         (const std::string& pattern = "", const std::string& name = "outputTree");
    void setWeight       (const float* w, const std::string& pattern = "");
    void setSumWeights   (const float* w);


    // Get method(s).
    const SelectionPtrs&                        selections (const std::string& category) const;
    const std::map<std::string, SelectionPtrs>& selections () const;
    void clearSelections ();

    const std::map<std::string, std::shared_ptr<TTree> >& trees ();
    std::shared_ptr<TTree> tree (const std::string& category = "");

    std::shared_ptr<TFile> file ();
    void writeTree  (const std::string& category);
    void writeTrees ();

    // High-level management method(s).
    void openOutput (const std::string& filename);
    void setOutput  (std::shared_ptr<TFile> outfile);
    inline void setOutput (TFile* outfile) { setOutput(std::shared_ptr<TFile>(outfile)); return; }
    void closeOutput ();
    bool hasOutput   ();

    bool run (const std::string& category, const unsigned& current, const unsigned& maximum, const int& DSID);
    bool run (const std::string& category, const unsigned& current, const unsigned& maximum);
    bool run (const std::string& category);

    void save ();

    void print ();

  protected:
    void setup_ ();


  private:

    /// Data member(s).
    std::shared_ptr<TFile> m_outfile = nullptr;
    std::map<std::string, std::shared_ptr<TTree> > m_outtree;

    std::map<std::string, const float*> m_weight;
    const float* m_sum_weights = nullptr;

    std::map<std::string, SelectionPtrs> m_selections;

    std::clock_t m_start;

  };

}

#endif
