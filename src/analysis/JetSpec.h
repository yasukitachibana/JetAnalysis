#ifndef JETSPEC_H
#define JETSPEC_H

#include "AnalysisModuleStandard.h"
#include "negative_recombiner.h"
#include <iostream>

class JetSpec : public AnalysisModuleStandard
{
private:
  // Allows the registration of the module so that it is available to be used by the Analysis framework.
  static RegisterAnalysisModule<JetSpec> reg;
  const std::string name;

  void OneEventAnalysis(std::vector<std::shared_ptr<Particle>> particle_list, int i_tag_particle);
  // void SetObservable(int i_tag_particle, fastjet::PseudoJet jet,
  //                    std::vector<std::shared_ptr<Particle>> particle_list,
  //                    int ir, int ijp, int ijr);

  void CombineHist(int iv, int ir, int ijp, int ijr, int ipp, int ipr, int ip);

  int ui;

public:
  JetSpec(std::string name_in = "JetSpec");
  ~JetSpec();
  std::string Name() { return name; }
};

#endif // JetSpec_H
