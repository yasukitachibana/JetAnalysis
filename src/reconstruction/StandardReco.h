#ifndef STANDARDRECO_H
#define STANDARDRECO_H

#include "ReconstructionModuleBase.h"
#include <iostream>

class StandardReco : public ReconstructionModuleBase {
private:
  
  // Allows the registration of the module so that it is available to be used by the Analysis framework.
  static RegisterReconstructionModule<StandardReco> reg;
  
public:
  StandardReco(std::string name_in = "StandardReco");
  ~StandardReco();
  std::string Name(){return name;}
  
  std::vector<fastjet::PseudoJet>
  JetReco( double r_cone, std::vector<std::shared_ptr<Particle>> particle_list );

private:
  const std::string name;

  
};

#endif // StandardReco_H
