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
#include "AnalysisTools/Logger.h"

// Templated aliases for brevity
template <class T>
using basicContainer_t = std::map< std::string, const T* >;

template <class T> 
using vectorContainer_t = std::map< std::string, const std::vector<T>* >;


namespace AnalysisTools {

/**
 * Utility class for storing and handling basic auxiliary information.
 *
 * ...
 */
class BasicInfo {
    
 public:

  /// Set method(s).
  /**
   * Attempt to store a pointer to auxiliary information. An expection is thrown
   * if a pointer with the same name and type already exists.
   */
  template<class T>
    void addInfo (const std::string& name, const T* value, const bool& overwrite = false) {
    if (!overwrite && infoContainer<T>().find(name) != infoContainer<T>().end()) {
      FCTERROR("Info variable '%s' already exists.", name.c_str());
    }
    //infoContainer_<T>().emplace(name, value);
    infoContainer_<T>()[name] = value;
    return;
  }


  /// Get method(s).
  /**
   * Attempt to retrieve pointer to auxiliary information. An exception is 
   * thrown if no pointer exists with the requested name and type.
   */
  template<class T>
  const T* info (const std::string& name) {
    assert(infoContainer<T>().find(name) != infoContainer<T>().end());
    return infoContainer<T>().at(name);
  }

  /**
   * Return a const reference to the auxiliary information container of the 
   * templated type. This is done by const-casting the non-const reference
   * returned by a purely internal function.
   */
  template<class T>
  inline const basicContainer_t<T>& infoContainer () {
    return const_cast<const basicContainer_t<T>&>(infoContainer_<T>());
  }


 private:

  /// Internal method(s).
  /**
   * Return a non-const reference to the auxiliary information container of the 
   * templated type.
   */
  template<class T>
  basicContainer_t<T>& infoContainer_ ();


 private:

  /// Data member(s).
  /**
   * Type-specific auxiliary information containers.
   */
  basicContainer_t<unsigned> m_unsigneds;
  basicContainer_t<double>   m_doubles;
  basicContainer_t<float>    m_floats;
  basicContainer_t<bool>     m_bools;
  basicContainer_t<int>      m_ints;
  
 };
 

/**
 * Utility class for storing and handling vector-like auxiliary information.
 *
 * ...
 */
class VectorInfo {

 public:

  /// Set method(s).
  /**
   * Attempt to store a pointer to auxiliary information. An expection is thrown
   * if a pointer with the same name and type already exists.
   */
  template<class T>
  void addInfo (const std::string& name, const std::vector<T>* value, const bool& overwrite = false) {
    if (!overwrite and infoContainer<T>().find(name) != infoContainer<T>().end()) {
      FCTERROR("Info variable '%s' already exists.", name.c_str());
    }
    //infoContainer_<T>().emplace(name, value);
    infoContainer_<T>()[name] = value;
    return;
  }


  /// Get method(s).
  /**
   * Attempt to retrieve pointer to auxiliary information. An exception is
   * thrown if no pointer exists with the requested name and type.
   */
  template<class T>
  const std::vector<T>* info (const std::string& name) {
    assert(infoContainer<T>().find(name) != infoContainer<T>().end());
    return infoContainer<T>().at(name);
  }

  /**
   * Return a const reference to the auxiliary information container of the
   * templated type. This is done by const-casting the non-const reference
   * returned by a purely internal function.
   */
  template<class T>
  inline const vectorContainer_t<T>& infoContainer () {
    return const_cast<const vectorContainer_t<T>&>(infoContainer_<T>());
  }
    

 private:

  /// Internal method(s).
  /**
   * Return a non-const reference to the auxiliary information container of the
   * templated type.
   */
  template<class T>
  vectorContainer_t<T>& infoContainer_ ();
    


 private:

  /// Data member(s).
  /**
   * Type-specific auxiliary information containers.
   */
  vectorContainer_t<unsigned> m_unsigneds;
  vectorContainer_t<double>   m_doubles;
  vectorContainer_t<float>    m_floats;
  vectorContainer_t<bool>     m_bools;
  vectorContainer_t<int>      m_ints;
  
};
 
} // namespace

#endif // AnalysisTools_Info_h
