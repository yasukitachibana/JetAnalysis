#ifndef RAPDIST_H
#define RAPDIST_H

#include "AnalysisModuleStandard.h"
#include <iostream>

class RapDist : public AnalysisModuleStandard {
private:

  // Allows the registration of the module so that it is available to be used by the Analysis framework.
  static RegisterAnalysisModule<RapDist> reg;
  const std::string name;
  
  void OneEventAnalysis(std::vector<std::shared_ptr<Particle>> particle_list, int i_tag_particle);
  void CombineHist(int iv, int ir, int ijp, int ijr, int ipp, int ipr, int ip);

  int ReadOptionParametersFromXML();
  std::string GetParamsTag( int i );
  
public:
  RapDist(std::string name_in = "RapDist");
  ~RapDist();
  std::string Name(){return name;}
  

};

#endif // RapDist_H
