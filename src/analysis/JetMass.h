#ifndef JETMASS_H
#define JETMASS_H

#include "AnalysisModuleStandard.h"
#include <iostream>

class JetMass : public AnalysisModuleStandard
{
private:
  // Allows the registration of the module so that it is available to be used by the Analysis framework.
  static RegisterAnalysisModule<JetMass> reg;
  const std::string name;

  void SetObservable(int i_tag_particle, fastjet::PseudoJet jet,
                     std::vector<std::shared_ptr<Particle>> particle_list,
                     int ir, int ijp, int ijr);

  void CombineHist(int iv, int ir, int ijp, int ijr, int ipp, int ipr, int ip);

  int ReadOptionParametersFromXML();
  std::string GetParamsTag(int i);

  static const int n_p = 1;

public:
  JetMass(std::string name_in = "JetMass");
  ~JetMass();
  std::string Name() { return name; }
};

#endif // JetMass_H
