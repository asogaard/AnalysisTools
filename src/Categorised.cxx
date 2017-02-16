#include "AnalysisTools/Categorised.h"

namespace AnalysisTools {
    
  /// Set method(s).
  void Categorised::addCategory (const std::string& category) {
    assert( canAddCategories() );
    m_categories.emplace_back(category);
    addCategory_(category); // To allow dervied classes to implement additional functionality.
    return;
  }
  
  void Categorised::addCategories (const std::vector<std::string>& categories) {
    for (const std::string& category : categories) {
      addCategory(category);
    }
    return;
  }
  
  void Categorised::setCategories (const std::vector<std::string>& categories) {
    clearCategories();
    addCategories(categories);
    return;
  }
  
  void Categorised::clearCategories () {
    m_categories.clear();
    clearCategories_(); // To allow dervied classes to implement additional functionality.
    return;
  }

  void Categorised::lockCategories () {
    m_categoriesLocked = true;
    return;
  }
    
  
  /// Get method(s).
  const std::vector<std::string>& Categorised::categories () {
    if (numCategories() == 0) {
      assert( !locked() );
      addCategory("Nominal");
    }
    return m_categories;
  }

  std::vector<std::string> Categorised::categories (const std::string& pattern) {
    if (pattern == "") { return categories(); }
    std::vector<std::string> output;
    // ...
    std::regex  pieces_regex(pattern);
    std::smatch pieces_match;
    for (const auto& category : categories()) {
      if (std::regex_match(category, pieces_match, pieces_regex)) {
	output.push_back(category);
      }
    }
    // ...
    return output;
  }
  
  unsigned Categorised::numCategories () {
    return m_categories.size();
  }
  
  bool Categorised::categoriesLocked () const {
    return m_categoriesLocked;
  }
  
  bool Categorised::canAddCategories () const {
    return !this->m_locked && !m_categoriesLocked;
  }

  bool Categorised::hasCategory (const std::string& category) const {
    return find(this->m_categories.begin(), this->m_categories.end(), category) != this->m_categories.end();
  }
  
} // namespace
