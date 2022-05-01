#ifndef NEGATIVERECO_H
#define NEGATIVERECO_H

#include "ReconstructionModuleBase.h"
#include "negative_recombiner.h"
#include <iostream>

class NegativeReco : public ReconstructionModuleBase {
private:
  
  // Allows the registration of the module so that it is available to be used by the Analysis framework.
  static RegisterReconstructionModule<NegativeReco> reg;
  
public:
  NegativeReco(std::string name_in = "NegativeReco");
  ~NegativeReco();
  std::string Name(){return name;}
  
  std::vector<fastjet::PseudoJet>
  JetReco( double r_cone, std::vector<std::shared_ptr<Particle>> particle_list );

private:
  const std::string name;
  int ui;
  
};

#endif // NegativeReco_H
