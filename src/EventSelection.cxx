#include "AnalysisTools/EventSelection.h"
#include "AnalysisTools/Analysis.h"
#include "AnalysisTools/ObjectDefinition.h"

namespace AnalysisTools {
    
    // Set method(s).
  /*
    void EventSelection::addCollection (const string& name, PhysicsObjects* collection) {
        assert( m_collections.count(name) == 0);
        m_collections[name] = collection;
        return;
	}
  */

  void EventSelection::addCollection (const string& name, const string& selection, const string& category) {
      m_collections[name] = std::make_pair(selection, category);
  }

    
    // Get methods(s).
    // ...
    
    
    // High-level management method(s).
    bool EventSelection::run () {
        DEBUG("Entering.");
        DEBUG("  Running EventSelection '%s'.", name().c_str());
        unsigned int iCat = 0;
	
        for (const auto& category : this->m_categories) {
	    DEBUG("  Category: '%s'", category.c_str());
	    
	    // Setting up collections. This is done separately for each category, since then we can trim the collection containers (overlap removal etc.)
	    map<string, PhysicsObjects> collectionLinks;
	    for (const auto& name_pair : m_collections) {
	      /**
	       * @TODO: - Better naming!
	       */
	      
	      ILocalised* parent = this->parent();
	      for (pair<ILocalised*, string> child_str : parent->children()) {
		DEBUG(" -- Comparing '%s' and '%s' (%s)", name_pair.second.first.c_str(), child_str.first->name().c_str(), child_str.second.c_str());
		
		if (name_pair.second.first == child_str.first->name()) {
		  DEBUG(" -- Got one!");
		  if (ObjectDefinition<TLorentzVector>* objdef = dynamic_cast<ObjectDefinition<TLorentzVector>*>(child_str.first)) {
		    DEBUG(" ---- Got one! Looking for '%s'.", name_pair.second.second.c_str());
		    
		    collectionLinks[name_pair.first] = *objdef->result(name_pair.second.second);
		    break;
		  }
		}
		// ... Add ...
	      }
	      
	      if (collectionLinks.count(name_pair.first) == 0) {
		//WARNING("Didn't manage to find an ObjectDefinition matching '%s' (%s). This might not be a problem, but an error will be raised if one does occur.", name_pair.second.first.c_str(), name_pair.first.c_str());
	      }
	      
	    }
	    
	    
	    // * Check if preceeding EventSelection exists, with same category.
	    /*
	      std::cout << std::endl;
	      cout << "Getting parent analysis..." << endl;
	      Analysis* analysis = dynamic_cast<Analysis*>( this->parent() );
	      cout << "Got it!" << endl;
	      cout << "Contains " << analysis->selections().size() << " selections." << endl;
	      EventSelection* previousEventSelection = nullptr;
	      EventSelection* tempEventSelection     = nullptr;
	      for (auto* selection : analysis->selections()) {
	      if (selection == this) { break; }
	      if (tempEventSelection = dynamic_cast<EventSelection*>(selection)) {
	      previousEventSelection = tempEventSelection;
	      }
	      }
	      std::cout << previousEventSelection << std::endl;
	      if (!previousEventSelection) {
	      std::cout << "No preceeding EventSelection was found." << std::endl;
	      } else {
	      std::cout << "A preceeding EventSelection was found!" << std::endl;
	      std::cout << "Has " << previousEventSelection->nCategories() << " categories." << std::endl;
	      for (auto cat : previousEventSelection->categories()) {
	      std::cout << " -- " << cat << " (" << category << ") " << (cat == category ? "<-- Matches 'this'!" : "") << std::endl;
	      }
	      }
	    */
	    
            // * Setup
            if (!this->hasCutflow(category)) { this->setupCutflow(category); }
	    
            m_events[category] = Event();
            m_passes[category] = true;

            for (const auto& name_val : m_info.container<double>()) {
                m_events[category].addInfo(name_val.first, (double) *name_val.second);
            }
            for (const auto& name_val : m_info.container<float>()) {
                m_events[category].addInfo(name_val.first, (double) *name_val.second);
            }
            for (const auto& name_val : m_info.container<bool>()) {
                m_events[category].addInfo(name_val.first, (double) *name_val.second);
            }            
            for (const auto& name_val : m_info.container<int>()) {
                m_events[category].addInfo(name_val.first, (double) *name_val.second);
            }

            for (auto& name_val : collectionLinks) { //m_collections) {
                m_events[category].addCollection(name_val.first, &name_val.second);
		DEBUG("    Adding collection '%s':", name_val.first.c_str());
		//std::cout << " [" << name_val.second << "]" << std::endl;
            }
            
            float weight = 1.;
            if (this->m_weight) {
                weight = *this->m_weight;
            }
            
            // * Run selection.
            unsigned iCut = 0;
            this->m_cutflow[category]->Fill(iCut++, weight);
	    for (IOperation* iop : this->operations(category)) { // IOperation*
	        DEBUG("    Cut %d/%d", iCut, this->m_operations[category].size());
                //bool branch = (this->m_branch >= 0) && (iCut > this->m_branch); // iCut in [1, N]
                //if (!branch) { continue; }
                
		DEBUG("      Casting IOperation '%s'", iop->name().c_str());
                // [Make use of branching?]
                bool passes = false;
                if        (Operation<Event>* op  = dynamic_cast< Operation<Event>* >(iop)) {
                    passes = op ->apply(this->m_events[category], weight);
                } else if (Cut<Event>*       cut = dynamic_cast< Cut<Event>* >(iop)) {
                    passes = cut->apply(this->m_events[category], weight);
                } else {
		  WARNING("Operation could not be cast to any known type.");
                }

		DEBUG("      Storing whether the cut was passed (%s).", (passes ? "Yes" : "No"));
                m_passes[category] &= passes;
                if (!m_passes[category]) { break; }
                if (dynamic_cast< Cut<Event>* >(iop) == nullptr) { continue; }
                this->m_cutflow[category]->Fill(iCut++, weight);
            }
            
        }
        
        this->m_hasRun = true;
	
	DEBUG("Exiting.");        

        return m_passes.size() == 0 || (std::count_if(m_passes.begin(), m_passes.end(), [](const pair<string, bool>& p) { return p.second; }) > 0);
    }
    
    bool EventSelection::result () {
        assert( this->hasRun() ); // Necessary?
        assert( this->nCategories() == 1);
        return m_passes.begin()->second;
    }
    
    bool EventSelection::result (const string& category) {
        assert( this->hasRun() ); // Necessary?
        assert( this->hasCategory(category) );
        return m_passes[category];
    }
    
    void EventSelection::print () const {
      INFO("  Configuration for event selection '%s':", this->name().c_str());
      for (const auto& cat_ops : this->m_operations) {
	INFO("    Category '%s':", cat_ops.first.c_str());
	for (const auto& iop : cat_ops.second) {
	  iop->print();
	}
      }
      return;
    }
    
    // Low-level management method(s).
    // ...

}
