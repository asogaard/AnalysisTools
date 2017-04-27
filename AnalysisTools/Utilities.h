#ifndef AnalysisTools_Utilities_h
#define AnalysisTools_Utilities_h

/**
 * @file Utilities.h
 * @author Andreas Sogaard
 */

// STL include(s).
#include <string>
#include <vector>
#include <fstream> /* std::ifstream */
#include <limits> /* std::numeric_limits<float>::infinity */
#include <algorithm> /* std::find, std::replace */
#include <iterator> /* std::distance */
#include <sstream> /* std::sstream */
#include <memory> /* std::unique_ptr */
#include <utility> /* std::move */
#include <sys/stat.h> /* struct stat */
#include <exception> /* std::exception */

// ROOT include(s).
#include "TLorentzVector.h"
#include "TFile.h"
#include "TTree.h"
#include "TBranch.h"
#include "TAxis.h"

// AnalysisTools include(s).
#include "AnalysisTools/Logger.h"

namespace AnalysisTools {

  // Global utility constants.
  const float inf =    std::numeric_limits<float>::infinity();
  const float eps = 1./std::numeric_limits<float>::max();
  const float pi  = 3.14159265359;

  // Square number
  template<class T>
  inline T sq (const T& x) { return x * x; }

  // Check whether vector containes specified value.
  template<class T>
  inline bool contains (const std::vector<T>& array, const T& value) {
    return std::find(array.begin(), array.end(), value) != array.end();
  }
  
  // Check whether file exists.
  inline bool fileExists (const std::string& filename) {
    std::ifstream f(filename.c_str());
    bool exists = f.good();
    f.close();
    return exists;
  }
  
  // Check whether directory exists.
  inline bool dirExists (const std::string& dir) {
    struct stat statbuf;
    bool exists = false;
    if (stat(dir.c_str(), &statbuf) != -1) {
      if (S_ISDIR(statbuf.st_mode)) { exists = true; }
    }
    return exists;
  }
  
  // Match element to other in vector, and return index of other.
  template <class T>
    inline int getMatchIndex (const T& p, std::vector<T>* vec) {
    typename std::vector<T>::iterator it = std::find(vec->begin(), vec->end(), p);
    int idx = std::distance(vec->begin(), it);
    if (idx == vec->size()) { idx = -1; }
    return idx;
  }
  
  // Check whether a string ends with another string.
  inline bool endsWith (std::string const &full, std::string const &ending) {
    if (full.length() >= ending.length()) {
      return (0 == full.compare (full.length() - ending.length(), ending.length(), ending));
    }
    return false;
  }
  
