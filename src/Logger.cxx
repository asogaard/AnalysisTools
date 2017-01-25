#include "AnalysisTools/Logger.h"

namespace AnalysisTools {
    
void Logger::fctprint_ (std::string fun, std::string level, std::string format, ...) {
    
    // Determine print level colour.
    std::string col = "\033[1m";
    if      (level == "ERROR")   { col = "\033[1;31m"; }
    else if (level == "WARNING") { col = "\033[1;31m"; }
    else if (level == "INFO")    { col = "\033[1;34m"; }

    // Format print statement.
    std::cout << "\033[1m" << std::left << std::setw(s_width) << ("<" + fun + "> ") << col << std::setw(7) << level << "\033[0m ";
    va_list args;
    va_start (args, format);
    vprintf (format.c_str(), args);
    va_end (args);
    std::cout << std::endl;

    return;
}

void Logger::print_ (std::string cls, std::string fun, std::string level, std::string format, ...) const {
    
    // Determine print level colour.
    std::string col = "\033[1m";
    if      (level == "ERROR")   { col = "\033[1;31m"; }
    else if (level == "WARNING") { col = "\033[1;31m"; }
    else if (level == "INFO")    { col = "\033[1;34m"; }

    // Format print statement.
    //std::cout << "\033[1m" << std::left << std::setw(s_width) << ("<" + cls + "::" + fun + "> ") << col << std::setw(7) << level << "\033[0m ";
    std::cout << "\033[1m" << std::left << std::setw(s_width) << ("<" + fun + "> ") << col << std::setw(7) << level << "\033[0m ";
    va_list args;
    va_start (args, format);
    vprintf (format.c_str(), args);
    va_end (args);
    std::cout << std::endl;

    return;
}

} // namespace
