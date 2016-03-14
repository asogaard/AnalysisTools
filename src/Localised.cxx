#include "AnalysisTools/Localised.h"

namespace AnalysisTools {

    // Constructor(s).
    // ...
    
    
    // Set method(s).
    void Localised::setName (const string& name) {
        assert( !locked() );
        m_name = name;
        return;
    }
    
    void Localised::addChild (ILocalised* other, const string& postfix) {
        if (hasChild(other)) { return; }
        m_children.push_back(pair<ILocalised*, string>(other, postfix) );
        return;
    }
    
    void Localised::popChild (ILocalised* other, const string& postfix) {
        if (!hasChild(other)) { return; }
        m_children.erase( std::remove_if(m_children.begin(), m_children.end(), [other, postfix](const pair<ILocalised*, string> p) { return (other == p.first) && (postfix == p.second); }), m_children.end() );
        return;
    }
    
    
    // Get method(s).
    string Localised::name () const {
        return m_name;
    }
    
    TDirectory* Localised::dir () const {
        return m_dir;
    }
    
    bool Localised::locked () const {
        return m_locked;
    }
    
    bool Localised::hasChild (ILocalised* other, const string& postfix) const {
        return std::find_if(m_children.begin(), m_children.end(), [other, postfix](const pair<ILocalised*, string> p) { return (other == p.first) && (postfix == p.second); }) != m_children.end();
    }
    
    ILocalised* Localised::parent () const {
        return m_parent;
    }
    
    
    // High-level management method(s).
    void Localised::grab (ILocalised* other, const string& postfix) {
        addChild(other, postfix);
        other->put(this, postfix);
        return;
    }
    
    void Localised::put (ILocalised* other, const string& postfix) {
        assert( !locked() );
        assert( m_name != "" );
        
        // * Don't raise error. This is to allow for cases where you e.g. add a Cut to a Selection, which hasn't been added to an Analysis yet.
        if (other->m_dir == nullptr) { return; }
        
        m_parent = other;
        
        // * Get directory of 'other' Localised, ie. the place where 'this' Localised should be put.
        TDirectory* putDir = other->m_dir;
        putDir->cd();
        
        // * Check whether the target directory already exists; create if not.
        if (postfix != "") {
            bool hasDir = (putDir->GetDirectory(postfix.c_str()) != nullptr);
            
            if (!hasDir) {
                putDir = putDir->mkdir(postfix.c_str());
            } else {
                putDir->cd(postfix.c_str());
                putDir = gDirectory;
            }
        }
        
        // * Create (unique) directory for this localised.
        this->setDir( putDir->mkdir(m_name.c_str()) );
        
        // * Grab all children to the this directory
        for (auto child_postfix : m_children) {
            child_postfix.first->put(this, child_postfix.second);
        }

        return;
    }
    
    
    // Low-level management method(s).
    void Localised::setDir (TDirectory* dir) {
        assert( !locked() );
        m_dir = dir;
        return;
    }
    
    void Localised::lock () {
        m_locked = true;
        return;
    }
    
    void Localised::lockAll () {
        lock();
        for (auto child_postfix : m_children) {
            child_postfix.first->lockAll();
        }
        return;
    }
    
    TDirectory* Localised::parentDir () {
        if (m_parent) {
            return m_parent->m_dir;
        }
        return nullptr;
    }
    
}