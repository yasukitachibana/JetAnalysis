#ifndef TAGGEDJETPAIR_H
#define TAGGEDJETPAIR_H

#include "AnalysisModuleStandard.h"
#include <iostream>

class TaggedJetPair : public AnalysisModuleStandard
{
private:
  // Allows the registration of the module so that it is available to be used by the Analysis framework.
  static RegisterAnalysisModule<TaggedJetPair> reg;
  const std::string name;

  void OneEventAnalysis(std::vector<std::shared_ptr<Particle>> particle_list, int i_tag_particle);
  // void SetObservable (int i_tag_particle, fastjet::PseudoJet jet,
  //                     std::vector<std::shared_ptr<Particle>> particle_list,
  //                     int ir, int ijp, int ijr );

  void CombineHist(int iv, int ir, int ijp, int ijr, int ipp, int ipr, int ip);

  //----------------------------------------------------------------------------
  void ShowParamsSetting();
  int ReadOptionParametersFromXML();
  std::string VariableSuffix(int i);
  int ReadVariablesFromXML(std::string tag);

  std::vector<double> rBinEdges;
  std::vector<double> aBinEdges;

  static const int n_var = 3;
  // 0:"xJJTag", 1:"rJJ", 2:"aJJTag"
  std::array<std::vector<int>, n_var> i_var;
  std::array<std::string, n_var> varNames = {"xJJTag", "rJJ", "aJJTag"};
  //----------------------------------------------------------------------------
  //=========================================
  std::unique_ptr<DeltaPhiCutBase> subjet_deltaphi_ptr;
  bool SubJetTrigger(fastjet::PseudoJet subjet, int ipp, int ipr);

  //----------------------------------------------------------------------------
  std::string GetParamsTag(int i);
  std::string GetParamsTag(std::array<int, 2> i);
  std::string GetParamsTag(int i_r, int i_a);
  std::string GetParamsTag(double r_l, double r_u, double a_l, double a_u);

  int GetParamIndex(int i_r, int i_a);
  int GetParamIndex(std::array<int, 2> i);
  std::array<int, 2> GetParamIndex(int i);

public:
  TaggedJetPair(std::string name_in = "TaggedJetPair");
  ~TaggedJetPair();
  std::string Name() { return name; }
};

#endif // TaggedJetPair_H
