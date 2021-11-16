#ifndef JETSHAPE_H
#define JETSHAPE_H

#include "AnalysisModuleStandard.h"
#include <iostream>

class InconeQuant : public AnalysisModuleStandard {
private:
  
  // Allows the registration of the module so that it is available to be used by the Analysis framework.
  static RegisterAnalysisModule<InconeQuant> reg;
  const std::string name;
  
  // void SetObservable
  // (fastjet::PseudoJet jet,
  //  std::vector<std::shared_ptr<Particle>> particle_list,
  //  int ir, int ijp, int ijr );

  // void CombineHist(int iv, int ir, int ijp, int ijr, int ipp, int ipr, int ip);
  
  int ReadOptionParametersFromXML();
  std::string GetParamsTag( int i );

  static const int n_p = 4;

  static const 

public:
  InconeQuant(std::string name_in = "InconeQuant");
  ~InconeQuant();
  std::string Name(){return name;}
  
  
};

#endif // JetShape_H
