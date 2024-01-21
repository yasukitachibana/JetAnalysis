#ifndef DIJET_H
#define DIJET_H

#include "AnalysisModuleStandard.h"
// #include "fastjet/contrib/SoftDrop.hh"
// #include "negative_recombiner.h"
#include <iostream>

class Dijet : public AnalysisModuleStandard
{
private:
  // Allows the registration of the module so that it is available to be used by the Analysis framework.
  static RegisterAnalysisModule<Dijet> reg;
  const std::string name;

  void OneEventAnalysis(std::vector<std::shared_ptr<Particle>> particle_list, int i_tag_particle);

  void CombineHist(int iv, int ir, int ijp, int ijr, int ipp, int ipr, int ip);

  int ReadOptionParametersFromXML();
  void ShowParamsSetting();

  std::string GetParamsTag(int i);
  std::string GetParamsTag(
      double pt_lead_min, double pt_lead_max,
      double pt_sub_min, double pt_sub_max);

  //-----------------------------------------
  int delta_phi_cut;
  double delta_phi_min;
  std::unique_ptr<DeltaPhiCutBase> dijet_deltaphi_ptr;
  //-----------------------------------------
  std::unique_ptr<TagXCutBase> x_dijet_ptr;
  //-----------------------------------------
  std::vector<double> pTLeadMin;
  std::vector<double> pTLeadMax;
  std::vector<double> pTSubMin;
  std::vector<double> pTSubMax;

  int ReadVariablesFromXML(std::string tag);
  std::string VariableSuffix(int i);

  static const int n_var = 5;
  // 0:"xJ", 1:"pTLead", 2:"pTSub", 3:"deltaPhi"
  std::array<std::vector<int>, n_var> i_var;
  std::array<std::string, n_var> varNames = {"xJ", "pTLead", "pTSub", "deltaPhi","deltaPt"};

public:
  Dijet(std::string name_in = "Dijet");
  ~Dijet();
  std::string Name() { return name; }
};

#endif // Dijet_H
