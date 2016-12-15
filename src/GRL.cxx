#include "AnalysisTools/GRL.h"

namespace AnalysisTools {

    // Constructor(s).
    // ...

    
    // Set method(s).
    void GRL::loadXML (const string& path) {

        
      // * Open stream to XML file.
        ifstream file ( path.c_str() );
        if (!file.is_open()) {
	  cout << "<GRL::loadXML> ERROR: GRL file '" << path << "' not found. Exiting." << endl;
	  return;
        }
        
        // * Reading good runs.
        m_goodRuns.clear();
        int run = 0;
        string value = "";
        
        while ( file.good() ) {
            std::getline ( file, value );
            if (value == "") {
                continue;
            } else if (value.find(",") == std::string::npos) {
                run = std::stoi(value);
            } else {
                assert(run > 0);
                int commapos = value.find(",");
                string Start = value.substr(0, commapos);
                string End   = value.substr(commapos + 1, value.size() - commapos - 1);
                m_goodRuns[run].push_back(Range(std::stoi(Start), std::stoi(End)));
            }
        }

        file.close();
        m_hasXML = true;
        
        return;
    }
    
    
    // Get method(s).
    // ...
    
    
    // High-level management method(s).
    bool GRL::contains (const int& run, const int& LB) const {
        bool isGood = false;
        assert(m_hasXML);
        if (run > -1) {
            if (m_goodRuns.count(run)) {
                for (Range goodRun : m_goodRuns.at(run)) {
                    isGood |= goodRun.contains(LB);
                    if (isGood) { break; }
                }
            } else {
                isGood = false;
            }
        } else {
            isGood = false;
        }
        return isGood;
    }

    
}
