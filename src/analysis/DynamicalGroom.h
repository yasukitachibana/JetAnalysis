#ifndef DYNAMICALGROOM_H
#define DYNAMICALGROOM_H

#include "AnalysisModuleStandard.h"
#include "fastjet/contrib/SoftDrop.hh"
//#include "negative_recombiner.h"
#include "dyGroomer.h"

#include <iostream>


class DynamicalGroom : public AnalysisModuleStandard
{
private:
  // Allows the registration of the module so that it is available to be used by the Analysis framework.
  static RegisterAnalysisModule<DynamicalGroom> reg;
  const std::string name;

  void OneEventAnalysis(std::vector<std::shared_ptr<Particle>> particle_list, int i_tag_particle);

  void CombineHist(int iv, int ir, int ijp, int ijr, int ipp, int ipr, int ip);

  int ReadOptionParametersFromXML();
  void ShowParamsSetting();

  std::string GetParamsTag(int i);
  std::string GetParamsTag(std::array<int, 2> i);
  std::string GetParamsTag(int i_beta, int i_zCut);
  std::string GetParamsTag(double beta_sd, double z_cut_sd);

  int GetParamIndex(int i_beta, int i_zCut);
  int GetParamIndex(std::array<int, 2> i);
  std::array<int, 2> GetParamIndex(int i);

  std::vector<double> beta;
  std::vector<double> zCut;

  int ReadVariablesFromXML(std::string tag);
  std::string VariableSuffix(int i);

  static const int n_var = 6;
  // 0:"zG", 1:"thetaG", 2:"rG", 3:"mG", 4:"mGOverPt", 5:"ktG", *:"pseudoMG", *:"pseudoMGOverPt",
  std::array<std::vector<int>, n_var> i_var;
  std::array<std::string, n_var> varNames = {"zG", "thetaG", "rG", "mG", "mGOverPt", "ktG"};

  int ui;

  double CosOpeningAngle(double pmod1, double px1, double py1, double pz1,
                         double pmod2, double px2, double py2, double pz2);
  // bool SDCondition(double z_g, double theta_g, double z_cut, double beta);

public:
  DynamicalGroom(std::string name_in = "DynamicalGroom");
  ~DynamicalGroom();
  std::string Name() { return name; }
};

#endif // DynamicalGroom_H
