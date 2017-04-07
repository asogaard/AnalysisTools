#include "AnalysisTools/EventSelection.h"
#include "AnalysisTools/Analysis.h"
#include "AnalysisTools/ObjectDefinition.h"

namespace AnalysisTools {
    
    // Set method(s).
  /*
   * @TODO: - Apply 'lock' to auxiliary info as well, somehow...
   */

  void EventSelection::addCollection (const string& name, const string& objdef, const string& objdefCategory, const string& category) {
    DEBUG("Adding collection '%s/%s' (named: '%s') to category '%s'.", objdef.c_str(), objdefCategory.c_str(), name.c_str(), category.c_str());

    assert( !locked() );
    std::tuple<std::string, std::string, std::string> tuple = std::make_tuple(name, objdef, objdefCategory);
    if (category == "") {
      for (const auto& cat : this->categories()) {
	DEBUG("-- '%s'", cat.c_str());
	m_collectionNames[cat].push_back(tuple);
      }
    } else {
      m_collectionNames[category].push_back(tuple);
    }
    return;
  }

    
    // Get methods(s).
    // ...
    
    
    // High-level management method(s).
    bool EventSelection::run () {
        DEBUG("Entering.");
        DEBUG("  Running EventSelection '%s'.", name().c_str());
	
	// Lock, such that no more modifications can be performed.
	lock();

	// Make sure that collection links have been cached.
	if (not m_hasCachedCollections) { cacheCollections_(); }

	// Clear values cache.
	if (this->performCaching()) {
	  this->valuesCache()->clear();
	}

	// Loop categories (Nominal, ...)
        for (const auto& category : this->categories()) {
	    DEBUG("  Category: '%s'", category.c_str());

             // Setup
	    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

	    // Initialise cutflow histogram.
            if (!this->hasCutflow(category)) { this->setupCutflow(category); }
	    
	    // Initialise new Event.
            m_events[category] = Event();
            m_passes[category] = true;

	    // Add all available auxiliary information.
            for (const auto& name_val : this->infoContainer<unsigned>()) {
                m_events[category].addInfo(name_val.first, (double) *name_val.second);
            }
            for (const auto& name_val : this->infoContainer<double>()) {
                m_events[category].addInfo(name_val.first, (double) *name_val.second);
            }
            for (const auto& name_val : this->infoContainer<float>()) {
                m_events[category].addInfo(name_val.first, (double) *name_val.second);
            }
            for (const auto& name_val : this->infoContainer<bool>()) {
                m_events[category].addInfo(name_val.first, (double) *name_val.second);
            }            
            for (const auto& name_val : this->infoContainer<int>()) {
                m_events[category].addInfo(name_val.first, (double) *name_val.second);
            }

	    // Add all collections found above.
            for (auto& name_pointer : m_collectionLinks[category]) {	      
                m_events[category].addCollection(name_pointer.first, name_pointer.second);
            }
            
	    // Set correct MC weight, if possibly.
            float weight = 1.;
            if (this->m_weight) {
                weight = *this->m_weight;
            }
            if (this->m_sum_weights && *this->m_sum_weights != 0) {
                weight /= *this->m_sum_weights;
            }
            

             // Run selection.
	    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

	    // Initialise the cut counter.
            unsigned iCut = 0;
	    // Fill first ('All') bin in cutflow
            this->m_cutflow[category]->Fill(iCut++, weight);

	    // Loop operations.
	    // @TODO: - Branching?
	    for (IOperation* iop : this->operations(category)) { 
	        DEBUG("    Cut %d/%d", iCut, this->m_operations[category].size());
                //bool branch = (this->m_branch >= 0) && (iCut > this->m_branch); // iCut in [1, N]
                //if (!branch) { continue; }
              
		// Cast IOperation to either Operation or Cut, in order to call
		// 'apply' with the correct signature.
		DEBUG("      Casting IOperation '%s'", iop->name().c_str());

                bool passes = false;
		if        (iop->operationType() == OperationType::Operation) {
		  Operation<Event>* op  = static_cast< Operation<Event>* >(iop);
		  passes = op->apply(this->m_events[category], weight);
		} else if (iop->operationType() == OperationType::Cut) {
		  Cut<Event>*       cut = static_cast< Cut<Event>* >      (iop);
		  passes = cut->apply(this->m_events[category], weight);
		} else {
		  WARNING("Operation could not be cast to any known type.");
		}

		// Store whether event passed the current operation.
		DEBUG("      Storing whether the cut was passed (%s).", (passes ? "Yes" : "No"));
                m_passes[category] &= passes;

		// If event didn't pass, stop or this category.
                if (!m_passes[category]) { break; }

		// Fill the cutflow, and increment cut counter, but only if the
		// current operation was in fact a cut.
		if (iop->operationType() != OperationType::Cut) { continue; }
                this->m_cutflow[category]->Fill(iCut++, weight);
            }
            
        }

	// Store that this selection has run.
        this->m_hasRun = true;
	
	DEBUG("Exiting.");        

	// Return whether any categories passed all cuts.
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
    
    // Internal method(s).
  void EventSelection::cacheCollections_ () {

    DEBUG("Entering.");

    assert( locked() );

    // Initialise string variables.
    std::string name, selectionName, selectionCategory;

    // Loop categories (Nominal, ...)
    for (const auto& category : this->categories()) {
      DEBUG("  Category: '%s'", category.c_str());
      
      // Managing collections.
      // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
      
      // Setting up collections of physics objects (jets, photons, ...). 
      // A full copy of the vector of physics object is performed, 
      // separately for each category, since then we can trim the 
      // collection containers (overlap removal etc.).
      // Since the collections are stored by name, getting the actual 
      // (pointer to the) collection requires looping through preceeding 
      // selections in the parent analysis.
      // @TODO: - Cache links?
      //        - Better naming.
      for (const auto& tuple : m_collectionNames[category]) {
	
	// Unpack tuple.
	std::tie(name, selectionName, selectionCategory) = tuple;

	// Get parent analysis.
	ILocalised* parent = this->parent();

	// Determine category to which 'this' belongs
	std::string this_category;
	for (const std::pair<ILocalised*, string>& child_str : parent->children()) {
	  
	  // Unpack pair
	  ILocalised* child;
	  std::tie(child, this_category) = child_str;

	  // Break if 'this' has been reached, we have found the category.
	  if (child == (ILocalised*) this) { break; }

	}
	DEBUG("    Deduced category '%s'", this_category.c_str());

	// Loop through analysis's children elements to find target collection.
	for (const std::pair<ILocalised*, string>& child_str : parent->children()) {
	  
	  // Unpack pair
	  ILocalised* child;
	  std::string child_category;
	  std::tie(child, child_category) = child_str;

	  // If we're in the wrong category, go to next.
	  if (child_category != this_category) { continue; }

	  // Break if 'this' has been reached, meaning that we are no 
	  // longer querying preceeding child elements.
	  if (child == (ILocalised*) this) { break; }
	  
	  // Check whether name of child matches name of target Selection-
	  // type instance.
	  DEBUG(" -- Comparing '%s' and '%s' (%s)", selectionName.c_str(), child->name().c_str(), child_category.c_str());
	  
	  if (selectionName == child->name()) {
	    DEBUG(" -- Got one!");
	    
	    // Make sure that the found element can be type-cast to 
	    // ObjectDefinition; the class from which to extract 
	    // collections of physics objects.
	    if (ObjectDefinition<TLorentzVector>* objdef = dynamic_cast<ObjectDefinition<TLorentzVector>*>(child)) {
	      DEBUG(" ---- Got one! Looking for '%s'.", selectionCategory.c_str());
	      
	      // Add a copy of the resulting collection.
	      // @TODO: - Pointer + masking?
	      m_collectionLinks[category][name] = objdef->result(selectionCategory);
	      DEBUG(" ------ Storing results pointer:");
	      break;
	    }
	  }
	}
	
	// Make a notice if no collection was found.
	if (m_collectionLinks[category].count(name) == 0) {
	  DEBUG("Didn't manage to find an ObjectDefinition matching '%s' (named '%s'). This might not be a problem, but an error will be raised later if one does occur.", selectionName.c_str(), name.c_str());
	}
	
      }
    }

    // Flag that collections have been cached.
    m_hasCachedCollections = true;

    DEBUG("Exiting.");

    return;
  }
  
}
