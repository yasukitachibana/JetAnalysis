#ifndef SOFTDROPGROOM_H
#define SOFTDROPGROOM_H

#include "AnalysisModuleStandard.h"
#include "fastjet/contrib/SoftDrop.hh"
#include "negative_recombiner.h"
#include <iostream>

class SDAdditionalCondition
{
public:
  virtual ~SDAdditionalCondition() {}
  virtual bool Trigger(double val) { return false; }
  virtual bool Trigger(double val1, double val2) { return false; }
  virtual bool Trigger(double val1, double val2, double val3) { return false; }
  virtual void ShowSettings(){}
};

class SDNoCondition : public SDAdditionalCondition
{
public:
  SDNoCondition() {}
  ~SDNoCondition() {}
  bool Trigger(double val) { return true; }
  bool Trigger(double val1, double val2) { return true; }
  bool Trigger(double val1, double val2, double val3) { return true; }
  void ShowSettings() {}
};

class SDDeltaRCut : public SDAdditionalCondition
{
public:
  SDDeltaRCut(double delta_r_min) : deltaRMin(delta_r_min) {}
  ~SDDeltaRCut() {}
  bool Trigger(double val)
  {
    if (val > deltaRMin)
    {
      return true;
    }
    else
    {
      return false;
    }
  }
  void ShowSettings()
  {
    std::cout << "[AnalyzeBase] *** Delta_R_cut: " << deltaRMin << std::endl;
  }

private:
  const double deltaRMin;
};

class SoftDropGroom : public AnalysisModuleStandard
{
private:
  // Allows the registration of the module so that it is available to be used by the Analysis framework.
  static RegisterAnalysisModule<SoftDropGroom> reg;
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
  //0:"zG", 1:"thetaG", 2:"rG", 3:"mG", 4:"mGOverPt", 5:"ktG", *:"pseudoMG", *:"pseudoMGOverPt", 
  std::array<std::vector<int>, n_var> i_var;
  std::array<std::string, n_var> varNames = {"zG", "thetaG", "rG", "mG", "mGOverPt", "ktG"};

  int ui;

  std::unique_ptr<SDAdditionalCondition> additional_cond_ptr;

  double CosOpeningAngle(double pmod1, double px1, double py1, double pz1,
                         double pmod2, double px2, double py2, double pz2);
  // bool SDCondition(double z_g, double theta_g, double z_cut, double beta);

public:
  SoftDropGroom(std::string name_in = "SoftDropGroom");
  ~SoftDropGroom();
  std::string Name() { return name; }
};

#endif // SoftDropGroom_H
