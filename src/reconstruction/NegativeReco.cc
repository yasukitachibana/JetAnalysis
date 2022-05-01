#include "NegativeReco.h"


// Register the module with the base class
RegisterReconstructionModule<NegativeReco> NegativeReco::reg("NegativeReco");

NegativeReco::NegativeReco(std::string name_in): name(name_in), ui(-123456)
{
  std::cout << "-@-Creating NegativeReco" << std::endl;
}

NegativeReco::~NegativeReco(){
  std::cout << "-$-Deleting NegativeReco"<<std::endl;
}

std::vector<fastjet::PseudoJet>
NegativeReco::JetReco( double r_cone, std::vector<std::shared_ptr<Particle>> particle_list ){
  
  // First pass only particles with stat >=0 to jet reconstruction algorithm
  std::vector <fastjet::PseudoJet> fj_inputs;
  for( auto p : particle_list ){
    fj_inputs.push_back(p->GetPseudoJet());
  }
  // Jet reconstruction
  fastjet::JetDefinition jetDef(fastjet::antikt_algorithm, r_cone);
  // create an instance of the negative energy recombiner, with a given flag ui
  NegativeEnergyRecombiner uir(ui);
  // tell jet_def to use negative energy recombiner
  jetDef.set_recombiner(&uir);
  fastjet::ClusterSequence clustSeq(fj_inputs, jetDef);
  std::vector <fastjet::PseudoJet> jets = sorted_by_pt( clustSeq.inclusive_jets( jetPtCut ) );
  
  return jets;
}
