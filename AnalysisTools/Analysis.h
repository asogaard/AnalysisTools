#ifndef AnalysisTools_Analysis_h
#define AnalysisTools_Analysis_h

/**
 * @file Analysis.h
 * @author Andreas Sogaard
 **/

// STL include(s).
#include <iostream>
#include <string>
#include <vector>
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

// xAOD include(s)
// ...

// ROOT include(s).
#include "TDirectory.h"
#include "TFile.h"

// Forward declaration(s).
namespace AnalysisTools {
    class ISelection;

    using SelectionPtrs = std::vector< std::unique_ptr<ISelection> >;
    //using SelectionPtrs = std::vector< ISelection* >;

}

// AnalysisTools include(s).
#include "AnalysisTools/ISelection.h"
#include "AnalysisTools/Localised.h"

#include "AnalysisTools/PlotMacro1D.h"
#include "AnalysisTools/Range.h"

using namespace std;

namespace AnalysisTools {
    
    class Analysis : public Localised {
        
    public:
        
        // Constructor(s).
        Analysis (const string &name) :
            Localised(name)
	{};
        
        // Destructor(s).
	~Analysis () {};
        
        
    public:
 
        // Set method(s).
	template<typename T >
	void addSelection    (T* selection); // ISelection*
        void addTree         (const string& name = "outputTree");
        void setWeight       (const float* w);
        
        
        // Get method(s).
	inline const SelectionPtrs& selections () const { return m_selections; }
	void   clearSelections ();
	std::shared_ptr<TTree> tree ();
	std::shared_ptr<TFile> file ();
        void   writeTree ();
        
        // High-level management method(s).
        void openOutput  (const string& filename);
               void setOutput (std::shared_ptr<TFile> outfile);
        inline void setOutput (TFile* outfile) { setOutput(std::shared_ptr<TFile>(outfile)); return; }
        void closeOutput ();
        bool hasOutput   ();
        
        bool run (const unsigned& current, const unsigned& maximum, const int& DSID);
        bool run (const unsigned& current, const unsigned& maximum);
        bool run ();
        
        void save ();

	void print () const;
        
    protected:
	void setup_ ();

        
    private:

	std::shared_ptr<TFile> m_outfile = nullptr;
	std::shared_ptr<TTree> m_outtree = nullptr;

        const float* m_weight = nullptr;
        
        SelectionPtrs m_selections;
        
        std::clock_t m_start;
        
    };

}

#endif
