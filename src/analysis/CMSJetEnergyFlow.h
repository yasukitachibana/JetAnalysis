#ifndef CMSJETENERGYFLOW_H
#define CMSJETENERGYFLOW_H

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
  virtual void ShowSettings() {}
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

class CMSJetEnergyFlow : public AnalysisModuleStandard
{
private:
  // Allows the registration of the module so that it is available to be used by the Analysis framework.
  static RegisterAnalysisModule<CMSJetEnergyFlow> reg;
  const std::string name;

  void OneEventAnalysis(std::vector<std::shared_ptr<Particle>> particle_list, int i_tag_particle);

  void CombineHist(int iv, int ir, int ijp, int ijr, int ipp, int ipr, int ip);

  int ReadOptionParametersFromXML();
  void ShowParamsSetting();

  double delta_R_match; 
  double match_jet_pt_min;
  double match_jet_pt_max;  

  int ui;





public:
  CMSJetEnergyFlow(std::string name_in = "CMSJetEnergyFlow");
  ~CMSJetEnergyFlow();
  std::string Name() { return name; }
};

#endif // CMSJetEnergyFlow_H
