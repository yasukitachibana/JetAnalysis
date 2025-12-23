#ifndef JETSHAPE2D_H
#define JETSHAPE2D_H

#include "AnalysisModuleMixedEvent.h"
#include "LegoParticle.h"
#include "Hist2D.h"
#include <iostream>

class JetShape2D : public AnalysisModuleMixedEvent
{
private:
  // Allows the registration of the module so that it is available to be used by the Analysis framework.
  static RegisterAnalysisModule<JetShape2D> reg;
  const std::string name;

  void SetObservable(int i_tag_particle, fastjet::PseudoJet jet,
                     std::vector<std::shared_ptr<Particle>> particle_list,
                     std::vector<std::shared_ptr<Particle>> mixed_event_particle_list,
                     int ir, int ijp, int ijr);
  void CombineHist(int iv, int ir, int ijp, int ijr, int ipp, int ipr, int ip);
  void CombineFinisher();
  void GetJetShape(int iv, int ir, int ijp, int ijr, int ipp, int ipr);
  //
  std::shared_ptr<Histogram> CreateHist(std::string hist_name, int iv);

  int ReadOptionParametersFromXML();
  void ShowParamsSetting();

  //
  int nBinPhi;
  int nBinEta;
  std::vector<double> boundPhi;
  std::vector<double> boundEta;

  //  void LoadMixedEvent();
  //  std::vector<std::vector<std::shared_ptr<Particle>>> mixEvParticleList;
  //  std::string GetMixedEventFileName(int iv, int ir, int ijp, int ijr, int ipp, int ipr);
  //  int GetMixedEvIndex( int iv, int ir, int ijp, int ijr, int ipp, int ipr);
  //
  std::vector<double> sidebandRap;

public:
  JetShape2D(std::string name_in = "JetShape2D");
  ~JetShape2D();
  std::string Name() { return name; }
};

#endif // JetShape2D_H
