#ifndef MOMFRACTAGJET_H
#define MOMFRACTAGJET_H

#include "AnalysisModuleStandard.h"
#include "SmearingModuleBase.h"
#include <iostream>

class MomFracTagJet : public AnalysisModuleStandard
{
private:
  // Allows the registration of the module so that it is available to be used by the Analysis framework.
  static RegisterAnalysisModule<MomFracTagJet> reg;
  const std::string name;

  void OneEventAnalysis(std::vector<std::shared_ptr<Particle>> particle_list, int i_tag_particle);
  // void SetObservable (int i_tag_particle, fastjet::PseudoJet jet,
  //                     std::vector<std::shared_ptr<Particle>> particle_list,
  //                     int ir, int ijp, int ijr );

  void CombineHist(int iv, int ir, int ijp, int ijr, int ipp, int ipr, int ip);

  //----------------------------------------------------------------------------
  void ShowParamsSetting();
  int ReadOptionParametersFromXML();
  std::string GetParamsTag(int i);
  //----------------------------------------------------------------------------
  int smearing;
  std::shared_ptr<SmearingModuleBase> smear_ptr;  

public:
  MomFracTagJet(std::string name_in = "MomFracTagJet");
  ~MomFracTagJet();
  std::string Name() { return name; }
};

#endif // MomFracTagJet_H
