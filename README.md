# AnalysisTools

A small package, providing various classes for easily implementing a modular analysis, including object definition, event selection, cutflows, and distributions.

Provides classes for selectable entities (`PhysicsObject`, `Event`), selection operations (`Operation`, `Cut`), types of selection (`Selection`, `EventSelection`, `ObjectDefinition`), analyses (`Analysis`), and various other helper classes (`Range`, `PlotMacro`). The only external dependency is [ROOT](https://root.cern.ch/).

A very simple analysis can be implemented as follows:

```
 // Stuff that goes into the analysis.
// -------------------------------------------------------------------

vector<TLorentzVector>* electrons = ...;
vector<TLorentzVector>* muons     = ...;


 // Setting up AnalysisTools.
// -------------------------------------------------------------------

Analysis analysis ("MyAnalysis");

analysis.openOutput("output.root");


 // Object definitions.
// -------------------------------------------------------------------

// -- Electrons
ObjectDefinition<TLorentzVector> ElectronObjdef ("Electrons");

ElectronObjdef.setInput(electrons);

ElectronObjdef.addInfo("id_medium",          el_id_medium);

// * pT
Cut<PhysicsObject> el_pT ("pT");
el_pT.setFunction( [](const PhysicsObject& p) { return p.Pt() / 1000.; } );
el_pT.setRange(20., inf);
ElectronObjdef.addCut(el_pT);

// * ID (medium).
Cut<PhysicsObject> el_ID ("MediumID");
el_ID.setFunction( [](const PhysicsObject& p) { return p.info("id_medium"); } );
ElectronObjdef.addCut(el_ID);

// -- Muons
ObjectDefinition<TLorentzVector> MuonObjdef ("Muons");

MuonObjdef.setInput(muons);

MuonObjdef.addInfo("id_medium",          mu_id_medium);

// * pT
Cut<PhysicsObject> mu_pT ("pT");
mu_pT.setFunction( [](const PhysicsObject& p) { return p.Pt() / 1000.; } );
mu_pT.setRange(20., inf);
MuonObjdef.addCut(mu_pT);

// * ID (medium).
Cut<PhysicsObject> mu_ID ("MediumID");
mu_ID.setFunction( [](const PhysicsObject& p) { return p.info("id_medium"); } );
MuonObjdef.addCut(mu_ID);

// -- Output
PhysicsObjects* SelectedElectrons = ElectronObjdef.result("Nominal"); /* Default category name. */
PhysicsObjects* SelectedMuons     = MuonObjdef    .result();


 // Event selection.
// -------------------------------------------------------------------

EventSelection eventSelection ("EventSelection");

eventSelection.addCollection("Electrons", SelectedElectrons);
eventSelection.addCollection("Muons",     SelectedMuons);

// * Lepton count
Cut<Event> event_Nleptons ("Nleptons");
event_Nleptons.setFunction( [](const Event& e) { return e.collection("Electrons")->size() + e.collection("Muons")->size(); });
event_Nleptons.addRange(2);
eventSelection.addCut(event_Nleptons);


 // Adding selections.
// -------------------------------------------------------------------

analysis.addSelection(&ElectronObjdef);
analysis.addSelection(&MuonObjdef);
analysis.addSelection(&eventSelection);


 // Run analysis.
// -------------------------------------------------------------------

for (unsigned int iEvent = 0; iEvent < nEvents; iEvent++) {

    /* Fill input collections, eg. with TTree::GetEvent(int). */

    analysis.run();

}

analysis.save();

```

Here we select a subset of leptons, from an input collection, and require that the event has exactly two leptons. The AnalysisTools::Analysis object then writes the approriate distribution and cutflow to the `ROOT` file specificied at the top.