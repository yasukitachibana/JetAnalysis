#ifndef JETSHAPE2DNOMIXEDEVENT_H
#define JETSHAPE2DNOMIXEDEVENT_H

#include "AnalysisModuleStandard.h"
#include "Hist2D.h"
#include <iostream>

class JetShape2DNoMixedEvent : public AnalysisModuleStandard
{
private:
  // // Allows the registration of the module so that it is available to be used by the Analysis framework.
  static RegisterAnalysisModule<JetShape2DNoMixedEvent> reg;
  const std::string name;

  void SetObservable(int i_tag_particle, fastjet::PseudoJet jet,
                     std::vector<std::shared_ptr<Particle>> particle_list,
                     int ir, int ijp, int ijr);

  void CombineHist(int iv, int ir, int ijp, int ijr, int ipp, int ipr, int ip);
  void CombineFinisher();
  void GetJetShape(int iv, int ir, int ijp, int ijr, int ipp, int ipr, int ip);
  // //
  std::shared_ptr<Histogram> CreateHist(std::string hist_name, int iv);

  int ReadOptionParametersFromXML();
  std::string GetParamsTag(int i);
  void ShowParamsSetting();

  //
  int nBinPhi;
  int nBinEta;
  std::vector<double> boundPhi;
  std::vector<double> boundEta;
  std::vector<double> sidebandRap;

public:
  JetShape2DNoMixedEvent(std::string name_in = "JetShape2DNoMixedEvent");
  ~JetShape2DNoMixedEvent();
  std::string Name() { return name; }
};

#endif // JetShape2DNoMixedEvent_H
