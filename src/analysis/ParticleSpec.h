#ifndef PARTICLESPEC_H
#define PARTICLESPEC_H

#include "AnalysisModuleBase.h"
#include <iostream>

class ParticleSpec : public AnalysisModuleBase {
private:

  // Allows the registration of the module so that it is available to be used by the Analysis framework.
  static RegisterAnalysisModule<ParticleSpec> reg;
  const std::string name;
  
  void OneEventAnalysis(std::vector<std::shared_ptr<Particle>> particle_list);
  void CombineHist(int iv, int ir, int ijp, int ijr, int ipp, int ipr, int ip);

public:
  ParticleSpec(std::string name_in = "ParticleSpec");
  ~ParticleSpec();
  std::string Name(){return name;}
  

};

#endif // ParticleSpec_H
