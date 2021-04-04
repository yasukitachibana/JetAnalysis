#ifndef JETSHAPE_H
#define JETSHAPE_H

#include "AnalysisModuleBase.h"
#include <iostream>

class JetShape : public AnalysisModuleBase {
private:
  
  // Allows the registration of the module so that it is available to be used by the Analysis framework.
  static RegisterAnalysisModule<JetShape> reg;
  const std::string name;
  
  void SetObservable
  (fastjet::PseudoJet jet,
   std::vector<std::shared_ptr<Particle>> particle_list,
   int ir, int ijp, int ijr );

  void CombineHist(int iv, int ir, int ijp, int ijr, int ipp, int ipr, int ip);
  
public:
  JetShape(std::string name_in = "JetShape");
  ~JetShape();
  std::string Name(){return name;}
  
  
};

#endif // JetShape_H
