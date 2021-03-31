#ifndef JETSHAPE2D_H
#define JETSHAPE2D_H

#include "AnalysisModuleBase.h"
#include "Hist2D.h"
#include <iostream>

class JetShape2D : public AnalysisModuleBase {
private:
  
  // Allows the registration of the module so that it is available to be used by the Analysis framework.
  static RegisterAnalysisModule<JetShape2D> reg;
  const std::string name;
  
  void SetObservable
  (fastjet::PseudoJet jet,
   std::vector<std::shared_ptr<Particle>> particle_list,
   int ir, std::vector<std::array<int, 2>> i_j );

  void CombineHist(int iv, int ir, int ijp, int ijr, int ipp, int ipr, int ip);
  
  std::shared_ptr<Histogram> CreateHist( std::string hist_name, int iv );
  
  int ReadOptionParametersFromXML();
  void ShowParamsSetting();
  std::string GetParamsTag( int i );
  
  int nBinPhi;
  int nBinEta;
  std::vector<double> boundPhi;
  std::vector<double> boundEta;

  int mixedEvent;
  void LoadMixedEvent();
  std::vector<std::vector<std::shared_ptr<Particle>>> mixEvParticleList;
  std::string GetMixedEventFileName(int iv, int ir, int ijp, int ijr, int ipp, int ipr);

  
  std::vector<double> sidebandRap;

  
public:
  JetShape2D(std::string name_in = "JetShape2D");
  ~JetShape2D();
  std::string Name(){return name;}
  
  
};

#endif // JetShape2D_H
