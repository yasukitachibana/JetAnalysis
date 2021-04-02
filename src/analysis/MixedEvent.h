#ifndef MIXEDEVENT_H
#define MIXEDEVENT_H

#include "AnalysisModuleBase.h"
#include "Hist2D.h"
#include <iostream>

class MixedEvent : public AnalysisModuleBase {
private:
  
  // Allows the registration of the module so that it is available to be used by the Analysis framework.
  static RegisterAnalysisModule<MixedEvent> reg;
  const std::string name;
  
  void OneEventAnalysis(std::vector<std::shared_ptr<Particle>> particle_list);

  void CombineHist(int iv, int ir, int ijp, int ijr, int ipp, int ipr, int ip);
  
  std::shared_ptr<Histogram> CreateHist( std::string hist_name, int iv );
  
  int ReadOptionParametersFromXML();
  
  int nBinPhi;
  int nBinEta;
  std::vector<double> boundPhi;
  std::vector<double> boundEta;

  
public:
  MixedEvent(std::string name_in = "MixedEvent");
  ~MixedEvent();
  std::string Name(){return name;}
  
  
};

#endif // MixedEvent_H
