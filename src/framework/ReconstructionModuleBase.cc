#include "ReconstructionModuleBase.h"
#include "SetXML.h"
#include "SetFile.h"

//###############################################################################################################
// ReconstructionModuleFactory, RegisterReconstructionModule
//###############################################################################################################
// Create an instance of the static map to register modules
ReconstructionModuleFactory::map_type *ReconstructionModuleFactory::moduleMap =
new ReconstructionModuleFactory::map_type;
//###############################################################################################################


void ReconstructionModuleBase::Init(std::shared_ptr<SubtractionModuleBase> sub_ptr_in)
{
  
  std::cout
  << "[ReconstructionModuleBase] Intialize " << Name()
  << " with " << sub_ptr_in->Name()
  << std::endl;
  
  sub_ptr = nullptr;
  sub_ptr=sub_ptr_in;
  
  void SetJetPtCut();
  //  ReadParametersFromXML();
  
}

std::vector<fastjet::PseudoJet>
ReconstructionModuleBase::JetReco( double r_cone, std::vector<std::shared_ptr<Particle>> particle_list ){
  std::vector <fastjet::PseudoJet> jets;
  return jets;
}

void ReconstructionModuleBase::SetJetPtCut(double pt_jet_cut){
  jetPtCut=pt_jet_cut;
  std::cout << "[ReconstructionModuleBase] pt_jet cut: " << jetPtCut << " GeV" << std::endl;
}



