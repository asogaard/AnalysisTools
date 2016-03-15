#include "AnalysisTools/EventSelection.h"

namespace AnalysisTools {
    
    // Set method(s).
    void EventSelection::addCollection (const string& name, shared_ptr<PhysicsObjects> collection) {
        assert( m_collections.count(name) == 0);
        m_collections[name] = collection;
        return;
    }
    
    
    // Get methods(s).
    // ...
    
    // High-level management method(s).
    bool EventSelection::run () {
        for (const auto& category : this->m_categories) {

            // * Setup
            if (!this->hasCutflow(category)) { this->setupCutflow(category); }
            
            m_events[category] = Event();
            m_passes[category] = true;
            
            for (const auto& name_val : this->m_infoFloat) {
                m_events[category].addInfo(name_val.first, (double) name_val.second->at(0));
            }
            for (const auto& name_val : this->m_infoInt) {
                m_events[category].addInfo(name_val.first, (double) name_val.second->at(0));
            }
            for (const auto& name_val : this->m_infoBool) {
                m_events[category].addInfo(name_val.first, (double) name_val.second->at(0));
            }
            /* @TODO: Switch away from vectors for 'EventSelection'. */
            for (const auto& name_val : m_collections) {
                m_events[category].addCollection(name_val.first, name_val.second);
            }
            
            // * Run selection.
            unsigned int iCut = 0;
            this->m_cutflow[category]->Fill(iCut++);
            for (IOperation* iop : this->m_operations[category]) {
                // [Make use of branching?]
                bool passes = false;
                if        (Operation<Event>* op  = dynamic_cast< Operation<Event>* >(iop)) {
                    passes = op->apply(this->m_events[category]);
                } else if (Cut<Event>*       cut = dynamic_cast< Cut<Event>* >(iop)) {
                    passes = cut->apply(this->m_events[category]);
                } else {
                    cout << "<EventSelection::run> Operation could not be cast to any known type." << endl;
                }

                m_passes[category] &= passes;
                if (!m_passes[category]) { break; }
                // dynamic_cast< Cut<T> > != NULL
                this->m_cutflow[category]->Fill(iCut++);
            }
            
        }
        
        this->m_hasRun = true;
        
        return (std::count_if(m_passes.begin(), m_passes.end(), [](const pair<string, bool>& p) { return p.second; }) > 0);
    }
    
    bool EventSelection::result () {
        assert( m_hasRun );
        if (this->nCategories() > 1) {
            //AnalysisTools::Warning("Call is ambiguous, since more than one category has been registered.");
            return false;
        }
        return m_passes.begin()->second;
    }
    
    bool EventSelection::result (const string& category) {
        assert( m_hasRun );
        assert( this->hasCategory(category) );
        return m_passes[category];
    }
    
    
    // Low-level management method(s).
    // ...

}