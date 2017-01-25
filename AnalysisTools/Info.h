#ifndef AnalysisTools_Info_h
#define AnalysisTools_Info_h

/**
 * @file   Info.h
 * @author Andreas Sogaard
 * @date   25 January 2017
 * @brief  Utility classes for storing and handling auxiliary information.
 */

// STL include(s).
#include <string> /* std::string */
#include <vector> /* std::vector */
#include <map> /* std::map */
#include <cassert> /* assert */

// AnalysisTools include(s).
// ...

namespace AnalysisTools {

/**
 * Utility class for storing and handling basic auxiliary information.
 *
 * ...
 */
class BasicInfo {
    
 public:

  /// Set method(s).
  // Add pointer to information given type and name.
  template<class T>
  void add (const std::string& name, const T* value) {

    // Make sure that no value with this name, of this type, exists.
    assert(container<T>().find(name) == container<T>().end());

    // Add value to container.
    container_<T>().emplace(name, value);
    return;
  }


  /// Get method(s).
  // Get pointer to information given type and name.
  template<class T>
  const T* get (const std::string& name) {

    // Make sure that a value with this name, of this type, exists.
    assert(container<T>().find(name) != container<T>().end());

    // Return pointer from container.
    return container<T>().at(name);
  }

  // Get const reference to container given type.
  template<class T>
  inline const std::map<std::string, const T*>& container () {
    // Const-cast the reference up.
    return const_cast<const std::map<std::string, const T*>&>(container_<T>());
  }


 private:

  /// Internal method(s).
  // Get non-const reference to container given type.
  template<class T>
  std::map<std::string, const T*>& container_ ();


 private:

  /// Data member(s).
  std::map<std::string, const double*> m_doubles;
  std::map<std::string, const float*>  m_floats;
  std::map<std::string, const bool*>   m_bools;
  std::map<std::string, const int*>    m_ints;
  
 };
 

/**
 * Utility class for storing and handling vector-like auxiliary information.
 *
 * ...
 */
class VectorInfo {

 public:

  /// Set method(s).
  // Add pointer to information given type and name.
  template<class T>
  void add (const std::string& name, const std::vector<T>* value) {

    // Make sure that no value with this name, of this type, exists.
    assert(container<T>().find(name) == container<T>().end());

    // Add value to container.
    container_<T>().emplace(name, value);
    return;
  }


  /// Get method(s).
  // Get pointer to information given type and name.
  template<class T>
  const std::vector<T>* get (const std::string& name) {

    // Make sure that a value with this name, of this type, exists.
    assert(container<T>().find(name) != container<T>().end());

    // Return pointer from container.
    return container<T>().at(name);
  }

  // Get const reference to container given type.
  template<class T>
  inline const std::map<std::string, const std::vector<T>*>& container () {
    // Const-cast the reference up.
    return const_cast<const std::map<std::string, const std::vector<T>*>&>(container_<T>());
  }
    

 private:

  /// Internal method(s).
  // Get non-const reference to container given type.
  template<class T>
  std::map<std::string, const std::vector<T>*>& container_ ();
    


 private:

  /// Data member(s).
  std::map<std::string, const std::vector<double>* > m_doubles;
  std::map<std::string, const std::vector<float>* >  m_floats;
  std::map<std::string, const std::vector<bool>* >   m_bools;
  std::map<std::string, const std::vector<int>* >    m_ints;
  
};
 
} // namespace

#endif // AnalysisTools_Info_h
