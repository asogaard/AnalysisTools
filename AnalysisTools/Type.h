#ifndef AnalysisTools_Type_h
#define AnalysisTools_Type_h

/**
 * @file   Type.h
 * @author Andreas Sogaard
 * @date   14 November 2016
 * @brief  Utility function to extract demangled class name.
 */

#include <iostream> /* cout, endl (TEMPORARY) */
#include <string> /* std::string */
#include <typeinfo> /* typeid */

/**
 * Taken from here: http://stackoverflow.com/a/4541470
 */

namespace AnalysisTools {

/// Utility functions to extract demangles type name.
// Demangle the result of built-in 'typeid' function. Compiler dependent. For internal use only.
std::string demangle(const char* name);

// Return the demangled type name of class 'T' as string. For public use.
template <class T>
std::string type(const T& t) {
  return demangle(typeid(t).name());
}

} // namespace

#endif
