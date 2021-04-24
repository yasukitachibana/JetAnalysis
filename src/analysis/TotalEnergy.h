#ifndef TOTALENERGY_H
#define TOTALENERGY_H

#include "AnalysisModuleStandard.h"
#include <iostream>

class TotalEnergy : public AnalysisModuleStandard {
private:

  // Allows the registration of the module so that it is available to be used by the Analysis framework.
  static RegisterAnalysisModule<TotalEnergy> reg;
  const std::string name;
  
  void OneEventAnalysis(std::vector<std::shared_ptr<Particle>> particle_list);
  void CombineHist(int iv, int ir, int ijp, int ijr, int ipp, int ipr, int ip);

public:
  TotalEnergy(std::string name_in = "TotalEnergy");
  ~TotalEnergy();
  std::string Name(){return name;}
  

};

#endif // TotalEnergy_H
