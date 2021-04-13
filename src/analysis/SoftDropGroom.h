#ifndef SOFTDROPGROOM_H
#define SOFTDROPGROOM_H

#include "AnalysisModuleStandard.h"
#include "fastjet/contrib/SoftDrop.hh"
#include "negative_recombiner.h"
#include <iostream>

class SoftDropGroom : public AnalysisModuleStandard {
private:
  
  // Allows the registration of the module so that it is available to be used by the Analysis framework.
  static RegisterAnalysisModule<SoftDropGroom> reg;
  const std::string name;
  
  
  void OneEventAnalysis(std::vector<std::shared_ptr<Particle>> particle_list);


  void CombineHist(int iv, int ir, int ijp, int ijr, int ipp, int ipr, int ip);

  int ReadOptionParametersFromXML();
  void ShowParamsSetting();
  
  std::string GetParamsTag( int i );
  std::string GetParamsTag( std::array<int, 2> i );
  std::string GetParamsTag( int i_beta, int i_zCut );
  std::string GetParamsTag( double beta_sd, double z_cut_sd );

  int GetParamIndex( int i_beta, int i_zCut );
  int GetParamIndex( std::array<int, 2> i );
  std::array<int, 2> GetParamIndex( int i);
  
  
  std::vector<double> beta;
  std::vector<double> zCut;
  
  int i_zG;
  int i_thetaG;
  
  int ui;
  
public:
  SoftDropGroom(std::string name_in = "SoftDropGroom");
  ~SoftDropGroom();
  std::string Name(){return name;}
  
  
};

#endif // SoftDropGroom_H
