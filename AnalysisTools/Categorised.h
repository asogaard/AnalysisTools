#ifndef AnalysisTools_Categorised_h
#define AnalysisTools_Categorised_h

/**
 * @file Categorised.h
 * @author Andreas Sogaard
 */

// STL include(s).
#include <string>
#include <vector>
#include <map>
#include <algorithm> /* std::find */
#include <cassert> /* assert */
#include <regex> /* std::regex, std::smatch, std::regex_match */

// AnalysisTools include(s).
#include "AnalysisTools/Localised.h"

namespace AnalysisTools {

  /**
   * Small class, extending Localised, allowing for the creation of child categories.
   */
  class Categorised : public Localised {
    
  public:
        
    /// Constructor(s).
    Categorised (const std::string &name) :
      Localised(name)
    {};
    
    
  public:
    
    /// Set method(s).
    void addCategory   (const std::string& category);
    void addCategories (const std::vector<std::string>& categories);
    void setCategories (const std::vector<std::string>& categories);
    
    void clearCategories ();
    void lockCategories  ();
    
    /// Get method(s).
    const std::vector<std::string>& categories ();
    std::vector<std::string>        categories (const std::string& pattern);
    unsigned numCategories ();

    bool categoriesLocked () const;
    bool canAddCategories () const;
    bool hasCategory      (const std::string& category) const;
    

  protected:
    
    /// Optional methods for derived classes
    virtual inline void addCategory_ (const std::string& category) {};
    virtual inline void clearCategories_ () {};

    
  private:
    
    /// Data member(s).
    /**
     * List of categories under this object. Will automatically assign a "Nominal" category if none are provided.
     *
     * Mutable property allows us to assign a "Nominal" category during an otherwise 'const' function call.
     */
    mutable std::vector<std::string> m_categories; 
    bool m_categoriesLocked = false;
    
  };
  
} // namespace

#endif