  // Trim string from start.
  static inline std::string& ltrim(std::string &s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(),
				    std::not1(std::ptr_fun<int, int>(std::isspace))));
    return s;
  }
  
  // Trim string from end.
  static inline std::string& rtrim(std::string &s) {
    s.erase(std::find_if(s.rbegin(), s.rend(),
			 std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
    return s;
  }
  
  // Trim string from both ends.
  static inline std::string& trim(std::string &s) {
    return ltrim(rtrim(s));
  }
  
  // Split a string by delimeter.
  inline std::vector<std::string>& split (const std::string& s, char delim, std::vector<std::string>& elems) {
    std::stringstream ss(s);
    std::string item;
    while (getline(ss, item, delim)) {
      if (item == "") { continue; }
      elems.push_back(item);
    }
    return elems;
  }
  
  inline std::vector<std::string> split (const std::string& s, char delim) {
    std::vector<std::string> elems;
    split(s, delim, elems);
    return elems;
  }
  
  // Replace all instanace of 'from'-string with 'to'-string.
  inline std::string replaceAll(std::string str, const std::string& from, const std::string& to) {
    unsigned start_pos = 0;
    while ((start_pos = str.find(from, start_pos)) < str.length()) {
      str.replace(start_pos, from.length(), to);
      start_pos += to.length(); // Handles case where 'to' is a substring of 'from'
    }
    return str;
  }
  
  // Format a floating point number to a string.
  inline std::string formatNumber (const float&    f,			\
				   const unsigned& precision = 2,	\
				   const unsigned& leadingPlaces = 8) {
    const unsigned l = 100;
    char buffer [l];
    int cx = snprintf ( buffer, l, "%*.*f",				\
			leadingPlaces,					\
			(f == 0 ? 0 : std::max(int(precision) - 1 - int(log10(f/2. + eps)), 0)), \
			f );
    std::string s = buffer;
    return s;
  }
  
  // Utility function for creating and moving unique pointer.
  template<typename T>
    inline  std::unique_ptr<T> makeUniqueMove (T* p) {
    return std::move(std::unique_ptr<T>(p));
  }
  
  // Add datasets to provided, existing list.
  inline void addDatasets (const std::string& path, std::vector< std::string >& datasets) {
    if (!fileExists(path)) { 
      FCTWARNING("No file exists at path '%s'.", path.c_str());
      return; 
    }
    
    if (endsWith(path, ".root")) {
      datasets.push_back(path);
    } else if (endsWith(path, ".txt") || endsWith(path, ".list")) {
      // Assuming the path is to a file containing a list of datasets.
      std::ifstream file ( path.c_str() );
      if (!file.is_open()) {
	FCTWARNING("Input file list '%s' not found.", path.c_str());
	return;
      }
      std::string filename = "";
      while (file.good()) {
	std::getline ( file, filename );
	if (filename == "") { continue; }
	addDatasets(filename, datasets);
      }
      file.close();
    } else {
      FCTWARNING("File format of path '%s' was not recognised.", path.c_str());
    }
    return;
  }
  
  // Return list of input datasets based on commandline arguments.    
  inline std::vector< std::string > getDatasetsFromCommandlineArguments(int argc, char* argv[]) {
    
    // Initialise output variable.
    std::vector< std::string > datasets;
    
    for (unsigned i = 1; i < argc; i++) {
      addDatasets(std::string(argv[i]), datasets);
    }
    
    // Return.
    return datasets;
  }
  
  // Return pointer to variable into which the requested branch is streamed.
  template<class T>
    inline std::unique_ptr<T> readBranch ( TTree* tree, const std::string& branchName) {
    std::unique_ptr<T> output (new T);
    tree->SetBranchAddress(branchName.c_str(), output.get());
    return std::move(output);
  }
  
  template<class T>
    inline std::unique_ptr< std::vector<T> > readBranchVector ( TTree* tree, const std::string& branchName) {
    //inline std::vector<T>* readBranchVector ( TTree* tree, const std::string& branchName) {
    /* @TODO: Move to unique_ptr? */
    /* -- * /
    std::vector<T>* output = nullptr;
    tree->SetBranchAddress(branchName.c_str(), &output);
    /* -- */
    /* -- */

    std::vector<T>* outputv = nullptr;
    tree->SetBranchAddress(branchName.c_str(), &outputv);
    std::unique_ptr< std::vector<T> > output (outputv);
    /* -- */
    return output;
  }
  
  // Return a pointer to a TTree in a ROOT file. Throw exception if it cannot be accessed.
  TTree* retrieveTree (const std::string& name, TFile* file);

  // Return a pointer to a ROOT histogram type in a ROOT file. Throw exception is it cannot be accessed.
  // Note: Since it's templated, it needs to be declared in header.
  template<class T>
  inline T* retrieveHist (const std::string& name, TFile* file) {
    T* hist = (T*) file->Get(name.c_str());
    if (!hist) {
      FCTWARNING("Histogram '%s' could not be retrieved from file '%s'.", name.c_str(), file->GetName());
      throw std::exception();
    }
    return hist;
  }
 
  // Return a vector of TLorentzVectors based on vectors of (pt, eta, phi, m) components.
  inline std::vector< TLorentzVector > createFourVectorsM (std::vector<float>* vec_pt,
							   std::vector<float>* vec_eta,
							   std::vector<float>* vec_phi,
							   std::vector<float>* vec_m) {
    // Initialise size of vectors.
    unsigned N = vec_pt->size();
    
    // Checks.
    if (vec_eta->size() != N) {
      FCTWARNING("Number of elements in pT (%d) and eta (%d) vectors don't agree.", N, vec_eta->size());
      return {};
    }
    if (vec_phi->size() != N) {
      FCTWARNING("Number of elements in pT (%d) and phi (%d) vectors don't agree.", N, vec_phi->size());
      return {};
    }
    if (vec_m->size() != N) {
      FCTWARNING("Number of elements in pT (%d) and m (%d) vectors don't agree.", N, vec_m->size());
      return {};
    } 
    
    // Initialise output vector.
    std::vector< TLorentzVector > output (N);
    
    // Fill output vector.
    for (unsigned i = 0; i < N; i++) {
      output[i].SetPtEtaPhiM( vec_pt ->at(i),
			      vec_eta->at(i),
			      vec_phi->at(i),
			      vec_m  ->at(i));
    }
    
    return output;
  }
  
  // Return a vector of TLorentzVectors based on vectors of (pt, eta, phi, e) components.
  inline std::vector< TLorentzVector > createFourVectorsE (std::vector<float>* vec_pt,
							   std::vector<float>* vec_eta,
							   std::vector<float>* vec_phi,
							   std::vector<float>* vec_e) {
    // Initialise size of vectors.
    unsigned N = vec_pt->size();
    
    // Checks.
    if (vec_eta->size() != N) {
      FCTWARNING("Number of elements in pT (%d) and eta (%d) vectors don't agree.", N, vec_eta->size());
      return {};
    }
    if (vec_phi->size() != N) {
      FCTWARNING("Number of elements in pT (%d) and phi (%d) vectors don't agree.", N, vec_phi->size());
      return {};
    }
    if (vec_e->size() != N) {
      FCTWARNING("Number of elements in pT (%d) and e (%d) vectors don't agree.", N, vec_e->size());
      return {};
    } 
    
    // Initialise output vector.
    std::vector< TLorentzVector > output (N);
    
    // Fill output vector.
    for (unsigned i = 0; i < N; i++) {
      output[i].SetPtEtaPhiE( vec_pt ->at(i),
			      vec_eta->at(i),
			      vec_phi->at(i),
			      vec_e  ->at(i));

    }
    
    return output;
  }

  // Snap a coordinate to the nearest bin separation
  enum class SnapDirection { Nearest, Up, Down, Middle };

  inline float snapToAxis (const float& x, const TAxis* axis, const SnapDirection& dir = SnapDirection::Nearest) {

    const float fallback = -inf;

    // Check(s)
    if (axis == nullptr) {
      FCTWARNING("Null pointer provided for axis.");
      return fallback;
    }

    const unsigned bin = axis->FindBin(x);
    
    if (bin <= 0) {
      FCTWARNING("Requested coordinate not within axis bounds");
      return axis->GetXmin();
    }
    if (bin > axis->GetNbins()) {
      FCTWARNING("Requested coordinate not within axis bounds");
      return axis->GetXmax();
    }
    
    // Compute snapped coordinate based on requested direction
    float down   = axis->GetBinLowEdge(bin);
    float up     = axis->GetBinUpEdge (bin);
    float middle = axis->GetBinCenter (bin);
    switch (dir) {
    case SnapDirection::Nearest:
      {
	float d1 = std::abs(x - down);
	float d2 = std::abs(x - up);
	if (d1 == d2) {
	  FCTINFO("Coordinate exactly in the middle of bin. Returning lower edge.");
	}
	if (d1 <= d2) { return down; } 
	else          { return up; }
      }
      break;
    case SnapDirection::Up:
      return down;

    case SnapDirection::Down:
      return up;

    case SnapDirection::Middle:
      return middle;

    default:
      FCTWARNING("Snap direction no recognised");
      break;
    }
    return fallback;
  }
  
}

#endif
