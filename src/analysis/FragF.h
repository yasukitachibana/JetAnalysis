#ifndef FRAGF_H
#define FRAGF_H

#include "AnalysisModuleBase.h"
#include <iostream>

class FragF : public AnalysisModuleBase {
private:
  
  // Allows the registration of the module so that it is available to be used by the Analysis framework.
  static RegisterAnalysisModule<FragF> reg;
  const std::string name;
  
  void SetObservable
  (fastjet::PseudoJet jet,
   std::vector<std::shared_ptr<Particle>> particle_list,
   int ir, int ijp, int ijr );


  void CombineHist(int iv, int ir, int ijp, int ijr, int ipp, int ipr, int ip);
  
public:
  FragF(std::string name_in = "FragF");
  ~FragF();
  std::string Name(){return name;}
  
  
};

#endif // FragF_H
