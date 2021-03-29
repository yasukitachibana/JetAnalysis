#include "StandardReco.h"


// Register the module with the base class
RegisterReconstructionModule<StandardReco> StandardReco::reg("StandardReco");

StandardReco::StandardReco(std::string name_in): name(name_in)
{
  std::cout << "-@-Creating StandardReco" << std::endl;
}

StandardReco::~StandardReco(){
  std::cout << "-$-Deleting StandardReco"<<std::endl;
}

std::vector<fastjet::PseudoJet>
StandardReco::JetReco( double r_cone, std::vector<std::shared_ptr<Particle>> particle_list ){
  
  // First pass only particles with stat >=0 to jet reconstruction algorithm
  std::vector <fastjet::PseudoJet> fj_inputs;
  for( auto p : particle_list ){
    if( p->pstat() >= 0 ){
      fj_inputs.push_back(p->GetPseudoJet());
    }
  }
  // Jet reconstruction
  fastjet::JetDefinition jetDef(fastjet::antikt_algorithm, r_cone);
  fastjet::ClusterSequence clustSeq(fj_inputs, jetDef);
  std::vector <fastjet::PseudoJet> jets = sorted_by_pt( clustSeq.inclusive_jets( jetPtCut ) );
  // Subtraction for particles with stat < 0
  jets = sub_ptr->JetSub(r_cone, jets, particle_list);

  return jets;
}
