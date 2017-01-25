#ifndef AnalysisTools_Logger_h
#define AnalysisTools_Logger_h

/**
 * @file   Logger.h
 * @author Andreas Sogaard
 * @date   14 November 2016
 * @brief  Mix-in class providing logging functionality, with instance-specific print levels.
 */

// STL include(s).
#include <iostream> /* std::cout, std::left */
#include <string> /* std::string */
#include <stdio.h> /* vprintf */
#include <stdarg.h> /* variadic functions */
#include <iomanip> /* std::setw */

// AnalysisTools include(s).
#include "AnalysisTools/Type.h"


/// Printing macros, with signature similar to 'printf'.
// Macros only to be called from classes inheriting from Logger.
#define ERROR(...)                        {this->print_(type(*this),  __FUNCTION__, "ERROR",   __VA_ARGS__);}
#define WARNING(...)                      {this->print_(type(*this),  __FUNCTION__, "WARNING", __VA_ARGS__);}
#define INFO(...)                         {this->print_(type(*this),  __FUNCTION__, "INFO",    __VA_ARGS__);}
#define DEBUG(...)   if (this->debug())   {this->print_(type(*this),  __FUNCTION__, "DEBUG",   __VA_ARGS__);}
#define VERBOSE(...) if (this->verbose()) {this->print_(type(*this),  __FUNCTION__, "VERBOSE", __VA_ARGS__);}
// Macros to be called from wherever.
#define FCTERROR(...)   {AnalysisTools::Logger::fctprint_(__FUNCTION__, "ERROR",   __VA_ARGS__);}
#define FCTWARNING(...) {AnalysisTools::Logger::fctprint_(__FUNCTION__, "WARNING", __VA_ARGS__);}
#define FCTINFO(...)    {AnalysisTools::Logger::fctprint_(__FUNCTION__, "INFO",    __VA_ARGS__);}


namespace AnalysisTools {

/**
 * Mix-in class providing logging functionality, with instance-specific print 
 * levels.
 *
 * All classes inheriting from Logger will be able to print nicely formatted 
 * logging information, at various print levels, using the above macros. In 
 * addition to 'ERROR', 'WARNING', and 'INFO' levels, which always print, each 
 * instance inheriting from Logger can be set to print 'DEBUG' and 'VERBOSE' 
 * statements as well.
 * 
 * Any function not called from within an instance inheriting from Logger can 
 * still print formatted 'ERROR', 'WARNING', and 'INFO' statements using the 
 * 'FCT...' macros.
 */
class Logger {
    
public:

  /// Static member variable(s).
  static const unsigned s_width = 20; // 34;

  
  /// Constructor(s).
  Logger () {};
  
  
  /// Destructor.
  ~Logger () {};
  
  
  /// Set method(s).
  inline void setDebug (const bool& debug = true) {
    m_debug    = debug;
    m_verbose &= debug;
    return;
  }
  
  inline void setVerbose (const bool& verbose = true) {
    m_verbose  = verbose;
    m_debug   |= verbose;
    return;
  }
  
  
  /// Get method(s).
  inline bool debug   () const { return m_debug; }
  inline bool verbose () const { return m_verbose; }
  
  
  /// Public print method(s).
  // Variadic function, formatting the printed statment (format, ...) using 
  // the calling non-member function (fct) as well as the print level.
  static void fctprint_ (std::string fun, std::string level, 
			 std::string format, ...);
  
  
 protected:
  
  /// Internal method(s).
  // Variadic function, formatting the printed statment (format, ...) using 
  // the calling class (cls) and member function (fct) as well as the print 
  // level.
  void print_ (std::string cls, std::string fun, std::string level, 
	       std::string format, ...) const;
  
  
 protected:
  
  /// Data member(s).
  bool m_debug   = false;
  bool m_verbose = false;
  
};
 
} // namespace

#endif // AnalysisTools_Logger_h
